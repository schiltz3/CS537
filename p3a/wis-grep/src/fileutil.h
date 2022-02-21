#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RED "\x1B[0;31m"
#define NC "\x1B[0m"

#define SEARCH_BUFF_LEN_DEF 100

typedef struct fileOption
{
  int error;
  FILE *fp;
  char *fileName;
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
 * @return int -1 = error, 0 = sucess
 */
int openFile(fileOption *fo, char *fileName);

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

#endif
