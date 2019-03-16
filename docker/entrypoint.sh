#!/usr/bin/env bash

set -e

for COMPILER in clang++ c++
do
    BUILDDIR=./build/${COMPILER}/
    cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_COMPILER=${COMPILER} \
        -DPOSTGRES_CXX_BUILD_TESTS=ON \
        -B${BUILDDIR} \
        -H.
    cmake --build ${BUILDDIR} -- -j8
done

if [[ $1 = "--run" ]]
then
    wait-for-it.sh postgres:5432

    for TEST in $(find ./build -name PostgresCxxClientTest)
    do
        ${TEST}
    done
fi
