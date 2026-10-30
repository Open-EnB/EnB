<#
.SYNOPSIS
  Installs Visual C++ 6.0 SP4 (the client's compiler, build 8799) by copying files from the two discs.

.DESCRIPTION
  Runs inside the build VM, as administrator, with the VC6 Professional disc 1 (volume label
  VSP600ENU1) and the SP4 disc (VS6SP4) attached as drives. Copies every file of vc6-files.tsv from
  its disc to C:\Program Files (x86)\Microsoft Visual Studio, checks each copy's SHA-1, and writes
  VC98\Bin\VCVARS32.BAT. No setup program runs: the list was made from a setup-installed VM
  (Custom, Visual C++ only, then SP4) and reproduces its compiler, linker, headers and libraries
  byte for byte (docs/setup-vc6.md, "How the install was derived").

.PARAMETER Manifest
  vc6-files.tsv (default: beside this script).
#>
param(
    [string]$Manifest = (Join-Path $PSScriptRoot 'vc6-files.tsv'),
    [string]$Target = 'C:\Program Files (x86)\Microsoft Visual Studio'
)
$ErrorActionPreference = 'Stop'

function Find-Disc([string]$label) {
    $v = Get-Volume | Where-Object { $_.FileSystemLabel -eq $label -and $_.DriveLetter } | Select-Object -First 1
    if (-not $v) { throw "disc '$label' not attached (attach the ISO made by tools/vc6/setup/media.py)" }
    return "$($v.DriveLetter):\"
}

$discs = @{ vc6 = (Find-Disc 'VSP600ENU1'); sp4 = (Find-Disc 'VS6SP4') }
Write-Host "VC6 disc $($discs.vc6), SP4 disc $($discs.sp4)"

$sha1 = [System.Security.Cryptography.SHA1]::Create()
$n = 0
foreach ($line in Get-Content $Manifest) {
    if ($line -match '^\s*(#|$)') { continue }
    $f = $line.Split("`t")
    $dest = Join-Path $Target $f[0]
    $src = Join-Path $discs[$f[1]] $f[2]
    New-Item -ItemType Directory -Force (Split-Path $dest) | Out-Null
    Copy-Item -LiteralPath $src -Destination $dest -Force
    Set-ItemProperty -LiteralPath $dest -Name IsReadOnly -Value $false   # CD files arrive read-only
    $bytes = [System.IO.File]::ReadAllBytes($dest)
    $h = -join ($sha1.ComputeHash($bytes) | ForEach-Object { $_.ToString('x2') })
    if ($h -ne $f[3]) { throw "$($f[0]): SHA-1 $h, expected $($f[3]) (from $src)" }
    $n++
}
Write-Host "copied and checked $n files"

# VCVARS32.BAT as setup writes it (short names where the volume has them)
function Short([string]$p) {
    $fso = New-Object -ComObject Scripting.FileSystemObject
    try { return $fso.GetFolder($p).ShortPath } catch { return $p }
}
$common = Short (Join-Path $Target 'Common')
$vc = Short (Join-Path $Target 'VC98')
$bat = @"
@echo off
rem
rem Root of Visual Developer Studio Common files.
set VSCommonDir=$common

rem
rem Root of Visual Developer Studio installed files.
rem
set MSDevDir=$common\msdev98

rem
rem Root of Visual C++ installed files.
rem
set MSVCDir=$vc

rem
rem VcOsDir is used to help create either a Windows 95 or Windows NT specific path.
rem
set VcOsDir=WIN95
if "%OS%" == "Windows_NT" set VcOsDir=WINNT

rem
echo Setting environment for using Microsoft Visual C++ tools.
rem

if "%OS%" == "Windows_NT" set PATH=%MSDevDir%\BIN;%MSVCDir%\BIN;%VSCommonDir%\TOOLS\%VcOsDir%;%VSCommonDir%\TOOLS;%PATH%
if "%OS%" == "" set PATH="%MSDevDir%\BIN";"%MSVCDir%\BIN";"%VSCommonDir%\TOOLS\%VcOsDir%";"%VSCommonDir%\TOOLS";"%windir%\SYSTEM";"%PATH%"
set INCLUDE=%MSVCDir%\ATL\INCLUDE;%MSVCDir%\INCLUDE;%MSVCDir%\MFC\INCLUDE;%INCLUDE%
set LIB=%MSVCDir%\LIB;%MSVCDir%\MFC\LIB;%LIB%

set VcOsDir=
set VSCommonDir=

"@
[System.IO.File]::WriteAllText((Join-Path $Target 'VC98\Bin\VCVARS32.BAT'), $bat.Replace("`r`n", "`n").Replace("`n", "`r`n"))

$out = cmd /c "call `"$Target\VC98\Bin\VCVARS32.BAT`" >nul && cl 2>&1"
$banner = ($out | Select-Object -First 1)
Write-Host $banner
if ($banner -notmatch 'Version 12\.00\.8804') { throw "cl does not answer as VC6 SP4 (12.00.8804): $banner" }
