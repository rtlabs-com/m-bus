How to create a Modbus TCP slave running RT-Kernel
==================================================

In this guide, an M-Bus `RT-Toolbox Workbench <https://docs.rt-labs.com/rt-toolbox>`_
library project is first created.
A slave Workbench application project is then created using the library project.

Prerequisites
-------------

* `Infineon XMC4800 Relax Kit <https://www.infineon.com/cms/en/product/evaluation-boards/kit_xmc48_rlx_ecat_v2.1>`_.
* Ethernet cable
* Windows or Linux PC
* `CMake <https://cmake.org/>`_ 3.28 or later
* `RT-Labs RT-Toolbox Workbench 2024.1 <https://docs.rt-labs.com/rt-toolbox/installation.html>`_ (previously rt-collab Workbench) or later

Instructions
------------

#. Open the :program:`Command Line` program in your RT-Toolbox installation.

#. Go to the directory where the M-Bus stack is located. Do not step into the
   M-Bus directory.

#. Create a Workbench project for the M-Bus stack using the following command::

      RTK=/opt/rt-tools/rt-kernel-xmc4 BSP=xmc48relax cmake \
         -B build.xmc48relax -S m-bus \
         -DCMAKE_TOOLCHAIN_FILE=cmake/tools/toolchain/rt-kernel.cmake \
         -DCMAKE_ECLIPSE_EXECUTABLE=/opt/rt-tools/workbench/Workbench \
         -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE \
         -G "Eclipse CDT4 - Unix Makefiles"

   The project will be created in the build directory `build.xmc48relax`.
   Do not change the placement of the build directory to be inside the
   source directory `m-bus`. Deprecation warnings and warnings for missing
   Doxygen or sphinx-build may be ignored.

#. Start :program:`RT-Toolbox Workbench` and open a workspace folder.

#. In the :guilabel:`File` menu, select :guilabel:`Import...`.

#. In the :guilabel:`Import` dialog, expand the :guilabel:`General` folder, select
   :guilabel:`Existing projects into Workspace` and press :guilabel:`Next`.

#. In the :guilabel:`Import Projects` dialog press :guilabel:`Browse...` and
   select the folder :file:`build.xmc48relax`. Press :guilabel:`Finish`.

#. In the :guilabel:`Project Explorer` view, the project should show up with name
   :guilabel:`MBUS-RelWithDebInfo@build.xmc48relax`. Indexing errors may be ignored.
   It should be possible to build the project by right-clicking the project
   and selecting  :guilabel:`Build`.

#. In the :guilabel:`File` menu, select :guilabel:`New` and then :guilabel:`Project`.

#. In the :guilabel:`New Project` dialog, expand the :guilabel:`C/C++` folder
   and select :guilabel:`C Project`, then click :guilabel:`Next`.

#. In the :guilabel:`C Project` dialog, enter :file:`slave-app` as project name.

#. In the same dialog, go to the :guilabel:`Project Type` view, expand the
   :guilabel:`RT-Kernel Projects` folder, and select :guilabel:`Application`.

#. In the same dialog, go to the :guilabel:`RT-Kernel` view, select
   :guilabel:`xmc4` and then press :guilabel:`Next`.
   If :guilabel:`xmc4` is not available, you will need to install it using the
   RT-Toolbox installer (see https://docs.rt-labs.com/rt-toolbox/installation.html).

#. In the :guilabel:`Select BSP` dialog, press the :guilabel:`Import` button.

#. In the :guilabel:`Import RT-Kernel reference BSP` dialog, select
   :guilabel:`rt-kernel-xmc4` as :guilabel:`Architecture` and :guilabel:`xmc48relax`
   as :guilabel:`BSP`. Enter :file:`xmc48relax-bsp` as BSP name. Press :guilabel:`Finish`.
   In the :guilabel:`Project Explorer` view, the projects :guilabel:`slave-app`
   and :guilabel:`xmc48relax-bsp` should show up.

#. In :guilabel:`Project Explorer`, go to project
   :guilabel:`MBUS-RelWithDebInfo@build.xmc48relax` and folder
   :file:`[Source directory]/sample`. Select and copy the files
   :file:`slave.c`, :file:`slave.h` and :file:`tcp_slave.c` to the
   :guilabel:`slave-app` project's :file:`src` folder.
   Delete the file :file:`src/main.c`

#. In :guilabel:`Project Explorer`, right-click on the the project
   :guilabel:`slave-app` and choose :guilabel:`Properties`.

#. In the :guilabel:`Properties for slave-app` dialog, click on
   :guilabel:`C/C++ General` and the :guilabel:`Paths and Symbols`.

#. In the :guilabel:`Includes` tab, select :guilabel:`CNU C` and then press
   :guilabel:`Add...`.

#. In the opened dialog box press :guilabel:`Workspace..` and then select project
   :guilabel:`MBUS-RelWithDebInfo@build.xmc48relax` with folder :file:`include`.
   Press :guilabel:`OK`.

#. Repeat previous step for the following folders:

   * :file:`[Source directory]/include`
   * :file:`[Subprojects]/OSAL/include`
   * :file:`[Subprojects]/OSAL/src/rt-kernel`

#. In the :guilabel:`Libraries` tab, press :guilabel:`Add...` then type
   :file:`mbus` and press :guilabel:`OK`.

#. Repeat previous step for :file:`osal`

#. In the :guilabel:`Library Paths` tab, press :guilabel:`Add...` and then
   :guilabel:`Workspace...`.
   Select :guilabel:`MBUS-RelWithDebInfo@build.xmc48relax` and press :guilabel:`OK`

#. Repeat previous step for
   :guilabel:`/MBUS-RelWithDebInfo@build.xmc48relax/_deps/osal-build`.

#. Press :guilabel:`Apply and Close`.

#. Make sure the projects were configured correctly by right-clicking on the
   project :guilabel:`slave-app` and selecting  :guilabel:`Build`. It should
   build without build errors. The indexer may show error messages. They may
   be ignored.
