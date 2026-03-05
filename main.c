/*
 * main.c
 *
 *  Created on: Mar 17 2017
 *      Author: david
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <string.h>
#include "dsh.h"

#define MAX_PROC 1024

int main(int argc, char *argv[]) {

	// DO NOT REMOVE THE BLOCK BELOW (FORK BOMB PREVENTION) //
	struct rlimit limit;
	limit.rlim_cur = MAX_PROC;
	limit.rlim_max = MAX_PROC;
	setrlimit(RLIMIT_NPROC, &limit);
	// DO NOT REMOVE THE BLOCK ABOVE THIS LINE //

	char *cmdline = malloc(MAXBUF); // buffer to store user input from commmand line

	while (1) {
		printf("dsh> ");
		if (fgets(cmdline, MAXBUF, stdin) == NULL) {
			break;
		}
		trim(cmdline);
		if (cmdline[0] == '\n') {
			continue;
		}
		if (cmdline[0] != '\0') {
			int i = handleCmd(cmdline);
			if (i == 1) {
				break;
			}
		}
	}

	free(cmdline);

	return 0;
}
