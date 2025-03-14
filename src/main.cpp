#include <iostream>
#include <vector>
#include <string>
#include <ranges>
#include <filesystem>

std::string BUILTINS[] = {"echo", "exit", "type"};
const char *PATH = std::getenv("PATH");

std::vector<std::string> split(std::string_view path, char delim = ':')
{
  std::vector<std::string> paths;
  for (auto &&part : std::views::split(path, delim))
  {
    paths.emplace_back(part.begin(), part.end());
  }
  return paths;
}

void run_shell()
{
  std::cout << "$ ";

  std::string input;
  std::getline(std::cin, input);

  if (input.starts_with("exit"))
  {
    std::exit(0);
  }

  if (input.starts_with("echo"))
  {
    std::cout << input.substr(5) << std::endl;
    return;
  }

  if (input.starts_with("type"))
  {
    std::string command = input.substr(5);
    bool found = false;

    // Check if the command is a shell builtin.
    for (std::string builtin : BUILTINS)
    {
      if (command == builtin)
      {
        std::cout << command << " is a shell builtin" << std::endl;
        found = true;
      }
    }

    // Check if the command is in the PATH.
    if (!found)
    {
      for (std::string path : split(PATH))
      {
        const std::string full_path = path + "/" + command;
        if (std::filesystem::exists(full_path))
        {
          std::cout << command << " is " << full_path << std::endl;
          found = true;
          break;
        }
      }
    }

    if (found)
    {
      return;
    }
    std::cout << command << ": not found" << std::endl;
    return;
  }

  std::cout << input << ": command not found" << std::endl;
}

int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true)
  {
    run_shell();
  }
}
