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
	if (argc == 1)
	{
		while (true)
		{
			printf("smash>");
			char *line = smashReadLine(stdin);
			//if (isempty(line))
			//{
			//	printf("Empty Line\n");
			//	continue;
			//}
			char **tokens = smashSplitLine(line);
			smashLaunch(tokens);
		}
	}
	else if (argc == 2)
	{
		printf("File to read from: %s\n\n", argv[1]);
		FILE *input = fopen(argv[1], "r");
		if (input == NULL)
		{
			perror("fopen");
			exit(EXIT_FAILURE);
		}
		while (true)
		{
			char *line = smashReadLine(input);
			if (isempty(line))
			{
				printf("\n");
				continue;
			}
			char **tokens = smashSplitLine(line);
			smashCommand(tokens);
			smashLaunch(tokens);
			printf("\n");
		}
		fclose(input);
	}

	return (0);
}
