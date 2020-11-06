README:

Note:
     The background launching of firefox does work. However, the implementation of the else-statement
never worked and so I decided to leave it out of the final version of the program. 

Code Compiling:

     To compile this code, it does require CMAKE, which was used for compiling during Computer Science
so the necessary installation should already be on the University's lab computers. Furthermore, to
implement compiling with CMAKE, all the necessary files and folders are already present in the zipped
folder that contains the code, and required subfolder. The process is as follows:

1) In a terminal, navigate so that it is centered in the directory "build", which will be present
        as a subfolder to the one containing the source code

2) When in the "build" directory, type and run the following command in the terminal, exactly as it
        is presented: "cmake .."

3) Once the command has finished, type and run the command "make"

4) The terminal should express that the compilation and building is complete. When this occurs
        type in the following to run the program: "./Lab1"


Using the Shell:

     To operate the shell, the only things that are necessary are to type in the command you wish to
execute. Each of the commands are run by submitting one capitalized letter that specifies the specific
command desired, with some of them requiring supplemental entries following the command to specify
files and/or destinations. To get the full list of available commands, enter "H" into the shell to see
what commands are available and which commands require the addition of file paths so they can be
executed.

Functions Present in the Shell:

-parseCommand:
     This was a function that was presented in the given materials for this assignment and is used to
          take the input that is read through the readCommand function and separates out the parts that
          are then stored in the necessary variables so they can be read later and implemented in the
          code that specifies what the input will do

-readCommand
     This was a function that was presented in the given materials for this assignment and is used to
          read in the input that the user enters into the shell. This input is then sent to the
          parseCommand function

-printPrompt
     This was a function that was presented in the given materials for this assignment and is used to
          print the input prompt of the shell to the screen so that the user is aware of normal
          operation and aware that input is required of them to execute commands

-helpPrint
     This function is purely for the purpose of printing text to the screen when the user wants to
          execute the 'HELP' command. It prints all of the available commands to the screen along with
          helpful information so the user will be able to correctly implement the shell

-callFork
     This function is implemented to call the fork() and execvp() functions after having the struct
          variable sent to it so the information contained within it can be used as arguments for the
          execvp() function

-setStructValues
     This function is implemented so that the values of the member variables within the 'command'
          struct variable can be set depending on what the user inputs. This function primarily
          utilizes a switch statement so that the struct variable's name and arguments variables
          can be correctly set. In addition, if no known commands are detected as input, then a bool
          varaible is set so that the errorMessage function can be called

-errorMessage
     This function is used to catch improper input that might be entered by the user and both prevent
          the incorrect input from being sent to the execvp() function and suggest a possible input the
          user may have been attempting, if there is a close subsitute.
