// fileutil.h
// John Schiltz
// 16/02/2022
// 02/23/2022

#include "fileutil.h"

fileOption *initFileOption()
{
  fileOption *fo = (fileOption *)malloc(sizeof(fileOption));
  fo->error = 0;
  return fo;
}

int openFile(fileOption *fo, char *fileName, char *mode)
{
  if (fo == NULL)
  {
    fprintf(stderr, RED "\nNull pointer:\nFILE: %s\nLINE: %d\n" NC, __FILE__, __LINE__);
    return -1;
  }
  if (fileName == NULL)
  {
    fprintf(stderr, RED "\nNo file name:\nFILE: %s\nLINE: %d\n" NC, __FILE__, __LINE__);
    return -1;
  }
  if (mode == NULL)
  {
    fprintf(stderr, RED "\nNo file mode:\nFILE: %s\nLINE: %d\n" NC, __FILE__, __LINE__);
    return -1;
  }
  fo->fileName = fileName;
  fo->fp = fopen(fileName, mode);
  if (fo->fp == NULL)
  {
    fo->error = errno;
    fprintf(stderr, RED "Error[%d]\n" NC, fo->error);
    fprintf(stderr, "%s: %s\n", strerror(fo->error), fileName);
    fprintf(stderr, RED "FILE: %s\nLINE: %d\n" NC, __FILE__, __LINE__);
    return -1;
  }
  return 0;
}

int closeFile(fileOption *fo)
{
  if (fo == NULL)
  {
    fprintf(stderr, RED "\nNull pointer:\nFILE: %s\nLINE: %d\n" NC, __FILE__, __LINE__);
    return -1;
  }
  if (fo->fp == NULL)
  {
    fprintf(stderr, RED "\nNull file pointer:\nFILE: %s\nLINE: %d\n" NC, __FILE__, __LINE__);
    free(fo);
    return -1;
  }
  fclose(fo->fp);
  free(fo);
  return 0;
}

int searchFile(fileOption *fo, char *searchTerm)
{
  if (validateFileOption(fo) == -1)
    return -1;

  char *line = (char *)malloc(sizeof(char) * SEARCH_BUFF_LEN_DEF);
  int lineMax = SEARCH_BUFF_LEN_DEF;
  int lineLen = 0;
  char ch;

  while ((ch = getc(fo->fp)) != EOF)
  {
    memset(line, '\0', sizeof(char) * lineMax);
    lineLen = 0;
    while (ch != '\n' && ch != EOF)
    {
      // Dynamically allocate memory for line
      if (lineLen >= lineMax)
      {
        lineMax *= 2;
        line = realloc(line, lineMax);
      }
      // Read line from file
      line[lineLen] = ch;
      lineLen++;
      ch = getc(fo->fp);
    }
    // Look for searchTerm match in line
    if (strstr(line, searchTerm) != NULL)
    {
      printf("%s\n", line);
    }
  }

  free(line);

  return 0;
}

int tarFile(fileOption *ft, fileOption *fo)
{
  if (validateFileOption(ft) == -1 || validateFileOption(fo) == -1)
    return -1;

  /* set up file name buffer */
  const uint8_t nameLen = 100;
  char nameBuff[nameLen];
  memset(nameBuff, '\0', nameLen);
  strncpy(nameBuff, fo->fileName, nameLen);

  /* set up file length buffer */
  const uint8_t fileLenBuffLen = 4;
  char fileLenBuff[fileLenBuffLen];
  memset(fileLenBuff, '\0', fileLenBuffLen);

  /* get file size */
  struct stat statbuff;
  if (fstat(fileno(fo->fp), &statbuff) == -1)
  {
    fo->error = errno;
    fprintf(stderr, RED "\nError[%d]\n" NC, fo->error);
    fprintf(stderr, "%s: %s\n", strerror(fo->error), fo->fileName);
    fprintf(stderr, RED "\nFILE: %s\nLINE: %d\n" NC, __FILE__, __LINE__);
    return -1;
  }

  /* write file name buffer*/
  fwrite(&nameBuff, 1, nameLen, ft->fp);

  /* write file size buffer*/
  //fwrite(&fileLenBuff, 1, fileLenBuffLen, ft->fp);

  uint64_t fileSize = statbuff.st_size;
  fwrite(&fileSize, 1, sizeof(fileSize), ft->fp);

  const uint16_t transferBuffLen = 4096;
  char transferBuff[transferBuffLen];

  /* write file contents */
  size_t size;
  while (feof(fo->fp) == 0)
  {
    size = fread(transferBuff, 1, transferBuffLen, fo->fp);
    fwrite(transferBuff, 1, size, ft->fp);
  }

  return 0;
}

int untarFile(fileOption *ft)
{
  if (validateFileOption(ft) == -1)
    return -1;

  while (feof(ft->fp) == 0)
  {
    /* set up file name buffer */
    const uint8_t nameLen = 100;
    char nameBuff[nameLen];
    memset(nameBuff, '\0', nameLen);

    /* read file name from file */
    fread(nameBuff, 1, nameLen, ft->fp);

    /* read file length from tar */
    uint64_t fileLen;
    fread(&fileLen, 1, sizeof(fileLen), ft->fp);

    if (feof(ft->fp) != 0)
      return 0;

    /* allocate file contents buffer*/
    char *transferBuff = malloc(fileLen);
    if (transferBuff == NULL)
      fprintf(stderr, RED "\nmalloc failed:\nFILE: %s\nLINE: %d\n" NC, __FILE__, __LINE__);

    /* read file contents from tar */
    fread(transferBuff, 1, fileLen, ft->fp);

    /* create file */
    fileOption *fo = initFileOption();
    openFile(fo, nameBuff, "w");

    /* write file */
    fwrite(transferBuff, 1, fileLen, fo->fp);

    closeFile(fo);
  }

  return 0;
}

int validateFileOption(fileOption *fo)
{
  if (fo == NULL)
  {
    fprintf(stderr, RED "\nNull fileObject pointer:\nFILE: %s\nLINE: %d\n" NC, __FILE__, __LINE__);
    return -1;
  }
  if (fo->fileName == NULL)
  {
    fprintf(stderr, RED "\nNull file Name pointer:\nFILE: %s\nLINE: %d\n" NC, __FILE__, __LINE__);
    return -1;
  }
  if (fo->fp == NULL)
  {
    fprintf(stderr, RED "\nNull file pointer:\nFILE: %s\nLINE: %d\n" NC, __FILE__, __LINE__);
    return -1;
  }
  if (fo->error != 0)
  {
    fprintf(stderr, RED "\nError[%d]\n" NC, fo->error);
    fprintf(stderr, "%s: %s\n", strerror(fo->error), fo->fileName);
    return -1;
  }
  return 0;
}