#include <iostream>
#include <vector>
#include <string>
#include <ranges>
#include <cstdlib>
#include <filesystem>

#include "commands.h"

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

std::vector<std::string> split_shell_command(std::string_view input)
{
  std::vector<std::string> tokens;
  std::string token;
  bool in_single_quotes = false;
  bool in_double_quotes = false;

  for (size_t i = 0; i < input.size(); ++i)
  {
    char c = input[i];

    if (c == '\\')
    {
      if (i + 1 < input.size())
      {
        char next = input[i + 1];

        // Inside double quotes, only escape \, ", $, `
        if (in_double_quotes && (next == '\\' || next == '"' || next == '$' || next == '`'))
        {
          token += next;
          ++i;
        }
        // Outside quotes, backslash escapes anything.
        else if (!in_single_quotes && !in_double_quotes)
        {
          token += next;
          ++i;
        }
        else
        {
          token += '\\'; // Backslash is literal.
        }
      }
      else
      {
        token += '\\'; // Lone backslash at the end.
      }
      continue;
    }

    if (c == '\'' && !in_double_quotes)
    {
      in_single_quotes = !in_single_quotes;
      continue;
    }

    if (c == '"' && !in_single_quotes)
    {
      in_double_quotes = !in_double_quotes;
      continue;
    }

    if (c == ' ' && !in_single_quotes && !in_double_quotes)
    {
      if (!token.empty())
      {
        tokens.push_back(token);
        token.clear();
      }
      continue;
    }

    token += c;
  }

  if (!token.empty())
    tokens.push_back(token);

  return tokens;
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

  std::vector<std::string> tokens = split_shell_command(input);

  // Go to next command if no input.
  if (tokens.size() == 0)
  {
    return;
  }

  std::string command = tokens[0];
  if (command == "")
  {
    return;
  }

  std::string args = "";
  if (tokens.size() > 1)
  {
    for (std::string token : tokens | std::views::drop(1))
    {
      args += token + " ";
    }
    args.pop_back(); // Remove the last space.
  }

  // exit builtin.
  if (command == "exit")
  {
    std::exit(0);
  }

  // echo builtin.
  if (command == "echo")
  {
    std::cout << args << std::endl;
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
  std::string command_path = get_command_path(command);
  if (command_path != "")
  {
    std::system(input.c_str());
    return;
  }

  // Command not found.
  std::cout << command << ": command not found" << std::endl;
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
