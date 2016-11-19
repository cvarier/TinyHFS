#include <Wire.h>
#define eeprom 0x54    //Address of 24LC256 eeprom chip
#define str_len 500
#define startAddress 0

unsigned char str_bytes[str_len];
static TwoWire orbitBoosterWire(0);

void setup(void)
{
  Serial.begin(9600);
  orbitBoosterWire.begin();

  Serial.println("DATA READ");

  // Read the data byte-by-byte and store in string str_bytes
  for(int i = startAddress; i < startAddress + str_len; i++)
    str_bytes[i-startAddress] = readByte(eeprom, i);

  // Print out the read data
  for(int i = 0; i < str_len; i++)
    Serial.print((char) str_bytes[i]);

  Serial.println();

}
 
void loop(){
}
 
char readByte(int deviceaddress, int eeaddress) 
{
  char rdata = 0xFF;
  
  orbitBoosterWire.beginTransmission(deviceaddress);
  orbitBoosterWire.write((int)(eeaddress >> 8));   // MSB
  orbitBoosterWire.write((int)(eeaddress & 0xFF)); // LSB
  orbitBoosterWire.endTransmission();
 
  orbitBoosterWire.requestFrom(deviceaddress, 1);
 
  if(orbitBoosterWire.available()) rdata = orbitBoosterWire.read();

  return rdata;

}
