#include "Defines.h"
#include <stdlib.h>
#include <OrbitBoosterPackDefs.h>
#include <FillPat.h>
#include <LaunchPad.h>
#include <Wire.h>
#include "Energia.h"
#include "TinyHFS.h"

String inData;
int strRcvd = 0;

void setup () {

  InitHFS();
  Serial.println("Enter a string...");
  
}

void loop () {

  while (Serial.available() > 0 && !strRcvd) {
      
    char received = Serial.read();
    inData += received;
  
    // Process string when escape key is pressed
      
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

      Serial.println("\n");

      writeFile(str_bytes, str_len);

      readFile(str_len, 0);

      //format();

      //readFile(0, 0);
      
    }
  }
}
