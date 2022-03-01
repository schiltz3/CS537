#include "shellUtil.h"

char *smashReadLine(FILE *stdhi)
{

	char *input = NULL;
	size_t inputLen = 0;
	ssize_t charsRead = 0;

	// printf("smash>");

	// TODO: filter out leading and trailing whitespace
	// TODO: filter out empty lines

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

void smashCommand(char **tokens){

	if(tokens == NULL){
		fprintf(stderr,RED"NULL passed to smashCommand"NC);
		exit(EXIT_FAILURE);
	}
	if (strcmp(tokens[0], "exit") == 0)
	{
		printf("\nEXITING SMASH\n");
		exit(EXIT_SUCCESS);
	}
	else if (strcmp(tokens[0], "cd") == 0){
		//chdir
	}
	else if (strcmp(tokens[0], "add") == 0){}
	else if (strcmp(tokens[0], "remove") == 0){}
	else if (strcmp(tokens[0], "clear") == 0){}
}