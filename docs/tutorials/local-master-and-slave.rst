Master and slave on local PC
============================

In this tutorial we will use the M-Bus stack to build and run two command line
programs, one acting as a Modbus slave (server) and one acting as a Modbus
master (client).
They will communicate with each other over a local TCP connection.
A Linux machine is used (WSL on Windows works fine).

Prerequisites
-------------

* `M-Bus <https://github.com/rtlabs-com/m-bus>`_ tree located in a directory ``m-bus``.
* `CMake <https://cmake.org>`_ tool at version 3.28 or higher.

Instructions
------------

#. Go to the ``m-bus`` directory::

      cd m-bus

#. Verify that the :command:`CMake` tool is installed::

      cmake --version

   Example output::

      cmake version 3.28.3

      CMake suite maintained and supported by Kitware (kitware.com/cmake).

#. Set up the build system using :command:`CMake`::

      cmake -B build.tutorial

   The output depends on the configuration of your particular machine, but should
   look something like the following (complaints about missing Python, Doxygen
   or Sphinx can be ignored)::

      -- The C compiler identification is GNU 13.3.0
      -- The CXX compiler identification is GNU 13.3.0
      -- Detecting C compiler ABI info
      -- Detecting C compiler ABI info - done
      -- Check for working C compiler: /usr/bin/cc - skipped
      -- Detecting C compile features
      -- Detecting C compile features - done
      -- Detecting CXX compiler ABI info
      -- Detecting CXX compiler ABI info - done
      -- Check for working CXX compiler: /usr/bin/c++ - skipped
      -- Detecting CXX compile features
      -- Detecting CXX compile features - done
      -- Current build type is: RelWithDebInfo
      -- Current install path is: /mnt/c/Users/rtlfrm/Documents/Projects/Products/Modbus/m-bus/build.tutorial/install
      -- Building for Linux
      -- Performing Test CMAKE_HAVE_LIBC_PTHREAD
      -- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
      -- Found Threads: TRUE
      -- Found Doxygen: /usr/bin/doxygen (found version "1.9.8") found components: doxygen dot
      -- Performing Test COMPILER_HAS_HIDDEN_VISIBILITY
      -- Performing Test COMPILER_HAS_HIDDEN_VISIBILITY - Success
      -- Performing Test COMPILER_HAS_HIDDEN_INLINE_VISIBILITY
      -- Performing Test COMPILER_HAS_HIDDEN_INLINE_VISIBILITY - Success
      -- Performing Test COMPILER_HAS_DEPRECATED_ATTR
      -- Performing Test COMPILER_HAS_DEPRECATED_ATTR - Success
      -- Found Python3: /mnt/c/Users/rtlfrm/Documents/Projects/Products/Modbus/m-bus/myvenv/bin/python3 (found version "3.12.3") found components: Interpreter
      -- Found Doxygen: /usr/bin/doxygen (found suitable version "1.9.8", required range is "1.9.0...<2.0.0") found components: doxygen dot
      -- Found Sphinx: /mnt/c/Users/rtlfrm/Documents/Projects/Products/Modbus/m-bus/myvenv/bin/sphinx-build
      -- Configuring done (28.6s)
      -- Generating done (3.7s)
      -- Build files have been written to: /mnt/c/Users/rtlfrm/Documents/Projects/Products/Modbus/m-bus/build.tutorial

#. Reconfigure the stack to show more logging information::

      cmake -B build.tutorial -DLOG_LEVEL=DEBUG

   Example output::

      -- Current build type is: RelWithDebInfo
      -- Current install path is: /mnt/c/Users/rtlfrm/Documents/Projects/Products/Modbus/m-bus/build.tutorial/install
      -- Building for Linux
      -- Found Doxygen: /usr/bin/doxygen (found version "1.9.8") found components: doxygen dot
      -- Found Doxygen: /usr/bin/doxygen (found suitable version "1.9.8", required range is "1.9.0...<2.0.0") found components: doxygen dot
      -- Configuring done (5.6s)
      -- Generating done (2.9s)
      -- Build files have been written to: /mnt/c/Users/rtlfrm/Documents/Projects/Products/Modbus/m-bus/build.tutorial

