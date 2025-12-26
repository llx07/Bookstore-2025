#!/bin/bash

set -e
SCRIPT_PATH="$(realpath "$0")"
SCRIPT_DIR="$(dirname "$SCRIPT_PATH")"
FRONTEND_DIR="$SCRIPT_DIR/frontend"
BUILD_DIR="$SCRIPT_DIR/build_server"
STATIC_TARGET_DIR="$BUILD_DIR/static"

echo "📂 1 Create build path"
if [ ! -d "$BUILD_DIR" ]; then
    echo "Create build path: $BUILD_DIR"
    mkdir -p "$BUILD_DIR"
else
    echo "Build path already exist: $BUILD_DIR, cleaning..."
    cd "$BUILD_DIR"
    rm -rf *
fi


echo "🛠️ 2. Build the backend"
cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)



echo "🧹 3. Clean backend build file"
find . -maxdepth 1 ! -name "server" ! -name "." -exec rm -rf {} +

echo "📂 4 Create static path"
if [ ! -d "$STATIC_TARGET_DIR" ]; then
    echo "Create build path: $STATIC_TARGET_DIR"
    mkdir -p "$STATIC_TARGET_DIR"
else
    echo "Build path already exist: $STATIC_TARGET_DIR, cleaning..."
    cd "$STATIC_TARGET_DIR"
    rm -rf *
fi
echo "🛠️ 5. Build the frontend"
cd "$FRONTEND_DIR"
npm install --silent
npm run build

cp -r "$FRONTEND_DIR/dist/"* "$STATIC_TARGET_DIR/"


echo "✅ Build Success! Build target in $BUILD_DIR."
