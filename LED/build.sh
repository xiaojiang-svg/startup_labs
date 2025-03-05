#!/bin/bash

echo "cleaning previous build folder ..."
rm -rf ./build

echo "create new build folder ..."
mkdir build && cd build

cmake -DCMAKE_TOOLCHAIN_FILE=./Cmake/toolchain.cmake ..
make