#. Now build the M-Bus stack as well as sample applications::

      cmake --build build.tutorial

   The output depends on the build system used, but should contain no errors.
   Example output::

      [  2%] Building C object _deps/osal-build/CMakeFiles/osal.dir/src/linux/osal.o
      [  5%] Building C object _deps/osal-build/CMakeFiles/osal.dir/src/linux/osal_log.o
      [  8%] Linking C static library libosal.a
      [  8%] Built target osal
      [ 11%] Building C object CMakeFiles/mbus.dir/src/ports/linux/mbal_tcp.o
      [ 14%] Building C object CMakeFiles/mbus.dir/src/ports/linux/mbal_rtu.o
      [ 17%] Building C object CMakeFiles/mbus.dir/src/mbus.o
      [ 20%] Building C object CMakeFiles/mbus.dir/src/mb_slave.o
      [ 23%] Building C object CMakeFiles/mbus.dir/src/mb_transport.o
      [ 26%] Building C object CMakeFiles/mbus.dir/src/mb_tcp.o
      [ 29%] Building C object CMakeFiles/mbus.dir/src/mb_rtu.o
      [ 32%] Building C object CMakeFiles/mbus.dir/src/mb_crc.o
      [ 35%] Linking C static library libmbus.a
      [ 35%] Built target mbus
      [ 38%] Building C object sample/CMakeFiles/mb_master.dir/ports/linux/mb_bsp.o
      [ 41%] Building C object sample/CMakeFiles/mb_master.dir/ports/linux/tcp_rtu_master.o
      [ 44%] Linking C executable mb_master
      [ 44%] Built target mb_master
      [ 47%] Building C object sample/CMakeFiles/mb_tcp_slave.dir/slave.o
      [ 50%] Building C object sample/CMakeFiles/mb_tcp_slave.dir/tcp_slave.o
      [ 52%] Linking C executable mb_tcp_slave
      [ 52%] Built target mb_tcp_slave
      [ 55%] Building C object sample/CMakeFiles/mb_rtu_slave.dir/ports/linux/mb_bsp.o
      [ 58%] Building C object sample/CMakeFiles/mb_rtu_slave.dir/ports/linux/rtu_slave.o
      [ 61%] Building C object sample/CMakeFiles/mb_rtu_slave.dir/slave.o
      [ 64%] Linking C executable mb_rtu_slave
      [ 64%] Built target mb_rtu_slave
      [ 67%] Building CXX object _deps/googletest-build/googletest/CMakeFiles/gtest.dir/src/gtest-all.o
      [ 70%] Linking CXX static library ../../../lib/libgtest.a
      [ 70%] Built target gtest
      [ 73%] Building CXX object _deps/googletest-build/googlemock/CMakeFiles/gmock.dir/src/gmock-all.o
      [ 76%] Linking CXX static library ../../../lib/libgmock.a
      [ 76%] Built target gmock
      [ 79%] Building CXX object test/CMakeFiles/mbus_test.dir/test_mbus.o
      [ 82%] Building CXX object test/CMakeFiles/mbus_test.dir/test_slave.o
      [ 85%] Building C object test/CMakeFiles/mbus_test.dir/__/sample/slave.o
      [ 88%] Building CXX object test/CMakeFiles/mbus_test.dir/mocks.o
      [ 91%] Building CXX object test/CMakeFiles/mbus_test.dir/mbus_test.o
      [ 94%] Building C object test/CMakeFiles/mbus_test.dir/__/src/mbus.o
      [ 97%] Building C object test/CMakeFiles/mbus_test.dir/__/src/mb_slave.o
      [100%] Linking CXX executable mbus_test
      [100%] Built target mbus_test

   This should produce the two command line programs to be used:

   * :command:`build.tutorial/sample/mb_master`: Master built from
     :file:`sample/ports/linux/tcp_rtu_master.c`.
   * :command:`build.tutorial/sample/mb_tcp_slave`: Slave built from
     :file:`sample/tcp_slave.c` and :file:`sample/slave.c`.

