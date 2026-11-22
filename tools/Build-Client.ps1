<#
.SYNOPSIS
  Builds the reconstructed client from a fresh clone. The host needs Windows with Hyper-V, Windows
  PowerShell, the OpenSSH client (built in) and 7-Zip; no Python (docs/build-your-own.md).

.DESCRIPTION
  Each step is skipped when its result is already there, so run it again after a failure or after
  pulling new source. Nothing is downloaded without asking: each download is listed with the
  third-party notice first (-AcceptDownloads answers yes for an unattended run).

   1. Checks: the clone at C:\EnBDevelopment\EnB (the VM mirrors that path), Hyper-V, OpenSSH, tar.
   2. The build VM: if none is set up, tools\vc6\setup\New-BuildVM.ps1 -NoBase (one VM, 30-60 minutes;
      docs/setup-vc6.md). Then it is started and reached over SSH.
   3. The SDKs (docs/toolchain.md section 2): DirectX 8.0 and RAD's Bink / Miles files in
      C:\EnBDevelopment\SDK, each checked against its SHA-256; if missing, offered for download from
      the repositories the project used, at their commits.
   4. Python inside the VM (python.org's embeddable package, SHA-256-checked, unpacked in the VM's
      C:\EnBTools\Python): the build tools are Python, and the VM is the only place they run.
   5. The tree and the SDKs copied into the VM (C:\EnBDevelopment, as on the host).
   6. In the VM: python tools\build_client.py --local (the import libraries, the generated workspace
      built by msdev /MAKE: every library, then the client).
   7. build\client\ copied back: client.exe (the reconstruction alone), client.map, the logs.

.EXAMPLE
  powershell -ExecutionPolicy Bypass -File tools\Build-Client.ps1
#>
param(
    [string]$VMName = '',
    [string]$ConfigFile = '',
    [switch]$AcceptDownloads,
    [switch]$Rebuild,
    [switch]$KeepRunning
)
$ErrorActionPreference = 'Stop'
$env:PSModulePath = (@([Environment]::GetEnvironmentVariable('PSModulePath', 'Machine'),
    (Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'WindowsPowerShell\Modules')) -join ';')
. (Join-Path $PSScriptRoot 'vc6\setup\EnBSetup.ps1')
$Repo = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$Root = 'C:\EnBDevelopment'
$Sdk = Join-Path $Root 'SDK'
$Media = Join-Path $Root 'Tools\Media'
$Out = Join-Path $Repo 'build\client'
$VMPython = 'C:\EnBTools\Python'

# docs/toolchain.md section 2: the repositories the project used, and the files the build reads from them
$SdkRepos = [ordered]@{
    directx8 = @{ Url = 'https://github.com/edgeforce/directx8'; Commit = 'e6fa10f5f92bf523c15fdb068206ce25c8d2121d'
                  Name = 'Microsoft DirectX 8.0 SDK (a copy on GitHub, edgeforce/directx8)' }
    radtools = @{ Url = 'https://github.com/edgeforce/radtools'; Commit = 'f42d146c164f264d22e593827a2a028e45fd83d6'
                  Name = 'RAD Game Tools Bink and Miles headers and libraries (a copy on GitHub, edgeforce/radtools)' }
}
$SdkHashes = [ordered]@{   # tools/build_client.py has the same list
    'directx8\include\d3d8.h'  = '3ca75d0512eb882de054bf8d93c1a25dc356e3681d83eaf4e15e7d46d2eb80b0'
    'directx8\include\d3dx8.h' = '4dda934ffc04ca0a7ee7551ee524c1eb9e5ec85cb345202ea0661c71feff122c'
    'directx8\lib\d3d8.lib'    = '3c9f0dc543c41c4b03d380ae47591133db4718dd4bd6c0b739c97a4aa9a493dd'
    'directx8\lib\d3dx8.lib'   = '32148094cebbbe9b55f7769787cdde5926ca01014e072844f5152d25072e1f44'
    'directx8\lib\dxguid.lib'  = '70a74a5e9c195f73db8d6a9bd082611825d07605b67fc7bceb10171d2c194cce'
    'radtools\include\bink.h'  = '25c9a43ae85386cd0d9e00fe5d1f3670814c7a6be2e7c5d8e9952a5fa8674f99'
    'radtools\include\mss.h'   = '721bac19260c63445524acc8a027e25307c4ddaa8e47990a72b7109567047377'
    'radtools\include\rad.h'   = '528d9fea30775f34af20c8779b44435e1d7e3ad1ad6c24e879aa81ac40719b69'
    'radtools\lib\binkw32.lib' = '4ef062e19174050cf251454c92670bca256e5be3dfee0ba6ae4aa2b54ce907d1'
}
$SdkCopy = @('directx8\include', 'directx8\lib', 'radtools\include', 'radtools\lib')   # what goes into the VM
$Python = @{ Url = 'https://www.python.org/ftp/python/3.12.10/python-3.12.10-embed-amd64.zip'
             Sha256 = '4acbed6dd1c744b0376e3b1cf57ce906f9dc9e95e68824584c8099a63025a3c3'
             File = 'python-3.12.10-embed-amd64.zip'
             Name = 'Python 3.12.10, the Windows embeddable package (Python Software Foundation), for inside the VM only' }

