#include "Defines.h"
#include <Wire.h>
#include "TinyHFS.h"

int fileHeaderPartitionSize = FILE_HEADER_PARTITION_UPPER_BOUND - FILE_HEADER_PARTITION_LOWER_BOUND + 1;
int maxFileHeaders = fileHeaderPartitionSize/(FILE_NAME_SIZE + 6);
int folderSize = FOLDER_NAME_SIZE + 4;
int fileHeaderSize = FILE_NAME_SIZE + 6;

short fileStartAddresses[maxFileHeaders] = {FILE_PARTITION_LOWER_BOUND};
short fileEndAddresses[maxFileHeaders] = {FILE_PARTITION_LOWER_BOUND};
short parentStartAddresses[maxFileHeaders] = {FOLDER_PARTITION_LOWER_BOUND};
int fileArrayIndex = 0;

 // The first byte in memory will indicate the number of files stored in the system
 int fileCount = (int)readByte(EEPROM_ADDRESS, 0);

int compVals (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

void InitHFS () {

  Serial.begin(9600);
  InitIO();

  /* The following code will obtain the start and end addresses of all files and their
   * corresponding parent folders
   */

  int fileIndex = FILE_HEADER_PARTITION_LOWER_BOUND + FILE_NAME_SIZE;
  short fileStartAddress = 0;
  short fileEndAddress = 0;
  short parentStartAddress = 0;

  for (int i = 0; i < fileCount; i++) {

	  fileStartAddress = assembleShort(readByte(EEPROM_ADDRESS, fileIndex),
			  readByte(EEPROM_ADDRESS, fileIndex + 1);

	  fileEndAddress = assembleShort(readByte(EEPROM_ADDRESS, fileIndex + 2),
			  readByte(EEPROM_ADDRESS, fileIndex + 3);

	  parentStartAddress = assembleShort(readByte(EEPROM_ADDRESS, fileIndex + 4),
			  readByte(EEPROM_ADDRESS, fileIndex + 5);

      fileIndex += fileHeaderSize;

	  // Do nothing if empty space is read
	  if (!(fileStartAddress || fileEndAddress)) continue;

      fileStartAddresses[i] = fileStartAddress;
      fileEndAddresses[i] = fileEndAddress;
      parentStartAddresses[i] = parentStartAddress;

  }

  /*
  for (int i = 0; i < 5; i++) {

     Serial.print(fileStartAddresses[i]);
     Serial.print(fileEndAddresses[i]);

  }

  Serial.println((readByte(EEPROM_ADDRESS, FILE_HEADER_PARTITION_LOWER_BOUND + 34) << 8) | (readByte(EEPROM_ADDRESS,  FILE_HEADER_PARTITION_LOWER_BOUND + 35)));
  Serial.println((readByte(EEPROM_ADDRESS, FILE_HEADER_PARTITION_LOWER_BOUND + 42) << 8) | (readByte(EEPROM_ADDRESS,  FILE_HEADER_PARTITION_LOWER_BOUND + 43)));
  */


}

void writeFile (char str_bytes[], short str_len) {

  double timeStart = millis()/1000.0;
  int spaceCount = 0;

  short fileStartAddress = (fileCount == 0 ? FILE_PARTITION_LOWER_BOUND : fileEndAddresses[fileCount - 1] + 1);
  short parentStartAddress =
    (fileCount == 0 ? FOLDER_PARTITION_LOWER_BOUND : parentStartAddresses[fileCount - 1]);

  Serial.println("FILE WRITE INITIATED");

  /*  Find Folder Address */

  // short startAddess = (readByte(EEPROM_ADDRESS, NEXT_AVAIL_FOLDER_ADDRESS_MEM_LOC) << 8) | readByte(EEPROM_ADDRESS, NEXT_AVAIL_FOLDER_ADDRESS_MEM_LOC + 1)

  /* for (int j = FILE_PARTITION_LOWER_BOUND; j <= FILE_PARTITION_UPPER_BOUND; j++) {
    spaceCount = readByte(EEPROM_ADDRESS, j) == 0 ? spaceCount++ : 0;
    if (spaceCount == str_len) { startAddress = j - spaceCount + 1 ; break; }22

  } */

  if (fileStartAddress + str_len - 1 > FILE_PARTITION_UPPER_BOUND ||
    !createFileHeader(fileStartAddress, parentStartAddress, str_len)) {

	Serial.println("ERROR: File space full; cannot save file");

    // Prompt user to organize file partition. This will close gaps between files.

    /* Determine if there is free space now. If not, tell user to delete at least one file if they wish
     * to create a new file.
     */

    return;

  }

  for(int i = fileStartAddress; i <= fileStartAddress + str_len - 1; i++)
    writeByte(EEPROM_ADDRESS, i, str_bytes[i - fileStartAddress]);
  
  double timeEnd = millis()/1000.0;
  
  Serial.print("\nFILE WRITTEN\n\nFILE LENGTH: ");
  Serial.print(str_len);
  Serial.print(" BYTES\n")
  Serial.println("FINISHED IN ");
  Serial.print(timeEnd - timeStart);
  Serial.print(" s\n\n");

  writeByte(EEPROM_ADDRESS, 0, ++fileCount);
  
}

void readFile (short fileStartAddress, short fileEndAddress) {
  
  short fileSize = fileEndAddress - fileStartAddress + 1;
  unsigned char str_bytes[fileSize];

  Serial.println("FILE READ INITIATED\n");

  double timeStart = millis()/1000.0;

  // Read the data byte-by-byte and store in str_bytes
  for(int i = fileStartAddress; i <= fileEndAddress; i++)
    str_bytes[i - fileStartAddress] = readByte(EEPROM_ADDRESS, i);

  double timeEnd = millis()/1000.0;

  // Print out the read data
  for(int i = 0; i < fileSize; i++)
    Serial.print((char) str_bytes[i]);

  Serial.print("\nFILE READ\n\nFILE LENGTH: ");
  Serial.print(fileSize);
  Serial.print(" BYTES\n")
  Serial.println("\nFINISHED IN ");
  Serial.print(timeEnd - timeStart);
  Serial.print(" s\n\n");
  
}

// Returns 1 if successful, else 0
int createFolder (short parentAddress) {

  char parentStartAddressLow = getLowByte(parentStartAddress);
  char parentStartAddressHigh = getHighByte(parentStartAddress);

  char *folderName = getName(FOLDER_NAME_SIZE);

  short folderStartAddress = 0;
  int spaceCount = 0;

  for (int j = FOLDER_PARTITION_LOWER_BOUND; j <= FOLDER_PARTITION_UPPER_BOUND; j++) {

    spaceCount = readByte(EEPROM_ADDRESS, j) == 0 ? ++spaceCount : 0;

    if (spaceCount == folderSize) { folderStartAddress = j - folderSize + 1; break; }
    
  }

  if (!folderStartAddress) {

	Serial.println("ERROR: Folder space full; cannot create folder.");

    /* Tell user to delete at least one folder if they wish to create a new folder.
     */

    return 0;

  }
  
  char folderStartAddressLow = getLowByte(folderStartAddress);
  char folderStartAddressHigh = getHighByte(folderStartAddress);

  for (int i = 0; i < FOLDER_NAME_SIZE; i++)
	  writeByte(EEPROM_ADDRESS, folderStartAddress + i, fileName[i]);

  // Write start address of folder
  writeByte(EEPROM_ADDRESS, folderStartAddress + FOLDER_NAME_SIZE, folderStartAddressHigh);
  writeByte(EEPROM_ADDRESS, folderStartAddress + FOLDER_NAME_SIZE + 1, folderStartAddressLow);

  // Write start address of parent folder
  writeByte(EEPROM_ADDRESS, folderStartAddress + FOLDER_NAME_SIZE + 2, parentStartAddressHigh);
  writeByte(EEPROM_ADDRESS, folderStartAddress + FOLDER_NAME_SIZE + 3, parentStartAddressLow);

  return 1;

}


// Returns 1 if successful, else 0
int createFileHeader(short fileStartAddress, short parentStartAddress, short fileSize) {

  short fileEndAddress = fileStartAddress + fileSize - 1;

  char fileStartAddressLow = getLowByte(fileStartAddress);
  char fileStartAddressHigh = getHighByte(fileStartAddress);
  char fileEndAddressLow = getLowByte(fileEndAddress);
  char fileEndAddressHigh = getHighByte(fileEndAddress);

  char parentStartAddressLow = getLowByte(parentStartAddress);
  char parentStartAddressHigh = getHighByte(parentStartAddress);

  short fileHeaderStartAddress = 0;
  int spaceCount = 0;

  char *fileName = getName(FILE_NAME_SIZE);

  for (int j = FILE_HEADER_PARTITION_LOWER_BOUND; j <= FILE_HEADER_PARTITION_UPPER_BOUND; j++) {

    spaceCount = readByte(EEPROM_ADDRESS, j) == 0 ? ++spaceCount : 0;

    if (spaceCount == fileHeaderSize) { fileHeaderStartAddress = j - fileHeaderSize + 1; break; }
    
  }

  if (!fileHeaderStartAddress) {

    Serial.println("ERROR: File header space full; cannot store file");

    // Prompt user to organize file partition. This will close gaps between files.

    /* Determine if there is free space now. If not, tell user to delete at least one folder if they wish
     * to create a new file.
     */

    return 0;

  }

  for (int i = 0; i < FILE_NAME_SIZE; i++)
	  writeByte(EEPROM_ADDRESS, fileHeaderStartAddress + i, fileName[i]);

  // Write start address of file
  writeByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE, fileStartAddressHigh);
  writeByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE + 1, fileStartAddressLow);

  // Write end address of file
  writeByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE + 2, fileEndAddressHigh);
  writeByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE + 3, fileEndAddressLow);

  // Write start address of parent folder
  writeByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE + 4, parentStartAddressHigh);
  writeByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE + 5, parentStartAddressLow);

  return 1;

}

