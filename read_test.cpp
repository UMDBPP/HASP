#include <cstdio>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include "RS232.h"
using namespace std;

int main(){
  //open file
  ofstream file;
  file.open("read_test.txt", ios::app);
  //setup USB from port
  BPP::RS232Serial usb;
  /*
    DOUBLE CHECK THIS! FIND PROPER PORT (/dev/tty...) AND BOD RATE
  */
  int portOpen = usb.portOpen("/dev/ttyACM0", B57600, 8, 'N', 1);
  if(portOpen != 0){
    cerr << "Error opening USB" << endl;
    return -1;
  }
  string input = "";
  int x = 0;
  while(x < 5){
    if(file.is_open()){
      int rcvdData = usb.rxData();
      //get data
      if(rcvdData > 0){
	input = usb.getData();
	printf(input.c_str());
	file << input << endl;
	usb.portFlush();
      }
      file.flush();
      x++;
      usleep(500);
    }
  }
  usb.portClose();
  file.close();
  return 0;
}
