Stack limitations
=================

* Modbus ASCII not supported
* Modbus RTU not supported on Windows
* The following functions codes are not supported:

   - 0x07: Read Exception Status
   - 0x0B: Get Comm Event Counter
   - 0x0C: Get Comm Event Log
   - 0x11: Report Server ID
   - 0x14: Read File Record
   - 0x15: Write File Record
   - 0x16: Mask Write Register
   - 0x18: Read FIFO Queue
   - 0x2B: Encapsulated Interface Transport
   - 0x2B/0x0D: CANopen General Reference Request and Response
   - 0x2B/0x0E: Read Device Identification
