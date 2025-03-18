Slave API
=========

The M-Bus slave API is defined in :file:`mb_slave.h`.
This chapter contains the documentation generated from said header file.
For an introduction to the API, see :ref:`The slave stack`.

.. toctree::
   :maxdepth: 2
   :caption: Contents

Initialisation and configuration
--------------------------------

.. doxygenstruct:: mb_iotable_t
   :members:
.. doxygenstruct:: mb_iomap_t
   :members:
.. doxygenstruct:: mb_vendor_func_t
   :members:
.. doxygenstruct:: mb_slave_cfg_t
   :members:
.. doxygenstruct:: mb_slave_t

.. doxygenfunction:: mb_slave_init
.. doxygenfunction:: mb_slave_id_set
.. doxygenfunction:: mb_slave_transport_get

De-initialisation
-----------------

.. doxygenfunction:: mb_slave_shutdown

Data encoding
-------------

.. doxygenfunction:: mb_slave_bit_set
.. doxygenfunction:: mb_slave_reg_set

Data decoding
-------------

.. doxygenfunction:: mb_slave_bit_get
.. doxygenfunction:: mb_slave_reg_get
