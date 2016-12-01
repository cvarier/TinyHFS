#ifndef DEFINES_H
#define DEFINES_H

// NOTE: The root folder's address is 37768. It is not represented in the EEPROM, as it is a hypothetical folder.
#define EEPROM_ADDRESS                      0x54    
#define MIN_ADDRESS                         0
#define MAX_ADDRESS                         32767
#define DELAY_TIME                          4
#define ROOT_ADDRESS                        32768

/***************************
 *    Element Name Sizes   *
 * *************************
 */
#define FILE_NAME_SIZE						          10
#define FOLDER_NAME_SIZE					          10

/***************************
 *     Partition Bounds    *
 * *************************
 */
#define FOLDER_PARTITION_LOWER_BOUND        1
#define FOLDER_PARTITION_UPPER_BOUND        1000
#define FILE_PARTITION_LOWER_BOUND          1001
#define FILE_PARTITION_UPPER_BOUND          28768
#define FILE_HEADER_PARTITION_LOWER_BOUND   28769
#define FILE_HEADER_PARTITION_UPPER_BOUND   32767

/***************************
 *     Partition Sizes     *
 * *************************
 */
const int fileHeaderPartitionSize = FILE_HEADER_PARTITION_UPPER_BOUND - FILE_HEADER_PARTITION_LOWER_BOUND + 1;
const int folderPartitionSize = FOLDER_PARTITION_UPPER_BOUND - FOLDER_PARTITION_LOWER_BOUND + 1;

/***************************
 *      Element Sizes      *
 * *************************
 */
const int folderSize = FOLDER_NAME_SIZE + 4;
const int fileHeaderSize = FILE_NAME_SIZE + 6;

/***************************
 *    Max Element Bounds   *
 * *************************
 */
// NOTE: maxFileHeaders is an upper bound on the number of files
const int maxFileHeaders = fileHeaderPartitionSize / (fileHeaderSize);
const int maxFolders = folderPartitionSize / (folderSize);

/***************************
 *      Command Names      *
 * *************************
 */
const char file_write[]      =   "file";
const char file_copy[]       =   "cpyfile";
const char file_move[]       =   "mvfile";
const char file_read[]       =   "read";
const char file_delete[]     =   "delfile";

const char folder_create[]   =   "mkdir";
const char folder_step_in[]  =   "in";
const char folder_step_out[] =   "out";
const char folder_copy[]     =   "cpyfol";
const char folder_move[]     =   "mvfol";
const char folder_delete[]   =   "delfol";

const char print_contents[]  =   "ls";
const char print_dir[]       =   "pwd";
const char format_sys[]      =   "format";
const char print_help[]      =   "help";

/***************************
 *      Miscellaneous      *
 * *************************
 */
const char fileTerminator    =   '~';

#endif
