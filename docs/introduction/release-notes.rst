Release notes
=============

Version 1.0.1
-------------

Summary
~~~~~~~

This release fixes cybersecurity issue CVE-2026-76815. Please see the
`cybersecurity advisory
<https://rt-labs.com/wp-content/uploads/2026/09/RRTL-260927-01.pdf>`_
for more information.

Issues fixed
~~~~~~~~~~~~

.. only:: not spelling

   .. csv-table::
      :file: issues/v1.0.1.csv
      :widths: 10, 60, 20, 10
      :header-rows: 1

Version 1.0.0
-------------

Summary
~~~~~~~

**Source code availability**

Starting with version 1.0.0, we have made the following changes
regarding source code availability:

- Porting layers and build support files are no longer published on GitHub.
- Evaluation binaries are available for common platforms. You can
  download these binaries to evaluate the software without needing to
  rebuild the sources.
- Full source code is available with a commercial license. If you
  require access to the complete source code, you can purchase a
  commercial license from us.

This change allows us to provide evaluation binaries for free while
reserving the complete source code for commercial customers. If you
have any further questions or need assistance, please contact
support@rt-labs.com.

New features
~~~~~~~~~~~~

- Added generated documentation of stack
- Allow building applications from pre-built M-Bus stack
- Upgrade to modern cmake versions

Issues fixed
~~~~~~~~~~~~

- Fix GCC 13 compilation warnings
- Fix MSVC compilation warnings
- Fix missing C++ footer

Version 0.1.0
-------------

Summary
~~~~~~~

Support for windows added to stack

New features
~~~~~~~~~~~~

- Windows support (Modbus TCP only)

Version 0.0.0
-------------

Summary
~~~~~~~

Initial release of M-Bus

- Modbus master (client)
- Modbus slave (server)
- Modbus TCP
- Modbus RTU
- All four primary tables (coils, inputs, holding registers, input registers)
- Vendor-defined function codes
- Diagnostics
- Implemented in C, supports C++
- Ports available for Linux and RT-Kernel
- Porting layer allows extending the stack to new ports
