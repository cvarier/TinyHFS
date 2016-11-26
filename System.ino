#include "Defines.h"
#include <Wire.h>
#include "TinyHFS.h"

/*
 *s
 */

 short fileStartAddresses[(FILE_HEADER_PARTITION_SIZE/(MAX_NAME_LENGTH + 8))] = {FILE_PARTITION_LOWER_BOUND};
 short fileEndAddresses[(FILE_HEADER_PARTITION_SIZE/(MAX_NAME_LENGTH + 8))] = {FILE_PARTITION_LOWER_BOUND};
 int fileArrayIndex = 0;
 int fileCount = (int)readByte(EEPROM_ADDRESS, 0);

int compVals (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}
 
void InitHFS () {

  Serial.begin(9600);
  InitIO();
   
  int increment = FILE_HEADER_PARTITION_LOWER_BOUND + 15; // this value will check each byte of memory that corresponds to the start address in the file header

  for (int i = 0; i < fileCount; i++) {

      fileStartAddresses[i] = (readByte(EEPROM_ADDRESS, increment) << 8) | (readByte(EEPROM_ADDRESS, increment + 1)); 
      fileEndAddresses[i] = (readByte(EEPROM_ADDRESS, increment + 2) << 8) | (readByte(EEPROM_ADDRESS, increment + 3)); 
      increment += 17;
      
  }

  qsort(fileStartAddresses, fileCount, sizeof(short), compVals);
  qsort(fileEndAddresses,   fileCount, sizeof(short), compVals);

  for (int i = 0; i < 5; i++) {

     Serial.print(fileStartAddresses[i]);
     Serial.print(fileEndAddresses[i]);
    
  }

  Serial.println((readByte(EEPROM_ADDRESS, FILE_HEADER_PARTITION_LOWER_BOUND + 34) << 8) | (readByte(EEPROM_ADDRESS,  FILE_HEADER_PARTITION_LOWER_BOUND + 35)));
  Serial.println((readByte(EEPROM_ADDRESS, FILE_HEADER_PARTITION_LOWER_BOUND + 42) << 8) | (readByte(EEPROM_ADDRESS,  FILE_HEADER_PARTITION_LOWER_BOUND + 43)));

  
      
}

