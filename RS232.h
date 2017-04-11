/* Link Telemetry v0.3 "Yankee Clipper"
   
   Copyright (c) 2015-2016 University of Maryland Space Systems Lab
   NearSpace Balloon Payload Program
   
   Written by Nicholas Rossomando
   2015-04-06

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

   RS232.h:

   Serial reading class. Replaces GPL licensed and not very friendly old library.
*/

#ifndef RS232_H
#define RS232_H

#include <string>

extern "C" {
	#include <termios.h>
	#include <unistd.h>
}

namespace BPP {

class RS232Serial {

	private:
		int port; // Com port file handle, because POSIX file interaction.
		struct termios originalPortConfig; // Store the original port settings for restore.
		struct termios portConfig; // termios settings structure for new settings.
		std::string data; // string to store serial data.

	public:
		RS232Serial(); // Default CTOR
		~RS232Serial(); // DTOR

		int portOpen(std::string _comPort, int _baudRate, int _dataBits, char _parity, int _stopBits); // Initialize to selected serial port.
		int rxData(); // Recieve data from serial port
		void portFlush(); // Clear data in the recieve buffer.
		void portClose(); // Close the serial port.

		std::string getData() const { return data; } // Data getter.

}; // RS232Serial

} // BPP

#endif
