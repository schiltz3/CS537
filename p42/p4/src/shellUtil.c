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
  cmd->cmd.redir = (struct redir_cmd_s *)malloc(sizeof(struct redir_cmd_s));
  if (cmd->cmd.exec == NULL)
  {
    returnPErr("Malloc");
    free(cmd);
    return NULL;
  }

  cmd->cmd.redir->cmd = arg_cmd;
  cmd->cmd.redir->file_name = file_name;

  return cmd;
}
struct cmd_s *listCmd(struct cmd_s *left, struct cmd_s *right)
{
  if (left == NULL)
  {
    returnErr("Null left passed to listCmd");
    return NULL;
  }
  if (right == NULL)
  {
    returnErr("Null right passed to listCmd");
    return NULL;
  }
  //printf("Left:\n");
  //printCmd(left, "");
  //printf("right:\n");
  //printCmd(right, "");

  struct cmd_s *cmd = malloc(sizeof(*cmd));
  if (cmd == NULL)
  {
    returnPErr("Malloc");
    return NULL;
  }
  cmd->type = LIST;
  cmd->cmd.list = (struct list_cmd_s *)malloc(sizeof(struct list_cmd_s));
  cmd->cmd.list->left = left;
  cmd->cmd.list->right = right;
  return cmd;
}

struct cmd_s *parseLine(char **ps, char *es)
{
    struct cmd_s *cmd = NULL;
    struct cmd_s *right = NULL;
  if (*ps >= es - 1)
  {
    printf("return NULL\n");
    return NULL;
  }
  printf("Created cmd:%s\n", *ps);
  cmd = parseExecCmd(ps, es);
  printf("Cmd created:%d\n",cmd);
  printCmd(cmd, "");
  //printf("%d,%d\n",*ps, es);

  if (*ps >= es - 1)
  {
    printf("return\n");
    return cmd;
  }

  //printCmd(cmd, "");
  if (peek(ps, es, ";"))
  {
    getToken(ps, es, 0, 0);
    printf("-----\n");
    printf("ParseLine\n");
    //struct cmd_s *right = parseLine(ps, es);
    right = parseExecCmd(ps, es);
    if (right == NULL)
    {
      printf("right == NULL\n");
      return cmd;
    }
    printf("right:%d\n",right);
    printCmd(right, "");

    printf("\n\n");

    printf("Creating list cmd\n");
    cmd = listCmd(cmd, right);
    printCmd(cmd, "");
  }
  return cmd;
}




struct cmd_s *parseExecCmd(char **ps, char *str_end)
{
  char *str_cmd;
  char *str_cmd_end;

  int tok;
  char *argv[MAX_ARGS];
  int argc = 0;
  struct cmd_s *cmd = execCmd(argv);

  while (!peek(ps, str_end, "|&;"))
  {
    if ((tok = getToken(ps, str_end, &str_cmd, &str_cmd_end)) == 0)
    {
      break;
    }
    if (tok != 'a' && tok != '+')
    {
      printf("syntax\n");
    }
    argv[argc] = createTok(str_cmd, str_cmd_end);
    argc++;
    if (argc >= MAX_ARGS)
    {
      printf("too many args\n");
    }
    cmd = parseRedirCmd(cmd, ps, str_end);
    // printf("Str:%s\n", *ps);
  }
  for (int i = argc; i < MAX_ARGS; i++)
  {
    argv[i] = NULL;
  }

  verifyCmd(cmd);

  return cmd;
}

struct cmd_s *parseRedirCmd(struct cmd_s *arg_cmd, char **ps, char *str_end)
{
  struct cmd_s *cmd = NULL;
  char *tok, *tok_end;

