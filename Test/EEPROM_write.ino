#include <Wire.h>
#include <string.h>
#define eeprom 0x54    //Address of 24LC256 eeprom chip
#define startAddress 0

static TwoWire orbitBoosterWire(0);
String inData;
int strRcvd = 0;
int str_len = 0;


void setup(void)
{
  Serial.begin(9600);
  orbitBoosterWire.begin();
  Serial.println("Enter a string...");
}

void loop() {
  
    while (Serial.available() > 0 && !strRcvd) {
        
        char recieved = Serial.read();
        inData += recieved; 

        // Process string when new line character is recieved
        if (recieved == '\n')
        {
          Serial.print("String received: ");
          strRcvd = 1;
  
          // Work out length of data
          int str_len = inData.length() - 1;
          char str_bytes[str_len];
        
          // Split data into an array of char arrays, each one byte long
          for(int j = 0; j < str_len; j++)
            str_bytes[j] = inData[j];
        
          Serial.println();

          // Print processed string
          for(int q = 0; q < str_len; q++)
            Serial.print(str_bytes[q]);
          
          // Write data to EEPROM
          for(int i = startAddress; i < startAddress + str_len; i++) writeByte(eeprom, i, str_bytes[i-startAddress]);
        
          Serial.print("\nDATA WRITTEN - length: ");
          Serial.print(str_len);
          Serial.println();
  
        }
    }
}

void writeByte(int deviceaddress, int eeaddress, char data ) 
{
  orbitBoosterWire.beginTransmission(deviceaddress);
  orbitBoosterWire.write((int)(eeaddress >> 8));   // MSB
  orbitBoosterWire.write((int)(eeaddress & 0xFF)); // LSB
  orbitBoosterWire.write(data);
  orbitBoosterWire.endTransmission();
 
  delay(5);
}
