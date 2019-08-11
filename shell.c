#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>

extern char **environ; //pointer to array of pointers to environment strings

char* internalCommands[] = {"clr","dir", "env","quit","cd"}; //internal commands which are executed using c functions
int commandIndex;
char commandLineIp[1024];
char commandLineIpTemp[1024];
char* command[5];
char cwd[50];
time_t start,end;
double timeToExeucte;

void clear() {
    /*The sequence of special characters '\33[3J\33[H\33[2J' is ascii escape sequence
    It essentially brings the cursor to the top left corner of the screen and clears the screen
    Discovered it by tracing the system calls made by 'clear' command in linux */
    write(1,"\33[3J\33[H\33[2J",11); 
}

void printEnv() {
    printf("Printing Environment Variables\n");
    int i = 0;
    while(*(environ + i) != NULL) {
        printf("%s\n",*(environ + i));
        i++;
    }
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
            printf("PWD before change : %s\n",getenv("PWD"));
            printf("OLDPWD before change : %s\n",getenv("OLDPWD"));
            strcpy(oldPwdCmd,"OLDPWD=");
            strcat(oldPwdCmd,cwd);
            strcpy(pwdCmd,"PWD=");
            strcat(pwdCmd,newCwd);
            putenv(pwdCmd);
            putenv(oldPwdCmd);
            strcpy(cwd,newCwd);
            printf("\nPWD after change : %s\n",getenv("PWD"));
            printf("OLDPWD after change : %s\n",getenv("OLDPWD"));
        }else {
                printf("Current directory : %s\n",cwd);
            }
    }
    else
        printf("Current directory : %s\n",cwd);
}

void main() {
    char *pwdCmd = malloc(sizeof(char)*100);
    char *oldPwdCmd = malloc(sizeof(char)*100);
    char *dirCommand = malloc(sizeof(char)*100);
    printf("Welcome to myshell, Enter a command to execute");
    while(1) {
        strcpy(dirCommand, "ls -l ");
        for(int i = 0; i<5; i++) {
            command[i] = NULL;
        }
        getcwd(cwd,sizeof(cwd));
        printf("\n%s>",cwd);
        scanf("%[^\n]",commandLineIp);
        getchar();
        strcpy(commandLineIpTemp,commandLineIp); //strtok() modifies the original string hence a copy is saved
        command[0] = strtok(commandLineIp," ");
        int i = 0;
        while(command[i]!= NULL) {
            i++;
            command[i] = strtok(NULL, " ");
        }
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
            start = clock();
            system(commandLineIpTemp);
            end = clock();
            break;
        }
        timeToExeucte = ((double)(end - start))/CLOCKS_PER_SEC;
        printf("Time taken : %lf",timeToExeucte);
    }
}