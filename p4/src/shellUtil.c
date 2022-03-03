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
	if (tokens == NULL)
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

			// TODO: Create path strings from tokens[2] to path

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
			// TODO: Search form and remove tokens[2]
			return 1;
		}

		// Path clear
		else if (strcmp(tokens->lines[1], "clear") == 0)
		{
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
	// TODO: check for & to run in paraless
	pid_t pid;
	pid_t wpid;
	int status;
	pid = fork();
	if (pid == 0)
	{
		if (execvp(args->lines[0], args->lines) == -1)
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

int addToPath(lines_s * path, char* update){
	if(path == NULL || update == NULL){
		perror(RED"Null passed to updatePath\n"NC);
		return -1;
	}

//	printf("Update:%s\n",update);
//	printf("Size before:%d\n",path->size);
//	printf("Size of update:%lu\n",strlen(update));
//	printf("Len before:%d\n",path->len);

	path->size = path->size + strlen(update); 

	path->lines = realloc(path->lines, path->size);
	if(path->lines == NULL){
		perror("realloc");
		exit(EXIT_FAILURE);
	}

	path->len++;

	//printf("Size after:%d\n",path->size);
	//printf("Len after:%d\n",path->len);

	path->lines[path->len-1] = update;

	printLines(path);
	return 0;
}