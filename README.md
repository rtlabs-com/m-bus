Modbus stack
=============
[![Build Status](https://github.com/rtlabs-com/m-bus/workflows/Build/badge.svg?branch=master)](https://github.com/rtlabs-com/m-bus/actions?workflow=Build)
[![CodeQL](https://github.com/rtlabs-com/m-bus/workflows/CodeQL/badge.svg?branch=master)](https://github.com/rtlabs-com/m-bus/actions?workflow=CodeQL)

This repository contains the M-Bus Modbus stack. The stack is written to an OS
abstraction layer and can also be used in a bare metal
application. Using the abstraction layer, the stack can run on Linux,
Windows or on an RTOS.

Cloning
=======

Clone the source:

```
$ git clone --recurse-submodules https://github.com/rtlabs-com/m-bus.git
```

This will clone the repository with submodules. If you already cloned
the repository without the `--recurse-submodules` flag then run this
in the `m-bus` folder:

```
$ git submodule update --init --recursive
```

Building
========

See the [manual](https://docs.rt-labs.com/m-bus/).

Contributions
=============

Contributions are welcome. If you want to contribute you will need to
sign a Contributor License Agreement and send it to us either by
e-mail or by physical mail. More information is available
[here](https://rt-labs.com/contribution).
