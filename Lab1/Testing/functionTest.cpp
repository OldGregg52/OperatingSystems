#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

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

int parseCommand(char *, struct command_t *);
void readCommand(char *buffer);
void printPrompt();

int main()
{
    int pid;
    int status;
    char cmdLine[MAX_LINE_LEN];
    struct command_t command;

    while(true)
    {
        printPrompt();
        readCommand(cmdLine);
        parseCommand(cmdLine, &command);
        command.argv[command.argc] = NULL;

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

void printPrompt()
{
    char promptString[] = "linux gjb52 |>";
    printf("%s", promptString);
}
