#include "Defines.h"
#include <stdlib.h>
#include <OrbitBoosterPackDefs.h>
#include <FillPat.h>
#include <LaunchPad.h>
#include <Wire.h>
#include "Energia.h"

String inData = "";
int strRcvd = 0;

void setup() {

    InitHFS();

}

void loop() {

    int strRcvd = 0;

    while (Serial.available() > 0 && !strRcvd) {

        char received = Serial.read();
        inData += received;

        if (received == '\n') {

            strRcvd = 1;

            // Work out length of data
            int str_len = inData.length();
            char str_bytes[str_len];

            // Split data into an array of char arrays, each one byte long
            for (int j = 0; j < str_len - 1; j++)
                str_bytes[j] = inData[j];

            str_bytes[str_len - 1] = '\0';

            parseCommand(str_bytes);

            Serial.println();

            inData = "";
            Serial.flush();

        }

    }


}
