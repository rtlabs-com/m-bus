The TCP transport data layer
============================

The TCP transport data layer (or TCP layer for short) is an implementation of
Modbus TCP, where Modbus data is transported over a TCP/IP network.
It is one of two means of data transport supported, the other being Modbus RTU.
The TCP layer may be used by either the slave stack or the master stack. The
user does not interact with the TCP layer directly, except when initialising
it (see :ref:`TCP transport API`).
This chapter will describe how the TCP layer interacts with the master/slave
instance which owns it and the underlying TCP/IP network.

TCP Layering
------------

The TCP layer sits between the application layer (the master or slave instance)
and the "physical" layer (the TCP/IP network stack). The TCP layer is
platform-agnostic while the physical layer is platform-dependent.

.. kroki:: svg
   :type: blockdiag
   :options: size: 800x600
   :caption: Data flow between the master or slave instance, the TCP layer and
             the physical layer

   blockdiag {
      orientation = portrait

      app <-> tcp [label = "PDU", fontsize = 8];
      tcp <-> port [label = "TCP frame", fontsize = 8];

      app [color = "lightblue", label = "Master or\n slave"];
      tcp [color = "lightblue", label = "TCP layer"];
      port [color = "lightblue", label = "Physical layer"];
   }

The master or slave instance exchanges data with the TCP layer by means of
PDUs, where a single byte header is appended to the data:

+-------------------+
| PDU               |
+===================+
| n bytes           |
+-------------------+

The TCP layer in turn exchanges data with the physical layer by means of TCP
frames, where a 7 byte header is appended to the PDU:

+-------------------+-------------------+-------------------+-------------------+-------------------+
| Transaction ID    | Protocol ID       | Length            | Unit ID           | PDU               |
+===================+===================+===================+===================+===================+
| 2 bytes           | 2 bytes           | 2 bytes           | 1 byte            | n bytes           |
+-------------------+-------------------+-------------------+-------------------+-------------------+

Establishing a TCP connection
-----------------------------

Establishing a TCP connection works differently for slave and master instances.
For a slave instance, the TCP layer will act as a TCP server, listening for
incoming connections on the local port configured by user.
For a master instance, the TCP layer will act as a TCP client and try to
connect to the remote port configured by user.

Sending a TCP frame
-------------------

When the master or slave instance wishes to send a PDU to the connected remote
peer, it calls the TCP layer, which then performs the following actions:

#. Constructs the TCP frame, by appending the header to the PDU.
#. Attempts to send the whole frame. The underlying physical layer may divide
   this into multiple transfers, until everything is sent.
#. On timeout or any other error, the attempt is aborted and the connection is closed.

Receiving a TCP frame
---------------------

When the master or slave instance wishes to receive a PDU from the connected remote
peer, it calls the TCP layer, which then performs the following actions:

#. Waits for anything to be received, with a timeout.
#. If timeout expires, the attempt is aborted.
#. On any other error, the attempt is aborted and the connection is also closed.
#. Waits to receive the header, with a timeout.
#. On timeout or any other error, the attempt is aborted and the connection is also closed.
#. Reads the PDU size in the received header.
#. Waits to receive the PDU with the given size, with a timeout.
#. On invalid Protocol ID field, the attempt is aborted.
#. On any other error, the attempt is aborted and the connection is also closed.
#. Hands over the received PDU to the slave/master instance.

