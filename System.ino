#include "Defines.h"
#include <Wire.h>

short fileStartAddresses[maxFileHeaders] = { FILE_PARTITION_LOWER_BOUND };
short fileEndAddresses[maxFileHeaders] = { FILE_PARTITION_LOWER_BOUND };

int fileArrayIndex = 0;
int fileCount = 0;

void InitHFS() {

    Serial.begin(9600);
    InitIO();

    Serial.println("Initializing your TinyHFS...\n");

    /* The first byte in memory will indicate the number of files stored in the system.
     * This will be referred to as the file count byte.
     */
    fileCount = (int) readByte(EEPROM_ADDRESS, 0);

    /* The following code will obtain the start and end addresses of all files and their
     * corresponding parent folders
     */

    int fileIndex = FILE_HEADER_PARTITION_LOWER_BOUND + FILE_NAME_SIZE;
    short fileStartAddress = 0;
    short fileEndAddress = 0;

    for (int i = 0; i < fileCount; i++) {

        fileStartAddress = assembleShort(readByte(EEPROM_ADDRESS, fileIndex), readByte(EEPROM_ADDRESS, fileIndex + 1));

        fileEndAddress = assembleShort(readByte(EEPROM_ADDRESS, fileIndex + 2),
            readByte(EEPROM_ADDRESS, fileIndex + 3));

        fileIndex += fileHeaderSize;

        // Do nothing if empty space is read
        if (!(fileStartAddress || fileEndAddress))
            continue;

        fileStartAddresses[i] = fileStartAddress;
        fileEndAddresses[i] = fileEndAddress;

    }

    qsort(fileStartAddresses, fileCount, sizeof(short), compVals);
    qsort(fileEndAddresses, fileCount, sizeof(short), compVals);

    updateCurrentDirectory(ROOT_ADDRESS);

    Serial.println("\nWelcome to your TinyHFS!");
    Serial.println("\nIf you are new to the system or would like a refresher, enter \'help\' at any time.");

}

void writeFile(char file_text[], short file_size, short parentStartAddress, char *fileName) {

    double timeStart = millis() / 1000.0;

    short fileStartAddress = (fileCount == 0 ? FILE_PARTITION_LOWER_BOUND : fileEndAddresses[fileCount - 1] + 1);
    Serial.println("\nFILE WRITE INITIATED");

    if (fileStartAddress + file_size - 1 > FILE_PARTITION_UPPER_BOUND
        || !createFileHeader(fileStartAddress, parentStartAddress, file_size, fileName)) {

        Serial.println("\nERROR: File space full; cannot save file");

        // Prompt user to organize file partition. This will close gaps between files.

        /* Determine if there is free space now. If not, tell user to delete at least one file if they wish
         * to create a new file.
         */

        return;

    }

    for (int i = fileStartAddress; i <= fileStartAddress + file_size - 1; i++)
        writeByte(EEPROM_ADDRESS, i, file_text[i - fileStartAddress]);

    double timeEnd = millis() / 1000.0;

    Serial.print("\nFILE WRITTEN\n\nFILE LENGTH: ");
    Serial.print(file_size);
    Serial.print(" BYTES\n");
    Serial.println("FINISHED IN ");
    Serial.print(timeEnd - timeStart);
    Serial.print(" s\n");

    fileStartAddresses[fileCount] = fileStartAddress;
    fileEndAddresses[fileCount++] = fileStartAddress + file_size - 1;

    writeByte(EEPROM_ADDRESS, 0, ++fileCount);

}

void readFile(short fileStartAddress, short fileEndAddress) {

    short fileSize = fileEndAddress - fileStartAddress + 1;
    unsigned char str_bytes[fileSize];

    Serial.println("FILE READ INITIATED\n");

    double timeStart = millis() / 1000.0;

    // Read the data byte-by-byte and store in str_bytes
    for (int i = fileStartAddress; i <= fileEndAddress; i++)
        str_bytes[i - fileStartAddress] = readByte(EEPROM_ADDRESS, i);

    double timeEnd = millis() / 1000.0;

    // Print out the read data
    for (int i = 0; i < fileSize; i++)
        Serial.print((char) str_bytes[i]);

    Serial.print("\nFILE READ\n\nFILE LENGTH: ");
    Serial.print(fileSize);
    Serial.print(" BYTES\n");
    Serial.println("\nFINISHED IN ");
    Serial.print(timeEnd - timeStart);
    Serial.print(" s\n");

}

