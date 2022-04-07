#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <wait.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define STARTING_TOK_BUFSIZE 64
#define TOKEN_DELIMINATORS " \t\n\r"

#define RED "\x1B[0;31m"
#define NC "\x1B[0m"

typedef struct Lines
{
  char **lines;
  int len;
  int size;
} lines_s;

/**
 * @brief read a line from a file
 *
 * @param stdhi file pointer to read from
 * @return char* line read form file
 */
lines_s *smashReadLine(FILE *stdhi);
lines_s *smashSplitLine(char *line, char *delims);
int smashCommand(lines_s *tokens, lines_s *path);
int smashLaunch(lines_s *args, lines_s *path);
bool isempty(const char *s);
int tokenLength(char **tokens); // TODO: remove if not needed
void printLines(lines_s *lines);
int addToPath(lines_s *path, char *update);
int removeFromPath(lines_s *path, char *update);
int redirect(lines_s *args, lines_s *path);

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
		//printf("File to read from: %s\n\n", argv[1]);
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
		if (input == stdin)
		{
			printf("smash>");
		}

		// read lines from the input
		lines_s *lines = smashReadLine(input);

		// Loop over Lines
		for (int i = 0; i < lines->len; i++)
		{

			// Ignore empty line
			if (isempty(lines->lines[i]))
			{
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
				//fprintf(stderr, RED "smashCommand Error" NC);
				exit(EXIT_FAILURE);
			}

			redirect(tokens, &path);
			// execute any system commands
			// smashLaunch(tokens, &path);

		}
		if (input == stdin)
		{
			printf("\n");
		}
	}

	// Close input
	fclose(input);

	return (0);
}

