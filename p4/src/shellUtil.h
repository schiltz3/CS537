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

#define STARTING_TOK_BUFSIZE 64
#define TOKEN_DELIMINATORS " \t\n\r"

#define RED "\x1B[0;31m"
#define NC "\x1B[0m"

typedef struct Lines
{
  char **lines;
  int len;
} lines_s;

/**
 * @brief read a line from a file
 *
 * @param stdhi file pointer to read from
 * @return char* line read form file
 */
lines_s *smashReadLine(FILE *stdhi);
lines_s *smashSplitLine(char *line, char *delims);
int smashCommand(lines_s *tokens);
int smashLaunch(lines_s *args);
bool isempty(const char *s);
int tokenLength(char **tokens);
void printLines(lines_s *lines);

#endif