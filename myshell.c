#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<signal.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include "myshell.h"

extern char **environ; //pointer to array of pointers to environment strings

char* internalCommands[] = {"clr","dir", "env","quit","cd"}; //internal commands which are executed using c functions
int commandIndex,status,isBackgroundTask = 0, backgroundProcessCount=0;
int isInternalCmd = 1, isSerialExecution, isParallelExecution;
int isIpFromFile,isOpToFile,appendToFile,isFileError;
int fileMode;
char commandLineIp[1024];
char commandLineIpTemp[1024];
char* command[10];
char* commandMultiple[10];
char cwd[50];
char* message;
char *inputFileName,*outputFileName;
time_t start,end;
double timeToExeucte;
FILE* fileptr;

void clear() {
    /*The sequence of special characters '\33[3J\33[H\33[2J' is ascii escape sequence
    It essentially brings the cursor to the top left corner of the screen and clears the screen
    Discovered it by tracing the system calls made by 'clear' command in linux */
    write(1,"\33[3J\33[H\33[2J",11); 
}

void printEnv() {
    // int i = 0;
    // while(*(environ + i) != NULL) {
    //     printf("%s\n",*(environ + i));
    //     i++;
    // } // changed for part 2
    printf("COURSE=%s\nASSINGMENT=%s\nPWD=%s\n",getenv("COURSE"),getenv("ASSINGMENT"),getenv("PWD"));
}

void listDirContent(char *dirCmd, char *dirLocation) {
    strcat(dirCmd,dirLocation);
    system(dirCmd);
}

void changeDirectory(char *pwdCmd, char *oldPwdCmd, char* newDirectory) {
    char newCwd[50];
    if(newDirectory) {
        if(strcmp(newDirectory,"-") == 0) //TO GO BACK TO PREVIOUS DIRECTORY (oldpwd)
            if(getenv("OLDPWD"))
                strcpy(newDirectory,getenv("OLDPWD"));
        chdir(newDirectory);
        getcwd(newCwd,sizeof(newCwd));
        if(strcmp(newCwd, cwd) !=0) {
            printf("BEFORE:\nOLDPWD=%s\n",getenv("OLDPWD"));
            printf("PWD=%s\n",getenv("PWD"));
            strcpy(oldPwdCmd,"OLDPWD=");
            strcat(oldPwdCmd,cwd);
            strcpy(pwdCmd,"PWD=");
            strcat(pwdCmd,newCwd);
            putenv(pwdCmd);
            putenv(oldPwdCmd);
            strcpy(cwd,newCwd);
            printf("AFTER:\nOLDPWD=%s\n",getenv("OLDPWD"));
            printf("PWD=%s\n",getenv("PWD"));
        }else {
            printf("ERROR: Directory does not exist\n");
            printf("BEFORE: ...\n");
            printf("AFTER: ...\n");
        }
    }
    else
        printf("Current directory : %s\n",cwd);
}

void reapChild(int sig) {
    message = malloc(sizeof(char)*60);
    backgroundProcessCount--;
    pid_t pid;
    pid = wait(NULL);
    if(fileMode)
        sprintf(message,"MyShell: Background process [%d] finished\n",pid);
    else 
        sprintf(message,"\nMyShell: Background process [%d] finished\n%s>",pid,cwd);
    write(1,message,strlen(message)+1);
    if(backgroundProcessCount == 0)
        signal(SIGCHLD,NULL);
}

void forkAndExecute(char* commandIp[10]) {
    char* path = malloc(sizeof(char)*10);
    strcpy(path,"/bin/");
    strcat(path,commandIp[0]);
    
    pid_t pid;
    switch (pid = fork())
    {
    case -1:
        printf("Error while forking !");
        break;
    case 0:
        // child process
        if(isIpFromFile) {
            if(access(inputFileName,R_OK) == 0)
                freopen(inputFileName,"r",stdin);
            else {
                _exit(2);
            }
        }
        if(isOpToFile) {
            if(appendToFile == 1)
                freopen(outputFileName,"a",stdout);
            else
                freopen(outputFileName,"w",stdout);
        }
        status = execv(path, commandIp);
        strcpy(path,"/usr/bin/");
        strcat(path,commandIp[0]);
        status = execv(path, commandIp);
        if(status == -1)
            _exit(1); //if command not found
    default:
        if(isParallelExecution == 0) {
            // parent process
            if(isBackgroundTask == 0) {
                wait(&status);
                if(status == 256) {
                    printf("Error: command does not exist\n");
                }else if(status == 512) {
                    printf("Error: File does not exist\n");
                }
            } else {
                backgroundProcessCount++;
                printf("[%d] %d\n",backgroundProcessCount,pid);
                isBackgroundTask = 0;
                signal(SIGCHLD,reapChild);
            }
        }
        break;
    }
}

