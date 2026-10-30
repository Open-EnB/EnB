<#
.SYNOPSIS
  Builds the VC6 SP4 build VM end to end, and from it the base disk of the VM pool.

.DESCRIPTION
  docs/setup-vc6.md explains every step. Needs Hyper-V (Windows Pro, Enterprise or Education), the
  account in the Hyper-V Administrators group (no elevation), Python 3, 7-Zip, OpenSSH client and
  internet access. Unattended from start to finish; about 30-60 minutes, most of it downloads.

   1. tools/vc6/setup/media.py: the VC6 and SP4 discs from the Internet Archive, hash-checked,
      turned into ISOs.
   2. Windows: Microsoft's "Windows 10 MSIX packaging environment" image (the Hyper-V Quick Create
      gallery's, downloaded directly and SHA-256-checked; Windows 10 Enterprise Evaluation, 90 days
      from first boot), or -WindowsVhdx with a generalized Windows 10 disk of your own.
   3. A seed ISO (built with Windows' IMAPI, no elevation): autounattend.xml (the account, no
      out-of-box screens, a one-time logon that runs the setup) and the guest scripts.
   4. The VM: generation 2, Secure Boot, the pool's hardware, the Windows disk, three DVDs (seed,
      VC6, SP4). First boot runs guest\Setup-Guest.ps1: OpenSSH with our key, VC6 by file copy
      (guest\Install-VC6.ps1, every file SHA-1-checked), the pool base's settings.
   5. Checks over SSH: cl answers 12.00.8804 and an object stamps @comp.id build 8799.
   6. Unless -NoBase: shuts the VM down, removes the DVDs and copies its disk to
      <PoolDir>\base-vc6.vhdx (read-only), the parent of every pool clone; the VM stays as the
      pool's hardware template (never started by the pool).
   7. Writes .work/vc6_config.json (or -ConfigFile) so vc6.py and pool.py use this VM.

.EXAMPLE
  powershell -ExecutionPolicy Bypass -File tools\vc6\setup\New-BuildVM.ps1
#>
param(
    [string]$Name = 'enb-vc6-template',
    [string]$User = 'user',
    [string]$PoolDir = 'C:\EnBDevelopment\Tools\VMPool',
    [string]$Prefix = 'enb-vc6-',
    [string]$MediaDir = 'C:\EnBDevelopment\Tools\Media',
    [string]$WindowsVhdx = '',
    [string]$Key = (Join-Path $HOME '.ssh\enb_vc6'),
    [string]$Switch = 'Default Switch',
    [int]$Cpu = [Math]::Min(8, [Environment]::ProcessorCount),
    [int]$StartupGB = 4,
    [string]$ConfigFile = '',
    [switch]$NoBase,
    [switch]$ReplaceBase,
    [int]$TimeoutMinutes = 90
)
$ErrorActionPreference = 'Stop'
# Started from PowerShell 7 or Git Bash, Windows PowerShell inherits pwsh's module path and cannot
# load its own modules (Get-FileHash "not found"): use the machine's path plus the user's own.
$env:PSModulePath = (@([Environment]::GetEnvironmentVariable('PSModulePath', 'Machine'),
    (Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'WindowsPowerShell\Modules')) -join ';')
function Sha256([string]$path) {
    $s = [System.IO.File]::OpenRead($path)
    try { return -join ([System.Security.Cryptography.SHA256]::Create().ComputeHash($s) | ForEach-Object { $_.ToString('X2') }) }
    finally { $s.Dispose() }
}
$Repo = (Resolve-Path (Join-Path $PSScriptRoot '..\..\..')).Path
if (-not $ConfigFile) { $ConfigFile = Join-Path $Repo '.work\vc6_config.json' }
$Gallery = @{   # the Quick Create gallery's entry (https://go.microsoft.com/fwlink/?linkid=851584)
    Uri    = 'https://download.microsoft.com/download/5/4/c/54cf292f-a9e3-47d3-9b4d-5659dc1c6189/MSIX%20Packaging%20Tool%2010182019.zip'
    Sha256 = '09C50382D496C5DF2C96034EB69F20D456B25308B3F672257D55FD8202DDF84B'
    File   = 'msix-packaging-environment.zip'
}
function Step([string]$m) { Write-Host "== $m" }
function Need([string]$cmd, [string]$hint) { if (-not (Get-Command $cmd -ErrorAction SilentlyContinue)) { throw "$cmd not found: $hint" } }

