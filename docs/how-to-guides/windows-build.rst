How to build the stack on Windows
=================================

Prerequisites
-------------

* PC with Windows 10 or later.
* `Visual Studio <https://visualstudio.microsoft.com/>`_ 2022 or later.
* `CMake <https://cmake.org/>`_ version 3.28 or later.
* M-Bus stack in a folder :file:`m-bus`. This should be the full commercial
  version, not the evaluation version.

Instructions
------------

#. Open a :program:`Developer Command Prompt for VS`.

#. Go to the :file:`m-bus` directory::

      cd m-bus

#. Create the build system for the M-Bus stack::

      cmake -B build.windows

#. Build the M-Bus stack::

      cmake --build build.windows

   This will create a solution file :file:`build.windows\MBUS.sln`
   which may be opened in Visual Studio.
