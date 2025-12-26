$ErrorActionPreference = "Stop"

$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Path
$FRONTEND_DIR = Join-Path $SCRIPT_DIR "frontend"
$BUILD_DIR = Join-Path $SCRIPT_DIR "build_server"
$STATIC_TARGET_DIR = Join-Path $BUILD_DIR "static"

Write-Host "📂 1. Create build path" -ForegroundColor Cyan
if (-not (Test-Path $BUILD_DIR)) {
    Write-Host "create path: $BUILD_DIR" -ForegroundColor Green
    New-Item -ItemType Directory -Path $BUILD_DIR -Force | Out-Null
} else {
    Write-Host "existed: $BUILD_DIR, cleaning..." -ForegroundColor Yellow
    Set-Location $BUILD_DIR
    Remove-Item * -Recurse -Force -ErrorAction SilentlyContinue
}

Write-Host "🛠️ 2. build backend" -ForegroundColor Cyan
Set-Location $BUILD_DIR
& cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release .. 

$cpuCores = (Get-CimInstance -ClassName Win32_ComputerSystem).NumberOfLogicalProcessors
if (-not $cpuCores) {
    $cpuCores = 4  
}

& mingw32-make -j $cpuCores

Write-Host "🧹 3. Clean build files" -ForegroundColor Cyan
Get-ChildItem -Path . | Where-Object {
    $_.Name -ne "server.exe" -and $_.Name -ne "static"
} | Remove-Item -Recurse -Force

Write-Host "📂 4. create static path" -ForegroundColor Cyan
if (-not (Test-Path $STATIC_TARGET_DIR)) {
    New-Item -ItemType Directory -Path $STATIC_TARGET_DIR -Force | Out-Null
} else {
    Set-Location $STATIC_TARGET_DIR
    Remove-Item * -Recurse -Force -ErrorAction SilentlyContinue
}

Write-Host "🛠️ 5. build frontend" -ForegroundColor Cyan
Set-Location $FRONTEND_DIR

& npm install --silent
& npm run build

Copy-Item -Path (Join-Path $FRONTEND_DIR "dist", "*") -Destination $STATIC_TARGET_DIR -Recurse -Force

Write-Host "✅ Build success! target at $BUILD_DIR." -ForegroundColor Green