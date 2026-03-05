#define MAXBUF 256  // max number of characters allowed on command line
#define MAXPATH 4096

// TODO: Any global variables go below

// TODO: Function declarations go below
int handleCmd(char *input);
char** findPath(char **cmd);
void runCmd(char **cmd);
void forkAndRun(char **cmd);
void runCd(char **cmd);
char** split(char *str, char *delim);
int countDelimiter(char *str, char delimiter);
void trim(char *str);