function Step([string]$m) { Write-Host "`n== $m" }
function Stop-Here([string]$m) { Write-Host "`nSTOPPED: $m"; exit 1 }

# ---------------------------------------------------------------- 1. checks
Step '1. checks'
if ($PSVersionTable.PSEdition -ne 'Desktop') { Stop-Here 'run with Windows PowerShell (powershell.exe), not pwsh' }
if ($Repo -ne (Join-Path $Root 'EnB')) { Stop-Here "the clone must be at $Root\EnB (it is $Repo): the VM mirrors $Root at the same path, so include paths and the client's __FILE__ strings match" }
if (-not (Get-Command Get-VM -ErrorAction SilentlyContinue)) { Stop-Here 'Hyper-V is not enabled (docs/build-your-own.md "What you need")' }
foreach ($c in 'ssh', 'scp', 'tar', 'curl.exe') { if (-not (Get-Command $c -ErrorAction SilentlyContinue)) { Stop-Here "$c not found (the OpenSSH client: Settings > Optional features)" } }
Write-Host '   clone, Hyper-V, OpenSSH, tar and curl found'

# ---------------------------------------------------------------- 2. the build VM
Step '2. the build VM'
$cfgFile = if ($ConfigFile) { $ConfigFile } else { Join-Path $Repo '.work\vc6_config.json' }
$cfg = @{ user = 'user'; key = (Join-Path $HOME '.ssh\enb_vc6'); template_vm = 'enb-vc6-template' }
if (Test-Path $cfgFile) { (Get-Content -Raw $cfgFile | ConvertFrom-Json).PSObject.Properties | ForEach-Object { $cfg[$_.Name] = $_.Value } }
if (-not $VMName) { $VMName = $cfg.template_vm }
$key = $cfg.key -replace '^~', $HOME
$made = $false
if (-not (Get-VM -Name $VMName -ErrorAction SilentlyContinue)) {
    $made = $true
    Write-Host "   no build VM '$VMName' yet: tools\vc6\setup\New-BuildVM.ps1 makes one (30-60 minutes, unattended)"
    & (Join-Path $PSScriptRoot 'vc6\setup\New-BuildVM.ps1') -Name $VMName -NoBase -ConfigFile $cfgFile -AcceptDownloads:$AcceptDownloads
    if (-not (Get-VM -Name $VMName -ErrorAction SilentlyContinue)) { Stop-Here 'New-BuildVM.ps1 did not make the VM (its message above; docs/setup-vc6.md section 6)' }
    $cfg = @{}; (Get-Content -Raw $cfgFile | ConvertFrom-Json).PSObject.Properties | ForEach-Object { $cfg[$_.Name] = $_.Value }
    $key = $cfg.key -replace '^~', $HOME
}
$started = $made                       # a VM this run made is shut down at the end too
$vm = Get-VM -Name $VMName
if ($vm.State -ne 'Running') { Write-Host "   starting $VMName"; Start-VM -VM $vm; $started = $true }
$ip = Get-VMAddress $VMName
$target = "$($cfg.user)@$ip"
$deadline = (Get-Date).AddMinutes(5)
while ((Invoke-VMSsh $target $key 'hostname' -Capture) -match 'Connection|refused|timed out|denied') {
    if ((Get-Date) -gt $deadline) { Stop-Here "ssh $target does not answer (docs/setup-vc6.md section 6)" }
    Start-Sleep 5
}
Write-Host "   $VMName at $target"

