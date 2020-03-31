#  SQL-CPP-Bridge
--------------
The purpose of this framework is to provide the most simplest way of mapping
an arbitrary structure of C ++ / STL derived classes on a flat SQL-table.
The framework also provides methods for reading and writing (synchronous /
asynchronous), delete and edit data both as whole containers and as
individual elements of them. Currently SQLite implementation for MacOSX and
iOS is used for data storing but you can work adapters further to be
compatible with any SQL-like library.
In addition, all the work on forming SQL statements is made inside the
library and does not require intervention and any improvements on the
outside.
Warning! This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

How to use this library
-------------
For using the library you need to add the framework "sqlcppbridge" into your
project and to include the file sqlcppbridge.h in your sources where it is required.
In 'Cases' folder there are several samples of using the library.

Key features:
-------------
* versioning of the database structure. That means that creation of all the
necessary tables/indexes is supported automatically; if necessary custom
SQL-scripts for updating tables/indexes are called or a simple deleting
followed by recreating the tables is made (this is the only place where you
may need SQL knowledge);
* storing of base types (here and below) arithmetic/enum/std::string by
string keys (the so-called KV-storage);
* storing iterable containers with the values ​​of all base types in the
KV-storage;
* storing of classes' instances with members of base types in a single
repository with creation of required indexes and control of objects'
uniqueness; with ability to load these objects on arbitrary criteria into in
an arbitrary provided iterable STL-container;
* storing of classes' instances with arbitrary hierarchy of inheritance
(including multiple and virtual) and with arbitrary size of the hierarchy
itself;
* storing of classes' instances with members containing other compound
classes or containers of base or compound types; in principle, there are no
depth limitations of such nesting at all.

Restrictions:
-------------
* At the current moment the public inheritance of iterable containers is not
supported;
* Containers stack, queue (obviously non-iterable) and forward_list
are not supported either.

Build:
-------------
You can use the script:

    ./make_framework

to create ./build/iOS/sqlcppbridge.framework and ./build/MacOS/sqlcppbridge.framework
which includes armv7, armv7s, armv7k, arm64, arm64e, i386  and x86_64 architectures
to use in the whole set of iOS-devices and iOS-simulators.

Alternatively you can simply run 'make' utility to create the static library for the default environment.

You can also build it using CMake (for iOS):

    mkdir build.cmake && cd build.cmake
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake -DPLATFORM=OS

For more details on available PLATFORM options see https://github.com/leetal/ios-cmake#options

Build it as a `static` library for the current platform:

    mkdir build.cmake && cd build.cmake
    cmake ..

Or a `shared` library for the current platform:

    mkdir build.cmake && cd build.cmake
    cmake -DBUILD_SHARED=ON ..

By default sqlite3 library is detected and built into the shared library OR specified as a requirement for the static library flavour (see below).
If you want to avoid including sqlite3 (e.g. you already have sqlite3 in your project) you can do so using the `-DREQUIRE_SQLITE=OFF` option:

    mkdir build.cmake && cd build.cmake
    cmake -DREQUIRE_SQLITE=OFF ..

**Note** that this is only possible for a static library; The shared library will force linkage against sqlite3 and contain it.

CMake can also be used for easy integration with SQLCppBridge (assuming SQLCppBridge is used as a git submodule or dropped to `lib/SQLCppBridge`):

    add_subdirectory ( "${CMAKE_SOURCE_DIR}/lib/SQLCppBridge" EXCLUDE_FROM_ALL )
    ...
    target_link_libraries (
      myTarget
        PRIVATE  lib::SQLCppBridge
        ... )

The above example will automatically link myTarget against SQLCppBridge as well as setup required include directories.

Install:
-------------
For Linux environment use "sudo make install" to put assembled with 'make' utility
static library & headers to system folders
