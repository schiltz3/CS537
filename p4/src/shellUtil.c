#include "shellUtil.h"

/*****************************Error Functions************************************/
int returnErr(char *error_msg)
{
  fprintf(stderr, RED "%s\n" NC, error_msg);
  return -1;
  // exit(EXIT_FAILURE);
}

int returnPErr(char *failed_func)
{
  perror(failed_func);
  return -1;
  // exit(EXIT_FAILURE);
}

/*****************************Command Functions**********************************/
struct cmd_s *execCmd(char **argv)
{

  struct cmd_s *cmd = malloc(sizeof(*cmd));
  if (cmd == NULL)
  {
    returnPErr("Malloc");
    return NULL;
  }

  cmd->type = EXEC;
  cmd->cmd.exec = (struct exec_cmd_s *)malloc(sizeof(struct exec_cmd_s));
  if (cmd->cmd.exec == NULL)
  {
    returnPErr("Malloc");
    free(cmd);
    return NULL;
  }
  cmd->cmd.exec->argv = argv;

  return cmd;
}

struct cmd_s *redirCmd(struct cmd_s *arg_cmd, char *file_name)
{
  if (arg_cmd == NULL)
  {
    returnErr("Null cmd passed to redirCmd");
    return NULL;
  }

  struct cmd_s *cmd = malloc(sizeof(*cmd));

  if (cmd == NULL)
  {
    returnPErr("Malloc");
    return NULL;
  }
  cmd->type = REDIR;
  cmd->cmd.redir = (struct redir_cmd_s*)malloc(sizeof(struct redir_cmd_s));
  if(cmd->cmd.exec == NULL){
    returnPErr("Malloc");
    free(cmd);
    return NULL;
  }

  cmd->cmd.redir->cmd = arg_cmd;
  cmd->cmd.redir->file_name = file_name;

  return cmd;
}

struct cmd_s *parseExecCmd(char **ps, char *str_end)
{
  char *str = *ps;
  char *str_cmd;
  char *str_cmd_end;

  int tok;
  char ret = ' ';
  char *argv[100];
  int argc = 0;
  struct cmd_s *cmd;

  while (!peek(&str, str_end, "|&;"))
  {
    if ((tok = getToken(&str, str_end, &str_cmd, &str_cmd_end)) == 0)
    {
      break;
    }
    if (tok != 'a')
    {
      printf("syntax\n");
    }
    argv[argc] = createTok(str_cmd, str_cmd_end);
    argc++;
    if (argc >= 100)
      printf("too many args\n");
    // ret = parseredirs(ret, str, str_end);
  }
  argv[argc] = 0;

  cmd = execCmd(argv);
  verifyCmd(cmd);

  return cmd;
}

struct cmd_s *parseRedirCmd(char **ps, char *str_end)
{
  struct cmd_s *cmd;
  char *tok, *tok_end;

  cmd = parseExecCmd(ps, str_end);
  if (peek(ps, str_end, ">"))
  {
    int redir = gettoken(ps, str_end, NULL, NULL);
    if (gettoken(ps, str_end, tok, tok_end) != 'a')
    {
      returnErr("No file for redirection");
      return NULL;
    }
    if (redir == '>')
    {
      cmd = redirCmd(cmd, createTok(tok, tok_end));
    }
  }
  verifyCmd(cmd);
  return cmd;
}

int runCmd(struct cmd_s *cmd, struct path_s *path)
{
  if (cmd == NULL)
  {
    return returnErr("Null cmd pointer passed to runCmd");
  }

  switch (cmd->type)
  {
  case EXEC:
  {
    if (cmd->cmd.exec == NULL)
    {
      return returnErr("Null exec pointer in cmd");
    }
    if (cmd->cmd.exec->argv == NULL)
    {
      return returnErr("Null argv pointer in cmd");
    }
    searchExecPath(path, cmd->cmd.exec->argv[0], cmd->cmd.exec->argv);
    break;
  }
  default:
    returnErr("Default runCmd");
  }

  return -1;
}

int searchExecPath(struct path_s *path, char *cmd, char **argv)
{
  bool found = false;
  for (int i = 0; i < path->len; i++)
  {
    // Create path to try to execute at
    char *lookup_path = malloc(strlen(path->paths[i]) + strlen(cmd));
    if (lookup_path == NULL)
    {
      return returnPErr("Malloc");
    }
    sprintf(lookup_path, "%s/%s", path->paths[i], cmd);

    // Check if file is available
    if (access(lookup_path, F_OK) == 0)
    {
      printf("Run:%s\n", lookup_path);
      found = true;

      // Run the exe
      if (execv(lookup_path, argv) == -1)
      {
        free(lookup_path);
        return returnPErr("Execv");
      }
      free(lookup_path);
      break;
    }
    else
    {
      free(lookup_path);
      return returnErr("Access Denied");
    }
    free(lookup_path);
  }
  if (found == false)
  {
    printf("Failed to find program in path");
  }

  return 0;
}

