<#
.SYNOPSIS
  First-boot setup of a build VM: SSH, the compiler, and the pool base's settings.

.DESCRIPTION
  New-BuildVM.ps1 copies this folder to C:\EnBSetup in the VM's disk and runs it from
  SetupComplete.cmd (as SYSTEM, once, at the end of Windows setup). It can also be run by hand, as
  administrator, in a VM installed some other way (docs/setup-vc6.md, "Manual route").

  1. OpenSSH Server (a Windows capability: needs the internet), started automatically, port 22
     open; key login for the administrators group from C:\EnBSetup\enb_vc6.pub.
  2. Install-VC6.ps1 (both discs attached).
  3. The pool base's settings (docs/toolchain.md 1.3): Windows Update, Update Orchestrator, SysMain
     and Windows Search off; Defender's real-time scan off where tamper protection allows, and
     exclusions for the build folders either way; a fixed 4 GB page file; no hibernation.
  4. C:\EnBDevelopment (vc6.py mirrors the host's folder there).

  Writes C:\EnBSetup\setup.log, and C:\EnBSetup\done.txt on success or failed.txt with the error.
#>
param([string]$Dir = $PSScriptRoot)
$ErrorActionPreference = 'Stop'
Start-Transcript -Path (Join-Path $Dir 'setup.log') -Append | Out-Null
try {
    # -- 1. SSH
    $deadline = (Get-Date).AddMinutes(15)
    while (-not (Test-NetConnection -ComputerName download.microsoft.com -Port 443 -InformationLevel Quiet)) {
        if ((Get-Date) -gt $deadline) { throw 'no internet connection (needed for the OpenSSH Server capability)' }
        Start-Sleep 10
    }
    $cap = Get-WindowsCapability -Online -Name 'OpenSSH.Server*' | Select-Object -First 1
    for ($i = 1; $cap.State -ne 'Installed'; $i++) {
        Write-Host "installing $($cap.Name) (attempt $i)"
        try { Add-WindowsCapability -Online -Name $cap.Name | Out-Null } catch { if ($i -ge 3) { throw } ; Start-Sleep 30 }
        $cap = Get-WindowsCapability -Online -Name $cap.Name
    }
    Set-Service sshd -StartupType Automatic
    Start-Service sshd
    if (-not (Get-NetFirewallRule -Name 'sshd-enb' -ErrorAction SilentlyContinue)) {
        New-NetFirewallRule -Name 'sshd-enb' -DisplayName 'OpenSSH Server (EnB build VM)' -Direction Inbound `
            -Protocol TCP -LocalPort 22 -Action Allow -Profile Any | Out-Null
    }
    $keys = 'C:\ProgramData\ssh\administrators_authorized_keys'
    Copy-Item (Join-Path $Dir 'enb_vc6.pub') $keys -Force
    icacls $keys /inheritance:r /grant 'SYSTEM:F' /grant '*S-1-5-32-544:F' | Out-Null   # Administrators
    Restart-Service sshd

    # -- 2. the compiler
    & (Join-Path $Dir 'Install-VC6.ps1') -Manifest (Join-Path $Dir 'vc6-files.tsv')

    # -- 3. the pool base's settings
    foreach ($s in 'wuauserv', 'UsoSvc', 'SysMain', 'WSearch') {
        Stop-Service $s -Force -ErrorAction SilentlyContinue
        Set-Service $s -StartupType Disabled
    }
    # WaaSMedicSvc (update repair) refuses Set-Service; its registry start value is the documented switch
    Set-ItemProperty 'HKLM:\SYSTEM\CurrentControlSet\Services\WaaSMedicSvc' -Name Start -Value 4 -ErrorAction SilentlyContinue
    New-Item 'HKLM:\SOFTWARE\Policies\Microsoft\Windows\WindowsUpdate\AU' -Force | Out-Null
    Set-ItemProperty 'HKLM:\SOFTWARE\Policies\Microsoft\Windows\WindowsUpdate\AU' -Name NoAutoUpdate -Value 1
    foreach ($p in 'C:\EnBDevelopment', 'C:\Program Files (x86)\Microsoft Visual Studio') {
        Add-MpPreference -ExclusionPath $p -ErrorAction SilentlyContinue
    }
    Set-MpPreference -DisableRealtimeMonitoring $true -ErrorAction SilentlyContinue
    $cs = Get-CimInstance Win32_ComputerSystem
    if ($cs.AutomaticManagedPagefile) { Set-CimInstance -InputObject $cs -Property @{ AutomaticManagedPagefile = $false } }
    $pf = Get-CimInstance Win32_PageFileSetting | Where-Object Name -like 'C:*'
    if ($pf) { Set-CimInstance -InputObject $pf -Property @{ InitialSize = 4096; MaximumSize = 4096 } }
    else { New-CimInstance -ClassName Win32_PageFileSetting -Property @{ Name = 'C:\pagefile.sys'; InitialSize = 4096; MaximumSize = 4096 } | Out-Null }
    powercfg /hibernate off

    # -- 4. the mirror root
    New-Item -ItemType Directory -Force 'C:\EnBDevelopment' | Out-Null

    Set-Content (Join-Path $Dir 'done.txt') (Get-Date -Format o)
    Write-Host 'build VM setup done'
} catch {
    Set-Content (Join-Path $Dir 'failed.txt') ($_ | Out-String)
    Write-Host "build VM setup FAILED: $_"
} finally {
    Stop-Transcript | Out-Null
}
