#include <FillPat.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include "Defines.h"
#include <stdlib.h>

short currentFolderStartAddress = ROOT_ADDRESS;
short currentContainedFolderStartAddresses[maxFolders] = { 0 };
short currentContainedFileStartAddresses[maxFileHeaders] = { 0 };

/**
 * Function: stepIn
 *
 * Allows the user to step down a level in the file hierarchy into the specified folder. All files and folders
 * created will now be linked to this folder.
 *
 * @Param folderName the name of the folder to step into
 */
void stepIn(char *folderName) {

    // Update the contents of the contained folder and file arrays

    updateCurrentDirectory(currentFolderStartAddress);

    int matches = 0;
    int index = 1;

    // TODO: FIX: what if folders have same name in hierarchy: IF same level, must throw error
    //                                                       ELSE must pass by address instead
    //                   SOLUTION: obtain arrays of all folders and files in current directory

    for (short i = FOLDER_PARTITION_LOWER_BOUND; i <= FOLDER_PARTITION_UPPER_BOUND; i += folderSize) {

        if (readByte(EEPROM_ADDRESS, i) == folderName[0]) {

            matches = 1;

            for (int j = i + 1; j < i + FOLDER_NAME_SIZE; j++) {

                if (!(readByte(EEPROM_ADDRESS, j) == folderName[index])) {

                    matches = 0;
                    break;

                }

                index++;

            }

            if (matches) {

                displayFolderContents(i);
                currentFolderStartAddress = i;

            }

        }

    }

}

/**
 * Function: stepOut
 *
 * Allows the user to step up a level in the file hierarchy into the specified folder. All files and folders
 * created will now be linked to this folder.
 */
void stepOut() {

    char parentFolderName[FOLDER_NAME_SIZE];
    int index = 0;

    short parentFolderAddress = (readByte(EEPROM_ADDRESS, currentFolderStartAddress + FOLDER_NAME_SIZE + 2) << 8)
        | readByte(EEPROM_ADDRESS, currentFolderStartAddress + FOLDER_NAME_SIZE + 2);

    for (int i = parentFolderAddress; i < parentFolderAddress + FOLDER_NAME_SIZE; i++)
        parentFolderName[index++] = readByte(EEPROM_ADDRESS, i);

    displayFolderContents(parentFolderAddress);
    currentFolderStartAddress = parentFolderAddress;

}

/**
 * Function: displayFolderContents
 *
 * Displays the contents of the current folder via the 'ls' command.
 *
 * @Param folderStartAddress the start address of the current folder
 */
void displayFolderContents(short folderStartAddress) {

    short startParentAddress;
    short fileParentAddress;

    /*
     * Obtain and print the folders that belong to the parent folder of the current folder.
     */
    for (int i = FOLDER_PARTITION_LOWER_BOUND + 1;
        i <= FOLDER_PARTITION_UPPER_BOUND;
        i += FOLDER_NAME_SIZE + 4) {

        startParentAddress = (readByte(EEPROM_ADDRESS, i) << 8) | readByte(EEPROM_ADDRESS, i + 1);

        if (startParentAddress == folderStartAddress) {

            for (int j = i - (FOLDER_NAME_SIZE + 2); j < i - 2; j++)

                Serial.print((char) readByte(EEPROM_ADDRESS, j));

        }

        Serial.print(" -- ");

    }

    /*
     * Obtain and print the files that belong to the parent folder of the current folder.
     */
    for (int i = FILE_HEADER_PARTITION_LOWER_BOUND + FILE_NAME_SIZE + 4; i <= FILE_HEADER_PARTITION_UPPER_BOUND;
        i += FILE_NAME_SIZE + 6) {

        fileParentAddress = (readByte(EEPROM_ADDRESS, i) << 8) | readByte(EEPROM_ADDRESS, i + 1);

        if (fileParentAddress == folderStartAddress) {

            for (int j = i - (FILE_NAME_SIZE + 4); j < i - 4; j++) {

                Serial.print((char) readByte(EEPROM_ADDRESS, j));

            }

            Serial.println(".txt");

        }

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

    int fileIndex = 0;

    // Update files
    for (int i = FILE_HEADER_PARTITION_LOWER_BOUND + FILE_NAME_SIZE + 4; i <= FILE_PARTITION_UPPER_BOUND;
        i += fileHeaderSize) {

        short currStartAddressHigh = readByte(EEPROM_ADDRESS, i);
        short currStartAddressLow = readByte(EEPROM_ADDRESS, i + 1);
        short currStartAddress = assembleShort(currStartAddressHigh, currStartAddressLow);

        if (currStartAddress == parentStartAddress) {
            currentContainedFolderStartAddresses[folderIndex] = currStartAddress;
            fileIndex++;
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

void readFile
