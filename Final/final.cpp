/*
 * Greg Brown
 * Operating Systems
 * Lab 1
 * October 12, 2020
 * Purpose: To produce a minimalist shell via the use of execvp() and fork() functions
 */

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_ARGS        64
#define MAX_ARG_LEN     16
#define MAX_LINE_LEN    80
#define WHITESPACE      " ,\t\n"

struct command_t
{
    char *name;
    int argc;
    char *argv[MAX_ARGS];
    char *args[4];
};

int parseCommand(char*, struct command_t*);
void readCommand(char*);
void printPrompt();
int callFork(struct command_t*, int*, int&, int&, int&);
void helpPrint();
bool setStructValues(struct command_t*);
void errorMessage(struct command_t*);

int main()
{
    int pid, status, foxPid, chPid;
    char cmdLine[MAX_LINE_LEN];
    struct command_t command;

    while(true)
    {
        printPrompt();
        readCommand(cmdLine);
        parseCommand(cmdLine, &command);

        command.argv[command.argc] = NULL;

        switch(*command.name)
        {
        case 'E':
            {
                printPrompt();
                int i = 1;
                while(command.argv[i] != NULL)
                {
                    std::cout << command.argv[i] << " ";
                    i++;
                }
                std::cout << std::endl;
            }
            break;
        case 'H':
            helpPrint();
            break;
        case 'L':
            std::cout << std::endl;
            setStructValues(&command);
            callFork(&command, &pid, status, foxPid, chPid);
            wait(&status);
            std::cout << std::endl;
            strcpy(command.name, "ls");
            strcpy(command.args[0], command.name);
            strcpy(command.args[1], "-l");
            command.args[2] = NULL;
            callFork(&command, &pid, status, foxPid, chPid);
            wait(&status);
            std::cout << std::endl;
            break;
        case 'Q':
            std::cout << "\n\nshell: Terminating successfully\n";
            return(0);
            break;
        default:
            if(*command.name != '\0')
            {
                if(!setStructValues(&command))
                {
                    callFork(&command, &pid, status, foxPid, chPid);

                    if(*command.name != 'f')
                        wait(&status);
                }
                else
                    errorMessage(&command);
            }
            break;
        }
    }

    return(0);
}

void readCommand(char* buffer)
{
    fgets(buffer, 80, stdin);
}

