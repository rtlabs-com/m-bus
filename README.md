Modbus stack
=============
[![Build Status](https://github.com/rtlabs-com/m-bus/workflows/build/badge.svg?branch=master)](https://github.com/rtlabs-com/m-bus/actions?workflow=build)

This repository contains a Modbus stack. The stack is written to an OS
abstraction layer and can also be used in a bare metal
application. Using the abstraction layer, the stack can run on Linux,
Windows or on an RTOS.

Prerequisites for all platforms
===============================

 * CMake 3.14 or later

Out-of-tree builds are recommended. Create a build directory and run
the following commands from that directory. The root folder for the
repo is assumed to be an absolute or relative path in an environment
variable named *repo*.

The cmake executable is assumed to be in your path. After running
cmake you can run ccmake or cmake-gui to change settings.

Windows
=======

 * Visual Studio 2013 or later

Start a developer command prompt, then:

```
C:\build> cmake %repo%
C:\build> msbuild ALL_BUILD.vcxproj
C:\build> msbuild RUN_TESTS.vcxproj
```

This builds the stack and runs the unit tests.

Linux
=====

 * GCC 4.6 or later

```console
user@host:~/build$ cmake $repo
user@host:~/build$ make all check
```

This builds the stack and runs the unit tests.

The clang static analyzer can also be used if installed. From a clean
build directory, run:

```console
user@host:~/build$ scan-build cmake $repo -DCMAKE_BUILD_TYPE=Debug
user@host:~/build$ scan-build make
```

rt-kernel
=========

 * Workbench 2017.1 or later

Set the following environment variables. You should use a bash shell,
such as for instance the Command Line in your Toolbox
installation. Set BSP and ARCH values as appropriate for your
hardware.


```console
user@host:~/build$ export COMPILERS=/opt/rt-tools/compilers
user@host:~/build$ export RTK=/path/to/rt-kernel
user@host:~/build$ export BSP=<bsp>
user@host:~/build$ export ARCH=<arch>
```

Standalone project
------------------

This creates standalone makefiles.

```console
user@host:~/build$ cmake $repo \
    -DCMAKE_TOOLCHAIN_FILE=$repo/cmake/toolchain/rt-kernel-$ARCH.cmake \
    -G "Unix Makefiles"
user@host:~/build$ make all
```

Workbench project
-----------------

This creates a Makefile project that can be imported to Workbench. The
project will be created in the build directory.

```console
user@host:~/build$ cmake $repo \
    -DCMAKE_TOOLCHAIN_FILE=$repo/cmake/toolchain/rt-kernel-$ARCH.cmake \
    -DCMAKE_ECLIPSE_EXECUTABLE=/opt/rt-tools/workbench/Workbench \
    -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE \
    -G "Eclipse CDT4 - Unix Makefiles"
```

A source project will also be created in the $repo folder. This
project can also be imported to Workbench. After importing,
right-click on the project and choose *New* -> *Convert to a C/C++
project*. This will setup the project so that the indexer works
correctly and the Workbench revision control tools can be used.

The library and the unit tests will be built. Note that the tests
require a stack of at least 6 kB. You may have to increase
CFG_MAIN_STACK_SIZE in your bsp include/config.h file.

Contributions
=============

Contributions are welcome. If you want to contribute you will need to
sign a Contributor License Agreement and send it to us either by
e-mail or by physical mail. More information is available
[here](https://rt-labs.com/contribution).