try {
    # ------------------------------------------------------------ 3. the SDKs
    Step '3. the SDKs'
    $missing = @($SdkRepos.Keys | Where-Object { -not (Test-Path (Join-Path $Sdk $_)) })
    if ($missing) {
        $items = @($missing | ForEach-Object { @{ Name = $SdkRepos[$_].Name; Url = "$($SdkRepos[$_].Url)/archive/$($SdkRepos[$_].Commit).zip" } })
        if (-not (Confirm-Download $items -Accept:$AcceptDownloads)) {
            Stop-Here "no SDKs: put your own copies in $Sdk\directx8 and $Sdk\radtools (the files and hashes are in docs/toolchain.md section 2), then run again"
        }
        foreach ($n in $missing) {
            $r = $SdkRepos[$n]
            $zip = Join-Path $Media "sdk-$n-$($r.Commit.Substring(0, 7)).zip"
            New-Item -ItemType Directory -Force $Media | Out-Null
            if (-not (Test-Path $zip)) {
                Write-Host "   downloading $($r.Url)/archive/$($r.Commit).zip"
                & curl.exe -fsSL -o "$zip.part" "$($r.Url)/archive/$($r.Commit).zip"
                if ($LASTEXITCODE) { Stop-Here "download failed: $($r.Url)" }
                Move-Item -Force "$zip.part" $zip
            }
            $x = Join-Path $Media "sdk-$n.x"
            if (Test-Path $x) { Remove-Item -Recurse -Force $x }
            New-Item -ItemType Directory -Force $x | Out-Null
            & tar -xf $zip -C $x
            if ($LASTEXITCODE) { Stop-Here "could not unpack $zip" }
            New-Item -ItemType Directory -Force $Sdk | Out-Null
            Move-Item (Get-ChildItem $x -Directory | Select-Object -First 1).FullName (Join-Path $Sdk $n)
            Remove-Item -Recurse -Force $x
        }
    }
    $bad = @($SdkHashes.Keys | Where-Object { -not (Test-Path (Join-Path $Sdk $_)) -or (Get-SdkSha (Join-Path $Sdk $_)) -ne $SdkHashes[$_] })
    if ($bad) { Stop-Here ("these SDK files are missing or differ from the copies the project used (docs/toolchain.md section 2): " + ($bad -join ', ')) }
    Write-Host "   $($SdkHashes.Count) SDK files match their hashes"

    # ------------------------------------------------------------ 4. Python in the VM
    Step '4. Python in the VM'
    $has = Invoke-VMSsh $target $key "cmd /c if exist $VMPython\python.exe echo present" -Capture
    if ($has -match 'present') { Write-Host "   $VMPython present" }
    else {
        $zip = Join-Path $Media $Python.File
        if (-not ((Test-Path $zip) -and (Get-FileSha $zip) -eq $Python.Sha256)) {
            if (-not (Confirm-Download @(@{ Name = $Python.Name; Url = $Python.Url; Size = '11 MB' }) -Accept:$AcceptDownloads)) {
                Stop-Here 'no Python for the VM: the build tools need it there (nothing is installed on this PC)'
            }
            Save-Download $Python.Url $zip $Python.Sha256
        }
        Copy-ToVM $target $key $zip "$VMWork\$($Python.File)"
        # the ._pth file would confine sys.path to the package: without it the tools import their neighbours
        Invoke-VMBat $target $key -NoVCVars @(
            "powershell -NoProfile -Command `$ProgressPreference='SilentlyContinue'; Expand-Archive -Force $VMWork\$($Python.File) $VMPython",
            "del $VMPython\python312._pth", "del $VMWork\$($Python.File)",
            "$VMPython\python.exe -c `"import sys; print('python', sys.version.split()[0])`"")
        if ($LASTEXITCODE) { Stop-Here 'could not unpack Python in the VM' }
    }

    # ------------------------------------------------------------ 5. the tree into the VM
    Step '5. the tree and the SDKs into the VM'
    $list = Join-Path $env:TEMP 'enb_build_client_files.txt'
    $files = $null
    if ((Test-Path (Join-Path $Repo '.git')) -and (Get-Command git -ErrorAction SilentlyContinue)) {
        # the committed files; of docs\ only the ledger's shards (docs\data\imports: the tools read the app's file list there)
        $files = & git -C $Repo ls-files | Where-Object { $_ -notmatch '^docs/' -or $_ -match '^docs/data/imports/' }
    } else {
        $files = Get-ChildItem $Repo -Recurse -File | ForEach-Object { $_.FullName.Substring($Repo.Length + 1).Replace('\', '/') } |
            Where-Object { ($_ -notmatch '^(\.git|build|\.work|docs)/' -or $_ -match '^docs/data/imports/') -and $_ -notmatch '/client_release/' }
    }
    $entries = @($files | ForEach-Object { "EnB/$_" }) + @($SdkCopy | ForEach-Object { 'SDK/' + $_.Replace('\', '/') })
    [System.IO.File]::WriteAllText($list, ($entries -join "`n") + "`n", [Text.Encoding]::ASCII)   # LF: tar -T keeps a CR in each name
    $tarFile = Join-Path $env:TEMP 'enb_build_client_tree.tar'
    & tar -cf $tarFile -C $Root -T $list
    if ($LASTEXITCODE) { Stop-Here 'could not pack the tree' }
    Write-Host ("   {0} files, {1:N1} MB" -f $entries.Count, ((Get-Item $tarFile).Length / 1MB))
    Copy-ToVM $target $key $tarFile "$VMWork\tree.tar"
    Remove-Item -Force $tarFile, $list
    # the VM's previous copy goes first (a file deleted here must not stay there), but its build outputs stay:
    # build\ and every project's client_release\, so the IDE build compiles only what changed
    Invoke-VMBat $target $key -NoVCVars @(
        "if exist $Root\EnB powershell -NoProfile -Command `"Get-ChildItem $Root\EnB -Recurse -File | Where-Object { `$_.FullName -notmatch '\\(build|client_release)\\' } | Remove-Item -Force`"",
        "tar -xf $VMWork\tree.tar -C $Root", "del $VMWork\tree.tar")
    if ($LASTEXITCODE) { Stop-Here 'could not unpack the tree in the VM' }

    # ------------------------------------------------------------ 6. the build, in the VM
    Step '6. the build, in the VM (tools\build_client.py --local)'
    $flags = @('--local') + $(if ($Rebuild) { '--rebuild' })
    Invoke-VMBat $target $key -NoVCVars @("cd /d $Root\EnB", "$VMPython\python.exe -u tools\build_client.py $($flags -join ' ')")
    if ($LASTEXITCODE) { Stop-Here 'the build stopped in the VM (its message above)' }

    # ------------------------------------------------------------ 7. results
    Step '7. results'
    if (Test-Path $Out) { Remove-Item -Recurse -Force $Out }
    New-Item -ItemType Directory -Force (Split-Path $Out) | Out-Null
    Copy-FromVM $target $key "$Root\EnB\build\client" (Split-Path $Out) -Recurse
    Write-Host "   $Out"
    Get-ChildItem $Out -File | ForEach-Object { Write-Host "     $($_.Name)" }
    Write-Host "`ndone. Running it: docs/build-your-own.md `"Running what you built`""
} finally {
    if ($started -and -not $KeepRunning) { Write-Host "   stopping $VMName"; Stop-VM -Name $VMName }
}
