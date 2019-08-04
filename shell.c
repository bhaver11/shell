#include<stdio.h>
#include<string.h>
#include<unistd.h>

void clear() {
    /*The sequence of special characters '\33[3J\33[H\33[2J' is ascii escape sequence
    It essentially brings the cursor to the top left corner of the screen and clears the screen
    Discovered it by tracing the system calls made by 'clear' command in linux */
    write(1,"\33[3J\33[H\33[2J",11); 
}
int main() {
    char command[20];
    while(1) {
        printf("Welcome to myshell, Enter a command to execute or ask a question..\n>");
        scanf("%[^\n]",command);
        getchar();
        if(strcmp("clr",command) == 0) {
            clear();
        }else {
        }
    }
}