void writeFile (char str_bytes[], int str_len) {

  double timeStart = millis()/1000.0;
  int spaceCount = 0;
  short startAddress = fileCount == 0 ? FILE_PARTITION_LOWER_BOUND : fileEndAddresses[fileCount - 1] + 1;


  /*  Find Folder Address */

  // short startAddess = (readByte(EEPROM_ADDRESS, NEXT_AVAIL_FOLDER_ADDRESS_MEM_LOC) << 8) | readByte(EEPROM_ADDRESS, NEXT_AVAIL_FOLDER_ADDRESS_MEM_LOC + 1)

  /* for (int j = FILE_PARTITION_LOWER_BOUND; j <= FILE_PARTITION_UPPER_BOUND; j++) {

    spaceCount = readByte(EEPROM_ADDRESS, j) == 0 ? spaceCount++ : 0;

    if (spaceCount == str_len) { startAddress = j - spaceCount + 1 ; break; }22
    
  } */

  if (startAddress + str_len > FILE_PARTITION_UPPER_BOUND || !createFileHeader(startAddress, str_len)) { 

    for (int i = 0; i < str_len; i++)
      Serial.println(str_bytes[i]);

    Serial.println();
    
    Serial.println( "File space full. Cannot store file. Try organizing memory" ); 
    
    return; 
    
  }

  
  
  
  
  // Write data to EEPROM_ADDRESS
  for(int i = startAddress; i < startAddress + str_len; i++) 
    writeByte(EEPROM_ADDRESS, i, str_bytes[i - startAddress]);
  
  double timeEnd = millis()/1000.0;
  
  Serial.print("\nDATA WRITE\nlength: ");
  Serial.print(str_len);
  Serial.println();
  Serial.println("FINISHED in ");
  Serial.print(timeEnd - timeStart);
  Serial.print(" s\n\n");

  writeByte(EEPROM_ADDRESS, 0, ++fileCount);
  
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

void createFolder () {

    //char          name[10]; <-- not needed
  short           dateCreated;
  short           dateLastModified;
  short         folderStartAddress;
  // short         stopAddress;      Only needed if varibale length
  short         parentStartAddress;
  // short         parentEndAddress  Only needed if variable length

  
  int           nameIndex       =       0;
  int           byteCount       =       0;
  int           written         =       0;
  int           spaceCount      =       0;
  
  int startAddress = 0;

  //short startAddess = (readByte(EEPROM_ADDRESS, NEXT_AVAIL_FOLDER_ADDRESS_MEM_LOC) << 8) | readByte(EEPROM_ADDRESS, NEXT_AVAIL_FOLDER_ADDRESS_MEM_LOC + 1)

  for (int j = FOLDER_PARTITION_LOWER_BOUND; j <= FOLDER_PARTITION_UPPER_BOUND; j++) {

    spaceCount = readByte(EEPROM_ADDRESS, j) == 0 ? spaceCount + 1 : 0;

    if (spaceCount == 18) { startAddress = j - 17 ; break; }
    
  }

  if (!startAddress) { Serial.println("Folder space full. Cannot create folder"); return; }
  

  for (int i = 0; i < MAX_NAME_LENGTH + 8; i++) {
    
//    if (byteCount++ < MAX_NAME_LENGTH) { writeByte(EEPROM_ADDRESS, startAddress + i, getName()[nameIndex++]); continue; }
    if (byteCount++ < MAX_NAME_LENGTH + 2) { writeByte(EEPROM_ADDRESS, startAddress + i, dateCreated); continue; }
    if (byteCount++ < MAX_NAME_LENGTH + 4) { writeByte(EEPROM_ADDRESS, startAddress + i, dateLastModified); continue; }
    if (byteCount++ < MAX_NAME_LENGTH + 6) { writeByte(EEPROM_ADDRESS, startAddress + i, folderStartAddress); continue; }
    if (byteCount++ < MAX_NAME_LENGTH + 8) { writeByte(EEPROM_ADDRESS, startAddress + i, parentStartAddress); continue; }

  }

}

int createFileHeader(short fileStartAddress, int fileSize) {

  
//char          name[10]; <-- not needed
  short           dateCreated;
  short           dateLastModified;
  //short         headerStartAddress;
  // short         stopAddress;      Only needed if varibale length
  short         parentStartAddress;
  // short         parentEndAddress  Only needed if variable length

  
  int           nameIndex       =       0;
  int           byteCount       =       0;
  int           spaceCount      =       0;


  
  int startAddress = 0;

  //short startAddess = (readByte(EEPROM_ADDRESS, NEXT_AVAIL_FOLDER_ADDRESS_MEM_LOC) << 8) | readByte(EEPROM_ADDRESS, NEXT_AVAIL_FOLDER_ADDRESS_MEM_LOC + 1)

  for (int j = FILE_HEADER_PARTITION_LOWER_BOUND; j <= FILE_HEADER_PARTITION_UPPER_BOUND; j++) {

    spaceCount = readByte(EEPROM_ADDRESS, j) == 0 ? spaceCount + 1 : 0;

    if (spaceCount == 18) { startAddress = j - 17 ; break; }
    
  }

  if (!startAddress) { Serial.println("File space full. Cannot save file"); return 0; }

  

  for (int i = 0; i < MAX_NAME_LENGTH + 8; i++){

//      if (byteCount++ < MAX_NAME_LENGTH) { writeByte(EEPROM_ADDRESS, startAddress + i, getName()[nameIndex++]); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 2) { writeByte(EEPROM_ADDRESS, startAddress + i, dateCreated); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 4) { writeByte(EEPROM_ADDRESS, startAddress + i, dateLastModified); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 6) { writeByte(EEPROM_ADDRESS, startAddress + i, startAddress); continue; }
      if (byteCount++ < MAX_NAME_LENGTH + 8) { writeByte(EEPROM_ADDRESS, startAddress + i, parentStartAddress); continue; }
      
    }



  return 1;

  
}

  



void copyFile (short copyFrom, short copyTo, short fileSize) {

 /* for (int i = copyTo; i < copyTo + fileSize; i++) {

    if (readByte(EEPROM_ADDRESS, i) != 0)


    
  }*/

  for (int i = copyFrom; i < copyFrom + fileSize; i++) writeByte(EEPROM_ADDRESS, copyTo, readByte(EEPROM_ADDRESS, i));

}

// Formats (Clears) EEPROM by overwriting all data with the null character (takes approximately 3 minutes)
void format () {

  double timeStart = millis()/1000.0;
  Serial.println("DATA FORMAT");

  for (int i = FOLDER_PARTITION_LOWER_BOUND; i <= MAX_ADDRESS; i++)
    writeByte(EEPROM_ADDRESS, i, 0);

  double timeEnd = millis()/1000.0;

  Serial.println("FINISHED in ");
  Serial.print(timeEnd - timeStart);
  Serial.print(" s\n\n");

}

/*char *getName(void) {

  char        name[MAX_NAME_LENGTH];
  short       nameIndex = 0;
  int isBreak = 0;
  
  while (!isBreak) {
  
    while (Serial.read() != '\n') {
    
      if (nameIndex >= MAX_NAME_LENGTH) { 
        
      Serial.print("Name too long");
      for (int i = 0; i <= MAX_NAME_LENGTH; i++) name[i] = 0; // <----- Is this needed?
      getName();
      
      }
      
      name[nameIndex] = Serial.read();   
      nameIndex++;
      int isBreak = 1;
      
    }
    
  }
  return name;

}*/

int searchFileHeaders(short searchParam) {

  short scannedAddress; 

  for (int i = FILE_HEADER_PARTITION_UPPER_BOUND + MAX_NAME_LENGTH + 5; i <= FILE_HEADER_PARTITION_UPPER_BOUND; i += MAX_NAME_LENGTH + 7) {

    scannedAddress = (readByte(EEPROM_ADDRESS, i) << 8) | (readByte(EEPROM_ADDRESS, i + 1)); 
    if (searchParam == scannedAddress)  return i - (MAX_NAME_LENGTH + 5);

  }

  return 0;

}


void organizeMemory(short startAddress, short endAddress) {

  int isOccupied = 1;
  int count = 0;

  do {
    
    isOccupied = readByte(EEPROM_ADDRESS, startAddress - count);         //Needs checking for small byte files of just zeros

    if (!isOccupied) count++;

  } while (!isOccupied && startAddress - count >= FILE_PARTITION_LOWER_BOUND);

  while (startAddress <= endAddress) 
  
    writeByte(EEPROM_ADDRESS, startAddress++ - count, readByte(EEPROM_ADDRESS, startAddress));

  fileStartAddresses[fileArrayIndex] -= count;
  fileEndAddresses[fileArrayIndex] -= count;

  if (fileArrayIndex >= fileCount) return;

  organizeMemory(fileStartAddresses[++fileArrayIndex], fileEndAddresses[fileArrayIndex]);

  return;
  
}

