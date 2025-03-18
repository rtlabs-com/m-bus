Errors and exceptions
=====================

The API for errors and exception is defined in :file:`mb_error.h`.

Modbus exceptions
-----------------

A slave callback can return these. A master read
or write operation may result in one of these exceptions.

See "MODBUS Application Protocol Specification" chapter 7:
"MODBUS Exception Responses".

.. doxygendefine:: EILLEGAL_FUNCTION
.. doxygendefine:: EILLEGAL_DATA_ADDRESS
.. doxygendefine:: EILLEGAL_DATA_VALUE
.. doxygendefine:: ESLAVE_DEVICE_FAILURE

Modbus communication errors
---------------------------

.. doxygendefine:: ECRC_FAIL
.. doxygendefine:: EFRAME_NOK
.. doxygendefine:: ESLAVE_ID
.. doxygendefine:: ETIMEOUT
.. doxygendefine:: EUNKNOWN_EXCEPTION


Error helper functions
----------------------

.. doxygenfunction:: mb_error_literal
