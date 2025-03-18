TCP transport API
=================

The Modbus TCP transport data layer API is defined in :file:`mb_tcp.h`.
This chapter contains the documentation generated from said header file.
For an explanation of how the layer works under the hood,
see :ref:`The TCP transport data layer`.

.. toctree::
   :maxdepth: 1
   :caption: Contents

TCP configuration
-----------------

.. doxygendefine:: MODBUS_DEFAULT_PORT
.. doxygenstruct:: mb_tcp_cfg_t
   :members:

TCP initialisation
------------------

.. doxygenfunction:: mb_tcp_init

