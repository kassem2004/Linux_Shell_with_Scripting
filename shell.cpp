#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_LETTERS 100
#define MAX_COMMS 100
#define clear() printf("\033[H\033[J")

void init_shell() {
    clear();
    std::cout << "\n******************************************";
    std::cout << "\n*****************MY SHELL*****************";
    std::cout << "\n******************************************";
    char* username = getenv("USER");
    std::cout << "\n\n\nWelcome: @" << username << "\n";
}

void printDir(){
    char cwd[100];
    getcwd(cwd, sizeof(cwd));
    std::cout << "Dir: " << cwd;
}

int takeUserInput(char *inputArgs){
    char input[MAX_LETTERS];
    std::cout << "\n>>>"; 
    std::cin.getline(input, MAX_LETTERS);
    if(strlen(input) != 0){
        strcpy(inputArgs, input);
        return 0;
    }
    return 1;
}

int main(){
    char inputArgs[MAX_LETTERS], *parsedArgs[MAX_COMMS], *parsedArgsPiped[MAX_COMMS];
    int exec_flag = 0;
    init_shell();

    while(1){
        printDir();
        if (takeUserInput(inputArgs)){
            std::cout << "Testing!";
        }
    }

    return 0;
}