#include "Defines.h"
#include <stdlib.h>
#include <OrbitBoosterPackDefs.h>
#include <FillPat.h>
#include <LaunchPad.h>
#include <Wire.h>
#include "Energia.h"
#include "TinyHFS.h"

//git test

char *getName(void);

String inData;
int strRcvd = 0;
 //Needs Changing

 void initHFS();

void setup () {

  InitHFS();
  Serial.println("Enter a string...");
  Serial.print(sizeof(String));
  
}




void loop () {

  while (Serial.available() > 0 && !strRcvd) {
      
    char received = Serial.read();
    inData += received; 
  
    // Process string when new line character is recieved
    if (received == '\n') {
      
      Serial.print("\nString received: ");
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

      Serial.println();



      /*
       * 
       *  Need to implement stop condition for overlapping files in the case of writing to previously used space.
       *  if end address >= next file's start address then dont write to free space
       *  
       *  How do we know the adjacent file's starting address?
       * 
       * 
       * 
       */

      //Checks to see if any space has been freed due to deleting files; concatenates bytes into one number
      short nextAvailableFileAddress = (readByte(EEPROM_ADDRESS, NEXT_AVAIL_FILE_ADDRESS_MEM_LOC) << 8) | readByte(EEPROM_ADDRESS, NEXT_AVAIL_FILE_ADDRESS_MEM_LOC + 1);

      short latestFreedFileSpace = 
      
        latestFreedFileSpaceAddress == FREE_FILE_SPACE_MEM_LOC_LOW ?//|| isInUse() ?

        0 : 
     
        (readByte(EEPROM_ADDRESS, latestFreedFileSpaceAddress - 4) << 8) | readByte(EEPROM_ADDRESS, latestFreedFileSpaceAddress - 3);
        

      if (!latestFreedFileSpace) {

        writeFile(str_bytes, str_len, nextAvailableFileAddress);
        nextAvailableFileAddress += str_len;
        writeByte(EEPROM_ADDRESS, NEXT_AVAIL_FILE_ADDRESS_MEM_LOC, nextAvailableFileAddress >> 8);
        writeByte(EEPROM_ADDRESS, NEXT_AVAIL_FILE_ADDRESS_MEM_LOC + 1, nextAvailableFileAddress & 0xFF);
       
      } else {

        writeFile(str_bytes, str_len, latestFreedFileSpace);
        latestFreedFileSpaceAddress -= 4;
        
        
      }
    
        
             
      //readFile(str_len, 0);

      //format();

      //readFile(100, 0);
      
    }
  }
}


short isInUse(short startAddress) {

 // loop through file headers and check range of files.

  
  
}
