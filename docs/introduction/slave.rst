The slave stack
===============

The slave API allows user application to register callback functions to be
called when receiving incoming requests from a remote modbus master.
It also allows the creation of the OS thread that will serve the incoming
requests. The callback functions will run on this OS thread and are free to
access helper functions provided by the slave API.
The choice to use TCP or RTU as transport data layer is selected by user when
instantiating the slave. The following figure shows the architecture of the
slave stack from the perspective of the user application:

.. kroki:: svg
   :type: blockdiag
   :caption: The M-Bus slave stack architecture. Solid arrows denote direct
             function calls while dashed arrows denotes calls through function
             pointers

   blockdiag {
      orientation = portrait

      app -> slave [label = "mb_slave_init()", fontsize = 6];
      slave -> transport-layer [style = "dashed",
                                label = "tx()\nrx()",
                                fontsize = 6];
      slave -> callbacks [style = "dashed",
                          folded];
      callbacks -> helpers [label = "mb_slave_reg_get\nmb_slave_reg_set",
                            fontsize = 5];

      app [label = "User application"];
      slave [color = "lightblue", label = "Slave thread"];
      transport-layer [color = "lightblue",
                       label = "Transport data layer"];
      callbacks [label = "User callbacks"];
      helpers [color = "lightblue", label = "Helper function"];
   }

Notice that the slave thread calls the transport data layer through function pointers. This means that only the layer actually used (TCP or RTU) needs to be
linked into the program binary, saving program memory.

The following sequence diagram shows a slave instance (an OS thread) with
callback functions registered by user. A remote master
makes a request and the M-Bus slave stack responds:

.. kroki:: svg
   :type: seqdiag
   :caption: Example communication between a remote master (in green) and a slave.
             The slave is composed of the mbus-stack (in blue) and user callback
             functions (in white)

   seqdiag {
      master -> slave [label = "Request: Read holding \nregister 23"];
      slave -> callbacks [label = "get(23, response_buffer, 1)"];
      callbacks -> helpers [label = "mb_slave_reg_set(\nresponse_buffer, 23, 0x1234)"];
      callbacks <-- helpers;
      slave <-- callbacks [label = "OK"];
      master <-- slave [label = "Response: 0x1234"];

      slave [color = "lightblue", label = "Slave thread"];
      master [color = "lightgreen", label = "Remote master"];
      callbacks [label = "User callbacks"];
      helpers [color = "lightblue", label = "Helper functions"];
   }

For a detailed description of the slave API, see :ref:`Slave API`.