# ---------------------------------------------------------------- 0. checks
Step 'checks'
if ($PSVersionTable.PSEdition -ne 'Desktop') { throw 'run with Windows PowerShell 5.1 (powershell.exe), not pwsh: the seed ISO writer needs it' }
Need 'Get-VM' 'enable Hyper-V (Windows features: Hyper-V) and add your account to Hyper-V Administrators'
Need 'python' 'install Python 3 (https://www.python.org/)'
Need 'ssh' 'enable the OpenSSH Client (Settings > Optional features)'
Need 'ssh-keygen' 'enable the OpenSSH Client (Settings > Optional features)'
if (Get-VM -Name $Name -ErrorAction SilentlyContinue) { throw "a VM named $Name exists already (remove it, or pick -Name)" }
if (-not (Get-VMSwitch -Name $Switch -ErrorAction SilentlyContinue)) { throw "Hyper-V switch '$Switch' not found" }
$base = Join-Path $PoolDir 'base-vc6.vhdx'
if (-not $NoBase -and (Test-Path $base) -and -not $ReplaceBase) { throw "$base exists (pass -ReplaceBase after destroying the pool's clones, or -NoBase)" }
if ($ReplaceBase -and (Get-VM | Where-Object { $_.Name.StartsWith($Prefix) -and $_.Name -ne $Name })) { throw "pool VMs $Prefix* still exist: destroy them first (python tools/vc6/pool.py destroy --all-free); their disks are children of the base" }
New-Item -ItemType Directory -Force $MediaDir, $PoolDir | Out-Null

# ---------------------------------------------------------------- 1. VC6 media
Step 'VC6 and SP4 media'
python -I (Join-Path $PSScriptRoot 'media.py') --media $MediaDir --download
if ($LASTEXITCODE) { throw 'media.py failed' }

# ---------------------------------------------------------------- 2. Windows disk
$vmDir = Join-Path $PoolDir $Name
New-Item -ItemType Directory -Force $vmDir | Out-Null
$disk = Join-Path $vmDir "$Name.vhdx"
if (Test-Path $disk) { Remove-Item -Force $disk }   # left by an earlier failed run (no VM uses it: checked above)
if (-not $WindowsVhdx) {
    Step 'Windows 10 image (Quick Create gallery: MSIX packaging environment)'
    $zip = Join-Path $MediaDir $Gallery.File
    if (-not (Test-Path $zip) -or (Sha256 $zip) -ne $Gallery.Sha256) {
        Write-Host "downloading $($Gallery.Uri) (4.7 GB)"
        & curl.exe -sSL -o "$zip.part" $Gallery.Uri
        if ($LASTEXITCODE) { throw 'download failed' }
        Move-Item -Force "$zip.part" $zip
        $h = (Sha256 $zip)
        if ($h -ne $Gallery.Sha256) { throw "$zip SHA-256 $h, expected $($Gallery.Sha256)" }
    }
    $x = Join-Path $MediaDir 'msix.x'
    if (Test-Path $x) { Remove-Item -Recurse -Force $x }
    $sz = (Get-Command 7z -ErrorAction SilentlyContinue).Source
    if (-not $sz) { $sz = 'C:\Program Files\7-Zip\7z.exe' }
    & $sz x -y "-o$x" $zip | Out-Null
    if ($LASTEXITCODE) { throw '7-Zip could not extract the image' }
    $WindowsVhdx = (Get-ChildItem $x -Recurse -Filter *.vhdx | Select-Object -First 1).FullName
    if (-not $WindowsVhdx) { throw 'no .vhdx in the gallery image' }
    Move-Item $WindowsVhdx $disk
    Remove-Item -Recurse -Force $x
} else {
    Step "Windows disk: copying $WindowsVhdx"
    Copy-Item $WindowsVhdx $disk
}

