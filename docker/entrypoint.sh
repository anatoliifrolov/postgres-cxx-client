#!/usr/bin/env bash

cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DPOSTGRES_CXX_BUILD_TESTS=ON \
    -B./build/ \
    -H.

cmake --build ./build/ -- -j8
