#include <fcntl.h> // Include the header file for open() function
#include <unistd.h> // Include the header file for close() function
#include <iostream>
#include <cstring>
#include <string>
#include <vector>

#define STD_INPUT_FD 0
#define STD_OUTPUT_FD 1

int main(int argc, char *argv[])
{
    int outFileFD = open("outfile.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
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

        std::vector<std::string> userStrings;

        enum class Command { EndInput, Quit, Continue };
        Command cmd = Command::Continue;
        while (true)
        {
            // Read input from standard input
            int bytesRead = read(STD_INPUT_FD, userInput, sizeof(userInput) - 1);
            if (!bytesRead)
            {
                write(STD_OUTPUT_FD, "Read error or EOF.\n", 20);
                continue; // Skip to the next iteration if input is empty
            }

            userInput[bytesRead] = '\0';
            std::string inputStr(userInput);

            if (inputStr == "quit\n" || inputStr == "exit\n")
            {
                cmd = Command::Quit;
                break;
            }
            else if (inputStr == "---END---\n")
            {
                cmd = Command::EndInput; // End input on empty line
                break;
            }

            userStrings.push_back(inputStr);
        }

        if (cmd == Command::Quit)
            break;

        // Write the input to a file
        for (const auto &str : userStrings)
        {
            write(outFileFD, str.c_str(), str.size());
        }
    }

    close(outFileFD);
    write(STD_OUTPUT_FD, "Exiting program.\n", 18);

    return 0;
}