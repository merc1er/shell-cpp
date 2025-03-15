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

void run_shell()
{
  std::cout << "$ ";

  std::string input;
  std::getline(std::cin, input);

  // Exit builtin.
  if (input.starts_with("exit"))
  {
    std::exit(0);
  }

  // Echo builtin.
  if (input.starts_with("echo"))
  {
    std::cout << input.substr(5) << std::endl;
    return;
  }

  // Type builtin.
  if (input.starts_with("type"))
  {
    return execute_type_command(input);
  }

  // PWD builtin.
  if (input.starts_with("pwd"))
  {
    std::cout << std::filesystem::current_path().string() << std::endl;
    return;
  }

  // Execute the command, if found in path.
  std::vector<std::string> parts = split(input, ' ');
  std::string command = parts[0];
  std::string args = "";
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
