#ifndef DEFINES_H
#define DEFINES_H

#define EEPROM_ADDRESS                         0x54   

#define MIN_ADDRESS                            0
#define MAX_ADDRESS                            32767


#define SYSTEM_INFO_LOWER_BOUND                0
#define SYSTEM_INFO_UPPER_BOUND                999  // may be too large
#define FOLDER_LOWER_BOUND                     1000
#define FOLDER_UPPER_BOUND                     3999
#define FILE_HEADER_LOWER_BOUND                27768
#define FILE_HEADER_UPPER_BOUND                37767
#define FILE_HEADER_SIZE                       18
#define FOLDER_HEADER_SIZE                     18
    
#define MAX_NAME_LENGTH                        10

#define NEXT_AVAIL_FOLDER_ADDRESS_MEM_LOC            0
#define NEXT_AVAIL_FILE_ADDRESS_MEM_LOC        2
#define NEXT_AVAIL_FILE_HEADER_ADDRESS_MEM_LOC 4
#define FREE_FILE_SPACE_MEM_LOC_LOW            6
#define FREE_FILE_SPACE_MEM_LOC_HIGH           500
#define FREE_FOLDER_SPACE_MEM_LOC_LOW          501
#define FREE_FOLDER_SPACE_MEM_LOC_HIGH         700
#define FREE_FILE_HEADER_SPACE_MEM_LOC_LOW     701
#define FREE_FLER_HEADER_SPACE_MEM_LOC_HIGH    900



#endif
