#!/usr/bin/env bash

set -e

cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_VERBOSE_MAKEFILE=ON \
    -DPOSTGRES_CXX_BUILD_TESTS=ON \
    -B./build/ \
    -H.
cmake --build ./build/ -- -j2

if [[ $1 = "--run" ]]
then
    wait-for-it.sh postgres:5432

    cd ./build/
    ctest -V
    make install

    cd ../tests/integration
    cmake -B./build/ -H.
    cmake --build ./build/

    cd ./build/
    ctest -V
fi
