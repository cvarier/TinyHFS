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
 * 
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

    byte parentFolderStartAddressHigh = readByte(EEPROM_ADDRESS, currentFolderStartAddress + FOLDER_NAME_SIZE + 2);
    byte parentFolderStartAddressLow = readByte(EEPROM_ADDRESS, currentFolderStartAddress + FOLDER_NAME_SIZE + 3);
    short parentFolderStartAddress = assembleShort(parentFolderStartAddressHigh, parentFolderStartAddressLow);

    if (currentFolderStartAddress == ROOT_ADDRESS) {

        Serial.println("ERROR: Cannot step out of root");
        return;

    }

    updateCurrentDirectory(parentFolderStartAddress);

    currentFolderStartAddress = parentFolderStartAddress;

}

/**
 * Function: displayFolderContents
 *
 * Displays the contents of the current folder via the 'ls' command.
 *
 * @Param folderStartAddress the start address of the current folder
 */
void displayFolderContents() {

    /*
     * Obtain and print the folders that belong to the current folder.
     */
    updateCurrentDirectory(currentFolderStartAddress);

    int isPrinted = 0;

    for (int i = 0; i < maxFolders; i++) {

        if (currentContainedFolderStartAddresses[i] != 0) {

            for (int j = 0; j < FOLDER_NAME_SIZE; j++) {

                Serial.print((char) readByte(EEPROM_ADDRESS, currentContainedFolderStartAddresses[i] + j));
                isPrinted = 1;

            }

        }

        if (isPrinted)
            Serial.println();

        isPrinted = 0;

    }

    /*
     * Obtain and print the files that belong to the current folder.
     */

    isPrinted = 0;
    for (int i = 0; i < fileCount; i++) {

        for (int j = 0; j < FILE_NAME_SIZE; j++) {

            if (currentContainedFileHeaderStartAddresses[i] != 0) {

                char charToPrint = (char) readByte(EEPROM_ADDRESS, currentContainedFileHeaderStartAddresses[i] + j);

                if (charToPrint)
                    Serial.print(charToPrint);

                isPrinted = 1;

            }
        }

        if (isPrinted) {
            Serial.println(".txt");
        }

        isPrinted = 0;
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
    if (!strcmp(command, print_contents)) {

        displayFolderContents();

    } else if (!strcmp(command, folder_step_in)) {

        Serial.println("\nPlease enter a folder name");

        char *folderName = getName(FOLDER_NAME_SIZE);

        short addressIn = findStartAddressFromName(folderName, "folder");

        if (!addressIn) {

            Serial.println("\nERROR: The folder does not exist.");
            return;

        }

        stepIn(addressIn);
        free(folderName);
        Serial.println("...done");

    } else if (!strcmp(command, folder_step_out)) {

        stepOut();
        Serial.println("...done");

    } else if (!strcmp(command, print_dir)) {

        printCurrentDirectory();

    } else if (!strcmp(command, file_write)) {

        Serial.println("\nPlease enter a file name");

        char *fileName = getName(FILE_NAME_SIZE);

        if (!fileName)
            return;

        if (findStartAddressFromName(fileName, "file")) {

            Serial.println("\nERROR: The file already exists.");
            return;

        }

        Serial.println("\nPlease enter your text. When you are finished, enter the '~' character.");
        acceptFile(fileName);
        free(fileName);

        updateCurrentDirectory(currentFolderStartAddress);

    } else if (!strcmp(command, print_help)) {

        printHelp();

    } else if (!strcmp(command, file_read)) {

        Serial.println("\nPlease enter a file name (omit extension).");
        char *fileName = getName(FILE_NAME_SIZE);
        short fileHeaderStartAddress = findStartAddressFromName(fileName, "file");

        if (!fileHeaderStartAddress) {

            Serial.println("\nERROR: The file does not exist.");
            return;

        }

        byte fileStartAddressHigh = readByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE);
        byte fileStartAddressLow = readByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE + 1);
        short fileStartAddress = assembleShort(fileStartAddressHigh, fileStartAddressLow);

        byte fileEndAddressHigh = readByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE + 2);
        byte fileEndAddressLow = readByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE + 3);
        short fileEndAddress = assembleShort(fileEndAddressHigh, fileEndAddressLow);

        readFile(fileStartAddress, fileEndAddress);
        free(fileName);

    } else if (!strcmp(command, folder_create)) {

        Serial.println("\nPlease enter a folder name");

        createFolder(currentFolderStartAddress);

        updateCurrentDirectory(currentFolderStartAddress);

    } else if (!strcmp(command, folder_move)) {

    } else if (!strcmp(command, folder_rename)) {

        Serial.println("\nPlease enter a folder name.");
        char *folderName = getName(FOLDER_NAME_SIZE);
        short folderStartAddress = findStartAddressFromName(folderName, "folder");

        if (!folderStartAddress) {

            Serial.println("\nERROR: The folder does not exist.");
            return;

        }

        Serial.println("\nPlease enter a new folder name.");
        char *folderNameNew = getName(FOLDER_NAME_SIZE);
        short folderHeaderStartAddressCheck = findStartAddressFromName(folderNameNew, "folder");

        if (folderHeaderStartAddressCheck) {

            Serial.println("\nERROR: The folder name is in use.");
            return;

        }

        rename(folderStartAddress, folderNameNew, FOLDER_NAME_SIZE);
        free(folderName);
        free(folderNameNew);

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
        Serial.println("...done");

        updateCurrentDirectory(currentFolderStartAddress);

    } else if (!strcmp(command, file_move)) {

    } else if (!strcmp(command, format_sys)) {

        format();

    } else if (!strcmp(command, folder_copy)) {

    } else if (!strcmp(command, file_rename)) {

        Serial.println("\nPlease enter a file name.");
        char *fileName = getName(FILE_NAME_SIZE);
        short fileHeaderStartAddress = findStartAddressFromName(fileName, "file");

        if (!fileHeaderStartAddress) {

            Serial.println("\nERROR: The file does not exist.");
            return;

        }

        Serial.println("\nPlease enter a new file name.");
        char *fileNameNew = getName(FILE_NAME_SIZE);

        short fileHeaderStartAddressCheck = findStartAddressFromName(fileNameNew, "file");

        if (fileHeaderStartAddressCheck) {

            Serial.println("\nERROR: The file name is in use.");
            return;

        }

        rename(fileHeaderStartAddress, fileNameNew, FILE_NAME_SIZE);
        free(fileName);
        free(fileNameNew);

    } else if (!strcmp(command, file_copy)) {

        Serial.println("\nPlease enter a file name");

        char *fileName = getName(FILE_NAME_SIZE);
        short fileHeaderStartAddress = findStartAddressFromName(fileName, "file");

        if (!fileHeaderStartAddress) {

            Serial.println("\nERROR: The file does not exist.");
            return;

        }

        byte fileStartAddressHigh = readByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE);
        byte fileStartAddressLow = readByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE + 1);
        short fileStartAddress = assembleShort(fileStartAddressHigh, fileStartAddressLow);

        byte fileEndAddressHigh = readByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE + 2);
        byte fileEndAddressLow = readByte(EEPROM_ADDRESS, fileHeaderStartAddress + FILE_NAME_SIZE + 3);
        short fileEndAddress = assembleShort(fileEndAddressHigh, fileEndAddressLow);

        Serial.println("\nPlease enter a destination path");

        char *destArr = getName((FOLDER_NAME_SIZE + 1) * maxFolders + 2);
        short destStartAddress = getAddressFromPathName(String(destArr), 1, 0, fileName, "file");

        if (!destStartAddress) {

            Serial.println("\nERROR: The file already exists in the specified path or the path does not exist.");
            return;

        }

        copyFile(fileStartAddress, fileEndAddress, destStartAddress, fileName);
        free(destArr);
        free(fileName);

    } else if (!strcmp(command, file_delete)) {

        Serial.println("\nPlease enter a file name (omit extension).");
        char *fileName = getName(FILE_NAME_SIZE);
        short fileHeaderStartAddress = findStartAddressFromName(fileName, "file");

        if (!fileHeaderStartAddress) {

            Serial.println("\nERROR: The file could not be found.");
            return;

        }

        deleteFile(fileHeaderStartAddress);
        Serial.println("...done");

        updateCurrentDirectory(currentFolderStartAddress);

    } else if (!strcmp(command, organize_mem)) {

        organizeMemory();
        Serial.println("...done");

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

    for (int i = 0; i < maxFolders; i++)
        currentContainedFolderStartAddresses[i] = 0;

    for (int i = 0; i < maxFileHeaders; i++)
        currentContainedFileHeaderStartAddresses[i] = 0;

    // Update folders
    for (int i = FOLDER_PARTITION_LOWER_BOUND + FOLDER_NAME_SIZE + 2; i <= FOLDER_PARTITION_UPPER_BOUND; i +=
        folderSize) {

        byte currStartAddressHigh = readByte(EEPROM_ADDRESS, i);
        byte currStartAddressLow = readByte(EEPROM_ADDRESS, i + 1);
        short currStartAddress = assembleShort(currStartAddressHigh, currStartAddressLow);

        if (currStartAddress == parentStartAddress) {

            currentContainedFolderStartAddresses[folderIndex] = i - FOLDER_NAME_SIZE - 2;

            folderIndex++;
        }

    }

    int fileHeaderIndex = 0;

    // Update file headers
    for (int i = FILE_HEADER_PARTITION_LOWER_BOUND + FILE_NAME_SIZE + 4; i <= FILE_HEADER_PARTITION_UPPER_BOUND; i +=
        fileHeaderSize) {

        byte currStartAddressHigh = readByte(EEPROM_ADDRESS, i);
        byte currStartAddressLow = readByte(EEPROM_ADDRESS, i + 1);
        short currStartAddress = assembleShort(currStartAddressHigh, currStartAddressLow);

        if (currStartAddress == parentStartAddress) {
            currentContainedFileHeaderStartAddresses[fileHeaderIndex] = i - FILE_NAME_SIZE - 4;

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

    if (currentFolderStartAddress == ROOT_ADDRESS) {

        Serial.println();
        Serial.println("E:/");
        return;

    }

    for (int i = currentFolderStartAddress + FOLDER_NAME_SIZE - 1; i >= currentFolderStartAddress; i--)
        directory = (char) readByte(EEPROM_ADDRESS, i) + directory;

    directory = getDirectory(directory, currentFolderStartAddress);

    Serial.println();
    Serial.print(directory);
    Serial.println("/");

}

/*
 * Function : getDirectory
 * 
 * Recursively generates the file path of the current working directory.
 * 
 * @Param directory the current file path: MUST be initialized to the current folder name
 * @Param currentAddress the address of the current folder in the call stack: MUST be initialized to the address of
 * the current folder
 */
String getDirectory(String directory, short currentAddress) {

    short parentFolderAddressLoc = currentAddress + FOLDER_NAME_SIZE + 2;
    short parentFolderAddress = assembleShort(readByte(EEPROM_ADDRESS, parentFolderAddressLoc),
        readByte(EEPROM_ADDRESS, parentFolderAddressLoc + 1));

    if (parentFolderAddress == ROOT_ADDRESS) {

        directory = "E:/" + directory;
        return directory;

    }

    directory = "/" + directory;

    for (int i = parentFolderAddress + FOLDER_NAME_SIZE - 1; i >= parentFolderAddress; i--)
        directory = (char) readByte(EEPROM_ADDRESS, i) + directory;

    return getDirectory(directory, parentFolderAddress);

}

/*
 * Function : getAddressFromPathName
 *
 * Recursively finds the start address of the innermost folder from the given path.
 *
 * @Param path the given path declared as E:/folder1/folder2/.../
 * @Param atRoot indicates the current path String is at root level; initially set to 1
 * @Param steps indicates the number of times stepIn has been called; initially set to 0
 */
short getAddressFromPathName(String path, int atRoot, int steps, const char *name, const char *specifier) {

    if (!strcmp(path.c_str(), "E:") && atRoot)
        return ROOT_ADDRESS;

    String folderStr = "";

    for (int i = 0; path[i] != '/'; i++) {

        folderStr += path[i];

    }

    atRoot = 0;
    short addressRet = 0;

    if (folderStr = "E:") {

        addressRet = ROOT_ADDRESS;

    } else {

        addressRet = findStartAddressFromName(name, specifier);
        stepIn(addressRet);
        steps++;

    }

    if (addressRet) {

        int folderStrLen = folderStr.length();
        folderStr = "";

        for (unsigned int i = 0; i < path.length(); i++)
            folderStr += path[i];

        for (int i = 0; i <= folderStrLen; i++)
            folderStr.remove(0);

        if (folderStr == "") {

            if (findStartAddressFromName(name, "file")) {

                return 0;

            }

            for (int i = 0; i < steps; i++)
                stepOut();

            return addressRet;

        } else {

            return getAddressFromPathName(folderStr, 0, steps, name, specifier);

        }

    }

    return 0;

}

/**
 * Function: print
 *
 * Prints the list of commands and what they do.
 */
void printHelp() {

    Serial.println("\n*****COMMANDS LIST*****\n");
    Serial.print(file_write);
    Serial.println("    ->  Creates and stores new text file with a given file name");
    Serial.print(file_read);
    Serial.println("    ->  Displays the contents of a specified file");
    Serial.print(file_delete);
    Serial.println(" ->  Deletes the specified file");
    /*Serial.print(file_copy);
     Serial.println(" -> Copies a specified file to a specified path of the form E:/folder1/folder2/.../");
     Serial.print(file_move);
     Serial.println("  -> Moves a specified file to a specified path of the form E:/folder1/folder2/.../");*/
    Serial.print(file_rename);
    Serial.println("  -> Renames a specified file\n");

    Serial.print(folder_create);
    Serial.println("   ->  Creates new folder with a given directory name");
    Serial.print(folder_step_in);
    Serial.println("      ->  Steps into a specified folder, down a level");
    Serial.print(folder_step_out);
    Serial.println("     ->  Steps up to the parent folder");
    Serial.print(folder_delete);
    Serial.println("  ->  Deletes the specified folder");
    /*Serial.print(folder_copy);
     Serial.println("  -> Copies a specified folder to a specified path of the form E:/folder1/folder2/.../");
     Serial.print(folder_move);
     Serial.println("   -> Moves a specified folder to a specified path of the form E:/folder1/folder2/.../");*/
    Serial.print(folder_rename);
    Serial.println("   -> Renames a specified folder\n");

    Serial.print(print_contents);
    Serial.println("      ->  Display the current folder's contents");
    Serial.print(print_dir);
    Serial.println("     ->  Displays the current working directory");
    Serial.print(organize_mem);
    Serial.println("    ->  Organizes the memory by removing gaps between adjacent files");
    Serial.print(format_sys);
    Serial.println("  ->  Clears the entire file system (takes approximately 144 seconds)");

}

/**
 * Name : findStartAddressFromName
 *
 * Returns the start address of the specified file header or folder within the current directory.
 * Returns 0 if start address was not found.
 *
 * @Param searchFor the name of the file/folder
 * @Param specifier can be either "file" or "folder, and indicates whether the given name is for a file or folder
 */
short findStartAddressFromName(const char *searchFor, const char *specifier) {

    int numIters;
    int nameSize;
    short arraySize;
    int matches = 0;

    if (!strcmp(specifier, "folder")) {

        numIters = maxFolders;
        nameSize = FOLDER_NAME_SIZE;
        arraySize = maxFolders;

    }

    if (!strcmp(specifier, "file")) {

        numIters = maxFileHeaders;
        nameSize = FILE_NAME_SIZE;
        arraySize = maxFileHeaders;

    }

    short arrayOfAddresses[arraySize];

    if (arraySize == maxFolders) {

        for (int i = 0; i < maxFolders; i++)
            arrayOfAddresses[i] = currentContainedFolderStartAddresses[i];

    } else if (arraySize == maxFileHeaders) {

        for (int i = 0; i < maxFileHeaders; i++)
            arrayOfAddresses[i] = currentContainedFileHeaderStartAddresses[i];

    }

    for (int i = 0; i < numIters; i++) {

        char charToComp = (char) readByte(EEPROM_ADDRESS, arrayOfAddresses[i]);

        if (charToComp == searchFor[0]) {

            matches = 1;

            for (int j = 0; j < nameSize; j++) {

                charToComp = (char) readByte(EEPROM_ADDRESS, arrayOfAddresses[i] + j);

                if (charToComp != searchFor[j])
                    matches = 0;

            }

            if (matches) {

                short retAddress = arrayOfAddresses[i];
                return retAddress;

            }

        }

    }

    return 0;

}

/* Name : acceptFile
 *
 * Accepts input from the user. This will be the data written to the EEPROM.
 *
 * @Param name of file to write
 */
void acceptFile(char *fileName) {

    int strRcvd = 0;
    String inData = "";
    Serial.flush();

    while (1) {

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

                writeFile(str_bytes, str_len, currentFolderStartAddress, fileName);

                return;

            }

        }

    }

}
