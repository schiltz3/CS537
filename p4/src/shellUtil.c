#include "shellUtil.h"

char *smashReadLine(FILE *stdhi)
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

	printf("Input:%s\n", input);

	return input;
}

#define STARTING_TOK_BUFSIZE 64
#define TOKEN_DELIMINATORS " \t\n\r"

char **smashSplitLine(char *line)
{
	if (line == NULL)
	{
		fprintf(stderr, "NULL Pointer\n");
		exit(EXIT_FAILURE);
	}

	int bufSize = STARTING_TOK_BUFSIZE;
	int bufIndex = 0;

	char **tokens = malloc(bufSize * sizeof(char *));
	char *token;

	if (tokens == NULL)
	{
		perror("Malloc");
		exit(EXIT_FAILURE);
	}

	while ((token = strsep(&line, TOKEN_DELIMINATORS)) != NULL)
	{
		if (isempty(token))
		{
			continue;
		}
		printf("Token:%s\n", token);
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
	return tokens;
}

int smashLaunch(char **args)
{
	pid_t pid;
	pid_t wpid;
	int status;
	pid = fork();
	if (pid == 0)
	{
		if (execvp(args[0], args) == -1)
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

int smashCommand(char **tokens)
{
	if (tokens == NULL)
	{
		fprintf(stderr, RED "NULL passed to smashCommand" NC);
		exit(EXIT_FAILURE);
	}
	int tokenLen = tokenLength(tokens);
	printf("TokenLen:%d\n",tokenLen);

	if (strcmp(tokens[0], "exit") == 0)
	{
		printf("\nEXITING SMASH\n");
		exit(EXIT_SUCCESS);
	}
	// cd command
	else if (strcmp(tokens[0], "cd") == 0)
	{
		printf("Change dir\n");
		if (tokenLen <2 || tokenLen > 2)
		{
			fprintf(stderr, RED "cd [path]\n"NC);
			return 1;
		}
		if (chdir(tokens[0]) != 0)
		{
			perror("chdir");
			exit(EXIT_FAILURE);
		}
		return 1;
	}
	// Path commands
	else if (strcmp(tokens[0], "path") == 0)
	{
		if(tokens[1] == NULL){
			fprintf(stderr, RED"Path [add], [remove], [clear]\n"NC);
			return 1;
		}
		printf("Path:\n%s\n",path);
		if (strcmp(tokens[1], "add") == 0)
		{
			if(tokens[2] == NULL){
				fprintf(stderr, RED "No path provided\n"NC);
				return 1;
			}
			int pathLen = 0;
			if(path != NULL){
				pathLen = strlen(path);
			}

			//TODO: Create path string from tokens[2]
			//TODO: concatinate path here

			return 1;
		}
		else if (strcmp(tokens[1], "remove") == 0)
		{
			if(tokens[2] == NULL){
				fprintf(stderr, RED "No path provided\n"NC);
				return 1;
			}
			//TODO: Search form and remove tokens[2]
			return 1;
		}
		else if (strcmp(tokens[1], "clear") == 0)
		{
			if(path != NULL){
				free(path);
			}
			return 1;
		}
		else{
			fprintf(stderr, RED "Incorrect path argument%s\n"NC, tokens[1]);
			return 1;
		}
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