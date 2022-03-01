#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "shellUtil.h"

int main(int argc, char *argv[])
{
	if (argc > 2)
	{
		printf("Too many arguments\n");
		exit(EXIT_FAILURE);
	}
	FILE *input = NULL;
	if (argc == 1)
	{
		input = stdin;
	}
	else if (argc == 2)
	{
		printf("File to read from: %s\n\n", argv[1]);
		input = fopen(argv[1], "r");
	}

	if (input == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	while (true)
	{
		printf("smash>");
		char *line = smashReadLine(input);
		if (isempty(line))
		{
			printf("\n");
			continue;
		}
		char **tokens = smashSplitLine(line);

		int ret = smashCommand(tokens);
		if (ret == 1)
		{
			continue;
		}
		else if (ret == -1)
		{
			fprintf(stderr, RED "smashCommand Error" NC);
			exit(EXIT_FAILURE);
		}
		smashLaunch(tokens);
		printf("\n");
	}
	fclose(input);

	return (0);
}
