// ACADEMIC INTEGRITY PLEDGE
//
// - I have not used source code obtained from another student nor
//   any other unauthorized source, either modified or unmodified.
//
// - All source code and documentation used in my program is either
//   my original work or was derived by me from the source code
//   published in the textbook for this course or presented in
//   class.
//
// - I have not discussed coding details about this project with
//   anyone other than my instructor. I understand that I may discuss
//   the concepts of this program with other students and that another
//   student may help me debug my program so long as neither of us
//   writes anything during the discussion or modifies any computer
//   file during the discussion.
//
// - I have violated neither the spirit nor letter of these restrictions.
//
//
//
// Signed: Greg Brown
// Date: 11/24/2020

//filesys.c
//Based on a program by Michael Black, 2007
//Revised 11.3.2020 O'Neil

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>

void directoryList(FILE*, char []);
void printFile(FILE*, char [], char []);
void deleteFile(FILE*, char [], char []);
void createFile(FILE*, char [], char []);

int main(int argc, char* argv[])
{
    char map[512];
    char dir[512];
    int input;

	FILE* floppy;
	floppy = fopen("floppyaTest.img", "r+");
	if (floppy == 0)
	{
		printf("floppyaTest.img not found\n");
		return 0;
	}

	while(input != 5)
    {
        fseek(floppy, 512 * 256, SEEK_SET);
        for(int i = 0; i < 512; i++)
            map[i] = fgetc(floppy);

        fseek(floppy, 512 * 257, SEEK_SET);
        for (int i = 0; i < 512; i++)
            dir[i] = fgetc(floppy);

        std::cin.sync();

        std::cout << "Which would you like to accomplish:" << std::endl;
        std::cout << "> (1) Display the Directory\n"
                  << "> (2) Print the Contents of a File\n"
                  << "> (3) Delete a File\n"
                  << "> (4) Create a File\n"
                  << "> (5) Exit\n"
                  << ">>> ";
        std::cin >> input;

        switch(input)
        {
        case 1:
            directoryList(floppy, dir);
            break;
        case 2:
            printFile(floppy, dir, map);
            break;
        case 3:
            deleteFile(floppy, dir, map);
            break;
        case 4:
            createFile(floppy, dir, map);
            break;
        case 5:
            return(0);
        default:
            system("CLS");
            std::cout << "!-- The inputted value is not an available option --!" << std::endl << std::endl;
            break;
        }

        fseek(floppy, 512 * 256, SEEK_SET);
        for (int i = 0; i < 512; i++)
            fputc(map[i], floppy);

        fseek(floppy, 512 * 257, SEEK_SET);
        for (int i = 0; i < 512; i++)
            fputc(dir[i], floppy);
    }

	fclose(floppy);
}

void directoryList(FILE* floppy, char* dir)
{
    int bytes = 0;

    system("CLS");
	printf("Disk directory:\n\n");
	printf("        NAME  :  SIZE\n");
	for (int i = 0; i < 512; i = i + 16)
    {
        if (dir[i] != 0)
        {
            for (int j = 0; j < 8; j++)
            {
                if (dir[i + j] == 0)
                    printf(" ");
            }

            for (int j = 0; j < 8; j++)
            {
                if (dir[i + j] != 0)
                    printf("%c", dir[i + j]);
            }

            if((dir[i + 8] == 't' || dir[i + 8] == 'T'))
                printf(".txt");
            else
                printf(".exe");

            printf("  :  ");

            printf("%i bytes\n", 512 * int(dir[i + 10]));

            bytes += (512 * int(dir[i + 10]));
        }
    }

    printf("\nSpace Used    : %6d bytes\n", bytes);
    printf("Space Free    : %6d bytes\n", (261632 - bytes));
    std::cout << std::endl << std::endl;

    system("PAUSE");
    system("CLS");
}

void deleteFile(FILE* floppy, char* dir, char* map)
{
    std::string nameInput;
    char nameSelected[8];
    char directoryName[8];
    int charMatch = 0, fileLocation = 0, fileSize = 0;
    bool found = false;

    system("CLS");
    std::cout << "Please input the name of the file you want to delete (only the first 8 characters are accepted)" << std::endl;
    std::cout << ">>> ";
    std::cin >> nameInput;

    if(nameInput.length() < 8)
    {
        for(int i = 0; i < nameInput.length(); i++)
            nameSelected[i] = nameInput.at(i);

        for(int i = nameInput.length(); i < 8; i++)
            nameSelected[i] = 0;
    }
    else
    {
        for (int i = 0; i < 8; i++)
            nameSelected[i] = nameInput.at(i);
    }

    for (int i = 0; i < 512; i = i + 16)
    {
        if (dir[i] != 0)
        {
            for (int j = 0; j < 8; j++)
                directoryName[j] = dir[i + j];

            for (int j = 0; j < 8; j++)
                if (nameSelected[j] == directoryName[j])
                    charMatch++;

            if (charMatch == 8)
            {
                found = true;
                fileLocation = int(dir[i + 9]);
                fileSize = int(dir[i + 10]);
                for (int j = (fileSize - 1); j >= 0; j--)
                    map[fileLocation + j] = 0;
                dir[i] = 0;

                std::cout << std::endl << std::endl << "!-- File Successfully Deleted --!" << std::endl << std::endl;
            }

            charMatch = 0;
        }
    }

    if(!found)
        std::cout << std::endl << std::endl << "!-- File Not Found --!" << std::endl << std::endl;

    system("PAUSE");
    system("CLS");
}

