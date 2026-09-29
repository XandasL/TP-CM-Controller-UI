$ErrorActionPreference = "Stop"
$Repo = "https://github.com/XandasL/TP-CM-Controller-UI.git"
$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$Temp = Join-Path ([System.IO.Path]::GetTempPath()) "TP-CM-Controller-UI-publish"

if (Test-Path $Temp) { Remove-Item -Recurse -Force $Temp }
git clone $Repo $Temp

Get-ChildItem -Force $ProjectRoot | Where-Object { $_.Name -notin @("PUSH-TO-GITHUB.ps1") } | ForEach-Object {
    Copy-Item $_.FullName -Destination $Temp -Recurse -Force
}
Copy-Item (Join-Path $ProjectRoot "PUSH-TO-GITHUB.ps1") -Destination $Temp -Force

Push-Location $Temp
git add -A
git diff --cached --quiet
if ($LASTEXITCODE -ne 0) {
    git commit -m "Prepare official multi-platform Dusklight build"
    git push origin main
} else {
    Write-Host "No changes to push."
}
Pop-Location

Write-Host "Repository updated: $Repo"
Write-Host "Check the Actions tab and wait for the Build workflow to finish."
