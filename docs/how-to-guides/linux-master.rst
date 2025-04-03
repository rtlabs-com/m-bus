How to create a Modbus master on Linux
======================================

Prerequisites
-------------

* Linux PC or Windows PC with :term:`WSL`.
* M-Bus stack in a folder :file:`m-bus`. This should be the full commercial
  version, not the evaluation version.
* `CMake <https://cmake.org/>`_ version 3.28 or later.
* Any C and C++ compiler.

Instructions
------------

#. Go to the :file:`m-bus` directory::

      cd m-bus

#. Create the stack build system::

      cmake -B build.linux

#. Build the M-Bus stack::

      cmake --build build.linux

#. Go to the parent directory::

      cd ..

#. Copy the files from the :file:`sample` directory::

      cp m-bus/sample/ports/linux/* .

#. Create a file :file:`CMakeLists.txt` for building the application::

      echo "
      cmake_minimum_required (VERSION 3.28)

      project (APP)

      add_subdirectory(m-bus)

      add_executable(app)

      target_sources(
        app
        PRIVATE
        mb_bsp.c
        tcp_rtu_master.c
        )

      target_link_libraries(app PRIVATE mbus)
      " > CMakeLists.txt

#. Create the build system for the application::

      cmake -B build.linux

#. Build the application::

      cmake --build build.linux

   This will create a binary :file:`build.linux/app`.

#. Test that application was build correctly by running the generated binary::

      build.linux/app --help
