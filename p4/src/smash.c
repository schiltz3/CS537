#include "shellUtil.h"
int peek(char **ps, char *es, char *toks)
{
  char *s;

  s = *ps;
  while (s < es && strchr(WHITESPACE, *s))
    s++;
  *ps = s;
  return *s && strchr(toks, *s);
}
int main(int argc, char *argv[])
{
  struct path_s *path = initPath();
  addPath(path, "/bin");

  char *str = "ls -a; hi there";
  char *str_end = str + strlen(str);
  char *str_cmd;
  char *str_cmd_end;

  int tok;
  char ret = ' ';
  char *argv1[100];
  int argc1 = 0;
  while (!peek(&str, str_end, "|&;"))
  {
    if ((tok = getToken(&str, str_end, &str_cmd, &str_cmd_end)) == 0)
    {
      printf("BREAK\n\n");
      break;
    }
    if (tok != 'a')
    {
      printf("syntax\n");
    }
    argv1[argc1] = createTok(str_cmd, str_cmd_end);
    argc1++;
    if (argc1 >= 100)
      printf("too many args\n");
    // ret = parseredirs(ret, str, str_end);
  }
  argv1[argc1] = 0;
  for (int i = 0; argv1[i] != 0; i++)
  {
    printf("argv1[%d]:%s\n", i, argv1[i]);
  }

  struct cmd_s *cmd = execCmd(path, argv1);
  verifyCmd(cmd);
  printCmd(cmd);
  exit(EXIT_SUCCESS);
}