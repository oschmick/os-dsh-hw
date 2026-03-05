/*
 * dsh.c
 *
 *  Created on: Aug 2, 2013
 *      Author: chiu
 */
#include "dsh.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

/**
 * Handle command logic
 *
 * @param input User input (command)
 * @return 1 if command is "exit", 0 otherwise
 */
int handleCmd(char *input) {
    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "exit") == 0) {
        return 1;
    }

    char **cmds = split(input, " ");
    if (cmds != NULL) runCmd(cmds);
    for (int j = 0; cmds[j] != NULL; j++) {
        free(cmds[j]);
    }
    free(cmds);
    return 0;
}

/**
 * Finds the path of the given cmd
 *
 * @param cmd The command
 * @return The command path
 */
char** findPath(char **cmd) {
    char *pathVar = getenv("PATH");
    if (pathVar == NULL) {
        return NULL;
    }
    char *pathCopy = strdup(pathVar);
    char *foundPath = cmd[0];
    char *token = strtok(pathCopy, ":");
    char fullPath[MAXPATH];

    while (token != NULL) {
        snprintf(fullPath, MAXPATH, "%s/%s", token, cmd[0]);
        if (access(fullPath, F_OK | X_OK) == 0) {
            foundPath = strdup(fullPath);
            free(cmd[0]);
            break;
        }
        token = strtok(NULL, ":");
    }

    free(pathCopy);
    char **path = cmd;
    path[0] = foundPath;
    return path;
}

/**
 * Runs the command given the path
 *
 * @param cmd The command
 */
void runCmd(char **cmd) {
    if (strcmp(cmd[0], "pwd") == 0) {
        char cwd[MAXPATH];
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
    } else if (strcmp(cmd[0], "cd") == 0) {
        runCd(cmd);
    } else {
        if (strncmp(cmd[0], "/", 1) == 0) {
            forkAndRun(cmd);
        } else {
            char **path = findPath(cmd);
            forkAndRun(path);
            //free(path);
        }
    }
}

/**
 * Forks, then starts the new process in the child process
 *
 * @param cmd The command to start
 */
void forkAndRun(char **cmd) {
    if (cmd[0] != NULL && access(cmd[0], F_OK | X_OK) == 0) {
        int i = 0;
        int background = 0;

        while (cmd[i] != NULL) {
            i++;
        }
        if (strcmp(cmd[i-1], "&") == 0) {
            background = 1;
            cmd[i-1] = NULL;
        }

        pid_t pid = fork();
        if (pid != 0) {
            // PARENT PROCESS
            if (!background) {
                waitpid(pid, NULL, 0);
            }
        } else {
            // CHILD PROCESS
            execv(cmd[0], cmd);
            printf("Error: execv failed\n");
            _exit(EXIT_FAILURE);
        }
    } else {
        printf("Error: %s not found!\n", cmd[0]);
    }
}

/**
 * Runs the builtin cd command
 *
 * @param cmd The command and parameters
 */
void runCd(char **cmd) {
    if (cmd[1] == NULL) {
        char *homeDir = getenv("HOME");
        if (chdir(homeDir) != 0) {
            printf("%s: no such file or directory", homeDir);
        }
    } else {
        if (chdir(cmd[1]) != 0) {
            printf("%s: no such file or directory", cmd[1]);
        }
    }
}

/**
 * Helper function to split the given string by the given delimiter
 *
 * @param str String
 * @param delimiter Delimiter
 * @return Char array of the split string
 */
char** split(char *str, char *delim) {
    int numTokens = countDelimiter(str, *delim)+1;
    char **array = malloc((numTokens+1)*sizeof(char*));

    char *token;
    int i = 0;
    token = strtok(str, delim);

    while (token != NULL && i < numTokens) {
        array[i] = malloc(strlen(token)+1);
        if (array[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(array[j]);
            }
            free(array);
            exit(1);
        }
        strcpy(array[i], token);
        token = strtok(NULL, delim);
        i++;
    }

    array[i] = NULL;

    return array;
}

/**
 * Helper function to count the amount of times the given delimiter appears in the string
 *
 * @param str String
 * @param delimiter Delimiter to search for
 * @return Number of times delimiter appeared
 */
int countDelimiter(char *str, char delimiter) {
    int count = 0;

    for (int i=0; str[i] != '\0'; i++) {
        if (str[i] == delimiter) {
            count++;
        }
    }

    return count;
}

/**
 * Helper function to trim the leading and trailing whitespace from a string
 *
 * @param str String
 */
void trim(char *str) {
    int i = 0;
    int begin = 0;
    int end = strlen(str)-1;

    while (isspace((unsigned char) str[begin])) {
        begin ++;
    }

    while (end >= begin && isspace((unsigned char) str[end])) {
        end--;
    }

    for (i=begin; i<=end; i++) {
        str[i-begin] = str[i];
    }

    str[i-begin] = '\0';
}