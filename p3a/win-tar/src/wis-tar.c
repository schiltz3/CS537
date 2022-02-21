#include <stdio.h>
#include <stdlib.h>
#include "fileutil.h"

int main(int argc, char *argv[])
{

  int i;
  switch (argc)
  {
  case 1: // Return syntax for calling wis-grep
  case 2: // search through standard in
  {
    printf("wis-tar: tar-file file []...]\n");
  }
  break;
  default: // Search through all the files
  {
    // Open tar file
    fileOption *ft = initFileOption();
    getFile(ft, argv[1]);

    // Concatenate all files onto it
    for (i = 2; i < argc; i++)
    {
      fileOption *fo = initFileOption();
      getFile(fo, argv[i]);
      tarFile(ft, fo);
      closeFile(fo);
    }
  }
  }
  exit(1);
}
