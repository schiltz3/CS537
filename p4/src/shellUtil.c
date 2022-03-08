#include "shellUtil.h"

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

struct cmd_s *execCmd(struct path_s *path, char *argv)
{

  struct cmd_s *cmd = malloc(sizeof(*cmd));
  if (cmd == NULL)
  {
    returnPErr("Malloc");
    return NULL;
  }

  cmd->type = EXEC;
  cmd->cmd = malloc(sizeof(struct exec_cmd_s));
  if (cmd->cmd == NULL)
  {
    returnPErr("Malloc");
    free(cmd);
    return NULL;
  }

  return cmd;
}

void printCmd(struct cmd_s *cmd)
{
  if (verifyCmd(cmd) != 0)
  {
    fprintf(stderr, RED "Invalid cmd passed to printCmd" NC);
    return;
  }

  switch (cmd->type)
  {
  case EXEC:
  {
    printf("EXEC:\n");
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
    fprintf(stderr, RED "Null Cmd Ptr\n" NC);
    return -1;
  }

  if (cmd->type == UNINIT)
  {
    fprintf(stderr, RED "Uninitialized Cmd Type\n" NC);
    return -1;
  }

  return 0;
}

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