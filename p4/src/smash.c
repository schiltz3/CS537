#include "shellUtil.h"
int main(int argc, char *argv[])
{
  struct path_s *path = initPath();
  addPath(path, "/bin");

  // char * test_input = "ls -a";
  char *input_buff;
  size_t input_buff_len;

  // getline(&input_buff, &input_buff_len, stdin);

  char *test_input = "ls -a > c";

  struct cmd_s *cmd = parseExecCmd(&test_input, test_input + strlen(test_input));
  // struct cmd_s *cmd = parseRedirCmd(&test_input, test_input + strlen(test_input));
  verifyCmd(cmd);
  printCmd(cmd, "");
  runCmd(cmd, path);
  exit(EXIT_SUCCESS);
}