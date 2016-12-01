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
void displayFolderContent () {

    /*
     * Obtain and print the folders that belong to the current folder.
     */

    for (int i = 0; i < maxFolders; i++) {

        for(int j = 0; j < FOLDER_NAME_SIZE; j++) {

            if (currentContainedFolderStartAddresses[i])
                Serial.print((char)readByte(EEPROM_ADDRESS, currentContainedFolderStartAddresses[i] + j));

        }

        Serial.println();

    }

    /*
     * Obtain and print the files that belong to the current folder.
     */

    for (int i = 0; i < fileCount; i++) {

        for (int j = 0; j < FILE_NAME_SIZE; j++) {

            if (currentContainedFileHeaderStartAddresses[i])
                Serial.print((char)readByte(EEPROM_ADDRESS, currentContainedFileHeaderStartAddresses[i] + j));

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

        stepIn(findStartAddressFromName(getName(FOLDER_NAME_SIZE), "folder"));

    } else if (!strcmp(command, folder_step_out)) {

        stepOut();

    } else if (!strcmp(command, file_write)) {

        Serial.println("\nPlease enter a file name");
        char *fileName = getName(FILE_NAME_SIZE);

        free(fileName);

    } else if (!strcmp(command, file_read)) {


    } else if (!strcmp(command, folder_create)) {


    } else if (!strcmp(command, folder_move)) {


    } else if (!strcmp(command, file_move)) {


    } else if (!strcmp(command, format_sys)) {


    } else if (!strcmp(command, folder_copy)) {


    } else if (!strcmp(command, file_copy)) {


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
    for (int i = FOLDER_PARTITION_LOWER_BOUND + FOLDER_NAME_SIZE + 2; i <= FOLDER_PARTITION_UPPER_BOUND;
        i += folderSize) {

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
    for (int i = FILE_HEADER_PARTITION_LOWER_BOUND; i <= FILE_HEADER_PARTITION_UPPER_BOUND;
        i += fileHeaderSize) {

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
 * Prints the current working director
 * - Specified via the 'pwd' command
 *
 * @Param folderStartAddress the start address of the current folder
 */
void printCurrentDirectory(short folderStartAddress) {

}

/**
 * Function: writeFileToDirectory
 *
 * Writes the file with a given file name to the current directory
 *
 * @Param fileName the name of the file
 */
void writeFileToDirectory(char *fileName) {

    /*
     * Check if file already exists within current directory. If not, continue as normal. If it does, prompt user
     * for overwrite. If yes, then write the file to the end of the files and update the file header's attributes.
     * If not, then return.
     */

    //    for (int i = FILE_HEADER_PARTITION_LOWER_BOUND; i <= FILE_HEADER_PARTITION_UPPER_BOUND); i++

}

/**
 * Function: findStartAddressFromName
 *
 * Returns the start address of the specified file or folder within the current directory.
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

    short *arrayOfAddresses = (short *) malloc(sizeof(short)*arraySize);

    if (arraySize == maxFolders) {

        for (int i = 0; i < maxFolders; i++)
            arrayOfAddresses[i] = currentContainedFolderStartAddresses[i];

    } else if (arraySize == maxFileHeaders) {

        for (int i = 0; i < maxFolders; i++)
            arrayOfAddresses[i] = currentContainedFileHeaderStartAddresses[i];

    }

    for (int i = 0; i < numIters; i++) {

        char charToComp = (char)readByte(EEPROM_ADDRESS, arrayOfAddresses[i]);

        if (charToComp == searchFor[0]) {


            for (int j = 0; j < nameSize; j++) {

                charToComp = (char)readByte(EEPROM_ADDRESS, arrayOfAddresses[i] + j);
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

    return -1;

}
