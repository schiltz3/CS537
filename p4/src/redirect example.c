#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
  // Open file to save standard out to
  int out = open("cout.log", O_RDWR | O_CREAT | O_APPEND, 0600);
  if (-1 == out)
  {
    perror("opening cout.log");
    return -1;
  }

  // Open file to save standard err to
  int err = open("cerr.log", O_RDWR | O_CREAT | O_APPEND, 0600);
  if (-1 == err)
  {
    perror("opening cerr.log");
    return -1;
  }

  // Save the file descriptors of standard out and standard error
  int save_out = dup(fileno(stdout));
  int save_err = dup(fileno(stderr));

  
  // Duplicate out to standard out (replace stdout with out)
  if (-1 == dup2(out, fileno(stdout)))
  {
    perror("cannot redirect stdout");
    return -1;
  }
  // Duplicate err to standard err (replace stderr with err)
  if (-1 == dup2(err, fileno(stderr)))
  {
    perror("cannot redirect stderr");
    return -1;
  }

  // This is where you put your functions to run while stdout and stderr are redirected to file
  printf("doing an ls or something now\n");

  // Flush out all data to files before closing, then close them
  fflush(stdout);
  close(out);
  fflush(stderr);
  close(err);

  // Replace stdout and stderr with saved file descriptors
  dup2(save_out, fileno(stdout));
  dup2(save_err, fileno(stderr));

  // Close the file descriptors
  close(save_out);
  close(save_err);

  printf("back to normal output\n");

  return 0;
}