#. Open a new terminal window. Go to the :file:`m-bus` directory and type::

      build.tutorial/sample/mb_tcp_slave

   This will start up the Modbus slave, which will begin listening for incoming
   requests at TCP port 8502. There shouldn't be any outputs. The
   program will continue to run for 30 minutes and then quit.

#. In the old terminal window, type::

      build.tutorial/sample/mb_master write tcp 127.0.0.1:8502 hold 0x0001 0x1234

   This wrote the value 0x1234 to the slave's holding register at address 0x0001.
   Output should look like this::

      [14:02:00 INFO ] Connection established
      [14:02:00 DEBUG] Sending header and 5 bytes data
      [14:02:00 DEBUG] Receiving header
      [14:02:00 DEBUG] Receiving 5 bytes data

   On the other terminal window (the slave), the output should look like this::

      [14:02:00 INFO ] Connection established
      [14:02:00 DEBUG] Receiving header
      [14:02:00 DEBUG] Receiving 5 bytes data
      [14:02:00 DEBUG] Sending header and 5 bytes data
      [14:02:00 DEBUG] Receiving header
      [14:02:00 INFO ] Connection closed

#. Read back the written value using the following command::

      build.tutorial/sample/mb_master read tcp 127.0.0.1:8502 hold 0x0001

   Output should look like this::

      [14:06:58 INFO ] Connection established
      [14:06:58 DEBUG] Sending header and 5 bytes data
      [14:06:58 DEBUG] Receiving header
      [14:06:58 DEBUG] Receiving 4 bytes data
      0x1234

   Note that the value printed out on the last line is the same as was
   previously written.

#. You have successfully created a Modbus master and a Modbus slave and made
   them communicate with each other. This concludes the tutorial.

   Feel free to experiment using other master commands
   as given by the argument :code:`--help`::

      build.tutorial/sample/mb_master --help

   Output::

      Read/write Modbus registers

      This purpose of this program is to demonstrate use of the M-Bus
      Modbus master stack. It may also be useful as a debugging tool.

      USAGE:
      build.tutorial/sample/mb_master [OPTIONS] cmd transport connection table address [value]

      cmd         {read, write}            Type of transaction
      transport   {tcp, rtu}               Type of transport
      connection  {IP:PORT, DEVICE:BAUDRATE:PARITY:UNIT}
                                           Connection details
      table       {coil, input, reg, hold} Modbus address table
      address                              Modbus address

      TCP CONNECTION DETAILS
      IP          IP-address, e.g. 192.168.0.1
      PORT        Modbus port, e.g. 502

      RTU CONNECTION DETAILS
      DEVICE      Serial device, e.g. /dev/ttyUSB0
      BAUDRATE    Modbus baudrate
      PARITY      Modbus parity {odd, even, none}

      OPTIONS:
       --repeat n Repeat command n times
       --delay ms Time to delay between Modbus transactions

      EXAMPLES:

      Repeatedly read coil 1 of Modbus TCP slave 192.168.10.134, port 8502.

      build.tutorial/sample/mb_master --repeat 100 read tcp 192.168.10.134:8502 coil 0x0001

      Write 42 to holding register 3 of Modbus RTU slave 2, accessed
      through /dev/ttyUSB0 with baudrate 115200, no parity.

      build.tutorial/sample/mb_master write rtu /dev/ttyUSB0:115200:none:2 hold 0x0003 42

   Additional printouts may be added to :file:`sample/slave.c` if desired.