void separateCommandAndArgs(char* cmdLineIp) {
    isIpFromFile=isOpToFile=appendToFile=0;
    inputFileName = NULL;
    outputFileName = NULL;
    for(int i = 0; i<10; i++) {
        command[i] = NULL;
    }
    command[0] = strtok(cmdLineIp," ");
    int i = 0;
    while(command[i]!= NULL) {
        i++;
        command[i] = strtok(NULL, " ");
        if(command[i]) {
            if(strcmp(command[i],"&") == 0) {
                isBackgroundTask = 1;
                command[i] = NULL;
            }
            if(command[i] && strcmp(command[i],"<") == 0) {
                isIpFromFile = 1;
                inputFileName = strtok(NULL, " ");
                command[i] = NULL;
                command[i] = strtok(NULL," ");
            }
            if(command[i] && strcmp(command[i],">") == 0) {
                isOpToFile = 1;
                outputFileName = strtok(NULL, " ");
                command[i] = NULL;
            } else {
                if(command[i] && strcmp(command[i],">>") == 0) {
                    isOpToFile = 1;
                    appendToFile = 1;
                    outputFileName = strtok(NULL, " ");
                    command[i] = NULL;
                }
            }
        }
    }
}

int decodeAndExecute(char * cmdLineIp, char * pwdCmd, char* oldPwdCmd, char* dirCommand) {
    isInternalCmd = 1;
    strcpy(dirCommand, "ls -l ");
    separateCommandAndArgs(cmdLineIp);    
    for (commandIndex = 0; commandIndex < sizeof(internalCommands)/sizeof(char*); commandIndex++)
    {
        if(strcmp(command[0],internalCommands[commandIndex])==0)
            break;
    }
    switch (commandIndex)
    {
    case 0:
        start = clock();
        clear();
        end = clock();
        break;
    case 1:
        start = clock();
        command[1] = command[1]?command[1]:""; //when no input to dir is given directory location = ""
        listDirContent(dirCommand,command[1]);
        end = clock();
        break;
    case 2:
        start = clock();
        printEnv();
        end = clock();
        break;
    case 3:
        free(dirCommand);
        _exit(0);
        break;
    case 4:
        start = clock();
        changeDirectory(pwdCmd, oldPwdCmd, command[1]);
        end = clock();
        break;
    default:
        isInternalCmd = 0;
        forkAndExecute(command);
        break;
    }
    if(isInternalCmd) {
        timeToExeucte = ((double)(end - start))/CLOCKS_PER_SEC;
        printf("Time taken : %lfs\n",timeToExeucte);
    }
    return 0;
}

int main(int argc, char** argv) {
    char *pwdCmd = malloc(sizeof(char)*100);
    char *oldPwdCmd = malloc(sizeof(char)*100);
    char *dirCommand = malloc(sizeof(char)*100);
    
    putenv("COURSE=CS_744");
    putenv("ASSINGMENT=ASSINGMENT_1");
    if(argv[1] != NULL) {
        fileMode = 1;
        fileptr = fopen(argv[1],"r");
        if(fileptr == NULL) {
            printf("Error: File does not exist\n");
            return 0;
        }
    } else {
        printf("Welcome to myshell, Enter a command to execute \n");
    }
    while(1) {
        isParallelExecution = isSerialExecution = isBackgroundTask = 0;
        strcpy(commandLineIp,"");
        getcwd(cwd,sizeof(cwd));
        
        if(fileMode) {
            if(fscanf(fileptr,"%[^\n]\n",commandLineIp) == EOF)
            break;
        }else {
            printf("%s>",cwd);
            scanf("%[^\n]",commandLineIp);
            getchar();
        }
        
        if(strcmp(commandLineIp,"") == 0)
            continue;

        strcpy(commandLineIpTemp,commandLineIp); //strtok() modifies the original string hence a copy is saved
        if(strstr(commandLineIp,"&&&") != NULL) {
            isParallelExecution = 1;
            commandMultiple[0] = strtok(commandLineIp,"&&&");
            int j = 0;
            while(commandMultiple[j] != NULL){
                j++;
                commandMultiple[j] = strtok(NULL,"&&&");
            }
            j = 0;
            while(commandMultiple[j] != NULL) {
                separateCommandAndArgs(commandMultiple[j]);
                forkAndExecute(command);
                j++;
            }
            for(int i = 0; i<j; i++) {
                wait(NULL);
            }
        }
        else if(strstr(commandLineIp,"&&") != NULL) {
                isSerialExecution = 1;
                commandMultiple[0] = strtok(commandLineIp,"&&");
                int j = 0;
                while(commandMultiple[j] != NULL){
                    j++;
                    commandMultiple[j] = strtok(NULL,"&&");
                }
                j = 0;
                while(commandMultiple[j] != NULL) {
                    decodeAndExecute(commandMultiple[j],pwdCmd,oldPwdCmd,dirCommand);
                    j++;
                }
            }
            else {
                decodeAndExecute(commandLineIp,pwdCmd,oldPwdCmd,dirCommand);
            }
    }
    free(pwdCmd);
    free(oldPwdCmd);
    free(dirCommand);
    return 0;
}