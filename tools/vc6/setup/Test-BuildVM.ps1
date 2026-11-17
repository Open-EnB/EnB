<#
.SYNOPSIS
  Proves a build VM compiles as the client's compiler: cl 12.00.8804, link 6.00.8447, and an object
  stamped @comp.id product 11, build 8799 (VC6 SP4). Exit status 1 on any difference.

.EXAMPLE
  powershell -ExecutionPolicy Bypass -File tools\vc6\setup\Test-BuildVM.ps1 -Target user@172.30.0.5
#>
param(
    [Parameter(Mandatory = $true)][string]$Target,
    [string]$Key = (Join-Path $HOME '.ssh\enb_vc6')
)
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'EnBSetup.ps1')
if (Test-BuildVM $Target $Key) { exit 0 } else { exit 1 }
