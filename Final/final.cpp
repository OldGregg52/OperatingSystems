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

using namespace std;

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
                    cout << command.argv[i] << " ";
                    i++;
                }
                cout << endl;
            }
            break;
        case 'H':
            helpPrint();
            break;
        case 'L':
            cout << endl;
            setStructValues(&command);
            callFork(&command, &pid, status, foxPid, chPid);
            wait(&status);
            cout << endl;
            *command.name = 'A';
            setStructValues(&command);
            callFork(&command, &pid, status, foxPid, chPid);
            wait(&status);
            cout << endl;
            break;
        case 'Q':
            cout << "\n\nshell: Terminating successfully\n";
            return(0);
            break;
        default:
            if(*command.name != '\0')
            {
                if(!setStructValues(&command))
                    callFork(&command, &pid, status, foxPid, chPid);
                else
                    errorMessage(&command);
            }
            break;
        }

    }
}

void readCommand(char* buffer)
{
    fgets(buffer, 80, stdin);
}

int parseCommand(char* cLine, struct command_t* cmd)
{
    int argc;
    char** clPtr;
    /* Initialization */
    clPtr = &cLine;
    argc = 0;
    cmd->argv[argc] = (char*) malloc(sizeof(cmd->argv[0]));
    /* Fill argv[v] */
    while ((cmd->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL)
    {
        cmd->argv[++argc] = (char*) malloc(MAX_ARG_LEN);
    }
    /* Set the command name and argc */
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
                cout << "Detecting firefox termination.\n";
                kill(foxPid, SIGTERM); foxPid = 0;
            }
        }
    }
    return 0;
}

void printPrompt()
{
    cout << "linux gjb52 |> ";
}

void helpPrint()
{
     cout << endl << "To use this console, please enter the command you wish"
          << "to execute with a singular capital letter (otherwise the command won't be read)"
          << "followed by the necessary information, such as filenames*. A list of commands"
          << "is available below:" << endl;

     cout << "\tCopy:\t\t" << "C {origin path} {destination path}" << endl
          << "\tDelete:\t\t" << "D {file path}" << endl
          << "\tEcho:\t\t" << "E {text to be printed}" << endl
          << "\tHelp:\t\t" << "H" << endl
          << "\tList:\t\t" << "L" << endl
          << "\tMake:\t\t" << "M {destination path}" << endl
          << "\tPrint:\t\t" << "P {origin path}" << endl
          << "\tQuit:\t\t" << "Q" << endl
          << "\tFirefox:\t" << "S" << endl
          << "\tWipe:\t\t" << "W" << endl
          << "\tExecute:\t" << "X {program}" << endl;
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
        cmd->args[1] = cmd->args[2] = NULL;
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
    case 'A':
        strcpy(cmd->name, "ls");
        strcpy(cmd->args[1], "-l");
        cmd->args[2] = NULL;
        break;
    default:
        errorBool = true;
        break;
    }

    return errorBool;
}

void errorMessage(struct command_t* cmd)
{
    cout << "\n\t!--The entered command doesn't match any preexisitng commands--!\n\n";
    cout << "\tCommand Entered: " << *cmd->name << endl << endl << '\t';

    char temp = *cmd->name;

    switch(temp)
    {
    case 'c':
        cout << "Closest Possible Command: C, for 'COPY'";
        break;
    case 'd':
        cout << "Closest Possible Command: D, for 'DELETE'";
        break;
    case 'e':
        cout << "Closest Possible Command: E, for 'ECHO'";
        break;
    case 'm':
        cout << "Closest Possible Command: M, for 'MAKE'";
        break;
    case 'p':
        cout << "Closest Possible Command: P, for 'CONTENTS'";
        break;
    case 's':
       cout << "Closest Possible Command: S, for 'FIREFOX'";
       break;
    case 'x':
       cout << "Closest Possible Command: X, for 'EXECUTE'";
       break;
    case 'l':
        cout << "Closest Possible Command: L, for 'LIST'";
        break;
    case 'w':
        cout << "Closest Possible Command: W, for 'CLEAR'";
        break;
    case 'h':
        cout << "Closest Possible Command: H, for 'HELP'";
        break;
    case 'q':
        cout << "Closest Possible Command: Q, for 'QUIT'";
        break;
    default:
        cout << "There are no commands that are similar, please refer to the Guide for a list of commands and"
             << " syntax. [Type 'H' and press 'ENTER']";
        break;
    }

    cout << endl << endl;
}
