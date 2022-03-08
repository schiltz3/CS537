#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "shellUtil.h"

lines_s path = {NULL, 0, 0};

int main(int argc, char *argv[])
{
	// Do not start program
	if (argc > 2 || argc < 1)
	{
		printf("Too many arguments\n");
		exit(EXIT_FAILURE);
	}

	FILE *input = NULL;

	// Read from stdin
	if (argc == 1)
	{
		input = stdin;
	}
	// Read from input file
	else if (argc == 2)
	{
		printf("File to read from: %s\n\n", argv[1]);
		input = fopen(argv[1], "r");
	}

	// verify input has been selected and properly opened
	if (input == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	addToPath(&path, "/bin");

	// Continue executing commands untill the flile is closed, EOF is reached, or the quit
	// command has been executed
	while (true)
	{
		printf("smash>");

		// read lines from the input
		lines_s *lines = smashReadLine(input);

		// Loop over Lines
		for (int i = 0; i < lines->len; i++)
		{

			// Ignore empty line
			if (isempty(lines->lines[i]))
			{
				printf("\n");
				continue;
			}

			// Split Line up into tokens seperated by whitespace
			lines_s *tokens = smashSplitLine(lines->lines[i], TOKEN_DELIMINATORS);

			// Execute any shell commands
			int ret = smashCommand(tokens, &path);
			if (ret == 1)
			{
				continue;
			}
			else if (ret == -1)
			{
				fprintf(stderr, RED "smashCommand Error" NC);
				exit(EXIT_FAILURE);
			}

			redirect(tokens, &path);
			// execute any system commands
			// smashLaunch(tokens, &path);

			printf("\n");
		}
	}

	// Close input
	fclose(input);

	return (0);
}