int verifyCmd(struct cmd_s *cmd)
{
  if (cmd == NULL)
  {
    return returnErr("Null Cmd Ptr");
  }

  switch (cmd->type)
  {
  case UNINIT:
    fprintf(stderr, RED "Uninitialized Cmd Type\n" NC);
    return 1;
  default:
    break;
  }

  return 0;
}

void printCmd(struct cmd_s *cmd)
{
  if (verifyCmd(cmd) != 0)
  {
    returnErr("Invalid cmd passed to printCmd");
    return;
  }

  switch (cmd->type)
  {
  case EXEC:
  {
    printf("EXEC:\n");
    printf("└Argv[");
    for (int i = 0; cmd->cmd.exec->argv[i] != 0; i++)
    {
      if (i == 0)
      {
        printf("%s", cmd->cmd.exec->argv[i]);
      }
      else
      {
        printf(",%s", cmd->cmd.exec->argv[i]);
      }
    }
    printf("]\n");
    break;
  }
  default:
    printf("DEFAULT\n");
  }
}

/*****************************Path Functions*************************************/
struct path_s *initPath()
{
  struct path_s *path = malloc(sizeof(*path));
  path->paths = NULL;
  path->len = 0;
  path->size = 0;
  return path;
}

int addPath(struct path_s *path, char *add)
{
  if (path == NULL || add == NULL)
  {
    return returnErr("Null passed to addToPath\n");
  }

  path->size = path->size + strlen(add);

  path->paths = realloc(path->paths, path->size);
  if (path->paths == NULL)
  {
    return returnPErr("realloc");
  }

  path->len++;

  for (int i = path->len - 1; i > 0; i--)
  {
    path->paths[i] = path->paths[i - 1];
  }

  path->paths[0] = add;

  return 0;
}

int removePath(struct path_s *path, char *remove)
{
  if (path == NULL || remove == NULL)
  {
    return returnErr("Null passed to removePath");
  }

  int found = -1;

  for (int i = 0; i < path->len; i++)
  {
    if ((found = strcmp(path->paths[i], remove)) == 0)
    {
      for (int j = i; i < path->len - 1; i++)
      {
        path->paths[j] = path->paths[j + 1];
      }
      path->paths[path->len - 1] = NULL;
      --path->len;
      break;
    }
  }

  if (found != 0)
  {
    return returnErr("Could not find path");
  }

  return 0;
}

int printPath(struct path_s *path)
{
  printf("path:\n");
  for (int i = 0; i < path->len; i++)
  {
    printf("%s\n", path->paths[i]);
  }
  return 0;
}

/*****************************Line functions*************************************/
int getToken(char **str_p, char *str_end_p, char **str_tok_p, char **str_tok_end_p)
{
  char *str = NULL;
  int ret = -1;

  str = *str_p;
  // scan string pointer over whitespace
  while (str < str_end_p && strchr(WHITESPACE, *str) != NULL)
  {
    str++;
  }
  // Set the command
  if (str_tok_p != NULL)
  {
    *str_tok_p = str;
  }
  // Set return to the current position of the str
  ret = *str;

  switch (*str)
  {
  case '\0':
    break;
  case '>':
    ++str_end_p;
    if (*str == '>')
    {
      ret = '+';
      str++;
    }
    break;
  default:
    ret = 'a';
    while (str < str_end_p && strchr(WHITESPACE, *str) == NULL && strchr(SYMBOLS, *str) == NULL)
    {
      ++str;
    }
    break;
  }

  if (str_tok_end_p != NULL)
  {
    *str_tok_end_p = str;
  }
  while (str < str_end_p && strchr(WHITESPACE, *str) != NULL)
  {
    str++;
  }
  *str_p = str;
  return ret;
}

char *createTok(char *str, char *str_end)
{
  int len = str_end - str;
  char *ret = malloc(len + 1);
  if (ret == NULL)
  {
    returnPErr("Malloc failed");
    return NULL;
  }
  strncpy(ret, str, len);
  ret[len] = '\0';
  return ret;
}

int peek(char **ps, char *es, char *toks)
{
  char *s;

  s = *ps;
  while (s < es && strchr(WHITESPACE, *s))
    s++;
  *ps = s;
  return *s && strchr(toks, *s);
}

bool isempty(const char *s)
{
  while (*s)
  {
    if (!isspace(*s))
      return false;
    s++;
  }
  return true;
}