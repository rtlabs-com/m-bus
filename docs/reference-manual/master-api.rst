Master API
==========

The M-Bus master API is defined in :file:`mbus.h`.
This chapter contains the documentation generated from said header file.
For an introduction to the API, see :ref:`The master stack`.

.. toctree::
   :maxdepth: 2
   :caption: Contents

Initialisation
--------------

.. doxygenstruct:: mbus_cfg_t
   :members:
.. doxygenstruct:: mbus_t

.. doxygenfunction:: mbus_create
.. doxygenfunction:: mbus_init

Connection handling
-------------------

.. doxygenfunction:: mbus_connect
.. doxygenfunction:: mbus_disconnect
.. doxygenfunction:: mbus_transport_get

Data access
-----------

.. doxygenenum:: mb_table_t
.. doxygentypedef:: mb_address_t
.. doxygendefine:: MB_ADDRESS

.. doxygenfunction:: mbus_read
.. doxygenfunction:: mbus_write
.. doxygenfunction:: mbus_write_single

Raw data access
---------------

.. doxygenfunction:: mbus_send_msg
.. doxygenfunction:: mbus_get_msg

Diagnostics
-----------

.. doxygenfunction:: mbus_loopback
