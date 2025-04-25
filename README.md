Modbus stack
=============

This repository contains the M-Bus Modbus stack. The stack is written to an OS
abstraction layer and can also be used in a bare metal
application. Using the abstraction layer, the stack can run on Linux,
Windows or on an RTOS.

Web resources
-------------

* Source repository: [https://github.com/rtlabs-com/m-bus](https://github.com/rtlabs-com/m-bus)
* Documentation: [https://rt-labs.com/docs/m-bus](https://rt-labs.com/docs/m-bus)
* RT-Labs (stack integration, certification services and training): [https://rt-labs.com](https://rt-labs.com)

Features
--------

* Modbus master (client)
* Modbus slave (server)
* Modbus TCP
* Modbus RTU
* All four primary tables (coils, inputs, holding registers, input registers)
* Vendor-defined function codes
* Diagnostics
* Implemented in C, supports C++
* Ports available for Linux, Windows (Modbus TCP only) and RT-Kernel
* Porting layer allows extending the stack to new ports

License
-------

This software is dual-licensed, with GPL version 3 and a commercial license. See
LICENSE.md for more details.

Requirements
------------

cmake

* cmake 3.28 or later

For Linux:

* gcc 4.6 or later

For rt-kernel:

* Workbench 2018.1 or later


Contributions
-------------

Contributions are welcome. If you want to contribute you will need to sign a
Contributor License Agreement and send it to us either by e-mail or by physical
mail. More information is available
on [https://rt-labs.com/contribution](https://rt-labs.com/contribution).
