The master stack
================

The master API allows user application to access slaves using blocking function
calls, such as :func:`mbus_read` and :func:`mbus_write`.
The choice to use TCP or RTU as transport data layer is selected by user when
instantiating the master. The following figure shows the architecture of the
master stack from the perspective of the user application:

.. kroki:: svg
   :type: blockdiag
   :options: size: 800x600
   :caption: The M-Bus master stack architecture. Solid arrows denote direct
             function calls while dashed arrows denotes calls through function
             pointers

   blockdiag {
      orientation = portrait

      app -> master [label = "mbus_read()\nmbus_write()", fontsize = 6];
      master -> transport-layer [style = "dashed",
                                 label = "tx()\nrx()", fontsize = 6];

      app [label = "User application"];
      master [color = "lightblue", label = "Master instance"];
      transport-layer [color = "lightblue",
                       label = "Transport data layer"];
   }

Notice that the master instance calls the transport data layer through function pointers. This means that only the layer actually used (TCP or RTU) needs to be
linked into the program binary, saving program memory.

The following sequence diagram shows a master instance
reading a register from a remote slave:

.. kroki:: svg
   :type: seqdiag
   :caption: Example communication between a local master and a remote slave
            (in green). The master is composed of the mbus-stack
             (in blue) and user application (in white).

   seqdiag {
      app -> master [label = "mbus_read(master, slave,      MB_ADDRESS(4,23), 1, buffer)"];
      master -> slave [label = "Request: Read holding \nregister 23"];
      master <-- slave [label = "Response: 0x1234"];
      app <-- master [label = "Ok (result put in buffer)"];

      app [label = "User application"];
      master [color = "lightblue", label = "Master instance"];
      slave [color = "lightgreen", label = "Remote slave"];
   }

For a detailed description of the master API, see :ref:`Master API`.
