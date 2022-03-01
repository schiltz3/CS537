#ifndef SHELL_UTIL_H
#define SHELL_UTIL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <wait.h>
#include <ctype.h>
#include <pwd.h>

#define RED "\x1B[0;31m"
#define NC "\x1B[0m"

/**
 * @brief read a line from a file
 * 
 * @param stdhi file pointer to read from
 * @return char* line read form file
 */
char *smashReadLine(FILE *stdhi);
char **smashSplitLine(char *line);
int smashCommand(char **tokens);
int smashLaunch(char **args);
bool isempty(const char *s);
int tokenLength(char **tokens);

#endif