void copyFile (short copyFrom, short copyTo, short fileSize) {

  // In order to copy, this function will be used in conjunction with write file

  for (int i = copyFrom; i < copyFrom + fileSize; i++) writeByte(EEPROM_ADDRESS, copyTo, readByte(EEPROM_ADDRESS, i));

}

// Formats (Clears) EEPROM by overwriting all data with the null character (takes approximately 3 minutes)
void format () {

  double timeStart = millis()/1000.0;
  Serial.println("EEPROM FORMAT INITIATED");

  for (int i = FOLDER_PARTITION_LOWER_BOUND; i <= FILE_HEADER_PARTITION_UPPER_BOUND; i++)
    writeByte(EEPROM_ADDRESS, i, 0);

  double timeEnd = millis()/1000.0;

  Serial.print("\nEEPROM FORMATTED\n\n");
  Serial.println("FINISHED IN ");
  Serial.print(timeEnd - timeStart);
  Serial.print(" s\n\n");

}

char *getName(int maxNameSize) {

  int strRcvd = 0
  char name[maxNameSize];
  char received;
  String inData;
  
  
  while (Serial.available() > 0 && !strRcvd) {

    char recieved = Serial.read();
	inData += recieved;

	// Process string when new line character is recieved
	if (recieved == '\n') {

	  strRcvd = 1;

      // Work out length of string
      int str_len = inData.length() - 1;
    
      if (str_len > maxNameSize) {
        
        Serial.print("ERROR: Name too long; please try again.");
        getName(maxNameSize);
      
      }

      // Split string into an array of char arrays, each one byte long
      for(int j = 0; j < str_len; j++)
        name[j] = inData[j];

	}

  }
    
  return name;

}

/*
int searchFileHeaders(short searchParam) {

  short scannedAddress;

  for (int i = FILE_HEADER_PARTITION_LOWER_BOUND + MAX_NAME_LENGTH + 5;
		  i <= FILE_HEADER_PARTITION_UPPER_BOUND; i += MAX_NAME_LENGTH + 7) {

    scannedAddress = (readByte(EEPROM_ADDRESS, i) << 8) | (readByte(EEPROM_ADDRESS, i + 1));
    if (searchParam == scannedAddress)  return i - (MAX_NAME_LENGTH + 5);

  }

  return 0;

}
*/

void organizeMemory(short startAddress, short endAddress) {

  qsort(fileStartAddresses, fileCount, sizeof(short), compVals);
  qsort(fileEndAddresses,   fileCount, sizeof(short), compVals);

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

char getLowByte(short num) {

    return (char) (num & 0xff);

}

char getHighByte(short num) {

	return (char) (num >> 8 & 0xff);

}

short assembleShort(char highByte, char lowByte) {

	return (short) (lowByte | highByte << 8);

}
