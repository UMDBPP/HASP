/* Link Telemetry v0.3 "Yankee Clipper"
   
   Copyright (c) 2015-2016 University of Maryland Space Systems Lab
   NearSpace Balloon Payload Program
   
   Written by Nicholas Rossomando
   2015-04-07

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

   RS232.cpp:

   Definitions for serial port control.
   Open, read, close.
*/

#include "RS232.h"

#include <iostream>
#include <cstring> // For memset. Once. Bah.

extern "C" {
	#include <fcntl.h>
	#include <sys/file.h>
	#include <sys/ioctl.h>
}

// CTOR just initializes some values.
// Does not actually open the port!
BPP::RS232Serial::RS232Serial() {
	memset(&portConfig, 0, sizeof(portConfig));
	data = "";
}

BPP::RS232Serial::~RS232Serial() {
	portClose(); // DTOR cleans up port if user hasn't.
}

// Open serial port, or try to anyway.
// _comPort: will be something like "/dev/ttyUSB0" or somesuch. Should be specified by user.
// _baudRate: Use Linux "B#" constants, i.e. for 9600 _baudRate = B9600
// _dataBits: how many? Between 5 and 8.
// _parity: 'N', 'E', 'O' (That's an 'oh', not a 'zero'.)
// _stopBits: How many stop bits? 1 or 2.
// Our radios are 8N1, so...portOpen("/dev/ttyWhatever", B9600, 8, 'N', 1, minLength);
int BPP::RS232Serial::portOpen(std::string _comPort, int _baudRate, int _dataBits, char _parity, int _stopBits) {
	int dBits = CS8; // Set internal initial values: data bits
	int parity = 0; // Parity bits
	int ignoreParity = IGNPAR; // Ignore parity setting
	int sBits = 0; // stop bits

	// Convert from int of data bits to OS const
	// Mapping is fairly simple, just prefix # of bits with "CS".
	switch(_dataBits) {
		case 5:
			dBits = CS5;
			break;
		case 6:
			dBits = CS6;
			break;
		case 7:
			dBits = CS7;
			break;
		case 8:
			dBits = CS8;
			break;
		default:
			std::cout << "Port not open: Invalid number of data bits: " << _dataBits << std::endl;
			std::cout << "Please use between 5 and 8\n";
			return 1;
			break;
	}

	// Do the same for parity (accepting both lowercase and uppercase chars)
	// This is more complicated and gets into terminal option stuff.
	// Also sets whether to look for or ignore parity.
	switch(_parity) {
		case 'N':
		case 'n':
			parity = 0; // No parity
			ignoreParity = IGNPAR; // Ignore parity
			break;
		case 'E':
		case 'e':
			parity = PARENB; // Even parity bitmask
			ignoreParity = INPCK; // Don't ignore parity
			break;
		case 'O':
		case 'o':
			parity = (PARENB | PARODD); // Odd parity bitmask
			ignoreParity = INPCK; // Don't ignore parity
			break;
		default:
			std::cout << "Invalid parity setting: " << _parity << std::endl;
			std::cout << "Please use 'N', 'E', or 'O'.\n";
			return 1;
			break;
	}

	// And finally do the same for stop bits.
	// Again, this consists of setting the right values/bitmasks for termios.
	switch(_stopBits) {
		case 1:
			sBits = 0;
			break;
		case 2:
			sBits = CSTOPB;
			break;
		default:
			std::cout << "Invalid number of stop bits: " << _stopBits << std::endl;
			std::cout << "Please use 1 or 2.\n";
			return 1;
			break;
	}

	port = open(_comPort.c_str(), O_RDWR | O_NOCTTY | O_NDELAY); // Open the serial port. Using POSIX C file control. Ugh.
	if(port == -1) {
		std::cerr << "Unable to open com port: " << _comPort << std::endl;
		std::cerr << "Check connections and ensure you are using the right port number!\n";
		return 1;
	}

	// Ensure we're actually opening a serial port!
	if(!isatty(port)) {
		close(port);
		std::cerr << "Specified file " << _comPort << " is not a com port!\nPlease enter a valid com port of the form /dev/tty*\n";
		return 1;
	}

	// Lock access to the port; if port is already locked, inform and exit.
	if(flock(port, LOCK_EX | LOCK_NB) != 0) {
		close(port);
		std::cerr << "Com port locked by another process.\nEnsure you don't have Arduino serial monitor or somesuch open as well.\n";
		return 1;
	}

	// Load up the initial port settings.
	if(tcgetattr(port, &originalPortConfig) < 0) {
		close(port);
		std::cerr << "Could not get com port settings.\n";
		return 1; 
	}

	// Set the new port settings in termios struct:
	portConfig.c_cflag = dBits | parity | sBits | CLOCAL | CREAD; // Set rs-232 mode (i.e. 8N1)
	portConfig.c_iflag = ignoreParity; // Set whether to ignore parity or not.
	portConfig.c_oflag = 0; // More or less standards.
  	portConfig.c_lflag = 0;
  	portConfig.c_cc[VMIN] = 0;
  	portConfig.c_cc[VTIME] = 0;

  	// Set TX and RX baud (why would they ever be different???):
  	cfsetispeed(&portConfig, _baudRate);
  	cfsetospeed(&portConfig, _baudRate);

  	// Write all the new settings.
  	if(tcsetattr(port, TCSANOW, &portConfig) < 0) {
  		close(port);
  		std::cerr << "Unable to set new port settings.\n";
  		return 1;
  	}

  	// Get port status:
  	int status;
  	if(ioctl(port, TIOCMGET, &status) == -1) {
    	close(port);
    	std::cerr << "Unable to get port status.\n";
    	return 1;
  	}

  	// Turn on DTR and RTS
  	status |= TIOCM_DTR;
  	status |= TIOCM_RTS;

  	// Now reset port status:
  	if(ioctl(port, TIOCMSET, &status) == -1) {
  		close(port);
  		std::cerr << "Unable to set port RTS and DTR.\n";
  		return 1;
  	}

  	return 0;
}

