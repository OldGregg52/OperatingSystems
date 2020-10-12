/*
 * Greg Brown
 * Operating Systems
 * Lab 1
 * October 12, 2020
 * Purpose: To produce a minimalist shell via the use of execvp() and fork() functions
 */

#include <iostream>     //Included for C++ standard input and output
#include <stdio.h>      //Included for C standard input and output
#include <string.h>     //Included for functions dealing with strings, such as strcpy()
#include <unistd.h>     //Included for various Unix functions such as fork() and execvp()
#include <stdlib.h>     //Included for C standard functions used in this program
#include <sys/wait.h>   //Included for the wait() function

//Defining of necessary global constants for dynamic variable allocation
//      and variable initialization
#define MAX_ARGS        64
#define MAX_ARG_LEN     16
#define MAX_LINE_LEN    80
#define WHITESPACE      " ,\t\n"

//Declaring the struct variable 'command_t' that will be used to hold the
//      values entered by the user and the values that will be passed to
//      the execvp() function for process calling
struct command_t
{
    char *name;             //Char pointer used for the string needed for the command calls
    int argc;               //Integer variable used to determine how many entries the user input
    char *argv[MAX_ARGS];   //Char pointer array to hold all the words/symbols the user inputs
    char *args[4];          //Char pointer array to hold the arguments that will be passed to execvp()
};

//Function descriptions and purposes are labeled after the main() function
int parseCommand(char*, struct command_t*);
void readCommand(char*);
void printPrompt();
int callFork(struct command_t*, int*, int&);
void helpPrint();
bool setStructValues(struct command_t*);
void errorMessage(struct command_t*);

int main()
{
    int pid, status;
    char cmdLine[MAX_LINE_LEN];
    struct command_t command;

    while(true)     //While-loop initialized to constantly run for continuous command execution
    {
        printPrompt();      //Function prints the prompt to the screen so the user knwos to enter input
        readCommand(cmdLine);   //Function reads what the user inputs and stores it in a character array
        parseCommand(cmdLine, &command);    //Function parses the inputted array so words are stored in individual variables

        command.argv[command.argc] = NULL;    //The end of the character array used for parsing is set to NULL

        switch(*command.name)       //The 'name' member variable of the 'command' struct is used to
        {                           //    determine what action will be taken
        case 'E':
            {                                     //If 'E' is input then the prompt is printed to the screen again
                printPrompt();                    //     and a while-loop is established to iterate through the
                int i = 1;                        //     'argv' character pointer array and print all of the contents to
                while(command.argv[i] != NULL)    //     the terminal how they were entered, with only spaces between
                {
                    std::cout << command.argv[i] << " ";
                    i++;
                }
                std::cout << std::endl;           //New line is set and the switch-statement breaks
            }
            break;
        case 'H':
            helpPrint();     //If 'H' is input then the helpPrint() function is called to display the
            break;           //     correct information, then the switch-statement breaks
        case 'L':
            setStructValues(&command);        //If 'L' is input then the setStructValues() function is called to
            callFork(&command, &pid, status); //     set the "name" and "args" values before calling the callFork()
            wait(&status);                    //     function to implement the correct process
            std::cout << std::endl;           //A new line is established and waits until the callFork() function finishes
            strcpy(command.name, "ls");              //After the path is displayed, the "name" and "args" values are
            strcpy(command.args[0], command.name);   //     changed so the callFork() function can be called again
            strcpy(command.args[1], "-l");           //     to display a list of files and folders in the current directory
            command.args[2] = NULL;
            callFork(&command, &pid, status);
            wait(&status);
            std::cout << std::endl;     //A new line is established and waits until the callFork() function finishes
            break;                      //     then the switch-statement breaks
        case 'Q':
            std::cout << "\n\tshell: Terminating successfully\n";
            return(0);                  //If 'Q' is input, then a message is displayed and main() function terminates
            break;
        default:
            if(*command.name != '\0')           //If none of the above options are chosen then the setStructValues()
            {                                   //     function is called, which returns a bool value depending on if
                if(!setStructValues(&command))  //     chosen input doesn't match any of the commands
                {
                    callFork(&command, &pid, status);     //If there is a matching command then the "name" and "args"
                                                          //     values are set and the callFork() function is called
                    if(*command.name != 'f')     //If the called function is not firefox, then the wait() function
                        wait(&status);           //     is used to ensure commands are performed sequentially
                }
                else                             //If there isn't a matching command to the input, then the
                    errorMessage(&command);      //     errorMessage() function is called to display the correct
            }                                    //     information. Then the switch-statement breaks and the
            break;                               //     indefinite while-loop iterates
        }
    }

    return(0);
}

/*
 * readCommand() function takes a character pointer as an input and reads in
 *     80 characters from the standard terminal input. Nothing is returned
 *     but the pointer "buffer" is set to the position of the input string
 */
