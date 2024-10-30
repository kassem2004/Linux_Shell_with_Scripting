#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sstream>
#include <vector>
#include <sys/wait.h>
#include <readline/history.h>
#include <readline/readline.h>

#define MAX_LETTERS 100
#define MAX_COMMS 100

//char *cmd_history[50];

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

/*void add_his(char * arg, int count){
    cmd_history[count] = arg;
}*/

int takeUserInput(std::string &inputArgs, int count){
    //std::cout << "\n>>>"; 
    //std::getline(std::cin, inputArgs);
    inputArgs = readline("\n>>>");
    if (!inputArgs.empty()) {
        add_history(strdup(inputArgs.c_str()));
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
        sleep(1);
        exit(0);
    }
}

void execSys(std::vector<std::string> &parsedArgs){
    pid_t c_pid = fork();
    int veclen = parsedArgs.size();
    char *parsedParsedArgs[veclen];
    for(int i = 0; i < veclen; i++){
        parsedParsedArgs[i] = (char *)parsedArgs[i].c_str();
    }

    parsedParsedArgs[veclen] = NULL;
  
    if (c_pid == -1) { 
        perror("fork"); 
        exit(EXIT_FAILURE); 
    } else if (c_pid > 0) {  
        wait(NULL);
    } else {
        //USE EXECVP BECAUSE YOU NEED TO USE FIRST WORD, THEN REST OF WORD!!! COMMENTED TO REMIND MYSELF WHEN I COME BACK!
        execvp(parsedArgs[0].c_str(), parsedParsedArgs); 
    }
}

void execPiped(std::vector<std::string> &parsedArgs) {
    int veclen = parsedArgs.size();
    int pipeIndex = -1;

    for (int i = 0; i < veclen; i++) {
        if (parsedArgs[i] == "|") {
            pipeIndex = i;
            break;
        }
    }

    if (pipeIndex == -1) { 
        std::cerr << "No pipe '|' found in the input.\n";
        return;
    }

    char **beforePipe = new char*[pipeIndex + 1];
    for (int i = 0; i < pipeIndex; i++) {
        beforePipe[i] = strdup(parsedArgs[i].c_str());
    }
    beforePipe[pipeIndex] = nullptr; 

    char **remainingWordsAfterPipe = new char*[veclen - pipeIndex];
    for (int i = pipeIndex + 1; i < veclen; i++) {
        remainingWordsAfterPipe[i - pipeIndex - 1] = strdup(parsedArgs[i].c_str()); //to remember, c_string returns temp pointer, so I had to allocate 
    }                                                                               // memory and duplicate what it pointed to in that memory, because once you fork,
    remainingWordsAfterPipe[veclen - pipeIndex - 1] = nullptr;                      //the new process might have access to different memory

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t child_a = fork();

    if (child_a == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (child_a > 0) {
        // Parent process
    } else {
        // Child A code
        close(pipefd[0]); 
        dup2(pipefd[1], STDOUT_FILENO);     
        close(pipefd[1]); 
        execvp(beforePipe[0], beforePipe);
    }

    pid_t child_b = fork();
    
    if (child_b == 0) {
        // Child B code
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO); 
        close(pipefd[0]); 
        execvp(remainingWordsAfterPipe[0], remainingWordsAfterPipe);
    }

    // Parent code
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(child_b, NULL, 0);

    for (int i = 0; i < pipeIndex; i++) {
        free(beforePipe[i]);
    }
    delete[] beforePipe;

    for (int i = 0; i < (veclen - pipeIndex - 1); i++) {
        free(remainingWordsAfterPipe[i]);
    }
    delete[] remainingWordsAfterPipe;
}

int main(){
    std::string inputArgs;
    std::vector<std::string> parsedArgs;
    int exec_flag = 0;
    int count = 0;
    init_shell();

    while(1){
        printDir();
        if (takeUserInput(inputArgs, count)){
            exec_flag = processInput(inputArgs, parsedArgs);

            if(exec_flag == 0){
                execBuiltIn(parsedArgs);
            } else if (exec_flag == 1){
                execSys(parsedArgs);
            } else {
                execPiped(parsedArgs);
            }
            parsedArgs.clear();
        }
        count++;
        if (count == 49){
            count = 0;
        }
    }

    return 0;
}