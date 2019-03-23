# Modern C++ client for PostgreSQL

This library is a wrapper around libpq designed 
to make your work with Postgres easy and safe in a modern C++ way. 
Features:
* C++17.
* Minimal dependencies.
* Connection pool.
* Asynchronous and row-by-row modes.
* Statements generation.
* Prepared statements.
* Transactions.
* Passing arguments in binary format.
* Working with timestamps and NULLs.

## Table of Contents

## Getting started

Prerequisites:
* CMake 3.8 or newer.
* A C++17-compliant compiler.
* libpq-dev.
* Google Test (only to run the tests).

The project is built and tested using GCC 7.3 and Clang 6.0 on a machine running Linux.
As there is no platform specific code, it should also work on the other platforms as well,
but the author cannot guarantee this.

You can integrate the library into your project in two ways:
* As a CMake subproject.
* As a prebuilt library.

If you're experienced with CMake you probably already know what to do.
Anyway, lets go through the process step by step.
First we'll create a new CMake-based project which will serve us as an example.
Initially the project will have the following structure:
```
.
├── CMakeLists.txt
└── main.cpp
```
Lets fill out the CMakeLists.txt with:
```cmake
cmake_minimum_required(VERSION 3.8)
project(MyProject)

add_executable(MyProject main.cpp)
```
and write some code using the library in the main.cpp:
```cpp
#include <postgres/Connection.h>

int main() {
    return postgres::Connection::ping();
}
```
Now we have two ways to make the project compile as mentioned a bit earlier.
Lets consider each of them in turn.

### CMake subproject

We have to bring the PostgreSQL client library files into our project somehow.
How to achieve this is up to you.
We could just download the repository or use a package manager,
but we'll utilize git submodules for this purpose:
```bash
$ git init
$ git submodule add \
    https://github.com/anatoliifrolov/postgres-cxx-client.git \
    ./deps/postgres-cxx-client/
```
Now we're able to add the library to our project's dependencies in the CMakeLists.txt:
```cmake
cmake_minimum_required(VERSION 3.8)
project(MyProject)

add_executable(MyProject main.cpp)

add_subdirectory(deps/postgres-cxx-client)
target_link_libraries(MyProject PostgresCxxClient::PostgresCxxClient)
```  
There are two new lines at the bottom doing the job.
And that's it, our project is ready to run.

### Prebuilt library

Lets say we keep libraries in `~/lib/` directory.
Then the steps are as follows:
```bash
$ cd ~/lib/
$ git clone https://github.com/anatoliifrolov/postgres-cxx-client.git
$ cd postgres-cxx-client/
$ cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=~/lib/ \
    -B./build/release/ \
    -H.
$ cmake --build ./build/release/ --target install
```
Next we'll modify the CMakeLists.txt to look like:
```cmake
cmake_minimum_required(VERSION 3.8)
project(MyProject)

add_executable(MyProject main.cpp)

find_package(PostgresCxxClient)
target_link_libraries(MyProject PostgresCxxClient::PostgresCxxClient)
```
What's left is just to tell CMake where to lookup for the libraries
using the option `-DCMAKE_PREFIX_PATH=~/lib/`.
After this step our project should've become able to compile and run.

### Running the tests

## License

This project is licensed under the MIT License - see the LICENSE.md file for details.
