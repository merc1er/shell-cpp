#include <iostream>
#include <string>
#include <filesystem>

const char *HOME = std::getenv("HOME");

void execute_cd_command(std::string &input)
{
    std::string path = input.substr(3);
    if (path == "")
    {
        std::cerr << "cd: missing argument" << std::endl;
        return;
    }

    if (path == "~")
    {
        std::filesystem::current_path(HOME);
        return;
    }

    if (std::filesystem::exists(path))
    {
        std::filesystem::current_path(path);
        return;
    }

    std::cerr << "cd: " << path << ": No such file or directory" << std::endl;
    return;
}
