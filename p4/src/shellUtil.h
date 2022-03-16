#ifndef SHELL_UTIL_H
#define SHELL_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>

#define STARTING_TOK_BUFSIZE 64
#define TOKEN_DELIMINATORS " \t\n\r"
#define WHITESPACE " \t\r\n\v"
#define SYMBOLS ">&;"

#define RED "\x1B[0;31m"
#define NC "\x1B[0m"

struct path_s
{
  char **paths;
  int len;
  int size;
};

enum cmd_t
{
  UNINIT,
  EXEC,
  REDIR,
  LIST,
  PARALELL,
};

struct exec_cmd_s
{
  struct path_s path;
  char **argv;
};

struct redir_cmd_s
{
  struct cmd_s *cmd;
  char *file_name;
  FILE *fp;
  int stdio_backup;
  int stderr_backup;
};

struct list_cmd_s
{
  struct cmd_s *left;
  struct cmd_s *right;
};

struct paralell_cmd_s
{
  struct cmd_s *left;
  struct cmd_s *right;
};

union cmd_u
{
  struct exec_cmd_s *exec;
  struct redir_cmd_s *redir;
  struct list_cmd_s *list;
  struct paralell_cmd_s *paralell;
};

struct cmd_s
{
  enum cmd_t type;
  union cmd_u cmd;
};

// Return functions
int returnErr(char *error_msg);
int returnPErr(char *failed_func);

// Init cmd functions
struct cmd_s *execCmd(struct path_s *path, char **argv);
struct cmd_s *redirCmd(struct cmd_s *cmd, char *file_name, FILE *fp, int stdio_backup, int stderr_backup);
struct cmd_s *listCmd(struct cmd_s *left, struct cmd_s *right);
struct cmd_s *paralellCmd(struct cmd_s *left, struct cmd_s *right);

// Cmd utils
int runCmd(struct cmd_s *cmd, struct path_s *path);
int searchExecPath(struct path_s *path, char* cmd, char **argv);
int verifyCmd(struct cmd_s *cmd);
void printCmd(struct cmd_s *cmd);

// Path functions
struct path_s *initPath();
int addPath(struct path_s *path, char *add);
int removePath(struct path_s *path, char *remove);
int printPath(struct path_s *path);

// Line functions
/**
 * @brief Get the Token object
 * 
 * @param str_p 
 * @param str_end_p 
 * @param str_cmd 
 * @param str_cmd_args 
 * @return int -1 = error, 0 = sucess, 1 = syntax error
 */
int getToken(char **str_p, char *str_end_p, char **str_cmd, char **str_cmd_args);
char* createTok(char*str, char* str_end);
int getLine(FILE *stream, char *buff, int *len);
int peek(char **ps, char *es, char *toks);
bool isempty(const char *s);

#endif