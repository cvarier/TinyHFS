#include <FillPat.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include "TinyHFS.h"
#include <stdlib.h>

//need fixed root folder and condition that prohibits user from deleting it.
short     currentFolderAddress     =     FOLDER_PARTITION_LOWER_BOUND; 


/* Name        :    stepIn
 *  
 * Parameters  :    name of folder to step into. (this will be read by the
 *                  serial monitor
 *      
 * return      :    void
 * 
 * Description :    The user will input a folder name in the serial monitor
 *                  and this function will 'step into' it. All files and folders
 *                  created will now be linked to this new folder
 * 
 */

void stepIn(char *folderName) {

  int matches = 0;
  int index = 1;
  
  for (short i = FOLDER_PARTITION_LOWER_BOUND; i <= FOLDER_PARTITION_UPPER_BOUND; i += folderSize) {

    if (readByte(EEPROM_ADDRESS, i) == folderName[0]) {

      matches = 1;

      for (int j = i + 1; j < i + FOLDER_NAME_SIZE; j++) {

        matches = !readByte(EEPROM_ADDRESS, j) == folderName[index++] ? 0 : 1;

        if (!matches) break;
        
      }

    if (matches) {

      displayCurrentFolderContents(i); //need global variable to keep track of current folder's address.
      currentFolderAddress = i;
      
    } 
    } 
  }  
}

/* Name        :    stepOut
 *  
 * Parameters  :    none
 *      
 * return      :    void
 * 
 * Description :    The user will step back into the parent folder.
 */
 
void stepOut() {

  char parentFolderName[FOLDER_NAME_SIZE];
  int index = 0;

  short parentFolderAddress = (readByte(EEPROM_ADDRESS, currentFolderAddress + FOLDER_NAME_SIZE + 2) << 8) | readByte(EEPROM_ADDRESS, currentFolderAddress + FOLDER_NAME_SIZE + 2);

  for (int i = parentFolderAddress; i < parentFolderAddress + FOLDER_NAME_SIZE; i++) 
    parentFolderName[index++] = readByte(EEPROM_ADDRESS, i);

  displayCurrentFolderContents(parentFolderAddress);
  currentFolderAddress = parentFolderAddress;


  
}


void displayCurrentFolderContents(short currentFolderAddress) {

  short folderParentAddress;
  short fileParentAddress;
  char holder;

  Serial.println("Current Folder");
  for (int i = 1; i < 5; i++) 
    Serial.println(readByte(EEPROM_ADDRESS, i));

  Serial.print("*** ");
  
  for (int i = currentFolderAddress; i < currentFolderAddress + FOLDER_NAME_SIZE; i++) {
    
    holder = (char)readByte(EEPROM_ADDRESS, i);
    
    if (holder == '*') {}
    else
      Serial.print(holder); 
    
  } 

  Serial.println(" ***\n Folders:\n");

  for (int i = FOLDER_PARTITION_LOWER_BOUND + (FOLDER_NAME_SIZE + 3) + FOLDER_NAME_SIZE + 2; i <= FOLDER_PARTITION_UPPER_BOUND; i += FOLDER_NAME_SIZE + 4) {

      folderParentAddress = (readByte(EEPROM_ADDRESS, i) << 8) | readByte(EEPROM_ADDRESS, i + 1);

      if (folderParentAddress == currentFolderAddress) {

        for (int j = i - (FOLDER_NAME_SIZE + 2); j < i - 2; j++) {

          char holder;

          holder = (char)readByte(EEPROM_ADDRESS, j);
          if (holder == '*') {}
          else { Serial.print((char)readByte(EEPROM_ADDRESS, j)); }
          
        }
        
        Serial.print(" -- ");
        
      }
    
  }

  for (int i = FILE_HEADER_PARTITION_LOWER_BOUND + FILE_NAME_SIZE + 4; i <= FILE_HEADER_PARTITION_UPPER_BOUND; i += FILE_NAME_SIZE +6) {

      fileParentAddress = (readByte(EEPROM_ADDRESS, i) << 8) | readByte(EEPROM_ADDRESS, i + 1);

      if (fileParentAddress == currentFolderAddress) {

        for (int j = i - (FILE_NAME_SIZE + 4); j < i - 4; j++) {

          Serial.print((char)readByte(EEPROM_ADDRESS, j));
          
        }
        
        Serial.println(".txt");
        
      }
    
  }

    
/*  for (int i = fileHeaderStartAddress; i < fileHeaderStartAddress + FILE_NAME_SIZE; i++) {

    if ((char)readByte(EEPROM_ADDRESS, i) == '*') {}
    else
      Serial.println((char)readByte(EEPROM_ADDRESS, i)); 
    
  } */
}


// accepts a long string from the user and stores it in the eeprom

void acceptFile() {

  Serial.flush();

  Serial.println("Enter a string...");

  while(1) {
  //if switch gets turned while in function, return

  while (Serial.available() > 0 && !strRcvd) {

    char received = Serial.read();
    inData += received;
  
    if (received == '\n') {
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
      
      //format();

      //readFile(0, 0);
     return; 
    }
  }
  }
}

