#ifndef DEFINES_H
#define DEFINES_H

#define EEPROM_ADDRESS                      0x54    
#define MIN_ADDRESS                         0
#define MAX_ADDRESS                         32767
#define DELAY_TIME                          4

#define FILE_NAME_SIZE						10
#define FOLDER_NAME_SIZE					10

#define FOLDER_PARTITION_LOWER_BOUND        1
#define FOLDER_PARTITION_UPPER_BOUND        1000
#define FILE_PARTITION_LOWER_BOUND          1001
#define FILE_PARTITION_UPPER_BOUND          28768
#define FILE_HEADER_PARTITION_LOWER_BOUND   28769
#define FILE_HEADER_PARTITION_UPPER_BOUND   32767

// NOTE: The root folder's address is 37768. It is not represented in the EEPROM, as it is a hypothetical folder.
#define ROOT_ADDRESS                        32768

const int fileHeaderPartitionSize = FILE_HEADER_PARTITION_UPPER_BOUND - FILE_HEADER_PARTITION_LOWER_BOUND + 1;
const int folderPartitionSize = FOLDER_PARTITION_UPPER_BOUND - FOLDER_PARTITION_LOWER_BOUND + 1;

const int folderSize = FOLDER_NAME_SIZE + 4;
const int fileHeaderSize = FILE_NAME_SIZE + 6;

// maxFileHeaders => upper bound on number of files
const int maxFileHeaders = fileHeaderPartitionSize / (fileHeaderSize);
const int maxFolders = folderPartitionSize / (folderSize);

#endif
