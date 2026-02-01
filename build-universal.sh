#!/bin/bash

# Build script for universal macOS bundle (arm64 + x86_64)

set -e  # Exit on error

echo "========================================="
echo "Building Universal macOS Bundle for adhd"
echo "========================================="

# Configuration
APP_NAME="adhd"
BUILD_DIR="build-universal"
BUILD_TYPE="Release"
ARCHITECTURES="arm64;x86_64"
QT_PATH="$HOME/Qt/6.8.6/macos"
APPLE_DEVELOPMENT_IDENTITY="Developer ID Application: Arjun Patel (RK9WWM4Q99)"

# Clean previous build (optional - comment out to keep incremental builds)
# rm -rf "$BUILD_DIR"

# Configure CMake
echo ""
echo "Configuring CMake for universal binary..."
echo "Minimum macOS version: 11.0 (Big Sur)"
cmake -B "$BUILD_DIR" \
    -DCMAKE_OSX_ARCHITECTURES="$ARCHITECTURES" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="11.0" \
    -DCMAKE_PREFIX_PATH="$QT_PATH" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

# Build
echo ""
echo "Building..."
cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" -j$(sysctl -n hw.ncpu)

# Verify the binary is universal
echo ""
echo "Verifying binary architectures..."
if [ -f "$BUILD_DIR/$APP_NAME.app/Contents/MacOS/$APP_NAME" ]; then
    lipo -info "$BUILD_DIR/$APP_NAME.app/Contents/MacOS/$APP_NAME"
    echo "✓ Binary is universal"
else
    echo "✗ Error: Binary not found at expected location"
    exit 1
fi

# Deploy Qt dependencies
echo ""
echo "Deploying Qt dependencies..."
$QT_PATH/bin/macdeployqt "$BUILD_DIR/$APP_NAME.app" -always-overwrite -verbose=1
echo "✓ Qt dependencies deployed"

# Code signing
echo ""
echo "========================================="
echo "Listing identities for code-signing"
echo "========================================="
security find-identity -v -p codesigning

echo ""
echo "========================================="
echo "Code signing .app bundle"
echo "========================================="
echo "Using: $APPLE_DEVELOPMENT_IDENTITY"

cd "$BUILD_DIR"

codesign --deep --force --timestamp --verify --verbose=1 \
  --sign "$APPLE_DEVELOPMENT_IDENTITY" \
  --options runtime "$APP_NAME.app"

echo ""
echo "Verifying code signature..."
codesign --verify --deep --strict --verbose=1 "./$APP_NAME.app"
echo "✓ Code signing complete"

# Create DMG
echo ""
echo "========================================="
echo "Creating DMG"
echo "========================================="
echo "NOTE: Install create-dmg if needed: https://github.com/sindresorhus/create-dmg"

if ! command -v create-dmg &> /dev/null; then
    echo "⚠ Warning: create-dmg not found. Install with: brew install create-dmg"
    echo "Skipping DMG creation, notarization, and stapling."
    cd ..
    echo ""
    echo "========================================="
    echo "Build completed (without DMG/notarization)"
    echo "App bundle location: $BUILD_DIR/$APP_NAME.app"
    echo "========================================="
    exit 0
fi

create-dmg "$APP_NAME.app" --identity="$APPLE_DEVELOPMENT_IDENTITY" --dmg-title="$APP_NAME"
# WORKAROUND: https://github.com/sindresorhus/create-dmg/issues/52
mv *.dmg "$APP_NAME.dmg" 2>/dev/null || true

echo "✓ DMG created: $APP_NAME.dmg"

# Notarization
echo ""
echo "========================================="
echo "Starting Notarization"
echo "========================================="
echo "NOTE: Make sure you've run 'xcrun notarytool store-credentials' first"

xcrun notarytool submit "$APP_NAME.dmg" \
  --keychain-profile "default" \
  --wait \
  --timeout "10m"

echo ""
echo "========================================="
echo "Stapling notarization ticket"
echo "========================================="
xcrun stapler staple -v "$APP_NAME.dmg"
echo "✓ Notarization complete"

# Optional: Create zip
echo ""
read -p "Create zip file? (y/N): " CREATE_ZIP
if [[ "$CREATE_ZIP" == "y" || "$CREATE_ZIP" == "Y" ]]; then
    ZIP_NAME="$APP_NAME-universal.zip"
    zip -9 "$ZIP_NAME" "$APP_NAME.dmg"
    echo "✓ Created: $ZIP_NAME"
fi

cd ..

echo ""
echo "========================================="
echo "Build completed successfully!"
echo "========================================="
echo "App bundle: $BUILD_DIR/$APP_NAME.app"
echo "DMG: $BUILD_DIR/$APP_NAME.dmg"
echo ""
echo "To test: open $BUILD_DIR/$APP_NAME.dmg"
echo "========================================="
