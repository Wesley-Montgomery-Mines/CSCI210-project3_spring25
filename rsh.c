#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define N 12

//This is the environment pointer for posix_spawnp
extern char **environ;

//This is the list of commands
char *allowed[N] = {
    "cp", "touch", "mkdir", "ls", "pwd", "cat", "grep", "chmod", "diff",
    "cd", "exit", "help"
};

//This returns 1 if the command is allowed or 0 otherwise
int isAllowed(const char *cmd) {
    for (int i = 0; i < N; i++) {
        if (strcmp(cmd, allowed[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

//This prints the list of allowed commands with numbering
void printHelp() {
    printf("The allowed commands are:\n");
    for (int i = 0; i < N; i++) {
        printf("%d: %s\n", i + 1, allowed[i]);
    }
}

int main() {
    char line[256]; //This is the input buffer

    while (1) {
        //This will display the prompt to stderr
        fprintf(stderr, "rsh>");

        //This will read a line from input
        if (fgets(line, sizeof(line), stdin) == NULL) continue;

        //This will remove the trailing newline
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

        //This will skip empty input
        if (strlen(line) == 0) continue;

        //This will tokenize the input into argv array
        char *argv[64];
        int argc = 0;
        char *token = strtok(line, " ");
        while (token != NULL && argc < 63) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;

        //This will check if command is allowed
        if (!isAllowed(argv[0])) {
            printf("NOT ALLOWED!\n");
            continue;
        }

        //This is exit
        if (strcmp(argv[0], "exit") == 0) {
            return 0;
        }

        //This is help
        else if (strcmp(argv[0], "help") == 0) {
            printHelp();
        }

        //This is cd
        else if (strcmp(argv[0], "cd") == 0) {
            if (argc > 2) {
                printf("-rsh: cd: too many arguments\n");
            } else if (argc == 2) {
                if (chdir(argv[1]) != 0) {
                    perror("-rsh: cd");
                }
            }
        }

        //This is the command for posix_spawnp
        else {
            pid_t pid;
            int status;

            //This will spawn a child process for the command
            if (posix_spawnp(&pid, argv[0], NULL, NULL, argv, environ) == 0) {
                waitpid(pid, &status, 0); //This will make the child wait for the child process
            } else {
                perror("posix_spawnp");
            }
        }
    }

	//This ends the program
    return 0;
}