int parseCommand(char* cLine, struct command_t* cmd)
{
    int argc;
    char** clPtr;

    clPtr = &cLine;
    argc = 0;
    cmd->argv[argc] = (char*) malloc(sizeof(cmd->argv[0]));

    while ((cmd->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL)
    {
        cmd->argv[++argc] = (char*) malloc(MAX_ARG_LEN);
    }

    cmd->argc = argc-1;
    cmd->name = (char*) malloc(sizeof(cmd->argv[0]));
    strcpy(cmd->name, cmd->argv[0]);
    return 1;
}


int callFork(struct command_t* cmd, int* pid, int &status, int &foxPid, int &chPid)
{
    if((*pid = fork()) == 0)
    {
        execvp(cmd->name, cmd->args);
        perror(cmd->name); return -1;
        foxPid = *pid;
    }
    if((*pid = fork()) != 0)
    {
        wait(&status);
        if(foxPid > 0)
        {
            chPid = waitpid(foxPid, &status, WNOHANG);
            if(chPid == foxPid)
            {
                std::cout << "Detecting firefox termination.\n";
                kill(foxPid, SIGTERM); foxPid = 0;
            }
        }
    }
    return 0;
}

void printPrompt()
{
    std::cout << "linux gjb52 |> ";
}

void helpPrint()
{
     std::cout << std::endl << "To use this console, please enter the command you wish to execute with a singular" << std::endl
          << "capital letter (otherwise the command won't be read) followed by" << std::endl
          << "the necessary information, such as filenames*." << std::endl
          << "A list of commands is available below:" << std::endl;

     std::cout << "\tCopy:\t\t" << "C {Origin File Path} {Destination File Path}" << std::endl
          << "\tDelete:\t\t" << "D {File Path}" << std::endl
          << "\tEcho:\t\t" << "E {Text to be Printed}" << std::endl
          << "\tHelp:\t\t" << "H" << std::endl
          << "\tList:\t\t" << "L" << std::endl
          << "\tMake:\t\t" << "M {Destination Path}" << std::endl
          << "\tPrint:\t\t" << "P {File Path}" << std::endl
          << "\tQuit:\t\t" << "Q" << std::endl
          << "\tFirefox:\t" << "S" << std::endl
          << "\tWipe:\t\t" << "W" << std::endl
          << "\tExecute:\t" << "X {Program}" << std::endl;

     std::cout << "\nPlease also note that to ensure the commmands that require file paths work" << std::endl
          << "\tthe path should be the absolute path to avoid the program from" << std::endl
          << "\tselecting a file relative to its location accidentally" << std::endl << std::endl;

     return;
}

bool setStructValues(struct command_t* cmd)
{
    bool errorBool = false;

    cmd->args[0] = (char *) malloc(MAX_ARG_LEN);
    cmd->args[1] = (char *) malloc(MAX_ARG_LEN);
    cmd->args[2] = (char *) malloc(MAX_ARG_LEN);
    cmd->args[3] = (char *) malloc(MAX_ARG_LEN);

    cmd->args[0] = cmd->name;
    *cmd->args[1] = *cmd->args[2] = '\0';
    cmd->args[3] = NULL;

    switch(*cmd->name)
    {
    case 'C':
        strcpy(cmd->name, "cp");
        strcpy(cmd->args[1], cmd->argv[1]);
        strcpy(cmd->args[2], cmd->argv[2]);
        break;
    case 'D':
        strcpy(cmd->name, "rm");
        strcpy(cmd->args[1], cmd->argv[1]);
        cmd->args[2] = NULL;
        break;
    case 'L':
        strcpy(cmd->name, "pwd");
        cmd->args[1] = cmd->args[2] = cmd->args[3];
        cout << endl;
        break;
    case 'M':
        strcpy(cmd->name, "nano");
        strcpy(cmd->args[1], cmd->argv[1]);
        cmd->args[2] = NULL;
        break;
    case 'P':
        strcpy(cmd->name, "more");
        strcpy(cmd->args[1], cmd->argv[1]);
        cmd->args[2] = NULL;
        break;
    case 'S':
        strcpy(cmd->name, "firefox");
        cmd->args[1] = cmd->args[2] = NULL;
        break;
    case 'W':
        strcpy(cmd->name, "clear");
        cmd->args[1] = cmd->args[2] = NULL;
        break;
    case 'X':
        strcpy(cmd->name, cmd->argv[1]);
        cmd->args[0] = cmd->args[1] = cmd->args[2] = NULL;
        break;
    default:
        errorBool = true;
        break;
    }

    return errorBool;
}

void errorMessage(struct command_t* cmd)
{
    std::cout << "\n\t!--The entered command doesn't match any preexisitng commands--!\n\n";
    std::cout << "\tCommand Entered: " << *cmd->name << std::endl << std::endl << '\t';

    char temp = *cmd->name;

    switch(temp)
    {
    case 'c':
        std::cout << "Closest Possible Command: C, for 'COPY'";
        break;
    case 'd':
        std::cout << "Closest Possible Command: D, for 'DELETE'";
        break;
    case 'e':
        std::cout << "Closest Possible Command: E, for 'ECHO'";
        break;
    case 'm':
        std::cout << "Closest Possible Command: M, for 'MAKE'";
        break;
    case 'p':
        std::cout << "Closest Possible Command: P, for 'CONTENTS'";
        break;
    case 's':
        std::cout << "Closest Possible Command: S, for 'FIREFOX'";
        break;
    case 'x':
        std::cout << "Closest Possible Command: X, for 'EXECUTE'";
        break;
    case 'l':
        std::cout << "Closest Possible Command: L, for 'LIST'";
        break;
    case 'w':
        std::cout << "Closest Possible Command: W, for 'CLEAR'";
        break;
    case 'h':
        std::cout << "Closest Possible Command: H, for 'HELP'";
        break;
    case 'q':
        std::cout << "Closest Possible Command: Q, for 'QUIT'";
        break;
    default:
        std::cout << "There are no commands that are similar, please refer to the Guide" << std::endl
             << "\tfor a list of commands and syntax." << std::endl
             << "\t[Type 'H' and press 'ENTER']";
        break;
    }

    std::cout << std::endl << std::endl;
}
