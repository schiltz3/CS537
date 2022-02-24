// fileutil.h
// John Schiltz
// 16/02/2022
// 02/23/2022

#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define RED "\x1B[0;31m"
#define NC "\x1B[0m"

#define SEARCH_BUFF_LEN_DEF 100 /* starting size of search buffer*/

/**
 * @brief Struct to hold open file pointer, name and error which is a copy of errno if thrown.
 *
 */
typedef struct fileOption
{
  int error;      //!< Copy of errno if thrown
  FILE *fp;       //!< Pointer to open file
  char *fileName; //!< Name of file
} fileOption;

/**
 * @brief Allocate memory for fileOption struct
 *
 * @return fileOption* Pointer to allocated fileOption struct
 */
fileOption *initFileOption();

/**
 * @brief Open the file specified by fileName populate the fileOption struct
 *
 * @param fo fileOption pointer
 * @param fileName name of file
 * @param mode fopen mode to open the file with
 * @return int -1 = error, 0 = sucess
 */
int openFile(fileOption *fo, char *fileName, char *mode);

/**
 * @brief Close the open file and free the memory in the fileOption struct
 *
 * @param fo filoption pointer
 * @return int -1 = error, 0 = sucess
 */
int closeFile(fileOption *fo);

/**
 * @brief Search through file for a searchTerm and print any line that contains the search term
 *
 * @param fo fileOption pointer
 * @param searchTerm string to search for
 * @return int -1 = error, 0 = sucess
 */
int searchFile(fileOption *fo, char *searchTerm);

/**
 * @brief concatinate one file onto another
 *
 * @param ft tar file to concatinate into
 * @param fo file to concatinate onto the tar file
 * @return int -1 = error, 0 = sucess
 */
int tarFile(fileOption *ft, fileOption *fo);

/**
 * @brief Create files from a tar file
 *
 * @param ft tar file to explode
 * @return int -1 = error, 0 = sucess
 */
int untarFile(fileOption *ft);

/**
 * @brief check if file has any null pointers or err is set
 *
 * @param fo file pointer to check
 * @return int -1 = error, 0 = sucess
 */
int validateFileOption(fileOption *fo);

#endif
