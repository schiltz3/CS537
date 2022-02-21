#include <stdio.h>
#include <stdlib.h>
#include "fileutil.h"

int main(int argc, char *argv[])
{

  int i;
  switch (argc)
  {
  case 1: // Return syntax for calling wis-grep
    printf("wis-grep: searchterm [file ...]\n");
    break;
  case 2: // search through standard in
  {
    fileOption *fo = initFileOption();
    fo->fileName = "stdio";
    fo->fp = stdin;
    searchFile(fo, argv[1]);
    free(fo);
  }
  break;
  default: // Search through all the files
  {
    for (i = 2; i < argc; i++)
    {
      fileOption *fo = initFileOption();
      getFile(fo, argv[i]);
      searchFile(fo, argv[1]);
      closeFile(fo);
    }
  }
  }
  exit(1);
}
