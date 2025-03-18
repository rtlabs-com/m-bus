RTU transport API
=================

The Modbus RTU transport data layer API is defined in :file:`mb_rtu.h`.
This chapter contains the documentation generated from said header file.
For an explanation of how the layer works under the hood,
see :ref:`The RTU transport data layer`.

.. toctree::
   :maxdepth: 1
   :caption: Contents

RTU configuration
-----------------

.. doxygenstruct:: mb_rtu_cfg_t
   :members:
.. doxygenenum:: mb_rtu_parity_t
.. doxygenstruct:: mb_rtu_serial_cfg_t
   :members:

RTU initialisation
------------------

.. doxygenfunction:: mb_rtu_init

RTU reconfiguration
-------------------

.. doxygenfunction:: mb_rtu_serial_cfg

