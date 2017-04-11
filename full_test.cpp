#include <cstdio>
#include <fstream>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <raspicam/raspicam.h>
#include "RS232.h"

using namespace std;

int main(){
  //open file
  ofstream file;
  file.open("full_test.txt", ios::app);
  if(!file.is_open()){
    cerr << "Error opening file" << endl;
    return -2;
  }
  //open usb line
  BPP::RS232Serial usb;
  int portOpen = usb.portOpen("/dev/ttyACM0", B57600, 8, 'N', 1);
  if(portOpen != 0){
    cerr << "Error opening usb line" << endl;
    return -1;
  }
  //open camera
  raspicam::RaspiCam Camera;
  if(!Camera.open()){
    cerr << "Error Opening camera" << endl;
    return -3;
  }
  //sleep for 3 sec to let camera init
  sleep(3);
  //init variables
  string input = "";
  int x = 0;
  unsigned char *data = new unsigned char[Camera.getImageTypeSize(raspicam::
						 RASPICAM_FORMAT_RGB)];
  while(x < 50){
    //get data from usb line (every 500 msecs)
    int rcvdData = usb.rxData();
    if(rcvdData > 0) {
      input = usb.getData();
      cout << input << endl;
      file << input << endl;
      usb.portFlush();
    }
    file.flush();
    //take a picture (every 5 secs)
    if(x%10 == 0){
      Camera.grab();
      int i = x/10;
      string begining = "imag_test";
      string number = (char *) i;
      string end = ".ppm";
      string pictureName = begining + number + end;
      Camera.retrieve(data, raspicam::RASPICAM_FORMAT_RGB);
      std::ofstream output(pictureName, std::ios::binary);
      output << "P6\n" << Camera.getWidth() << " " << Camera.getHeight() <<
	" 255/n";
      output.write( (char*) data, Camera.getImageTypeSize(raspicam::
							  RASPICAM_FORMAT_RGB));
      Camera.release();
    }
    usleep(500000);
  }
  //close and free
  delete data;
  usb.portClose();
  file.close();
  return 0;
}
