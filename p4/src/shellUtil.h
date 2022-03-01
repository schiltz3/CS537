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

char *smashReadLine(FILE *stdhi);
char **smashSplitLine(char *line);
int smashLaunch(char **args);
bool isempty(const char *s);
int smashCommand(char **tokens);
int tokenLength(char **tokens);

char* path = NULL;

#endif