  while (peek(ps, str_end, ">"))
  {
    char redir = (char)getToken(ps, str_end, NULL, NULL);
    if (getToken(ps, str_end, &tok, &tok_end) != 'a')
    {
      printf("PANIK\n");
    }
    if (redir == '>')
    {
      cmd = redirCmd(arg_cmd, createTok(tok, tok_end));
      break;
    }
  }
  if (cmd == NULL)
  {
    cmd = arg_cmd;
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

    searchExecPath(path, &cmd->cmd.exec->argv[0], cmd->cmd.exec->argv);
    // pid_t pid;
    // pid_t wpid;
    // int status;
    // pid = fork();
    // // Child
    // if (pid == 0)
    // {

    //   // Failed fork
    // }
    // else if (pid < 0)
    // {
    //   perror("fork");
    //   exit(EXIT_FAILURE);
    // }
    // // Child
    // else
    // {
    //   do
    //   {
    //     wpid = waitpid(pid, &status, WUNTRACED);
    //     if (wpid == -1)
    //     {
    //       perror("waitpid");
    //       exit(EXIT_FAILURE);
    //     }
    //   } while (WIFEXITED(status) == false && WIFSIGNALED(status) == false);
    // }

    break;
  }
  case REDIR:
  {
    int new = open(cmd->cmd.redir->file_name, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (new < 0)
    {
      return returnErr("Can't open file");
    }

    fflush(stdout);
    fflush(stderr);

    int stdout_backup = dup(fileno(stdout));
    int stderr_backup = dup(fileno(stderr));

    if (dup2(new, fileno(stdout)) == -1)
    {
      return returnPErr("Cannot redirect stdout");
    }
    if (dup2(new, fileno(stderr)) == -1)
    {
      return returnPErr("Cannot redirect stderr");
    }
    close(new);
    runCmd(cmd->cmd.redir->cmd, path);

    fflush(stdout);
    fflush(stderr);

    dup2(stdout_backup, fileno(stdout));
    dup2(stderr_backup, fileno(stderr));

    close(stdout_backup);
    close(stderr_backup);
    printf("redirected stdout\n");
    break;
  }
  default:
    returnErr("Default runCmd");
  }

  return 0;
}

int searchExecPath(struct path_s *path, char **cmd_p, char **argv)
{
  if (cmd_p == NULL)
  {
    return returnErr("Null cmd_p");
  }
  if (path == NULL)
  {
    return returnErr("Null path");
  }
  char *cmd = *cmd_p;
  if (cmd == NULL)
  {
    return returnErr("Null cmd");
  }
  int cmd_len = strlen(cmd);

  bool found = false;
  for (int i = 0; i < path->len; i++)
  {
    // Create path to try to execute at
    int path_len = strlen(path->paths[i]);
    char *lookup_path = (char *)malloc(cmd_len + path_len + 1);
    if (lookup_path == NULL)
    {
      return returnPErr("Malloc");
    }

    // Create lookup str
    for (int j = 0; j < path_len; j++)
    {
      lookup_path[j] = path->paths[i][j];
    }
    lookup_path[path_len] = '/';
    for (int j = 0; j < cmd_len; j++)
    {
      lookup_path[j + path_len + 1] = cmd[j];
    }

    // Check if file is available
    if (access(lookup_path, F_OK) == 0)
    {
      // printf("Run:%s\n", lookup_path);
      found = true;

      // Run the exe
      if (execv(lookup_path, argv) == -1)
      {
        // free(lookup_path);
        return returnPErr("Execv");
      }
      // free(lookup_path);
      break;
    }
    else
    {
      // free(lookup_path);
      return returnErr("Access Denied");
    }
    // free(lookup_path);
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

void printCmd(struct cmd_s *cmd, char *l_padding)
{
  if (verifyCmd(cmd) != 0)
  {
    returnErr("Invalid cmd passed to printCmd");
    return;
  }
  if (l_padding == NULL)
  {
    returnErr("Null l_padding passed to printCmd");
    return;
  }

  switch (cmd->type)
  {
  case EXEC:
  {
    printf("%sEXEC:\n", l_padding);
    printf("%s└Argv[", l_padding);
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
  case REDIR:
  {
    printf("%sREDIR:\n", l_padding);
    printf("%s└File:%s\n", l_padding, cmd->cmd.redir->file_name);
    printCmd(cmd->cmd.redir->cmd, " ");
    break;
  }
  case LIST:
  {
    printf("%sLIST:\n", l_padding);
    printf("%sleft:\n", l_padding);
    printCmd(cmd->cmd.list->left, " ");
    printf("%sright:\n", l_padding);
    printCmd(cmd->cmd.list->right, " ");
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
char getToken(char **str_p, char *str_end_p, char **str_tok_p, char **str_tok_end_p)
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
  {
    str++;
    if (*str == '>')
    {
      ret = '+';
      str++;
    }
    break;
  }
  case ';':
  {
    str++;
    break;
  }
  default:
  {
    ret = 'a';
    while (str < str_end_p && strchr(WHITESPACE, *str) == NULL && strchr(SYMBOLS, *str) == NULL)
    {
      ++str;
    }
    break;
  }
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

  char *ret = strchr(toks, *s);
  return *s && ret;
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