#include <iostream>
#include <vector>
#include <string>
#include <ranges>
#include <cstdlib>
#include <filesystem>

std::string BUILTINS[] = {"echo", "exit", "type", "pwd"};
const char *PATH = std::getenv("PATH");

std::vector<std::string> split(std::string_view path, char delim)
{
  std::vector<std::string> paths;
  for (auto &&part : std::views::split(path, delim))
  {
    paths.emplace_back(part.begin(), part.end());
  }
  return paths;
}

std::string get_command_path(std::string &command)
{
  for (std::string path : split(PATH, ':'))
  {
    const std::string full_path = path + "/" + command;
    if (std::filesystem::exists(full_path))
    {
      return full_path;
    }
  }
  return "";
}

void execute_type_command(std::string &input)
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
      return;
    }
  }

  const std::string full_path = get_command_path(command);
  if (full_path != "")
  {
    std::cout << command << " is " << full_path << std::endl;
    return;
  }

  std::cout << command << ": not found" << std::endl;
}

void execute_cd_command(std::string &input)
{
  std::string path = input.substr(3);
  if (path == "")
  {
    std::cerr << "cd: missing argument" << std::endl;
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

void run_shell()
{
  std::cout << "$ ";

  std::string input;
  std::getline(std::cin, input);
  std::vector<std::string> parts = split(input, ' ');
  std::string command = parts[0];
  std::string args = "";

  // exit builtin.
  if (command == "exit")
  {
    std::exit(0);
  }

  // echo builtin.
  if (command == "echo")
  {
    std::cout << input.substr(5) << std::endl;
    return;
  }

  // type builtin.
  if (command == "type")
  {
    return execute_type_command(input);
  }

  // pwd builtin.
  if (command == "pwd")
  {
    std::cout << std::filesystem::current_path().string() << std::endl;
    return;
  }

  // cd builtin.
  if (command == "cd")
  {
    return execute_cd_command(input);
  }

  // Execute the command, if found in path.
  if (parts.size() > 1)
  {
    args = input.substr(command.size() + 1);
  }

  if (command == "")
  {
    return;
  }
  std::string command_path = get_command_path(command);
  if (command_path != "")
  {
    std::system((command + " " + args).c_str());
    return;
  }

  // Command not found.
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
