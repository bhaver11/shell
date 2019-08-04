#include<stdio.h>
#include<string.h>
#include<unistd.h>

extern char **environ; //pointer to array of pointers to environment strings

char* internalCommands[] = {"clr","dir", "env","quit"}; //internal commands which are executed using c functions
int commandIndex;

void clear() {
    /*The sequence of special characters '\33[3J\33[H\33[2J' is ascii escape sequence
    It essentially brings the cursor to the top left corner of the screen and clears the screen
    Discovered it by tracing the system calls made by 'clear' command in linux */
    write(1,"\33[3J\33[H\33[2J",11); 
}

void printEnv() {
    printf("Printing Environment Variables\n");
    for(int i = 0; i < sizeof(*environ)/sizeof(**environ); i++) 
    {
        printf("%s\n",*(environ + i));
    }
}

void main() {
    char commandLineIp[20];
    char* command[5];
    char cwd[50];
    
    while(1) {
        getcwd(cwd,sizeof(cwd));
        printf("Welcome to myshell, Enter a command to execute or ask a question..\n>");
        if(cwd!=NULL)
        printf("%s$",cwd);
        scanf("%[^\n]",commandLineIp);
        getchar();

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
            clear();
            break;
        case 1:
            printf("Changing directory to : %s",command[1]);
            chdir(command[1]);
            break;
        case 2:
            printEnv();
            break;
        case 3:
            _exit(0);
            break;
        default:
            break;
        }
    }
}