void createFolder(short parentStartAddress) {

    char parentStartAddressLow = getLowByte(parentStartAddress);
    char parentStartAddressHigh = getHighByte(parentStartAddress);

    char *folderName = getName(FOLDER_NAME_SIZE);

    short folderStartAddress = 0;
    int spaceCount = 0;

    for (int j = FOLDER_PARTITION_LOWER_BOUND; j <= FOLDER_PARTITION_UPPER_BOUND; j++) {

        spaceCount = (readByte(EEPROM_ADDRESS, j) == 0 ? spaceCount + 1 : 0);

        if (spaceCount == folderSize) {
            folderStartAddress = j - folderSize + 1;
            break;
        }

    }

    if (!folderStartAddress) {

        Serial.println("\nERROR: Folder space full; cannot create folder.");

        /* Tell user to delete at least one folder if they wish to create a new folder.
         */

        Serial.println("\nPlease delete at least one folder if you wish to create a new one.");

        return;

    }

    char folderStartAddressLow = getLowByte(folderStartAddress);
    char folderStartAddressHigh = getHighByte(folderStartAddress);

    for (int i = 0; i < FOLDER_NAME_SIZE; i++)
        writeByte(EEPROM_ADDRESS, folderStartAddress + i, folderName[i]);

    // Write start address of folder
    writeByte(EEPROM_ADDRESS, folderStartAddress + FOLDER_NAME_SIZE, folderStartAddressHigh);
    writeByte(EEPROM_ADDRESS, folderStartAddress + FOLDER_NAME_SIZE + 1, folderStartAddressLow);

    // Write start address of parent folder
    writeByte(EEPROM_ADDRESS, folderStartAddress + FOLDER_NAME_SIZE + 2, parentStartAddressHigh);
    writeByte(EEPROM_ADDRESS, folderStartAddress + FOLDER_NAME_SIZE + 3, parentStartAddressLow);

}

