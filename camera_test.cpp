#include <unistd.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <raspicam/raspicam.h>
using namespace std;

int main(){
  raspicam::RaspiCam Camera;
  if(!Camera.open()){
    cerr << "Error opening camera!" << endl;;
    return -1;
  }
  sleep(3);
  cout << "CHEESE!" << endl;
  //capture
  Camera.grab();
  unsigned char *data = new unsigned char[Camera.getImageTypeSize(
					  raspicam::RASPICAM_FORMAT_RGB)];
  //extract image
  Camera.retrieve(data, raspicam::RASPICAM_FORMAT_RGB);
  std::ofstream output("imag_test.ppm",std::ios::binary);
  output << "P6\n" << Camera.getWidth() << " " << Camera.getHeight()
	    << " 255\n";
  output.write( (char*) data, Camera.getImageTypeSize(raspicam::
						      RASPICAM_FORMAT_RGB));
  cout << "Image saved, test complete" << endl;
  Camera.release();
  //free!
  delete data;
  return 0;
}