void readCommand(char* buffer)
{
    fgets(buffer, 80, stdin);
}

/*
 * parseCommand() function takes a character pointer and a struct variable pointer as input
 *     the character pointer is the same one the readCommand() function uses. The string that
 *     is pointed to is then broken down into individual words that are then individually
 *     assigned to a pointer in the "argv" member variable array. In addition, the variable
 *     "argc" is updated to hold the number of words that were inputted into the terminal.
 *     Finally, the "name" member variable is updated to the value that the initial "argv"
 *     pointer is pointing to. An integer '1' is returned when the function is complete.
 */
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

/*
 * callFork() function takes a struct and integer pointers as input, as well as an
 *     integer variable that was passed by reference. It then determines if a new
 *     function can be initiated. If it can, then the member variables of the struct
 *     "args" and "name" are used to pass values to the execvp() function to start
 *     the process. It also include the perror() function to prevent abnormal behavior if
 *     the value in "name" is not recognized.
 */
int callFork(struct command_t* cmd, int* pid, int &status)
{
    if((*pid = fork()) == 0)
    {
        execvp(cmd->name, cmd->args);
        perror(cmd->name); return -1;
    }
    return 0;
}

/*
 * printPrompt() function is used to print the input prompt to the console whenever
 *     it is called.
 */
void printPrompt()
{
    std::cout << "linux gjb52 |> ";
}

/*
 * helpPrint() function takes no input and only prints text to the screen to indicate how
 *     to properly use the terminal, along with a list of available commands and which of
 *     those commands require file paths in addition to the command itself.
 */
void helpPrint()
{
     std::cout << std::endl << "To use this console, please enter the command you wish to execute with a" << std::endl
          << "singular capital letter (otherwise the command won't be read) followed by" << std::endl
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

     std::cout << "\n*Please also note that to ensure the commmands that require file paths work" << std::endl
          << "\tthe path should be the absolute path to avoid the program from" << std::endl
          << "\tselecting a file relative to its location accidentally" << std::endl << std::endl;

     return;
}

/*
 * setStructValues() function takes a struct pointer as a variable and uses it to reset the "args"
 *     values before deteremining what values must be assigned to those pointers based on what input
 *     was detected via switch-statement. For the commands that require arguments, the strcpy()
 *     function is utilized to transfer the string that is held in the corresponding "argv" variable
 *     to the necessary "args" variable. If the command doens't require input, the "args" values are
 *     set to either '\0' or NULL, depending on what the corresponding command requires. In addition to
 *     this, if input is detected that doesn't match any of the switch-statement cases, then a bool
 *     value is assigned to the variable "errorBool" and returned to show that the errorMessage()
 *     function should be called.
 */
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
        if(cmd->argv[1] != NULL)
            strcpy(cmd->args[1], cmd->argv[1]);
        if(cmd->argv[2] != NULL)
            strcpy(cmd->args[2], cmd->argv[2]);
        break;
    case 'D':
        strcpy(cmd->name, "rm");
        if(cmd->argv[1] != NULL)
            strcpy(cmd->args[1], cmd->argv[1]);
        cmd->args[2] = NULL;
        break;
    case 'L':
        strcpy(cmd->name, "pwd");
        strcpy(cmd->args[1], "-L");
	cmd->args[2] = cmd->args[3];
        std::cout << std::endl;
        break;
    case 'M':
        strcpy(cmd->name, "nano");
        if(cmd->argv[1] != NULL)
            strcpy(cmd->args[1], cmd->argv[1]);
        cmd->args[2] = NULL;
        break;
    case 'P':
        strcpy(cmd->name, "more");
        if(cmd->argv[1] != NULL)
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
        if(cmd->argv[1] != NULL)
            strcpy(cmd->name, cmd->argv[1]);
        cmd->args[0] = cmd->args[1] = cmd->args[2] = NULL;
        break;
    default:
        errorBool = true;
        break;
    }
    return errorBool;
}

/*
 * errorMessage() function takes a struct pointer as input and prints to the screen that there
 *     has been an error. From there, it determines if the input was just a command that was
 *     lowercase instead of uppercase, or if was an entirely different input altogether. If
 *     the input is just lowercase, then the switch-statement is directed to print a message
 *     telling the user of the input they possibly meant instead. Otherwise, the message is
 *     sent that this function doesn't know what the user wanted and directs the user to
 *     the help guide for more information.
 */
void errorMessage(struct command_t* cmd)
{
    std::cout << "\n\t!--The entered command doesn't match any preexisitng commands--!\n\n";
    std::cout << "\tCommand Entered: " << *cmd->name << std::endl << std::endl << '\t';

    switch(*cmd->name)
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
