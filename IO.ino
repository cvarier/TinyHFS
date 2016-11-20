#include <Wire.h>

static TwoWire orbitBoosterWire(0);

void InitIO() {

  orbitBoosterWire.begin();

}

void writeByteEEPROM(int deviceaddress, int eeaddress, char data) 
{
  orbitBoosterWire.beginTransmission(deviceaddress);
  orbitBoosterWire.write((int)(eeaddress >> 8));   // MSB
  orbitBoosterWire.write((int)(eeaddress & 0xFF)); // LSB
  orbitBoosterWire.write(data);
  orbitBoosterWire.endTransmission();
 
  delay(DELAY_TIME);
}

char readByteEEPROM(int deviceaddress, int eeaddress) 
{
  char rdata = 0xFF;
  
  orbitBoosterWire.beginTransmission(deviceaddress);
  orbitBoosterWire.write((int)(eeaddress >> 8));   // MSB
  orbitBoosterWire.write((int)(eeaddress & 0xFF)); // LSB
  orbitBoosterWire.endTransmission(0);
 
  orbitBoosterWire.requestFrom(deviceaddress, 1);
 
  int byteGot = 0;

  while (orbitBoosterWire.available() && !byteGot) {
	  if(orbitBoosterWire.available()) {
		  rdata = orbitBoosterWire.read();
		  byteGot = 1;
	  }
  }

  return rdata;

}