# ---------------------------------------------------------------- 3. seed ISO
Step 'seed ISO'
if (-not (Test-Path $Key)) { ssh-keygen -q -t ed25519 -N '""' -f $Key -C 'enb-vc6' ; if ($LASTEXITCODE) { throw 'ssh-keygen failed' } }
$seed = Join-Path $vmDir 'seed'
if (Test-Path $seed) { Remove-Item -Recurse -Force $seed }
New-Item -ItemType Directory -Force (Join-Path $seed 'EnBSetup') | Out-Null
Copy-Item (Join-Path $PSScriptRoot 'guest\*.ps1'), (Join-Path $PSScriptRoot 'vc6-files.tsv') (Join-Path $seed 'EnBSetup')
Copy-Item "$Key.pub" (Join-Path $seed 'EnBSetup\enb_vc6.pub')
$chars = [char[]]'abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ23456789'
$password = -join (1..20 | ForEach-Object { $chars | Get-Random })
$tz = (Get-TimeZone).Id
$first = 'powershell -NoProfile -ExecutionPolicy Bypass -Command "$d = (Get-Volume | Where-Object FileSystemLabel -eq ''ENBSEED'').DriveLetter; Copy-Item -Recurse -Force ($d + '':\EnBSetup'') C:\; & C:\EnBSetup\Setup-Guest.ps1"'
$unattend = @"
<?xml version="1.0" encoding="utf-8"?>
<unattend xmlns="urn:schemas-microsoft-com:unattend" xmlns:wcm="http://schemas.microsoft.com/WMIConfig/2002/State">
  <settings pass="specialize">
    <component name="Microsoft-Windows-Shell-Setup" processorArchitecture="amd64" publicKeyToken="31bf3856ad364e35" language="neutral" versionScope="nonSxS">
      <ComputerName>ENB-VC6</ComputerName>
      <TimeZone>$tz</TimeZone>
    </component>
  </settings>
  <settings pass="oobeSystem">
    <component name="Microsoft-Windows-International-Core" processorArchitecture="amd64" publicKeyToken="31bf3856ad364e35" language="neutral" versionScope="nonSxS">
      <InputLocale>en-US</InputLocale><SystemLocale>en-US</SystemLocale><UILanguage>en-US</UILanguage><UserLocale>en-US</UserLocale>
    </component>
    <component name="Microsoft-Windows-Shell-Setup" processorArchitecture="amd64" publicKeyToken="31bf3856ad364e35" language="neutral" versionScope="nonSxS">
      <OOBE>
        <HideEULAPage>true</HideEULAPage>
        <HideOEMRegistrationScreen>true</HideOEMRegistrationScreen>
        <HideOnlineAccountScreens>true</HideOnlineAccountScreens>
        <HideWirelessSetupInOOBE>true</HideWirelessSetupInOOBE>
        <HideLocalAccountScreen>true</HideLocalAccountScreen>
        <ProtectYourPC>3</ProtectYourPC>
      </OOBE>
      <UserAccounts>
        <LocalAccounts>
          <LocalAccount wcm:action="add">
            <Name>$User</Name><DisplayName>$User</DisplayName><Group>Administrators</Group>
            <Password><Value>$password</Value><PlainText>true</PlainText></Password>
          </LocalAccount>
        </LocalAccounts>
      </UserAccounts>
      <AutoLogon>
        <Enabled>true</Enabled><LogonCount>1</LogonCount><Username>$User</Username>
        <Password><Value>$password</Value><PlainText>true</PlainText></Password>
      </AutoLogon>
      <FirstLogonCommands>
        <SynchronousCommand wcm:action="add">
          <Order>1</Order>
          <CommandLine>$([System.Security.SecurityElement]::Escape($first))</CommandLine>
          <Description>EnB build VM setup</Description>
        </SynchronousCommand>
      </FirstLogonCommands>
    </component>
  </settings>
</unattend>
"@
foreach ($n in 'autounattend.xml', 'unattend.xml') { [System.IO.File]::WriteAllText((Join-Path $seed $n), $unattend) }
$seedIso = Join-Path $vmDir 'seed.iso'
$fsi = New-Object -ComObject IMAPI2FS.MsftFileSystemImage
$fsi.FileSystemsToCreate = 3   # ISO 9660 + Joliet
$fsi.VolumeName = 'ENBSEED'
$fsi.Root.AddTree($seed, $false)
$img = $fsi.CreateResultImage()
$cp = New-Object CodeDom.Compiler.CompilerParameters
$cp.CompilerOptions = '/unsafe'
if (-not ('EnB.IsoWriter' -as [type])) {
    Add-Type -CompilerParameters $cp -TypeDefinition @'
namespace EnB { public static class IsoWriter {
  public static void Write(string path, object stream, int blockSize, int blocks) {
    var s = (System.Runtime.InteropServices.ComTypes.IStream)stream;
    using (var f = System.IO.File.Create(path)) {
      byte[] buf = new byte[blockSize]; System.IntPtr none = System.IntPtr.Zero;
      for (int i = 0; i < blocks; i++) { s.Read(buf, blockSize, none); f.Write(buf, 0, blockSize); }
    } } } }
'@
}
$stream = $img.ImageStream
[EnB.IsoWriter]::Write($seedIso, $stream, $img.BlockSize, $img.TotalBlocks)
# IMAPI keeps the seed's files open until its COM objects are released
foreach ($o in $stream, $img, $fsi) { [void][Runtime.InteropServices.Marshal]::ReleaseComObject($o) }
[GC]::Collect(); [GC]::WaitForPendingFinalizers()
Remove-Item -Recurse -Force $seed

