#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sstream>
#include <vector>

#define MAX_LETTERS 100
#define MAX_COMMS 100

void init_shell() {
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

int takeUserInput(std::string &inputArgs){
    std::cout << "\n>>>"; 
    std::getline(std::cin, inputArgs);
    if (!inputArgs.empty()) {
        return 1;
    }
    return 0;
}

void parseString(std::string &inputArgs, std::vector<std::string> &parsedArgs){
    char delimeter = ' ';

    std::istringstream stream(inputArgs);
    std::string token;
    while(getline(stream, token, delimeter)){
        parsedArgs.push_back(token);
    }
}

int processInput(std::string &inputArgs, std::vector<std::string> &parsedArgs, std::vector<std::string> &parsedArgsPiped){
    //determines if built-in, system, or piped
    parseString(inputArgs, parsedArgs);
    
    return 0;
}

int main(){
    std::string inputArgs;
    std::vector<std::string> parsedArgs, parsedArgsPiped;
    int exec_flag = 0;
    init_shell();

    while(1){
        printDir();
        if (takeUserInput(inputArgs)){
            std::cout << inputArgs << "\n"; //testing
            exec_flag = processInput(inputArgs, parsedArgs, parsedArgsPiped);
            for (std::string &arg : parsedArgs) { //testing
                std::cout << arg << "\n";
            }
            parsedArgs.clear();
            parsedArgsPiped.clear();
        }
    }

    return 0;
}