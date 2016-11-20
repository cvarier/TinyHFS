#include "Defines.h"
#include <Wire.h>

void InitHFS () {

  Serial.begin(9600);
  InitIO();
   
}

void writeFile (char str_bytes[], int str_len, int startAddress) {

  double timeStart = millis()/1000.0;
  
  // Write data to EEPROM_ADDRESS_ADDRESS
  for(int i = startAddress; i < startAddress + str_len; i++) 
    writeByte(EEPROM_ADDRESS, i, str_bytes[i - startAddress]);
  
  double timeEnd = millis()/1000.0;
  
  Serial.print("\nDATA WRITE\nlength: ");
  Serial.print(str_len);
  Serial.println();
  Serial.println("FINISHED in ");
  Serial.print(timeEnd - timeStart);
  Serial.print(" s\n\n");
  
}

void readFile (int str_len, int startAddress) { 
  
  unsigned char str_bytes[str_len];

  Serial.println("DATA READ");

  double timeStart = millis()/1000.0;

  // Read the data byte-by-byte and store in str_bytes
  for(int i = startAddress; i < startAddress + str_len; i++)
    str_bytes[i - startAddress] = readByte(EEPROM_ADDRESS, i);

  double timeEnd = millis()/1000.0;

  // Print out the read data
  for(int i = 0; i < str_len; i++)
    Serial.print((char) str_bytes[i]);

  Serial.println();
  Serial.println("FINISHED in ");
  Serial.print(timeEnd - timeStart);
  Serial.print(" s\n\n");
  
}

// Formats (Clears) EEPROM by overwriting all data with the null character
void format () {

  double timeStart = millis()/1000.0;
  Serial.println("DATA FORMAT");

  for (int i = MIN_ADDRESS; i <= MAX_ADDRESS; i++)
    writeByte(EEPROM_ADDRESS, i, 0);

  double timeEnd = millis()/1000.0;

  Serial.println("FINISHED in ");
  Serial.print(timeEnd - timeStart);
  Serial.print(" s\n\n");

}

