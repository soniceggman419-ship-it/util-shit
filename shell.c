#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

int main(void) {
    char line[MAX_LINE];

    while (1) {
        printf("shell$ ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;
        }

        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';

        // Ignore empty input
        if (strlen(line) == 0)
            continue;

        // Split input into arguments
        char *args[MAX_ARGS];
        int argc = 0;

        char *token = strtok(line, " \t");
        while (token != NULL && argc < MAX_ARGS - 1) {
            args[argc++] = token;
            token = strtok(NULL, " \t");
        }
        args[argc] = NULL;

        if (argc == 0)
            continue;

        // Built-in: exit
        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        // Built-in: cd
        if (strcmp(args[0], "cd") == 0) {
            const char *dir = args[1];

            if (dir == NULL) {
                dir = getenv("HOME");
            }

            if (chdir(dir) != 0) {
                perror("cd");
            }

            continue;
        }

        // Create child process
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            // Child: execute command
            execvp(args[0], args);

            // Only reached if execvp fails
            perror("exec");
            exit(EXIT_FAILURE);
        }

        // Parent: wait for child
        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}
