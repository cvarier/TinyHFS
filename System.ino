#include "Defines.h"
#include <Wire.h>
#include "TinyHFS.h"

short       lastFolderAddress         =       FOLDER_LOWER_BOUND;
short       lastFileHeaderAddress     =       FILE_HEADER_LOWER_BOUND;
int         freeSpaceIndex            =       FREE_FILE_HEADER_SPACE_MEM_LOC_LOW;
//folderNode 



/*
 *  Persistent Variable:          Mem Range:
 * 
 *  lastFolderAddress             0 - 1  
 *  nextAvailableFileAddress               1 - 2
 *  freeFileSpace                 3 - 500
 *  freeFolderSpace               501 - 700
 *  freeFileHeaderSpace           701 - 900
 * 
 */



void InitHFS () {

  Serial.begin(9600);
  InitIO();
  
   
}

void writeFile (char str_bytes[], int str_len, short startAddress) {
  
  short         fileSize          =         0;
  char          name[10];
  short         nameIndex         =         0;
  double        timeStart         =         millis()/1000.0;
  
  if (Serial.available()) {
       
    // Write data to EEPROM_ADDRESS
    for(int i = startAddress; i < startAddress + str_len; i++) {
      writeByte(EEPROM_ADDRESS, i, str_bytes[i - startAddress]);
      fileSize++;
    }

    createFileHeader(getName(), fileSize);
  
    double timeEnd = millis()/1000.0;
  
    Serial.print("\nDATA WRITE\nlength: ");
    Serial.print(str_len);
    Serial.println();
    Serial.println("FINISHED in ");
    Serial.print(timeEnd - timeStart);
    Serial.print(" s\n\n");

  }
  
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

// Formats (Clears) EEPROM by overwriting all data with the null character (takes approximately 3 minutes)   *** This isnt really needed; we only need to reset the relevent address that we save in our first partition
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

void createFolder(int subLevel) { // sublevel 0 is root


  //char          name[10]; <-- not needed
  short           dateCreated;
  short           dateLastModified;
  short         startAddress;
  // short         stopAddress;      Only needed if varibale length
  short         parentStartAddress;
  // short         parentEndAddress  Only needed if variable length

  
  int           nameIndex       =       0;
  int           byteCount       =       0;
  

  short nextAvailableFolderAddress = (readByte(EEPROM_ADDRESS, NEXT_AVAIL_FOLDER_ADDRESS_MEM_LOC) << 8) | readByte(EEPROM_ADDRESS, NEXT_AVAIL_FOLDER_ADDRESS_MEM_LOC + 1);

  short latestFreedFolderSpace = 
      
    latestFreedFolderSpaceAddress == FREE_FOLDER_SPACE_MEM_LOC_LOW ?

    0 : 
     
    (readByte(EEPROM_ADDRESS, latestFreedFolderSpaceAddress - 2) << 8) | readByte(EEPROM_ADDRESS, latestFreedFolderSpaceAddress - 1);
        

  if (!latestFreedFolderSpace) {

    for (int i = 0; i < MAX_NAME_LENGTH + 8; i++){
      
      if (byteCount++ < MAX_NAME_LENGTH) { writeByte(EEPROM_ADDRESS, nextAvailableFolderAddress + i, getName()[nameIndex++]); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 2) { writeByte(EEPROM_ADDRESS, nextAvailableFolderAddress + i, dateCreated); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 4) { writeByte(EEPROM_ADDRESS, nextAvailableFolderAddress + i, dateLastModified); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 6) { writeByte(EEPROM_ADDRESS, nextAvailableFolderAddress + i, startAddress); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 8) { writeByte(EEPROM_ADDRESS, nextAvailableFolderAddress + i, parentStartAddress); continue; }
      
    }
    
    nextAvailableFolderAddress += MAX_NAME_LENGTH + 8;
    writeByte(EEPROM_ADDRESS, NEXT_AVAIL_FOLDER_ADDRESS_MEM_LOC, nextAvailableFolderAddress >> 8);
    writeByte(EEPROM_ADDRESS, NEXT_AVAIL_FOLDER_ADDRESS_MEM_LOC + 1, nextAvailableFolderAddress & 0xFF);
       
  } else {

    for (int i = 0; i < MAX_NAME_LENGTH + 8; i++){

      if (byteCount++ < MAX_NAME_LENGTH) { writeByte(EEPROM_ADDRESS, latestFreedFolderSpace + i, getName()[nameIndex++]); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 2) { writeByte(EEPROM_ADDRESS, latestFreedFolderSpace + i, dateCreated); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 4) { writeByte(EEPROM_ADDRESS, latestFreedFolderSpace + i, dateLastModified); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 6) { writeByte(EEPROM_ADDRESS, latestFreedFolderSpace + i, startAddress); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 8) { writeByte(EEPROM_ADDRESS, latestFreedFolderSpace + i, parentStartAddress); continue; }
      
    }
    
    latestFreedFolderSpaceAddress -= 2;
               
  }






/*
  Serial.println("Enter Folder Name");

  if (Serial.available()) {

    /*
     * 
     *    ADD DATE INFO
     * 
     *
     *
     *

     // if the latest freed space is equal to the lower bound, return the rightmost folder address, else return the previously freed address stored in memory
     // i.e if previusly used memory has been free, store there before appending on the "right side"

  short latestFreedFolderSpace = 
  
    latestFreedFolderSpaceAddress == FREE_FOLDER_SPACE_MEM_LOC_LOW ?
                                                                    
    (readBytes(EEPROM_ADDRESS, LATEST_FOLDER_ADDRESS_MEM_LOC) << 8) | readByte(EEPROM_ADDRESS, LATEST_FOLDER_ADDRESS_MEM_LOC) :
    
    (readByte(EEPROM_ADDRESS, latestFreedFolderSpaceAddress - 4) << 8) | readByte(EEPROM_ADDRESS, latestFreedFolderSpaceAddress - 3);


  for (int i = 0; i < MAX_NAME_LENGTH + 8; i++){

    if (byteCount < MAX_NAME_LENGTH) { writeByte(EEPROM_ADDRESS, latestFreedFolderSpace + i, getName()[nameIndex++]); continue; }
    if (byteCount < MAX_NAME_LENGTH + 2) { writeByte(EEPROM_ADDRESS, latestFreedFolderSpace + i, dateCreated); continue; }
    if (byteCount < MAX_NAME_LENGTH + 4) { writeByte(EEPROM_ADDRESS, latestFreedFolderSpace + i, dateLastModified); continue; }
    if (byteCount < MAX_NAME_LENGTH + 6) { writeByte(EEPROM_ADDRESS, latestFreedFolderSpace + i, startAddress); continue; }
    if (byteCount < MAX_NAME_LENGTH + 8) { writeByte(EEPROM_ADDRESS, latestFreedFolderSpace + i, parentStartAddress); continue; }
      
  }

  if (latestFreedFolderSpace == lastFolderAddress) {
    lastFolderAddress += MAX_NAME_LENGTH + 8;  // maybe try to implement variable folder sizes -- still need to implement persistence
    
    // stores the last 
    writeByte(EEPROM_ADDRESS, LAST_FOLDER_ADDRESS_MEM_LOC, lastFolderAddress >> 8);     //Most significant Bit first
    writeByte(EEPROM_ADDRESS, LAST_FOLDER_ADDRESS_MEM_LOC + 1, lastFolderAddress & 0xFF);  //Going to need a sort/organize memory function

  }
    */
}




//Write null character to create "free" space; stores space in persistent array that may be accessed when space is needed
void folderDelete(short startAddress) {
  
  
  


  
  
  

}


void createFileHeader(char *name, short fileSize) {

//char          name[10]; <-- not needed
  short           dateCreated;
  short           dateLastModified;
  short         startAddress;
  // short         stopAddress;      Only needed if varibale length
  short         parentStartAddress;
  // short         parentEndAddress  Only needed if variable length

  
  int           nameIndex       =       0;
  int           byteCount       =       0;
  

  short nextAvailableFileHeaderAddress = (readByte(EEPROM_ADDRESS, NEXT_AVAIL_FILE_HEADER_ADDRESS_MEM_LOC) << 8) | readByte(EEPROM_ADDRESS, NEXT_AVAIL_FILE_HEADER_ADDRESS_MEM_LOC + 1);

  short latestFreedFileHeaderSpace = 
      
    latestFreedFileHeaderSpaceAddress == FREE_FOLDER_SPACE_MEM_LOC_LOW ?

    0 : 
     
    (readByte(EEPROM_ADDRESS, latestFreedFileHeaderSpaceAddress - 2) << 8) | readByte(EEPROM_ADDRESS, latestFreedFileHeaderSpaceAddress - 1);
        

  if (!latestFreedFileHeaderSpace) {

    for (int i = 0; i < MAX_NAME_LENGTH + 8; i++){

      if (byteCount++ < MAX_NAME_LENGTH) { writeByte(EEPROM_ADDRESS, nextAvailableFileHeaderAddress + i, getName()[nameIndex++]); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 2) { writeByte(EEPROM_ADDRESS, nextAvailableFileHeaderAddress + i, dateCreated); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 4) { writeByte(EEPROM_ADDRESS, nextAvailableFileHeaderAddress + i, dateLastModified); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 6) { writeByte(EEPROM_ADDRESS, nextAvailableFileHeaderAddress + i, startAddress); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 8) { writeByte(EEPROM_ADDRESS, nextAvailableFileHeaderAddress + i, parentStartAddress); continue; }
      
    }
    
    nextAvailableFileHeaderAddress += MAX_NAME_LENGTH + 8;
    writeByte(EEPROM_ADDRESS, NEXT_AVAIL_FILE_HEADER_ADDRESS_MEM_LOC, nextAvailableFileHeaderAddress >> 8);
    writeByte(EEPROM_ADDRESS, NEXT_AVAIL_FILE_HEADER_ADDRESS_MEM_LOC + 1, nextAvailableFileHeaderAddress & 0xFF);
       
  } else {

    for (int i = 0; i < MAX_NAME_LENGTH + 8; i++){

      if (byteCount++ < MAX_NAME_LENGTH) { writeByte(EEPROM_ADDRESS, latestFreedFileHeaderSpace + i, getName()[nameIndex++]); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 2) { writeByte(EEPROM_ADDRESS, latestFreedFileHeaderSpace + i, dateCreated); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 4) { writeByte(EEPROM_ADDRESS, latestFreedFileHeaderSpace + i, dateLastModified); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 6) { writeByte(EEPROM_ADDRESS, latestFreedFileHeaderSpace + i, startAddress); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 8) { writeByte(EEPROM_ADDRESS, latestFreedFileHeaderSpace + i, parentStartAddress); continue; }
      
    }
    
    latestFreedFileHeaderSpaceAddress -= 2;
 





/*
  //char          name[10]; <-- not needed
  short           dateCreated;
  short           dateLastModified;
  short         startAddress;
  // short         stopAddress;      Only needed if varibale length
  short         parentStartAddress;
  // short         parentEndAddress  Only needed if variable length

  
  int           nameIndex       =       0;
  int           byteCount       =       0;
  

  Serial.println("Enter Folder Name");

  if (Serial.available()) {

    /*
     * 
     *    ADD DATE INFO
     * 
     *

    for (int i = 0; i < MAX_NAME_LENGTH + 8; i++){

      if (byteCount < MAX_NAME_LENGTH)     { writeByte(EEPROM_ADDRESS, nextAvailableFileHeaderAddress + i, getName()[nameIndex++]); continue; }
      if (byteCount < MAX_NAME_LENGTH + 2) { writeByte(EEPROM_ADDRESS, nextAvailableFileHeaderAddress + i, dateCreated); continue; }
      if (byteCount < MAX_NAME_LENGTH + 4) { writeByte(EEPROM_ADDRESS, nextAvailableFileHeaderAddress + i, dateLastModified); continue; }
      if (byteCount < MAX_NAME_LENGTH + 6) { writeByte(EEPROM_ADDRESS, nextAvailableFileHeaderAddress + i, startAddress); continue; }
      if (byteCount < MAX_NAME_LENGTH + 8) { writeByte(EEPROM_ADDRESS, nextAvailableFileHeaderAddress + i, parentStartAddress); continue; }
      
    }

    nextAvailableFileHeaderAddress += MAX_NAME_LENGTH + 8;  // maybe try to implement variable folder sizes -- still need to implement persistence
    
  }

 */ 
}
}

void saveSystemData() {

  

}

char *getName(void) {

  char        name[MAX_NAME_LENGTH];
  short       nameIndex = 0;
  

  while (Serial.read() != '\n') {
    
      if (nameIndex >= MAX_NAME_LENGTH) { 
        
      Serial.print("Name too long");
      for (int i = 0; i <= MAX_NAME_LENGTH; i++) name[i] = 0; // <----- Is this needed?
      getName();
      
      }
      
      name[nameIndex] = Serial.read();   
      nameIndex++;
      
    }

  return name;

}

void deleteFile(short startAddress, short endAddress) {

  for (int i = 0; i < endAddress - startAddress + 1; i++)    //<--- possibly not needed
    writeByte(EEPROM_ADDRESS, startAddress + i, 0);

  writeByte(EEPROM_ADDRESS, latestFreedFileSpaceAddress, startAddress >> 8);
  writeByte(EEPROM_ADDRESS, latestFreedFileSpaceAddress + 1, startAddress & 0xFF);       // <--- Code needs trimming, leaving as is for now for readability
  writeByte(EEPROM_ADDRESS, latestFreedFileSpaceAddress + 2, endAddress >> 8);
  writeByte(EEPROM_ADDRESS, latestFreedFileSpaceAddress + 3, endAddress & 0xFF);

  latestFreedFileSpaceAddress += 4;


  // Delete file header

  short isFileHeader;

  short newFileHeaderSpace;
  
  for (int i = FILE_HEADER_LOWER_BOUND + MAX_NAME_LENGTH + 6; i < FILE_HEADER_UPPER_BOUND; i += MAX_NAME_LENGTH + 8) {

      isFileHeader = (readByte(EEPROM_ADDRESS, i - 2) << 8) | readByte(EEPROM_ADDRESS, i - 1);

      if (isFileHeader == startAddress){
        newFileHeaderSpace = i - (MAX_NAME_LENGTH + 6);
        break;
      }

    writeByte(EEPROM_ADDRESS, latestFreedFileHeaderSpaceAddress, newFileHeaderSpace >> 8);
    writeByte(EEPROM_ADDRESS, latestFreedFileHeaderSpaceAddress + 1, newFileHeaderSpace & 0xFF);
    latestFreedFileHeaderSpaceAddress += 2;
    
  
  }

  

}

  


