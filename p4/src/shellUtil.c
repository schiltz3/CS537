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
struct cmd_s *execCmd(struct path_s *path, char **argv)
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

struct cmd_s *parseCmd(char *s)
{
  char *es;
  struct cmd_s *cmd = NULL;

  es = s + strlen(s);
  printf("Len:%d\n", (int)strlen(s));
  printf("s:%s\n", s);
  printf("es:%s\n", es);

  return cmd;
}

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
   if (*str == '>')
   {
     ++str_end_p;
     if (*str == '>')
     {
       ret = '+';
       str++;
     }
   }
   else
   {
     ret = 'a';
     while (str < str_end_p && strchr(WHITESPACE, *str) == NULL && strchr(SYMBOLS, *str) == NULL)
     {
       ++str;
     }
   }
   if(str_tok_end_p != NULL){
     *str_tok_end_p = str;
   }
   while(str < str_end_p && strchr(WHITESPACE, *str) != NULL){
     str++;
   }
   *str_p = str;
   return ret;
 }

/*****************************Path Functions*************************************/
struct path_s *initPath()
{
  struct path_s *path = malloc(sizeof(*path));
  path->path = NULL;
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

  path->path = realloc(path->path, path->size);
  if (path->path == NULL)
  {
    return returnPErr("realloc");
  }

  path->len++;

  for (int i = path->len - 1; i > 0; i--)
  {
    path->path[i] = path->path[i - 1];
  }

  path->path[0] = add;

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
    if ((found = strcmp(path->path[i], remove)) == 0)
    {
      for (int j = i; i < path->len - 1; i++)
      {
        path->path[j] = path->path[j + 1];
      }
      path->path[path->len - 1] = NULL;
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
    printf("%s\n", path->path[i]);
  }
  return 0;
}

/*****************************Line functions*************************************/
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

int getLine(FILE *stream, char *buff, int *len)
{
  if (stream == NULL || buff == NULL)
  {
    return returnErr("Null passed to getLine");
  }
  size_t Len = 0;
  if (getline(&buff, &Len, stream) == -1)
  {
    return returnPErr("getline");
  }
  *len = Len;
  return 0;
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