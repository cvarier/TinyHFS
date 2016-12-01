#include <FillPat.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include "Defines.h"
#include <stdlib.h>

short currentFolderStartAddress = ROOT_ADDRESS;
short currentContainedFolderStartAddresses[maxFolders] = { 0 };
short currentContainedFileHeaderStartAddresses[maxFileHeaders] = { 0 };

/**
 * Function: stepIn
 *
 * Allows the user to step down a level in the file hierarchy into the specified folder. All files and folders
 * created will now be linked to this folder.
 *
 * @Param folderName the name of the folder to step into
 */
void stepIn(short folderStartAddress) {

    currentFolderStartAddress = folderStartAddress;
    updateCurrentDirectory(currentFolderStartAddress);

}

/**
 * Function: stepOut
 *
 * Allows the user to step up a level in the file hierarchy into the specified folder. All files and folders
 * created will now be linked to this folder.
 */
void stepOut() {

    short parentFolderStartAddressHigh = readByte(EEPROM_ADDRESS, currentFolderStartAddress + FOLDER_NAME_SIZE + 2);
    short parentFolderStartAddressLow = readByte(EEPROM_ADDRESS, currentFolderStartAddress + FOLDER_NAME_SIZE + 3);
    short parentFolderStartAddress = assembleShort(parentFolderStartAddressHigh, parentFolderStartAddressLow);

    if (parentFolderStartAddress == ROOT_ADDRESS) {

        Serial.println("ERROR: Cannot step out of root");
        return;

    }

    updateCurrentDirectory(parentFolderStartAddress);

    displayFolderContent();
    currentFolderStartAddress = parentFolderStartAddress;

}

/**
 * Function: displayFolderContents
 *
 * Displays the contents of the current folder via the 'ls' command.
 *
 * @Param folderStartAddress the start address of the current folder
 */
void displayFolderContent() {

    /*
     * Obtain and print the folders that belong to the current folder.
     */

    for (int i = 0; i < maxFolders; i++) {

        for (int j = 0; j < FOLDER_NAME_SIZE; j++) {

            if (currentContainedFolderStartAddresses[i])
                Serial.print((char) readByte(EEPROM_ADDRESS, currentContainedFolderStartAddresses[i] + j));

        }

        Serial.println();

    }

    /*
     * Obtain and print the files that belong to the current folder.
     */

    for (int i = 0; i < fileCount; i++) {

        for (int j = 0; j < FILE_NAME_SIZE; j++) {

            if (currentContainedFileHeaderStartAddresses[i])
                Serial.print((char) readByte(EEPROM_ADDRESS, currentContainedFileHeaderStartAddresses[i] + j));

        }

        Serial.println(".txt");
    }

}

/**
 * Function: parseCommand
 *
 * Parses the current command entered by the user and executes it accordingly.
 *
 * @Param command the command that the user entered
 */
