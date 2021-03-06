// wis-untar.c
// John Schiltz
// 16/02/2022
// 02/23/2022

#include <stdio.h>
#include <stdlib.h>
#include "fileutil.h"

int main(int argc, char *argv[])
{

  switch (argc)
  {
  case 1: // Return syntax for calling wis-untar
    printf("wis-untar: tar-file\n");
    break;
  case 2: // Un-tar files
  {
    fileOption *ft = initFileOption();
    openFile(ft, argv[1], "r");
    untarFile(ft);
    closeFile(ft);
  }
  break;
  default:
  {
    printf("Too many args\n");
  }
  }
  exit(1);
}