// Recieve data from the serial port.
// Saves data in C++ std::string.
// Data can be retrieved by getter.
// Return value is number of bytes read.
int BPP::RS232Serial::rxData() {
	int nBytes = 0; // Return value
	unsigned char rxBuf[4096]; // Temp storage buffer.

	nBytes = read(port, rxBuf, 4095); // Read up to 4095 characters from serial buffer.
	if(nBytes > 0) { // Do this only if we read data.
		rxBuf[nBytes] = 0; // Null terminate the C-string!
		data = reinterpret_cast<char*>(rxBuf); // Cast buffer to C-string, assign to data string.
	}

	return nBytes; // Report bytes recieved to user.
}

// Clear out recieve buffer.
// Should clean up data in the buffer in case buffer is filled slower than read.
void BPP::RS232Serial::portFlush() {
	tcflush(port, TCIFLUSH);
}

// Close connection to port, resetting original status.
// Then safely release lock on serial port.
void BPP::RS232Serial::portClose() {
	int status; // port status bitmask.
	if(ioctl(port, TIOCMGET, &status) == -1) {
    	std::cerr << "Unable to get port status.\n"; // No return - have to close port.
  	}

  	// Shut off DTR and RTS
  	status &= ~(TIOCM_DTR);
  	status &= ~(TIOCM_RTS);

  	if(ioctl(port, TIOCMSET, &status) == -1) {;
  		std::cerr << "Unable to reset port RTS and DTR.\n";
  	}

  	tcsetattr(port, TCSANOW, &originalPortConfig); // Reset port settings to original state.
  	close(port); // Close the port buffer.
  	flock(port, LOCK_UN); // Unlock port so other processes can use it.

  	std::cerr << "Serial port closed.\n" << std::endl; // Provide feedback to ensure thi happened.
}
