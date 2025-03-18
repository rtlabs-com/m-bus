The Modbus protocol
===================

Modbus is a communication protocol used in industrial automation systems
for transmitting data between electronic devices, such as Programmable Logic
Controllers (PLCs), sensors, and other control devices. The protocol allows for
data exchange between devices over various media such as RS-232, RS-485 and
Ethernet.

Modbus operates on a master-slave (or client-server) model:

1. **Master** (client)

   * Sends requests to slave devices and receives corresponding response
     messages. It can communicate with one or more
     slave devices.
   * Handles error checking and timeouts in communication.
   * Is usually a PLC or a computer.

2. **Slave** (server)

   - Listens for requests from the master device. It cannot
     initiate communication on its own.
   - Performs requested actions like
     reading/writing data to registers, controlling outputs, etc. and sends
     back the response message.
   - Returns an exception response to the master if error is encountered.
   - Has a unique address that
     allows the master to address and interact with it.
   - May be a sensor, an actuator, or a more complex device.

.. kroki:: svg
   :type: seqdiag
   :caption: Example communication between a master and two slaves.
             The slaves have addresses 45 and 46

   seqdiag {
      Master  -> Slave45 [label = "Request: Write value 0x1234 to holding register 34"];
      Master <-- Slave45 [label = "Response: OK"];
      Master  -> Slave46 [label = "Request: Read value from holding register 100"];
      Master <-- Slave46 [label = "Response: Exception (Illegal Data Address)"];
   }

The data model is organized into four data types (or *tables*):

- **Coils**: Single-bit values (ON/OFF or True/False).
- **Discrete Inputs**: Single-bit inputs (Read-only, typically used for sensor
  status).
- **Holding Registers**: 16-bit data registers (Read/Write, used to store data
  like process values).
- **Input Registers**: 16-bit data registers (Read-only, used for input data
  such as sensor measurements).

Several different media technologies may be used as data transport layer for the
Modbus protocol:

- **Modbus RTU (Remote Terminal Unit)**: A binary protocol that uses compact
  data frames for efficient communication. It's typically used in serial
  communication (RS-232, RS-485).
- **Modbus ASCII**: A human-readable text-based version of the Modbus
  protocol, used over serial communication.
- **Modbus TCP**: A version of Modbus that operates over TCP/IP networks
  (Ethernet).
