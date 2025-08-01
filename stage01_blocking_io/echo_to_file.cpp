#include <fcntl.h> // Include the header file for open() function
#include <unistd.h> // Include the header file for close() function
#include <iostream>
#include <cstring>
#include <string>

#define STD_INPUT_FD 0
#define STD_OUTPUT_FD 1

int main(int argc, char *argv[])
{
    int outFileFD = open("outfile.txt", O_WRONLY | O_CREAT, 0644);
    if (outFileFD < 0)
    {
        std::cerr << "Error opening file for writing." << std::endl;
        return -1;
    }

    std::string userPrompt = "Enter text (type quit to exit): ";
    char userInput[1024] = {0};

    while (true)
    {
        // Prompt user for input
        write(STD_OUTPUT_FD, userPrompt.c_str(), userPrompt.size());

        // Read input from standard input
        int bytesRead = read(STD_INPUT_FD, userInput, sizeof(userInput));
        if (!bytesRead)
        {
            write(STD_OUTPUT_FD, "Read error or EOF.\n", 20);
            continue; // Skip to the next iteration if input is empty
        }

        userInput[bytesRead] = '\0';
        std::string inputStr(userInput);

        if (inputStr == "quit\n" || inputStr == "exit\n")
            break;

        // Write the input to a file
        write(outFileFD, userInput, bytesRead);
    }

    close(outFileFD);
    write(STD_OUTPUT_FD, "Exiting program.\n", 18);

    return 0;
}