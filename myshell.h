void clear();

void printEnv();

void listDirContent(char *dirCmd, char *dirLocation);

void changeDirectory(char *pwdCmd, char *oldPwdCmd, char *newDirectory);

void reapChild(int sig);

void forkAndExecute(char *commandIp[10]);

void separateCommandAndArgs(char *cmdLineIp);

int decodeAndExecute(char *cmdLineIp, char *pwdCmd, char *oldPwdCmd, char *dirCommand);