// Returns 1 if successful, else 0
int createFileHeader(short fileStartAddress, short parentStartAddress, short fileSize, char *fileName) {

    short fileEndAddress = fileStartAddress + fileSize - 1;

    char fileStartAddressLow = getLowByte(fileStartAddress);
    char fileStartAddressHigh = getHighByte(fileStartAddress);
    char fileEndAddressLow = getLowByte(fileEndAddress);
    char fileEndAddressHigh = getHighByte(fileEndAddress);

    char parentStartAddressLow = getLowByte(parentStartAddress);
    char parentStartAddressHigh = getHighByte(parentStartAddress);

    short fileHeaderStartAddress = 0;
    int spaceCount = 0;

    for (int j = FILE_HEADER_PARTITION_LOWER_BOUND; j <= FILE_HEADER_PARTITION_UPPER_BOUND; j++) {

        spaceCount = (readByte(EEPROM_ADDRESS, j) == 0 ? spaceCount + 1 : 0);

        if (spaceCount == fileHeaderSize) {

            fileHeaderStartAddress = j - fileHeaderSize + 1;
            break;

        }

    }

    if (!fileHeaderStartAddress) {

        Serial.println("\nERROR: File header space full; cannot store file");

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

void copyFile(short copyFrom, short copyTo, short fileSize) {

    // In order to copy, this function will be used in conjunction with write file
    for (int i = copyFrom; i < copyFrom + fileSize; i++)
        writeByte(EEPROM_ADDRESS, copyTo, readByte(EEPROM_ADDRESS, i));

}

void moveFile() {

}

void deleteFile(short fileHeaderStartAddress) {

    short fileAddressesLocInHeader = fileHeaderStartAddress + FILE_NAME_SIZE;

    short fileStartAddressHighByte = getHighByte(readByte(EEPROM_ADDRESS, fileAddressesLocInHeader));
    short fileStartAddressLowByte  = getLowByte(readByte(EEPROM_ADDRESS, fileAddressesLocInHeader + 1));
    short fileStartAddress = assembleShort(fileStartAddressHighByte, fileStartAddressLowByte);

    short fileEndAddressHighByte  = getLowByte(readByte(EEPROM_ADDRESS, fileAddressesLocInHeader + 2));
    short fileEndAddressLowByte  = getLowByte(readByte(EEPROM_ADDRESS, fileAddressesLocInHeader + 3));
    short fileEndAddress = assembleShort(fileEndAddressHighByte, fileEndAddressLowByte);

    for (int i = fileStartAddress; i <= fileEndAddress; i++)
        writeByte(EEPROM_ADDRESS, i, 0);

    /*
     * Check if the deleted file is located in the middle of the file array. If it is, then shift all the remaining
     * files over by one.
     */

    int mustShift = 0;

    for (int i = 0; i < fileCount; i++) {

        if (mustShift)
            fileStartAddresses[i - 1] = fileStartAddresses[i];

        if (fileStartAddresses[i] == fileStartAddress)
            mustShift = 1;

    }

    fileStartAddresses[fileCount - 1] = 0;

    writeByte(EEPROM_ADDRESS, 0, (char) --fileCount);

    // Overwrite the file header with null characters
    for (int i = fileHeaderStartAddress; i < fileHeaderStartAddress + fileHeaderSize; i++)
        writeByte(EEPROM_ADDRESS, i, 0);


}

void deleteFolder(short folderStartAddress) {

    int folderEndAddress = folderStartAddress + folderSize - 1;

    // Prohibit user from deleting root folder
    if (folderStartAddress == ROOT_ADDRESS) {

        Serial.println("Cannot modify root folder");
        return;

    }

    // Overwrite the folder with null characters
    for (int i = folderStartAddress; i <= folderEndAddress; i++)
        writeByte(EEPROM_ADDRESS, i, 0);

    /*
     * Scan the parent folder start address attribute of all the file headers (15th and 16th addresses),
     * then delete the file headers and files whose parent folder start address corresponds to the folder
     * being deleted.
     */
    for (int i = FILE_HEADER_PARTITION_LOWER_BOUND + FILE_NAME_SIZE + 4; i <= FILE_HEADER_PARTITION_UPPER_BOUND; i +=
        fileHeaderSize) {

        char parentStartAddressCurrHigh = readByte(EEPROM_ADDRESS, i);
        char parentStartAddressCurrLow = readByte(EEPROM_ADDRESS, i + 1);
        short parentStartAddressCurr = assembleShort(parentStartAddressCurrHigh, parentStartAddressCurrLow);

        if (folderStartAddress == parentStartAddressCurr) {

            short fileHeaderStartAddressCurr = i - FILE_NAME_SIZE - 4;

            deleteFile(fileHeaderStartAddressCurr);

        }

    }

}

// Formats (Clears) EEPROM by overwriting all data with the null character (takes approximately 3 minutes)
void format() {

    double timeStart = millis() / 1000.0;
    Serial.println("\nEEPROM FORMAT INITIATED");

    for (int i = FOLDER_PARTITION_LOWER_BOUND; i <= FILE_HEADER_PARTITION_UPPER_BOUND; i++)
        writeByte(EEPROM_ADDRESS, i, 0);

    double timeEnd = millis() / 1000.0;

    Serial.print("\nEEPROM FORMATTED\n\n");
    Serial.println("FINISHED IN ");
    Serial.print(timeEnd - timeStart);
    Serial.print(" s\n");

}

void rename() {

}

char *getName(int maxNameSize) {

    int strRcvd = 0;
    char *name = (char *) malloc(sizeof(char) * maxNameSize);
    char received;
    String inData;

    Serial.flush();

    while (1) {
      
        while (Serial.available() > 0 && !strRcvd) {
    
            received = Serial.read();
            inData += received;
    
            // Process string when new line character is received
            if (received == '\n') {
    
                strRcvd = 1;
    
                // Work out length of string
                int str_len = inData.length() - 1;
    
                if (str_len > maxNameSize) {
    
                    Serial.print("ERROR: Name too long; please try again.");
                    return getName(maxNameSize);
    
                }
    
                // Split string into an array of char arrays, each one byte long
                for (int j = 0; j < str_len; j++)
                    name[j] = inData[j];
    
            }

            return name;
    
        }
    
    }

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
    qsort(fileEndAddresses, fileCount, sizeof(short), compVals);

    int isOccupied = 1;
    int count = 0;

    do {

        isOccupied = readByte(EEPROM_ADDRESS, startAddress - count); //Needs checking for small byte files of just zeros

        if (!isOccupied)
            count++;

    } while (!isOccupied && startAddress - count >= FILE_PARTITION_LOWER_BOUND);

    while (startAddress <= endAddress) {

        writeByte(EEPROM_ADDRESS, startAddress - count, readByte(EEPROM_ADDRESS, startAddress));
        startAddress++;

    }

    fileStartAddresses[fileArrayIndex] -= count;
    fileEndAddresses[fileArrayIndex] -= count;

    if (fileArrayIndex >= fileCount)
        return;

    fileArrayIndex++;
    organizeMemory(fileStartAddresses[fileArrayIndex], fileEndAddresses[fileArrayIndex]);

    return;

}

int compVals(const void * a, const void * b) {

    return (*(int*) a - *(int*) b);

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
