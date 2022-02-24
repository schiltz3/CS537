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
    ft->fileName = argv[1];
    ft->fp = fopen(argv[1], "w");
    // if error occures, print error, free memeory and exit
    if (ft->fp == NULL)
    {
      fprintf(stderr, RED "\nError[%d]\n", ft->error);
      fprintf(stderr, NC "%s: %s\n\n", strerror(ft->error), ft->fileName);
      closeFile(ft);
      exit(EXIT_FAILURE);
    }

    // Concatenate all files onto it
    for (i = 2; i < argc; i++)
    {
      fileOption *fo = initFileOption();
      openFile(fo, argv[i]);
      tarFile(ft, fo);
      closeFile(fo);
    }
    closeFile(ft);
  }
  }
  exit(1);
}