void parseCommand(char *command) {

    /*
     * Comparisons sorted in ascending order by length of command name to maximize efficiency.
     */
    if (!strcmp(command, folder_step_in)) {

        char *folderName = getName(FILE_NAME_SIZE);
        stepIn(findStartAddressFromName(folderName, "folder"));
        free(folderName);

    } else if (!strcmp(command, folder_step_out)) {

        stepOut();

    } else if (!strcmp(command, print_dir)) {

        printCurrentDirectory();

    } else if (!strcmp(command, file_write)) {

        Serial.println("\nPlease enter a file name");
        char *fileName = getName(FILE_NAME_SIZE);

        if (findStartAddressFromName(fileName, "file")) {

            Serial.println("\nERROR: The file already exists.");
            return;

        }

        Serial.println("\nPlease enter your text. When you are finished, enter the '`' character.");
        acceptFile(fileName);
        free(fileName);

    } else if (!strcmp(command, print_help)) {

        printHelp();

    } else if (!strcmp(command, file_read)) {

        Serial.println("\nPlease enter a file name");
        char *fileName = getName(FILE_NAME_SIZE);
        short fileHeaderStartAddress = findStartAddressFromName(fileName, "file");

        if (!fileHeaderStartAddress) {

            Serial.println("\nERROR: The file could not be found.");
            return;

        }

        short fileStartAddressHigh = readByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE);
        short fileStartAddressLow = readByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE + 1);
        short fileStartAddress = assembleShort(fileStartAddressHigh, fileStartAddressLow);

        short fileEndAddressHigh = readByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE + 2);
        short fileEndAddressLow = readByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE + 3);
        short fileEndAddress = assembleShort(fileEndAddressHigh, fileEndAddressLow);

        readFile(fileStartAddress, fileEndAddress);
        free(fileName);

    } else if (!strcmp(command, folder_create)) {

        Serial.println("\nPlease enter a folder name");
        char *folderName = getName(FOLDER_NAME_SIZE);

        if (findStartAddressFromName(folderName, "folder")) {

            Serial.println("\nERROR: The folder already exists.");
            return;

        }

        createFolder(currentFolderStartAddress);
        free(folderName);

    } else if (!strcmp(command, folder_move)) {

    } else if (!strcmp(command, folder_delete)) {

        Serial.println("\nPlease enter a folder name");
        char *folderName = getName(FOLDER_NAME_SIZE);
        short folderStartAddress = findStartAddressFromName(folderName, "folder");

        if (!folderStartAddress) {

            Serial.println("\nERROR: The folder does not exist.");
            return;

        }

        deleteFolder(folderStartAddress);
        free(folderName);

    } else if (!strcmp(command, file_move)) {

    } else if (!strcmp(command, format_sys)) {

        format();

    } else if (!strcmp(command, folder_copy)) {

    } else if (!strcmp(command, file_copy)) {

    } else if (!strcmp(command, file_delete)) {

        Serial.println("\nPlease enter a file name");
        char *fileName = getName(FILE_NAME_SIZE);
        short fileHeaderStartAddress = findStartAddressFromName(fileName, "file");

        if (!fileHeaderStartAddress) {

            Serial.println("\nERROR: The file could not be found.");
            return;

        }

        deleteFile(fileHeaderStartAddress);

    }

}

/**
 * Function: updateCurrentDirectory
 *
 * Updates the contained file and folder address arrays according to the current working directory.
 *
 * @Param parentStartAddress the start address of the parent folder
 */
void updateCurrentDirectory(short parentStartAddress) {

    int folderIndex = 0;

    // Update folders
    for (int i = FOLDER_PARTITION_LOWER_BOUND + FOLDER_NAME_SIZE + 2; i <= FOLDER_PARTITION_UPPER_BOUND; i +=
        folderSize) {

        short currStartAddressHigh = readByte(EEPROM_ADDRESS, i);
        short currStartAddressLow = readByte(EEPROM_ADDRESS, i + 1);
        short currStartAddress = assembleShort(currStartAddressHigh, currStartAddressLow);

        if (currStartAddress == parentStartAddress) {
            currentContainedFolderStartAddresses[folderIndex] = currStartAddress;
            folderIndex++;
        }

    }

    int fileHeaderIndex = 0;

    // Update file headers
    for (int i = FILE_HEADER_PARTITION_LOWER_BOUND; i <= FILE_HEADER_PARTITION_UPPER_BOUND; i += fileHeaderSize) {

        short currStartAddressHigh = readByte(EEPROM_ADDRESS, i);
        short currStartAddressLow = readByte(EEPROM_ADDRESS, i + 1);
        short currStartAddress = assembleShort(currStartAddressHigh, currStartAddressLow);

        if (currStartAddress == parentStartAddress) {
            currentContainedFileHeaderStartAddresses[fileHeaderIndex] = currStartAddress;
            fileHeaderIndex++;
        }

    }

}

/**
 * Function: printCurrentDirectory
 *
 * Prints the current working directory.
 */
void printCurrentDirectory() {

    String directory = "";

    for (int i = currentFolderStartAddress + FOLDER_NAME_SIZE - 1; i >= currentFolderStartAddress; i--)
              directory = (char)readByte(EEPROM_ADDRESS, i) + directory;

    directory = getDirectory(directory, currentFolderStartAddress);

    Serial.println();
    Serial.println(directory);

}

String getDirectory(String directory, short currentAddress) {

    short parentFolderAddressLoc = currentAddress + FOLDER_NAME_SIZE + 2;
    short parentFolderAddress = assembleShort(readByte(EEPROM_ADDRESS, parentFolderAddressLoc),
        readByte(EEPROM_ADDRESS, parentFolderAddressLoc + 1));

    directory = "/" + directory;

    for (int i = parentFolderAddress + FOLDER_NAME_SIZE - 1; i >= parentFolderAddressLoc; i--)
        directory = (char)readByte(EEPROM_ADDRESS, i) + directory;

    if (parentFolderAddress = ROOT_ADDRESS) {
        directory = "E:/" + directory;
        return directory;
    }

    return getDirectory(directory, parentFolderAddress);

}

