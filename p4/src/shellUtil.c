#include "shellUtil.h"

lines_s *smashReadLine(FILE *stdhi)
{

	char *input = NULL;
	size_t inputLen = 0;
	ssize_t charsRead = 0;

	charsRead = getline(&input, &inputLen, stdhi);
	if (errno != 0)
	{
		perror("getline");
		free(input);
		exit(EXIT_FAILURE);
	}

	if (feof(stdhi))
	{
		printf("EOF\n");
		free(input);
		exit(EXIT_SUCCESS);
	}

	input[charsRead - 1] = '\0';

	lines_s *lines = smashSplitLine(input, ";");

	return lines;
}

lines_s *smashSplitLine(char *line, char *delims)
{
	if (line == NULL)
	{
		fprintf(stderr, "NULL Pointer\n");
		exit(EXIT_FAILURE);
	}

	int bufSize = STARTING_TOK_BUFSIZE;
	int bufIndex = 0;

	lines_s *lines = malloc(sizeof(lines_s));
	char **tokens = malloc(bufSize * sizeof(char *));
	char *token;

	if (tokens == NULL)
	{
		perror("Malloc");
		exit(EXIT_FAILURE);
	}
	if (lines == NULL)
	{
		perror("Malloc");
		exit(EXIT_FAILURE);
	}

	while ((token = strsep(&line, delims)) != NULL)
	{
		if (isempty(token))
		{
			continue;
		}
		tokens[bufIndex] = token;
		++bufIndex;
		if (bufIndex == bufSize)
		{
			bufSize *= 2;
			tokens = realloc(tokens, bufSize);
			if (tokens == NULL)
			{
				perror("Realloc");
				exit(EXIT_FAILURE);
			}
		}
	}
	tokens[bufIndex] = NULL;

	lines->lines = tokens;
	lines->len = bufIndex;

	return lines;
}

int smashCommand(lines_s *tokens, lines_s *path)
{
	// Verify tokens not null
	if (tokens == NULL || path == NULL)
	{
		fprintf(stderr, RED "NULL passed to smashCommand" NC);
		exit(EXIT_FAILURE);
	}

	// Caluclate number of tokens
	// printf("Number of tokens:%d\n", tokens->len);

	// exit
	if (strcmp(tokens->lines[0], "exit") == 0)
	{
		printf("\nEXITING SMASH\n");
		exit(EXIT_SUCCESS);
	}

	// cd command
	else if (strcmp(tokens->lines[0], "cd") == 0)
	{
		// Verify path exists
		if (tokens->lines[1] == NULL)
		{
			fprintf(stderr, RED "NULL Passed to cd\n" NC);
			return 1;
		}

		// Verify number of arguments
		if (tokens->len < 2 || tokens->len > 2)
		{
			fprintf(stderr, RED "cd [path]\n" NC);
			return 1;
		}

		// Change directory
		if (chdir(tokens->lines[1]) != 0)
		{
			perror("chdir");
			exit(EXIT_FAILURE);
		}
		return 1;
	}

	// Path commands
	else if (strcmp(tokens->lines[0], "path") == 0)
	{
		// Path default
		// Verify path arg exists
		if (tokens->lines[1] == NULL)
		{
			fprintf(stderr, RED "Path [add], [remove], [clear]\n" NC);
			return 1;
		}
		// Path add
		if (strcmp(tokens->lines[1], "add") == 0)
		{
			// Verify path add arg exists
			if (tokens->lines[2] == NULL)
			{
				fprintf(stderr, RED "No path provided\n" NC);
				return 1;
			}

			addToPath(path, tokens->lines[2]);

			return 1;
		}

		// Path remove
		else if (strcmp(tokens->lines[1], "remove") == 0)
		{
			// Verify path remove arg exists
			if (tokens->lines[2] == NULL)
			{
				fprintf(stderr, RED "No path provided\n" NC);
				return 1;
			}
			removeFromPath(path, tokens->lines[2]);
			return 1;
		}

		// Path clear
		else if (strcmp(tokens->lines[1], "clear") == 0)
		{
			free(path->lines);
			path->lines = NULL;
			path->size = 0;
			path->len = 0;
			return 1;
		}

		// Path incorrect arg
		else
		{
			fprintf(stderr, RED "Incorrect path argument%s\n" NC, tokens->lines[1]);
			return 1;
		}
	}
	return 0;
}

int smashLaunch(lines_s *args, lines_s *path)
{
	// TODO: check for & to run in paralell
	pid_t pid;
	pid_t wpid;
	int status;
	pid = fork();
	if (pid == 0)
	{
		// TODO: refactor to use execv and iterate over path
		int ret = execvp(args->lines[0], args->lines);
		if (ret == -1)
		{
			perror("execvp");
			exit(EXIT_FAILURE);
		}
	}
	else if (pid < 0)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else
	{
		do
		{
			wpid = waitpid(pid, &status, WUNTRACED);
			if (wpid == -1)
			{
				perror("waitpid");
				exit(EXIT_FAILURE);
			}
		} while (WIFEXITED(status) == false && WIFSIGNALED(status) == false);
	}
	return 0;
}

bool isempty(const char *s)
{
	while (*s)
	{
		if (!isspace(*s))
			return false;
		s++;
	}
	return true;
}

int tokenLength(char **tokens)
{
	int len = 0;
	while (tokens[len] != NULL)
	{
		len++;
	}
	return len;
}

void printLines(lines_s *lines)
{
	printf("Lines:\n");
	for (int i = 0; i < lines->len; i++)
	{
		printf("%s\n", lines->lines[i]);
	}
}

int addToPath(lines_s *path, char *update)
{
	if (path == NULL || update == NULL)
	{
		perror(RED "Null passed to addToPath\n" NC);
		return -1;
	}

	path->size = path->size + strlen(update);

	path->lines = realloc(path->lines, path->size);
	if (path->lines == NULL)
	{
		perror("realloc");
		exit(EXIT_FAILURE);
	}

	path->len++;

	for (int i = path->len - 1; i > 0; i--)
	{
		path->lines[i] = path->lines[i - 1];
	}

	path->lines[0] = update;

	printLines(path);
	return 0;
}

int removeFromPath(lines_s *path, char *update)
{
	if (path == NULL || update == NULL)
	{
		perror(RED "Null passed to removeFromPath\n" NC);
		return -1;
	}

	bool found = false;

	for (int i = 0; i < path->len; i++)
	{
		if (strcmp(path->lines[i], update) == 0)
		{
			found = true;
			for (int j = i; i < path->len - 1; i++)
			{
				path->lines[j] = path->lines[j + 1];
			}
			path->lines[path->len - 1] = NULL;
			--path->len;
		}
	}

	if (found == false)
	{
		fprintf(stderr, RED "Could not find path" NC);
	}

	return 0;
}