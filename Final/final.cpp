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
};

int parseCommand(char*, struct command_t*);
void readCommand(char*);
void printPrompt();
int callFork(struct command_t*, int*, int&, char*);
void helpPrint();
void setStructValues(struct command_t*, char*);

int main()
{
    int pid, status, foxPid;
    char *args[3];
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
        case 'Q':
            cout << "\n\nshell: Terminating successfully\n";
            return(0);
        default:
            setStructValues(&command, argv);
            callFork(&command, &pid, &status, args);
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

int callFork(struct command_t* cmd, int * pid, int &status, char * args)
{
    if((*pid = fork()) == 0)
    {
        execvp(cmd->name, args);
        perror(cmd->name); return -1;
    }

    return 0;
}

/*int callFork(struct command_t* cmd, int* pid, int &status, int &foxPid)
{
    int chPid;

    if((*pid = fork()) == 0)
    {
        execvp(cmd->name, cmd->argv);
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
}*/

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

void setStructValues(struct command_t* cmd, char* args)
{
    args[0] = (char *) malloc(MAX_ARG_LEN);
    args[1] = (char *) malloc(MAX_ARG_LEN);
    args[2] = (char *) malloc(MAX_ARG_LEN);
    args[2] = NULL;

    switch(*cmd->name)
    {
    case 'C':
        strcpy(cmd->name, "cp");
        strcpy(args[0], cmd->argv[1]);
        strcpy(args[1], cmd->argv[0]);
        break;
    case 'D':
        strcpy(cmd->name, "rm");
        strcpy(args[0], cmd->argv[1]);
        *args[1] = '\0';
        break;
    case 'L':
        strcpy(cmd->name, "pwd");
        *args[0] = *args[1] = '\0';
    case 'M':
        strcpy(cmd->name, "nano");
        *args[0] = *args[1] = '\0';
        break;
    case 'P':
        strcpy(cmd->name, "more");
        *args[0] = *args[1] = '\0';
        break;
    case 'S':
        strcpy(cmd->name, "firefox");
        *args[0] = *args[1] = '\0';
        break;
    case 'W':
        strcpy(cmd->name, "clear");
        *args[0] = *args[1] = '\0';
        break;
    case 'X':
        strcpy(cmd->name, cmd->argv[1]);
        *args[0] = *args[1] = '\0';
        break;
    }

    return;
}
