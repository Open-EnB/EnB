<#
.SYNOPSIS
  Shared helpers of New-BuildVM.ps1 and tools\Build-Client.ps1 (dot-source it). Windows PowerShell 5.1
  only, no Python: the host needs nothing but Windows, Hyper-V, OpenSSH (built in) and 7-Zip.

  Downloads:   Confirm-Download (the third-party notice and a y/N question), Save-Download (curl.exe
               with a hash check), Get-FileSha (SHA-1 / SHA-256 without Get-FileHash's module).
  VC6 media:   Get-VC6Media (the two Internet Archive items, SHA-1-checked, unpacked with 7-Zip, each
               disc image turned into an ISO; was media.py).
  The VM:      Invoke-VMBat (batch lines run in the VM over SSH), Test-BuildVM (cl 12.00.8804, link
               6.00.8447 and an object's @comp.id build 8799; was check_vm.py).
#>

$EnBNotice = @'
These packages are third-party software, downloaded from the addresses shown. Open-EnB does not
distribute them, grants no licence to them, and makes no promise about them. Make sure you have
the right to download and use them before you continue.
'@

function Get-FileSha([string]$Path, [string]$Algorithm = 'SHA256') {
    $s = [System.IO.File]::OpenRead($Path)
    try {
        $h = [System.Security.Cryptography.HashAlgorithm]::Create($Algorithm)
        return -join ($h.ComputeHash($s) | ForEach-Object { $_.ToString('x2') })
    } finally { $s.Dispose() }
}

function Get-SdkSha([string]$Path) {
    <# An SDK file's SHA-256 as docs/toolchain.md section 2 records it: a header (.h) with CRLF line ends
       (the hashes were taken from a Windows checkout; GitHub's archives and an autocrlf=false clone
       have LF), anything else as it is. #>
    if ($Path -notmatch '\.h$') { return Get-FileSha $Path }
    $b = [System.IO.File]::ReadAllBytes($Path)
    $o = New-Object System.IO.MemoryStream
    for ($i = 0; $i -lt $b.Length; $i++) {
        if ($b[$i] -eq 10 -and ($i -eq 0 -or $b[$i - 1] -ne 13)) { $o.WriteByte(13) }
        $o.WriteByte($b[$i])
    }
    return -join ([System.Security.Cryptography.SHA256]::Create().ComputeHash($o.ToArray()) | ForEach-Object { $_.ToString('x2') })
}

function Confirm-Download([object[]]$Items, [switch]$Accept) {
    <# Items: @{ Name; Url; Size } each. Prints them and the notice; returns $true on "y" (or -Accept). #>
    Write-Host ''
    Write-Host 'This step downloads:'
    foreach ($i in $Items) { Write-Host ("  - {0}`n      {1}{2}" -f $i.Name, $i.Url, $(if ($i.Size) { " ($($i.Size))" } else { '' })) }
    Write-Host ''
    Write-Host $EnBNotice
    if ($Accept) { Write-Host '(accepted with -AcceptDownloads)'; return $true }
    $a = Read-Host 'Download them now? [y/N]'
    return ($a -match '^(y|yes)$')
}

function Save-Download([string]$Url, [string]$Path, [string]$Hash, [string]$Algorithm = 'SHA256') {
    <# Downloads Url to Path unless Path already has the hash; checks the hash either way. #>
    if ((Test-Path $Path) -and (Get-FileSha $Path $Algorithm) -eq $Hash.ToLower()) { return }
    New-Item -ItemType Directory -Force (Split-Path $Path) | Out-Null
    Write-Host "downloading $Url"
    & curl.exe -fsSL -o "$Path.part" $Url
    if ($LASTEXITCODE) { throw "download failed: $Url" }
    $got = Get-FileSha "$Path.part" $Algorithm
    if ($got -ne $Hash.ToLower()) { Remove-Item -Force "$Path.part"; throw "$Url`: $Algorithm $got, expected $Hash" }
    Move-Item -Force "$Path.part" $Path
}

function Get-SevenZip {
    $c = (Get-Command 7z -ErrorAction SilentlyContinue).Source
    foreach ($p in @($c, 'C:\Program Files\7-Zip\7z.exe', 'C:\Program Files (x86)\7-Zip\7z.exe')) {
        if ($p -and (Test-Path $p)) { return $p }
    }
    throw '7-Zip not found: install it (https://www.7-zip.org/) or put 7z on PATH'
}

# ---------------------------------------------------------------- the VC6 media (was media.py)
# name -> Internet Archive item, file in the item, SHA-1 from the item's metadata, volume label
$VC6Media = [ordered]@{
    'vc6pro-disc1' = @{ Item = 'X04-14444'; File = '1999-02_X04-14444_X04-14444_VSP600ENU1.7z'
                        Sha1 = '50a0674f8e881410c3f35fc7e09bb39213167797'; Label = 'VSP600ENU1'; Size = '560 MB' }
    'vs6sp4'       = @{ Item = 'X05-86594'; File = '2000-08_X05-86594_X05-86829_vs6sp4.7z'
                        Sha1 = '7ba4001a8e6a3af8f9807d85471b5ea83000eae8'; Label = 'VS6SP4'; Size = '140 MB' }
}

function Get-VC6MediaItems {
    foreach ($n in $VC6Media.Keys) {
        $m = $VC6Media[$n]
        @{ Name = "Visual Studio 6.0 media '$n' (Internet Archive item $($m.Item))"
           Url = "https://archive.org/download/$($m.Item)/$($m.File)"; Size = $m.Size }
    }
}

function Get-IsoLabel([string]$Iso) {
    $f = [System.IO.File]::OpenRead($Iso)
    try {
        $b = New-Object byte[] 2048
        [void]$f.Seek(16 * 2048, 'Begin'); [void]$f.Read($b, 0, 2048)
        if ([Text.Encoding]::ASCII.GetString($b, 1, 5) -ne 'CD001') { return $null }
        return [Text.Encoding]::ASCII.GetString($b, 40, 32).Trim()
    } finally { $f.Dispose() }
}

function Convert-MdfToIso([string]$Mdf, [string]$Iso) {
    <# An Alcohol 120% MDF of raw 2,448-byte sectors (2,352 of sector + 96 of subchannel), Mode 1:
       12 sync + 4 header bytes, then the 2,048 bytes of user data an ISO holds. #>
    $raw = 2448
    $len = (Get-Item $Mdf).Length
    if ($len % $raw) { throw "$Mdf`: $len bytes is not a whole number of $raw-byte sectors" }
    $src = [System.IO.File]::OpenRead($Mdf)
    $dst = [System.IO.File]::Create("$Iso.part")
    try {
        $s = New-Object byte[] $raw
        $sync = [byte[]](0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0)
        for ($i = 0; $i -lt $len / $raw; $i++) {
            [void]$src.Read($s, 0, $raw)
            for ($k = 0; $k -lt 12; $k++) { if ($s[$k] -ne $sync[$k]) { throw "$Mdf`: not a Mode 1 data sector" } }
            if ($s[15] -ne 1) { throw "$Mdf`: not a Mode 1 data sector" }
            $dst.Write($s, 16, 2048)
        }
    } finally { $src.Dispose(); $dst.Dispose() }
    Move-Item -Force "$Iso.part" $Iso
}

function Get-VC6Media([string]$MediaDir) {
    <# Writes MediaDir\vc6pro-disc1.iso and vs6sp4.iso (downloading the archives when missing). #>
    New-Item -ItemType Directory -Force $MediaDir | Out-Null
    $sz = Get-SevenZip
    foreach ($n in $VC6Media.Keys) {
        $m = $VC6Media[$n]
        $iso = Join-Path $MediaDir "$n.iso"
        if ((Test-Path $iso) -and (Get-IsoLabel $iso) -eq $m.Label) { Write-Host "$n`: $iso present (label $($m.Label))"; continue }
        $arc = @((Join-Path $MediaDir $m.File), (Join-Path $MediaDir "$n.7z")) | Where-Object { Test-Path $_ } | Select-Object -First 1
        if (-not $arc) { $arc = Join-Path $MediaDir $m.File }
        Save-Download "https://archive.org/download/$($m.Item)/$($m.File)" $arc $m.Sha1 'SHA1'
        Write-Host "$n`: $(Split-Path -Leaf $arc) SHA-1 OK"
        $work = Join-Path $MediaDir "$n.x"
        if (Test-Path $work) { Remove-Item -Recurse -Force $work }
        & $sz x -y "-o$work" $arc | Out-Null
        if ($LASTEXITCODE) { throw "7-Zip could not extract $arc" }
        $mdfs = @(Get-ChildItem $work -Recurse -Filter *.mdf)
        if ($mdfs.Count -ne 1) { throw "$arc`: expected one .mdf inside, found $($mdfs.Count)" }
        Convert-MdfToIso $mdfs[0].FullName $iso
        Remove-Item -Recurse -Force $work
        $label = Get-IsoLabel $iso
        if ($label -ne $m.Label) { throw "$iso`: volume label '$label', expected '$($m.Label)'" }
        Write-Host "$n`: wrote $iso (label $($m.Label))"
    }
}

# ---------------------------------------------------------------- the VM over SSH
$VMWork = 'C:\EnBDevelopment\_vc6'
$VCVars = 'C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\VCVARS32.BAT'

function Invoke-VMSsh([string]$Target, [string]$Key, [string]$Command, [switch]$Capture) {
    <# One command in the VM. Windows PowerShell 5.1 mangles double quotes inside native arguments, so
       Command must have none: put anything quoted into a batch file (Invoke-VMBat). #>
    $old = $ErrorActionPreference; $ErrorActionPreference = 'Continue'
    try {
        if ($Capture) { $out = & ssh -i $Key -o BatchMode=yes -o ConnectTimeout=10 $Target $Command 2>&1 | Out-String; return $out }
        & ssh -i $Key -o BatchMode=yes -o ConnectTimeout=10 $Target $Command
    } finally { $ErrorActionPreference = $old }
}

function Copy-ToVM([string]$Target, [string]$Key, [string]$Local, [string]$Remote) {
    $old = $ErrorActionPreference; $ErrorActionPreference = 'Continue'
    & scp -q -i $Key -o BatchMode=yes $Local "$Target`:$($Remote.Replace('\', '/'))"
    $rc = $LASTEXITCODE; $ErrorActionPreference = $old
    if ($rc) { throw "scp to the VM failed: $Local" }
}

function Copy-FromVM([string]$Target, [string]$Key, [string]$Remote, [string]$Local, [switch]$Recurse) {
    $old = $ErrorActionPreference; $ErrorActionPreference = 'Continue'
    $r = @('-q', '-i', $Key, '-o', 'BatchMode=yes')
    if ($Recurse) { $r += '-r' }
    & scp @r "$Target`:$($Remote.Replace('\', '/'))" $Local
    $rc = $LASTEXITCODE; $ErrorActionPreference = $old
    if ($rc) { throw "scp from the VM failed: $Remote" }
}

function Invoke-VMBat([string]$Target, [string]$Key, [string[]]$Lines, [switch]$Capture, [switch]$NoVCVars) {
    <# Batch lines run in the VM (after VCVARS32 unless -NoVCVars); returns the output with -Capture,
       else streams it. $LASTEXITCODE is the batch's. #>
    $name = 'run_ps_' + [guid]::NewGuid().ToString('N').Substring(0, 8) + '.bat'
    $local = Join-Path $env:TEMP $name
    $head = @('@echo off')
    if (-not $NoVCVars) { $head += "call `"$VCVars`" >nul" }
    [System.IO.File]::WriteAllText($local, (($head + $Lines) -join "`r`n") + "`r`n", [Text.Encoding]::ASCII)
    [void](Invoke-VMSsh $Target $Key "cmd /c if not exist $VMWork mkdir $VMWork" -Capture)
    Copy-ToVM $Target $Key $local "$VMWork\$name"
    Remove-Item -Force $local
    if ($Capture) { return (Invoke-VMSsh $Target $Key "cmd /c $VMWork\$name" -Capture) }
    Invoke-VMSsh $Target $Key "cmd /c $VMWork\$name"
}

function Get-CompId([string]$Obj) {
    <# The @comp.id symbol's value of a COFF object: (product -shl 16) -bor build. #>
    $b = [System.IO.File]::ReadAllBytes($Obj)
    $symOff = [BitConverter]::ToUInt32($b, 8); $nsym = [BitConverter]::ToUInt32($b, 12)
    $strtab = $symOff + 18 * $nsym
    $i = 0
    while ($i -lt $nsym) {
        $e = $symOff + 18 * $i
        if ([BitConverter]::ToUInt32($b, $e) -eq 0) {
            $p = $strtab + [BitConverter]::ToUInt32($b, $e + 4); $q = $p
            while ($b[$q] -ne 0) { $q++ }
            $name = [Text.Encoding]::ASCII.GetString($b, $p, $q - $p)
        } else { $name = [Text.Encoding]::ASCII.GetString($b, $e, 8).TrimEnd([char]0) }
        if ($name -eq '@comp.id') { return [BitConverter]::ToUInt32($b, $e + 8) }
        $i += 1 + $b[$e + 17]
    }
    return $null
}

function Test-BuildVM([string]$Target, [string]$Key) {
    <# Proves the VM compiles as the client's compiler (was check_vm.py): cl 12.00.8804, link 6.00.8447,
       and a C++ object (/O2 /GX /MT, docs/toolchain.md 1.2) stamped @comp.id product 11, build 8799. #>
    $src = Join-Path $env:TEMP 'enb_vmcheck.cpp'
    [System.IO.File]::WriteAllText($src, "struct A { virtual int f(int x) { return x * 3 + 1; } };`r`nint g(A *a) { return a->f(7); }`r`n")
    [void](Invoke-VMSsh $Target $Key "cmd /c if not exist $VMWork mkdir $VMWork" -Capture)
    Copy-ToVM $Target $Key $src "$VMWork\enb_vmcheck.cpp"
    $out = Invoke-VMBat $Target $Key @("cd /d $VMWork", 'cl 2>&1 | findstr Version', 'link 2>&1 | findstr Version',
        'cl /nologo /c /MT /O2 /GX enb_vmcheck.cpp >nul') -Capture
    Write-Host $out.Trim()
    $ok = ($out -match '12\.00\.8804') -and ($out -match '6\.00\.8447')
    if (-not $ok) { Write-Host 'FAIL: expected cl 12.00.8804 and link 6.00.8447 (VC6 SP4)' }
    $obj = Join-Path $env:TEMP 'enb_vmcheck.obj'
    if (Test-Path $obj) { Remove-Item -Force $obj }
    Copy-FromVM $Target $Key "$VMWork\enb_vmcheck.obj" $obj
    $cid = Get-CompId $obj
    $prod = if ($null -ne $cid) { $cid -shr 16 } else { $null }; $bld = if ($null -ne $cid) { $cid -band 0xFFFF } else { $null }
    Write-Host "@comp.id product $prod build $bld"
    if ($prod -ne 11 -or $bld -ne 8799) { Write-Host 'FAIL: expected product 11 (C++) build 8799 (VC6 SP4)'; $ok = $false }
    Remove-Item -Force $src, $obj -ErrorAction SilentlyContinue
    Write-Host $(if ($ok) { 'PASS' } else { 'FAIL' })
    return $ok
}

function Get-VMAddress([string]$Name, [int]$TimeoutSeconds = 300) {
    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    while ((Get-Date) -lt $deadline) {
        $ip = (Get-VMNetworkAdapter -VMName $Name).IPAddresses | Where-Object { $_ -match '^[0-9.]+$' -and $_ -notmatch '^169\.254\.' } | Select-Object -First 1
        if ($ip) { return $ip }
        Start-Sleep 5
    }
    throw "VM $Name has no IPv4 address after $TimeoutSeconds s"
}
