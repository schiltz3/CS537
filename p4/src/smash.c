#include "shellUtil.h"

int main(int argc, char *argv[])
{
  struct path_s *path = initPath();
  addPath(path, "/bin");

  struct cmd_s *cmd = execCmd(path, "");
  verifyCmd(cmd);
  printCmd(cmd);
  exit(EXIT_SUCCESS);
}