void printFile(FILE* floppy, char* dir, char* map)
{
    std::string nameInput;
    char nameSelected[8];
    char directoryName[8];
    char buffer[12288];
    int charMatch = 0, fileLocation = 0, fileSize = 0;
    bool found = false;

    system("CLS");
    std::cout << "Please input the name of the file you want to open (only the first 8 characters are accepted)" << std::endl;
    std::cout << ">>> ";
    std::cin >> nameInput;

    if(nameInput.length() < 8)
    {
        for(int i = 0; i < nameInput.length(); i++)
            nameSelected[i] = nameInput.at(i);

        for(int i = nameInput.length(); i < 8; i++)
            nameSelected[i] = 0;
    }
    else
    {
        for (int i = 0; i < 8; i++)
            nameSelected[i] = nameInput.at(i);
    }

    for (int i = 0; i < 512; i = i + 16)
    {
        if (dir[i] != 0)
        {
            for (int j = 0; j < 8; j++)
                directoryName[j] = dir[i + j];

            for (int j = 0; j < 8; j++)
                if (nameSelected[j] == directoryName[j])
                    charMatch++;

            if ((charMatch == 8) && ((dir[i + 8] == 't') || (dir[i + 8] == 'T')))
            {
                found = true;
                fileLocation = int(dir[i + 9]);
                fileSize = int(dir[i + 10]);
                fseek(floppy,512*fileLocation,SEEK_SET);
                for(int i = 0; i < 12288; i++)
                    buffer[i]=fgetc(floppy);

                for(int i = 0; i < 12288; i++)
                {
                    if (buffer[1] == '\0')
                        break;

                    std::cout << buffer[i];
                }
            }
            else if ((charMatch == 8) && !((dir[i + 8] == 't') || (dir[i + 8] == 'T')))
            {
                found = true;
                std::cout << std::endl << std::endl << "!-- File specified is an executable and cannot be printed --!" << std::endl << std::endl;
            }
            charMatch = 0;
        }
    }

    if(!found)
        std::cout << std::endl << std::endl << "!-- File Not Found --!" << std::endl << std::endl;

    std::cout << std::endl;
    system("PAUSE");
    system("CLS");
}

void createFile(FILE* floppy, char* dir, char* map)
{
    std::string input;
    char nameSelected[8];
    char directoryName[8];
    char buffer[12288];
    int charMatch = 0, fileLocation = 0;
    int emptyDir = 0, availableDir = 0;
    int filledSector = 0;
    bool found = false;

    system("CLS");
    std::cout << "Please input the name of the file you want to create (only the first 8 characters are accepted)" << std::endl;
    std::cout << ">>> ";
    std::cin >> input;

    if(input.length() < 8)
    {
        for(int i = 0; i < input.length(); i++)
            nameSelected[i] = input.at(i);

        for(int i = input.length(); i < 8; i++)
            nameSelected[i] = 0;
    }
    else
    {
        for (int i = 0; i < 8; i++)
            nameSelected[i] = input.at(i);
    }

    for (int i = 0; i < 512; i = i + 16)
    {
        if (dir[i] != 0)
        {
            for (int j = 0; j < 8; j++)
                directoryName[j] = dir[i + j];

            for (int j = 0; j < 8; j++)
                if (nameSelected[j] == directoryName[j])
                    charMatch++;

            if (charMatch == 8)
                found = true;

            charMatch = 0;
        }

        if ((dir[i] == 0) && (emptyDir == 0))
        {
            availableDir = i;
            emptyDir++;
        }

    }

    if(!found)
    {
        std::cout << "\nPlease input the string to be saved to the file" << std::endl;
        std::cout << ">>> ";
        std::cin >> input;

        if(input.length() < 12287)
        {
            for(int i = 0; i < input.length(); i++)
                buffer[i] = input.at(i);

            buffer[input.length()] = '\0';
        }
        else
        {
            for (int i = 0; i < 12287; i++)
                input[i] = input.at(i);

            buffer[12288] = '\0';
        }

        for (int i = 0; i < 8; i++)
        {
            dir[availableDir + i] = nameSelected[i];
        }

        dir[availableDir + 8] = 't';
        dir[availableDir + 10] = 1;

        for (int i = 0; i < 512; i++)
            if(map[i] != -1)
            {
                filledSector++;
                fileLocation = i;
                map[i] = 255;
                break;
            }

        if(filledSector > 511)
        {
            std::cout << std::endl << std::endl << "!-- Insufficient Disk Space --!" << std::endl << std::endl;
            system("PAUSE");
            system("CLS");
            return;
        }

        dir[availableDir + 9] = fileLocation;

        fseek(floppy, 512 * fileLocation, SEEK_SET);
        for(int i = 0; i < 12288; i++)
            fputc(buffer[i], floppy);

        std::cout << std::endl << std::endl << "!-- File Successfully Created --!" << std::endl << std::endl;
    }
    else
        std::cout << std::endl << std::endl << "!-- File already exists --!" << std::endl << std::endl;

    system("PAUSE");
    system("CLS");
}
