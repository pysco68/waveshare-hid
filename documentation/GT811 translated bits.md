Some (google-)translated bits out of the GT811 data sheet
=========================================================

Built-in capacitance detection circuit and high-performance MPU
- Touch scanning frequency adjustable, the range: 50Hz ~ 100Hz
- touch point coordinates real-time output
- Support configuration fixed touch key position
- Uniform software version for a variety of sizes of capacitive screen
- Single power supply, built-in 1.8V LDO
- Flash process, support online programming
- Capacitive screen sensor
- Capacitor screen size range: 5 "~ 7"
- Support floating channel design
- Support both ITO glass and ITO film
- Cover Lens thickness support: glass ≦ 2mm acrylic ≦ 0.9mm
- module automatic identification
- environmental adaptability
- Initialize the automatic calibration
- Automatic temperature drift compensation
- Operating temperature: -20 ℃ ~ +85 ℃, humidity: ≦ 95% RH
- Storage temperature: -60 ℃ ~ +120 ℃, humidity: ≦ 95% RH
- communication interface
- Standard I2C communication interface
- slave operating mode
- support 1.8V ~ 3.6V interface level

Application development support tool
- Touch screen module parameter detection and configuration parameters are automatically generated
- Touch screen module performance testing tool
- module production testing tools
- master software development reference to the driver code and document guidance

I2C Design
==========

GT811 provides a standard I2C communication interface, by the SCL and SDA and the main CPU to communicate. In the system GT811 always as a slave, all communication is initiated by the main CPU, the maximum communication speed of 600K bps. The supported I2C hardware circuit support timing is as follows:

GT811 from the device address has three sets of optional, in order to facilitate the master deployment. Three groups of addresses are: 0xBA / 0xBB, 0x6E / 0x6F and 0x28 / 0x29. The default address is 0b1010 101x. When the main CPU is addressing the GT811, it also sends the read and write control bits. The read and write control bits are attached to the slave address, "0" for write operation, "1" for read operation, The device address consists of one byte. That is: 0xBA - for the GT811 write operations; 0xBB - for GT811 read operation.
The I2C device is in the off state, in order to ensure reliable communication, the need to carry out the prefix communication to wake up the I2C device; in the I2C communication is complete, the suffix communication to inform the GT811 can turn off the I2C device. Prefix communication, suffix communication format is as follows:
Prefix communication: Use the write operation to find the register address 0x0FFF, and then send a stop signal.
Suffix Communication: Using the write operation to find the register address 0x8000, and then send a stop signal.
The host can perform one or more I2C communications between primary and postfix communications.

### A) Data transmission

Communication is always initiated by the main CPU, the effective start signal: SCL is maintained as "1", SDA issued
The transition from "1" to "0" occurs. The address information or data stream is transmitted after the start signal.
All slave devices connected to the I2C bus must detect 8 bits of data transmitted on the bus after the start signal
Address information, and make the right response. In the received address information with their match, GT811 in the first
9 clock cycles, the SDA to output, and set "0", as the response signal. If not received with their own
Matching address information, that is, non-0XAA or 0XAB, GT811 will remain idle.
The data on the SDA port is serially transmitted in 9 clock cycles. 9 bits of data: 8 bits of valid data + 1 bit of receiver
A response signal ACK or a non-acknowledge signal NACK. Data transfer is valid when SCL is "1".
When the communication is completed, the main CPU sends a stop signal. The stop signal is SDA when SCL is "1"
Transition of state from "0" to "1".

6.4 pulse mode call
-------------------

In order to effectively reduce the burden on the main CPU, GT811 only in the output information changes, it will notify the main CPU to read coordinates
information. By the INT output pulse signal. The main CPU can set the trigger by register bit "INT" of 0x6D9
the way. Set to "1" means that the rising edge of the trigger, that is, when a user operation, GT811 INT output will be rising edge of the jump
Change, notify the CPU; set to "0" that the falling edge of the trigger.

6.5 sleep mode
--------------

When the display goes off or in other situations where it is not necessary to operate the touch screen, the GT811 can be enabled via the I2C command
Into Sleep mode to reduce power consumption. When the GT811 needs to work properly, the / RSTB port set to "1" can be. In the back
After the Sleep mode GT811 automatically enter the Normal mode.

6.6 automatic calibration
-------------------------

### A) Initialize the calibration

Different temperature, humidity and physical space structure will affect the capacitance sensor in the idle state of the baseline value.
GT811 will automatically within 200ms of initialization according to the environment to obtain a new test basis. Finished touch
Initialization of touch screen detection.

### B) automatic temperature drift compensation

Temperature, humidity or dust and other environmental factors, the slow changes will also affect the capacitive sensor in the idle state
Of the reference value. GT811 real-time detection of data changes at all points, the historical data for statistical analysis, thus
To correct the detection reference. Thereby reducing the impact of environmental changes on the touch screen detection.