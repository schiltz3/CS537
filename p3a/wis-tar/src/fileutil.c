#include "fileutil.h"

fileOption *initFileOption()
{
  fileOption *fo = (fileOption *)malloc(sizeof(fileOption));
  fo->error = 0;
  return fo;
}

int openFile(fileOption *fo, char *fileName)
{
  fo->fileName = fileName;
  fo->fp = fopen(fileName, "r");
  if (fo->fp == NULL)
  {
    fo->error = errno;
    fprintf(stderr, RED "\nError[%d]\n", fo->error);
    fprintf(stderr, NC "%s: %s\n\n", strerror(fo->error), fileName);
    return -1;
  }
  return 0;
}

int closeFile(fileOption *fo)
{
  if (fo == NULL)
  {
    fprintf(stderr, RED "\nNull pointer:\nFILE: %s\nLINE: %d\n", __FILE__, __LINE__);
    return -1;
  }
  if (fo->fp == NULL)
  {
    fprintf(stderr, RED "\nNull file pointer:\nFILE: %s\nLINE: %d\n", __FILE__, __LINE__);
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
    fprintf(stderr, RED "\nError[%d]\n", fo->error);
    fprintf(stderr, NC "%s: %s\n\n", strerror(fo->error), fo->fileName);
    return -1;
  }

  /* convert file len to a string */
  sprintf(fileLenBuff, "%lu", statbuff.st_size);

  /* write file name buffer*/
  fwrite(&nameBuff, 1, nameLen, ft->fp);

  /* write file size buffer*/
  fwrite(&fileLenBuff, 1, fileLenBuffLen, ft->fp);

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

int validateFileOption(fileOption *fo)
{
  if (fo == NULL || fo->fileName == NULL || fo->fp == NULL)
  {
    fprintf(stderr, RED "\nNull pointer:\nFILE: %s\nLINE: %d\n", __FILE__, __LINE__);
    return -1;
  }
  if (fo->error != 0)
  {
    fprintf(stderr, RED "\nError in file: %s", fo->fileName);
    return -1;
  }
  return 0;
}