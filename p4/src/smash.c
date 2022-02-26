#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define RED "\x1B[0;31m"
#define NC "\x1B[0m"

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
			char *input = NULL;
			size_t inputLen = 0;
			ssize_t charsRead = 0;
			printf("smash>");
			charsRead = getline(&input, &inputLen, stdin);

			input[charsRead-1] = '\0';

			printf("Input:%s\n", input);

			if (errno != 0)
			{
				fprintf(stderr, RED "\nError[%d]\n" NC, errno);
				fprintf(stderr, "%s\n", strerror(errno));
				fprintf(stderr, RED "\nFILE: %s\nLINE: %d\n" NC, __FILE__, __LINE__);
				free(input);
				exit(EXIT_FAILURE);
			}
			printf("Strcmp: %d\n", strcmp(input, "exit"));

			if (strcmp(input, "exit") == 0 || feof(stdin) != 0)
			{
				free(input);
				exit(EXIT_SUCCESS);
			}

			free(input);
		}
	}
	else if (argc == 2)
	{
	}

	return (0);
}
