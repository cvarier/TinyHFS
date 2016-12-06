# TinyHFS
Hierarchical file system implemented for the Orbit Booster Pack in conjunction with the TM4C123GXL. The system has a UNIX-like command line UI accessed through the UART (Serial) interface. 

## Hardware Components

* 32KB 24LC256 EEPROM integrated into the Orbit Booster Pack
* Serial (UART) interface

## Files

* Files can be of variable length
* Files are plain text
* Files are stored at the end of the EEPROM
* Every file contains a header (not visible to user) with the following attributes (in order as stored in EEPROM):

File name
File start address
File end address
Parent folder start address


The name of the file is restricted to 10 characters (10 bytes)
The file writing procedure consists of the following:


Compute the size of the text file (number of bytes in the input string).
Set aside 2 bytes (a short int) each for the parent folder start address, parent folder stop address, start address, stop address, date created and date last modified.
Set aside 10 bytes for the file name.
Ask user for filename through serial.
Scan file system to see if filename already exists within current directory. If it does, ask user if they wish to overwrite file. If it doesn’t exist or if user says yes, continue as normal, otherwise abort.
If file doesn’t exist, set date created and date last modified to current date.
If file does exist, set date last modified to current date.
Compute file size (16 bytes (for the header) + the # bytes of text data)
Set start address to the next available address for the file (stop address of last file + 1) (if the last file is the file being overwritten, then the start address is the start address of this file (we will shift addresses if file size changes))
Write the file data (text) contiguously in EEPROM to the last byte of text
Write/update the header elements contiguously in the following order: name, date created, date last modified, start address, stop address, start address of parent folder, stop address of parent folder
Shift the addresses of all subsequent folders as needed

## Folders

## Partitions

* The EEPROM is partitioned to separate the file headers, folders and files themselves.
27,768 bytes will be allocated to the file partition, 4,000 bytes will be allocated to the file header partition and 1,000 bytes will be allocated to the folder partition
This will allow us to store a maximum of 71 folders and 250 file headers in all cases
This means, if on average a file is composed of 300 bytes, we can store 92 files
The system will be represented in memory as follows:


## UI

## Authors

Ben Upenieks and Chaitanya Varier