# ---------------------------------------------------------------- 4. the VM
Step "VM $Name"
$vm = New-VM -Name $Name -Generation 2 -Path $PoolDir -VHDPath $disk -MemoryStartupBytes ($StartupGB * 1GB) -SwitchName $Switch
Set-VM -VM $vm -ProcessorCount $Cpu -DynamicMemory -MemoryMinimumBytes 512MB -MemoryMaximumBytes 1TB `
    -AutomaticCheckpointsEnabled $false -AutomaticStartAction Nothing -AutomaticStopAction ShutDown -CheckpointType Disabled
Set-VMFirmware -VM $vm -EnableSecureBoot On -SecureBootTemplate MicrosoftWindows
Set-VMFirmware -VM $vm -FirstBootDevice (Get-VMHardDiskDrive -VM $vm)
Enable-VMIntegrationService -VM $vm -Name 'Guest Service Interface'
foreach ($iso in $seedIso, (Join-Path $MediaDir 'vc6pro-disc1.iso'), (Join-Path $MediaDir 'vs6sp4.iso')) { Add-VMDvdDrive -VM $vm -Path $iso }
$cred = New-Object PSCredential ".\$User", (ConvertTo-SecureString $password -AsPlainText -Force)
$cred | Export-Clixml (Join-Path $Repo ".work\$Name.cred.xml")   # DPAPI: readable by this Windows account only
Start-VM -VM $vm

# ---------------------------------------------------------------- 5. wait and check
Step "first boot and setup (up to $TimeoutMinutes minutes; progress: C:\EnBSetup\setup.log in the VM)"
$deadline = (Get-Date).AddMinutes($TimeoutMinutes)
$state = $null
while (-not $state) {
    if ((Get-Date) -gt $deadline) { throw "setup did not finish in $TimeoutMinutes minutes; look at the VM's console (vmconnect localhost $Name)" }
    Start-Sleep 30
    try {
        $state = Invoke-Command -VMName $Name -Credential $cred -ErrorAction Stop -ScriptBlock {
            if (Test-Path C:\EnBSetup\failed.txt) { 'failed: ' + (Get-Content -Raw C:\EnBSetup\failed.txt) }
            elseif (Test-Path C:\EnBSetup\done.txt) { 'done' }
            elseif (Test-Path C:\EnBSetup\setup.log) { $null; Write-Host ('  ' + ((Get-Content C:\EnBSetup\setup.log -Tail 1) -join '')) }
        }
    } catch { Write-Host "  waiting for Windows setup ($([int]((Get-Date) - $deadline.AddMinutes(-$TimeoutMinutes)).TotalMinutes) min)" }
}
if ($state -ne 'done') { throw "guest setup $state" }
# the seed holds the account's password: eject and delete it now
Get-VMDvdDrive -VM $vm | Where-Object Path -eq $seedIso | Remove-VMDvdDrive
Remove-Item -Force $seedIso
$ip = (Get-VMNetworkAdapter -VMName $Name).IPAddresses | Where-Object { $_ -match '^[0-9.]+$' } | Select-Object -First 1
Write-Host "VM address $ip"
# a new VM: forget whatever host had this DHCP address before, then accept this one's key
# (Windows PowerShell turns a native program's stderr into an error under 'Stop': relax it here)
$ErrorActionPreference = 'Continue'
ssh-keygen -R $ip 2>&1 | Out-Null
ssh -i $Key -o BatchMode=yes -o StrictHostKeyChecking=accept-new "$User@$ip" hostname 2>&1 | Out-Null   # one word: 5.1 mangles quoted arguments
$sshExit = $LASTEXITCODE
$ErrorActionPreference = 'Stop'
if ($sshExit) { throw "ssh $User@$ip failed (key login; the VM's C:\EnBSetup\setup.log says why)" }

$cfg = @{}
if (Test-Path $ConfigFile) { (Get-Content -Raw $ConfigFile | ConvertFrom-Json).PSObject.Properties | ForEach-Object { $cfg[$_.Name] = $_.Value } }
$cfg.user = $User; $cfg.key = $Key; $cfg.pool_dir = $PoolDir; $cfg.template_vm = $Name; $cfg.prefix = $Prefix; $cfg.switch = $Switch
New-Item -ItemType Directory -Force (Split-Path $ConfigFile) | Out-Null
[System.IO.File]::WriteAllText($ConfigFile, ($cfg | ConvertTo-Json))
$env:ENB_VC6_CONFIG = $ConfigFile
$env:ENB_VC6_HOSTS = "$User@$ip"
python -I (Join-Path $PSScriptRoot 'check_vm.py')
if ($LASTEXITCODE) { throw 'the VM does not compile as VC6 SP4 (check_vm.py)' }

# ---------------------------------------------------------------- 6. the pool's base
if (-not $NoBase) {
    Step "pool base $base"
    Stop-VM -VM $vm
    Get-VMDvdDrive -VM $vm | Remove-VMDvdDrive
    if (Test-Path $base) { Set-ItemProperty $base -Name IsReadOnly -Value $false; Remove-Item $base }
    Copy-Item $disk $base
    Set-ItemProperty $base -Name IsReadOnly -Value $true
    Write-Host "base ready; clones: python tools/vc6/pool.py lease --agent NAME"
}
Step "done: $Name ($User@$ip), settings in $ConfigFile"
