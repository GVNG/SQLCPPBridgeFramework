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
* At the current moment inheritance of iterable containers of basic types
is not supported;
* Containers stack, queue (obviously non-iterable) and forward_list 
are not supported either.

Build:
-------------
You can use the script:
./make_framework
to create ./build/sqlcppbridge.framework
which includes armv7, armv7s, arm64 and x86_64 architectures to use in the 
whole set of iOS-devices and iOS-simulators

...or you can simple run 'make' utility to create the static library for the default environment

Install:
-------------
For Linux environment use "sudo make install" to put assembled with 'make' utility 
static library & headers to system foders 
