#ifndef SHELL_UTIL_H
#define SHELL_UTIL_H

#define STARTING_TOK_BUFSIZE 64
#define TOKEN_DELIMINATORS " \t\n\r"

#define RED "\x1B[0;31m"
#define NC "\x1B[0m"

enum cmd_t{
  NONE,
  EXEC,
  REDIR,
  LIST
};

struct exec_cmd{
  enum cmd_t type;
  char *argv;
};

struct redir_cmd{
  enum cmd_t type;
  FILE *fp;
};

struct list_cmd{
  enum cmd_t type;
};

union cmd{
  struct exec_cmd *exec;
  struct redir_cmd *redir;
  struct list_cmd *list;
};

#endif