#include <Wire.h>

void InitIO() {

}

void writeByte(int deviceaddress, int eeaddress, char data, TwoWire wire) 
{
  wire.beginTransmission(deviceaddress);
  wire.write((int)(eeaddress >> 8));   // MSB
  wire.write((int)(eeaddress & 0xFF)); // LSB
  wire.write(data);
  wire.endTransmission();
 
  delay(5);
}

char readByte(int deviceaddress, int eeaddress, TwoWire wire) 
{
  char rdata = 0xFF;
  
  wire.beginTransmission(deviceaddress);
  wire.write((int)(eeaddress >> 8));   // MSB
  wire.write((int)(eeaddress & 0xFF)); // LSB
  wire.endTransmission();
 
  wire.requestFrom(deviceaddress, 1);
 
  if(wire.available()) rdata = wire.read();

  return rdata;

}

