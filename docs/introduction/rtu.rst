The RTU transport data layer
============================

The RTU transport data layer (or RTU layer for short) is an implementation of
Modbus RTU, where Modbus data is transported over a serial line.
It is one of two means of data transport supported, the other being Modbus TCP.
The RTU layer may be used by either the slave stack or the master stack.
The behaviour of the RTU layer is identical whether used by a master or slave
instance.
The user does not interact with the RTU layer directly, except when intitialising
it (see :ref:`RTU transport API`).
This chapter will describe how the RTU layer interacts with the master/slave
instance which owns it and the underlying serial line.

RTU layering
------------

The RTU layer is similar to the OSI layer 2 (data link layer) and sits between
the application layer (the master or slave instance)
and the physical layer. The RTU layer is
platform-agnostic while the physical layer is platform-dependent.

.. kroki:: svg
   :type: blockdiag
   :options: size: 800x600
   :caption: Data flow between the master or slave instance, the RTU layer and
             the physical layer

   blockdiag {
      orientation = portrait

      app <-> rtu [label = "PDU", fontsize = 8];
      rtu <-> phy [label = "RTU frame", fontsize = 8];

      app [color = "lightblue", label = "Master or\n slave"];
      rtu [color = "lightblue", label = "RTU layer"];
      phy [color = "lightblue", label = "Physical layer"];
   }

The master or slave instance exchanges data with the RTU layer by means of
PDUs. The RTU layer does not interpret the contents of the PDU, but sees it
as raw data:

+-------------------+
| PDU               |
+===================+
| 1 - 253 bytes     |
+-------------------+

The RTU layer in turn exchanges data with the physical layer by means of RTU
frames, where a single byte header and a two bytes footer is appended to the PDU:

+-------------------+-------------------+-------------------+
| Slave address     | PDU               | CRC               |
+===================+===================+===================+
| 1 byte            | 1 - 253 bytes     | 2 bytes           |
+-------------------+-------------------+-------------------+

RTU frame boundaries
--------------------

In Modbus RTU, there is no special signalling to determine the boundaries
between RTU frames. This is instead done by measuring the time elapsed between bytes.
If the time elapsed between two bytes is more than t3p5 (typically 1750 us),
it is interpreted as a
boundary between two different RTU frames. If the time elapsed between two bytes
is less than t1p5 (typically 750 us), the bytes are interpreted as belonging to
the same RTU frame.

The RTU layer uses two one-shot microsecond timers for this purpose, as
provided by user. User could then implement them using a single hardware timer
as they are always started in one go.

Sending an RTU frame
--------------------

When the master or slave instance wishes to send a PDU with a given Slave
address, it calls the RTU layer, which then performs the following actions:

#. Enables transmission by calling the user-supplied function :func:`enable_tx`.
   This may be done automatically by hardware, in which this step is skipped.
#. Sends the slave address.
#. Sends the PDU.
#. Calculates the CRC and sends it.
#. Makes sure all data has been sent.
#. Disables transmission by calling the user-supplied function :func:`enable_tx`.
   This may be done automatically by hardware, in which this step is skipped.
#. Waits for the inter-frame delay t3p5 by calling the user-supplied function
   :func:`tmr_start`.

Note that the maximum delay between bytes (t1p5) is not enforced by the RTU
layer when sending. It is assumed that the physical layer ensures that the delay
is short.
At higher baud rates (> 19.200 bps), where t1p5 is fixed at 750 us, this should
be easy to achieve.

Receiving an RTU frame
----------------------

When the master or slave instance wishes to receive a PDU with a given
Slave address, it calls the RTU layer, which then performs the following actions:

#. Waits to receive the first byte, which is the slave address. A timeout may
   be set. If timeout expires, the attempt is aborted.
#. Starts the inter-byte timer, t1p5, and the inter-frame timer,
   t3p5, by calling the user-supplied function :func:`tmr_start`.
#. Waits to receive PDU and CRC, with timeout set to t1p5.
#. Calculates the CRC and compares it to the received CRC.
#. Compares received slave address with expected value.
#. Waits for the inter-frame delay t3p5 by calling the user-supplied function
   :func:`tmr_start`.
#. On success, extracts the PDU from the frame and hands it over the slave/master
   instance. Any error is reported.
