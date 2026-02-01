#!/bin/bash -e

BUILD_DIR=build

if [ ! -d "$BUILD_DIR" ]; then
  cmake -B$BUILD_DIR -S. -GNinja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-Wall -Wextra" \
    -DCMAKE_PREFIX_PATH="$HOME/Qt/6.8.6/macos" \
    -DCMAKE_COLOR_DIAGNOSTICS=ON \
    -DCMAKE_CXX_FLAGS_INIT="-DQT_QML_DEBUG"
fi

cmake --build $BUILD_DIR --target all

$BUILD_DIR/adhd.app/Contents/MacOS/adhd