/**
 * Function: printHelp
 *
 * Prints the list of commands and what they do.
 *
 */
void printHelp() {

    Serial.println("*****COMMANDS LIST*****");
    Serial.println("\nfile -> Creates and stores new text file with a given file name.");
    Serial.println("read -> Displays the contents of a specified file.");
    Serial.println("delfile -> Deletes the specified file");
    Serial.println("cpyfile -> Copies a specified file to a specified path.");
    Serial.println("mvfile -> Moves a specified file to a specified path.\n");

    Serial.println("mkdir -> Creates new folder with a given directory name.");
    Serial.println("in -> Steps into a specified folder, down a level.");
    Serial.println("out -> Steps up to the parent folder.");
    Serial.println("delfol -> Deletes the specified folder");
    Serial.println("cpyfol -> Copies a specified folder to a specified path.");
    Serial.println("mvfol -> Moves a specified folder to a specified path.\n");

    Serial.println("format -> Clears the entire file system");
    Serial.println("pwd -> Displays the current working directory");

}

/**
 * Function: findStartAddressFromName
 *
 * Returns the start address of the specified file header or folder within the current directory.
 * Returns 0 if start address was not found.
 *
 * @Param searchFor the name of the file/folder
 * @Param specifier can be either "file" or "folder, and indicates whether the given name is for a file or folder
 */
short findStartAddressFromName(char *searchFor, const char *specifier) {

int numIters;
int nameSize;
short arraySize;
int matches = 0;

if (strcmp(specifier, "folder")) {

    numIters = maxFolders;
    nameSize = FOLDER_NAME_SIZE;
    arraySize = maxFolders;

}

if (strcmp(specifier, "file")) {

    numIters = maxFileHeaders;
    nameSize = FILE_NAME_SIZE;
    arraySize = maxFileHeaders;

}

short *arrayOfAddresses = (short *) malloc(sizeof(short) * arraySize);

if (arraySize == maxFolders) {

    for (int i = 0; i < maxFolders; i++)
        arrayOfAddresses[i] = currentContainedFolderStartAddresses[i];

} else if (arraySize == maxFileHeaders) {

    for (int i = 0; i < maxFolders; i++)
        arrayOfAddresses[i] = currentContainedFileHeaderStartAddresses[i];

}

for (int i = 0; i < numIters; i++) {

    char charToComp = (char) readByte(EEPROM_ADDRESS, arrayOfAddresses[i]);

    if (charToComp == searchFor[0]) {

        for (int j = 0; j < nameSize; j++) {

            charToComp = (char) readByte(EEPROM_ADDRESS, arrayOfAddresses[i] + j);
            if (charToComp != searchFor[j])
                matches = 0;

        }

        if (matches) {

            short retAddress = arrayOfAddresses[i];
            free(arrayOfAddresses);
            return retAddress;

        }
    }

}

free(arrayOfAddresses);
return 0;

}

/**
 * Function: writeFileToDirectory
 *
 * Writes the file with a given file name to the current directory
 *
 * @Param fileName the name of the file
 */
void acceptFile(char *fileName) {

/*
 * TODO TODO TODO
 * Check if file already exists within current directory. If not, continue as normal. If it does, prompt user
 * for overwrite. If yes, then write the file to the end of the files and update the file header's attributes.
 * If not, then return.
 */

int strRcvd = 0;
Serial.flush();

while (Serial.available() > 0 && !strRcvd) {

    char received = Serial.read();
    inData += received;

    if (received == fileTerminator) {

        strRcvd = 1;

        // Work out length of data
        int str_len = inData.length() - 1;
        char str_bytes[str_len];

        // Split data into an array of char arrays, each one byte long
        for (int j = 0; j < str_len; j++)
            str_bytes[j] = inData[j];

        Serial.println();

        short currentParentAddressHigh = readByte(EEPROM_ADDRESS, currentFolderStartAddress + FOLDER_NAME_SIZE + 2);
        short currentParentAddressLow = readByte(EEPROM_ADDRESS, currentFolderStartAddress + FOLDER_NAME_SIZE + 3);
        short currentParentAddress = assembleShort(currentParentAddressHigh, currentParentAddressLow);

        writeFile(str_bytes, str_len, currentParentAddress, fileName);

    }

}

}
