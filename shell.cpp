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

int isPiped(std::vector<std::string> &parsedArgs){
    for (std::string &arg : parsedArgs) { 
        if(!arg.compare("|")){
            return 1;
        };
    }
    return 0;
}

int processInput(std::string &inputArgs, std::vector<std::string> &parsedArgs){
    //determines if built-in, system, or piped
    parseString(inputArgs, parsedArgs);
    if(!parsedArgs[0].compare("cd") || !parsedArgs[0].compare("help") || !parsedArgs[0].compare("exit")){
        return 0;
    } else if (isPiped(parsedArgs)){
        return 2;
    } 
    return 1;
}

void showHelp(){
    std::cout << "\n***WELCOME TO MY SHELL'S HELP***"
                 "\n-Use the shell at your own risk..."
                 "\nList of Commands supported:"
                 "\n>cd"
                 "\n>ls"
                 "\n>exit"
                 "\n>all other general commands available in UNIX shell\n";
}

void execBuiltIn(std::vector<std::string> &parsedArgs){
    if(!parsedArgs[0].compare("cd")){
        if(parsedArgs.size() > 2){
            std::cerr << "cd: too many arguments\n";
        } else {
            if (chdir(parsedArgs[1].c_str()) != 0) {
                perror("cd");
            }
        }
    } else if(!parsedArgs[0].compare("help") && !(parsedArgs.size() > 1)){
        showHelp();
    } else if(!parsedArgs[0].compare("exit") && !(parsedArgs.size() > 1)){
        exit(0);
    }
}

int main(){
    std::string inputArgs;
    std::vector<std::string> parsedArgs;
    int exec_flag = 0;
    init_shell();

    while(1){
        printDir();
        if (takeUserInput(inputArgs)){
            //std::cout << inputArgs << "\n"; //testing
            exec_flag = processInput(inputArgs, parsedArgs);
            /*for (std::string &arg : parsedArgs) { //testing
                std::cout << arg << "\n";
            }*/

            if(exec_flag == 0){
                execBuiltIn(parsedArgs);
            } else if (exec_flag == 1){
                //execSys(parsedArgs);
            } else {
                //execPiped(parsedArgs);
            }

            parsedArgs.clear();
        }
    }

    return 0;
}