lines_s *smashReadLine(FILE *stdhi)
{

	char *input = NULL;
	size_t inputLen = 0;
	ssize_t charsRead = 0;

	charsRead = getline(&input, &inputLen, stdhi);
	if (errno != 0)
	{
		//		perror("getline");
		fprintf(stderr, RED "An error has occurred\n" NC);
		free(input);
		exit(EXIT_FAILURE);
	}

	if (feof(stdhi))
	{
		// printf("EOF\n");
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
		// fprintf(stderr, "NULL Pointer\n");
		fprintf(stderr, RED "An error has occurred\n" NC);
		exit(EXIT_FAILURE);
	}

	int bufSize = STARTING_TOK_BUFSIZE;
	int bufIndex = 0;

	lines_s *lines = malloc(sizeof(lines_s));
	char **tokens = malloc(bufSize * sizeof(char *));
	char *token;

	if (tokens == NULL)
	{
		// perror("Malloc");
		fprintf(stderr, RED "An error has occurred\n" NC);
		exit(EXIT_FAILURE);
	}
	if (lines == NULL)
	{
		// perror("Malloc");
		fprintf(stderr, RED "An error has occurred\n" NC);
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
				// perror("Realloc");
				fprintf(stderr, RED "An error has occurred\n" NC);
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
		// fprintf(stderr, RED "NULL passed to smashCommand" NC);
		fprintf(stderr, RED "An error has occurred\n" NC);
		exit(EXIT_FAILURE);
	}

	// Caluclate number of tokens
	// printf("Number of tokens:%d\n", tokens->len);

	// exit
	if (strcmp(tokens->lines[0], "exit") == 0)
	{
		exit(EXIT_SUCCESS);
	}

	// cd command
	else if (strcmp(tokens->lines[0], "cd") == 0)
	{
		// Verify path exists
		if (tokens->lines[1] == NULL)
		{
			// fprintf(stderr, RED "NULL Passed to cd\n" NC);
			fprintf(stderr, RED "An error has occurred\n" NC);
			return 1;
		}

		// Verify number of arguments
		if (tokens->len < 2 || tokens->len > 2)
		{
			// fprintf(stderr, RED "cd [path]\n" NC);
			fprintf(stderr, RED "An error has occurred\n" NC);
			return 1;
		}

		// Change directory
		if (chdir(tokens->lines[1]) != 0)
		{
			// perror("chdir");
			fprintf(stderr, RED "An error has occurred\n" NC);
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
			// fprintf(stderr, RED "Path [add], [remove], [clear]\n" NC);
			fprintf(stderr, RED "An error has occurred\n" NC);
			return 1;
		}
		// Path add
		if (strcmp(tokens->lines[1], "add") == 0)
		{
			// Verify path add arg exists
			if (tokens->lines[2] == NULL)
			{
				// fprintf(stderr, RED "No path provided\n" NC);
				fprintf(stderr, RED "An error has occurred\n" NC);
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
				// fprintf(stderr, RED "No path provided\n" NC);
				fprintf(stderr, RED "An error has occurred\n" NC);
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
			// fprintf(stderr, RED "Incorrect path argument%s\n" NC, tokens->lines[1]);
			fprintf(stderr, RED "An error has occurred\n" NC);
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
	// Child
	if (pid == 0)
	{
		int ret = 0;
		// Iterate over path looking for executable
		for (int i = 0; i < path->len; i++)
		{
			// Create path to executable to try
			char *lookup_path = malloc(strlen(path->lines[i]) + strlen(args->lines[0]));
			if (lookup_path == NULL)
			{
				// perror("malloc");
				fprintf(stderr, RED "An error has occurred\n" NC);
				exit(EXIT_FAILURE);
			}

			sprintf(lookup_path, "%s/%s", path->lines[i], args->lines[0]);

			// If try works
			if (access(lookup_path, F_OK) == 0)
			{

				// int redir = redirect(args);

				// printf("Run:%s\n", lookup_path);
				ret = 1;

				// Run the executable
				ret = execv(lookup_path, args->lines);
				free(lookup_path);
				break;
			}
			else
			{
				// printf("Access denied\n");
				fprintf(stderr, RED "An error has occurred\n" NC);
			}

			free(lookup_path);
		}
		if (ret == 0)
		{
			// printf("Failed to find program in path");
			fprintf(stderr, RED "An error has occurred\n" NC);
		}
		if (ret == -1)
		{
			// perror("execv");
			fprintf(stderr, RED "An error has occurred\n" NC);
			exit(EXIT_FAILURE);
		}
	}
	// Failed fork
	else if (pid < 0)
	{
		// perror("fork");
		fprintf(stderr, RED "An error has occurred\n" NC);
		exit(EXIT_FAILURE);
	}
	// Child
	else
	{
		do
		{
			wpid = waitpid(pid, &status, WUNTRACED);
			if (wpid == -1)
			{
				// perror("waitpid");
				fprintf(stderr, RED "An error has occurred\n" NC);
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
		// perror(RED "Null passed to addToPath\n" NC);
		fprintf(stderr, RED "An error has occurred\n" NC);
		return -1;
	}

	path->size = path->size + strlen(update);

	path->lines = realloc(path->lines, path->size);
	if (path->lines == NULL)
	{
		// perror("realloc");
		fprintf(stderr, RED "An error has occurred\n" NC);
		exit(EXIT_FAILURE);
	}

	path->len++;

	for (int i = path->len - 1; i > 0; i--)
	{
		path->lines[i] = path->lines[i - 1];
	}

	path->lines[0] = update;

	// printLines(path);
	return 0;
}

int removeFromPath(lines_s *path, char *update)
{
	if (path == NULL || update == NULL)
	{
		//	perror(RED "Null passed to removeFromPath\n" NC);
		fprintf(stderr, RED "An error has occurred\n" NC);
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
		// fprintf(stderr, RED "Could not find path" NC);
		fprintf(stderr, RED "An error has occurred\n" NC);
	}

	return 0;
}

int redirect(lines_s *args, lines_s *path)
{
	int redirect = -1;
	bool found_redirect = false;
	for (int i = 0; i < args->len; i++)
	{
		if (strcmp(args->lines[i], ">") == 0)
		{
			if (found_redirect == true)
			{
				fprintf(stderr, RED "Multiple redirects found\n" NC);
				return -1;
			}
			redirect = i;
			found_redirect = true;
		}
	}
	if (found_redirect == true)
	{
		if (redirect == 0)
		{
			fprintf(stderr, RED "Can't start with redirect\n" NC);
			return -1;
		}
		if (args->lines[redirect + 1] == NULL || args->lines[redirect + 2] != NULL)
		{
			fprintf(stderr, RED "Must have one arg after redirect\n" NC);
			return -1;
		}
		fflush(stdout);
		fflush(stderr);
		int stdio_backup = dup(1);

		//printf("Open:%s\n", args->lines[redirect + 1]);
		int new = open(args->lines[redirect + 1], O_WRONLY | O_CREAT | O_TRUNC);

		if (new < 0)
		{
			fprintf(stderr, RED "Can't open file\n" NC);
			return -1;
		}

		dup2(new, 1);
		close(new);

		for (int i = redirect; i < args->len; i++)
		{
			args->lines[i] = NULL;
		}
		args->len = redirect;

		//printf("Trimmed lines\n");
		// printLines(args);

		smashLaunch(args, path);

		fflush(stdout);
		dup2(stdio_backup, 1);
		close(stdio_backup);
	}
	else
	{
		smashLaunch(args, path);
	}
	return 0;
}