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
int callFork(struct command_t*, int*);
int callFork(struct command_t*, int*, int*);
void helpPrint();

int main()
{
    int pid, temp, status;
    int* foxPid;
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
        case 'C':
            strcpy(command.name, "cp");
            callFork(&command, &pid);
            wait(&status);
            break;
        case 'D':
            strcpy(command.name, "rm");
            callFork(&command, &pid);
            wait(&status);
            break;
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
            strcpy(command.name, "pwd");
            cout << endl;
            callFork(&command, &pid);
            wait(&status);
            strcpy(command.name, "ls");
            cout << endl;
            command.argv[1] = (char*) malloc(MAX_ARG_LEN);
            command.argv[2] = (char*) malloc(MAX_ARG_LEN);
            command.argv[2] = NULL;
            strcpy(command.argv[0], command.name);
            strcpy(command.argv[1], "-l");
            callFork(&command, &pid);
            wait(&status);
            break;
        case 'M':
            strcpy(command.name, "nano");
            callFork(&command, &pid);
            wait(&status);
            break;
        case 'P':
            strcpy(command.name, "more");
            callFork(&command, &pid);
            wait(&status);
            break;
        case 'Q':
            printf("\n\n shell: Terminating successfully\n");
            return(0);
        case 'S':
            strcpy(command.name, "firefox");
            command.argv[1] = (char*) malloc(MAX_ARG_LEN);
            command.argv[1] = NULL;
            *command.argv[0] = '&';
            strcpy(command.argv[0], "firefox");
            callFork(&command, &pid, &firefox);
            break;
        case 'W':
            strcpy(command.name, "clear");
            callFork(&command, &pid);
            wait(&status);
            break;
        case 'X':
            strcpy(command.name, command.argv[1]);
            callFork(&command, &pid);
            wait(&status);
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

int callFork(struct command_t* cmd, int * pid)
{
    int status;

    if((*pid = fork()) == 0)
    {
        execvp(cmd->name, cmd->argv);
        perror(cmd->name); return -1;
    }

    return 0;
}

int callFork(struct command_t* cmd, int* pid, int* foxPid)
{
    int status, chPid;

    if((*pid = fork()) == 0)
    {
        execvp(cmd->name, cmd->argv);
        perror(cmd->name); return -1;
        *foxPid = *pid;
    }
    if((*pid = fork()) != 0)
    {
        wait(&status);
        if(foxPid > 0)
        {
            chPid = waitpid(*foxPid, &status, WNOHANG);
            if(chPid == *foxPid)
            {
                cout << "Detecting firefox termination.\n";
                kill(foxPid, SIGTERM); *foxPid = 0;
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
          << "\tExecute:\t\t" << "X {program}" << endl;
}
