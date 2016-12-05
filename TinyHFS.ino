#include "Defines.h"
#include <stdlib.h>
#include <OrbitBoosterPackDefs.h>
#include <FillPat.h>
#include <LaunchPad.h>
#include <Wire.h>
#include "Energia.h"


int strRcvd = 0;
String data = "";

void setup() {

    InitHFS();

}

void loop() {
  
    
    int strRcvd = 0;

    while (Serial.available() > 0 && !strRcvd) {

        char received = Serial.read();
        data += received;

        if (received == '\n') {

            strRcvd = 1;

            // Work out length of data
            int str_len = data.length();
            char str_bytes[str_len];

            // Split data into an array of char arrays, each one byte long
            for (int j = 0; j < str_len - 1; j++)
                str_bytes[j] = data[j];

            str_bytes[str_len - 1] = '\0';

            // Echo the user's input
            Serial.println(">");
            Serial.print(data);
            Serial.println();

            parseCommand(str_bytes);

            Serial.println();

            data = "";
            Serial.flush();

        }

    }

}
