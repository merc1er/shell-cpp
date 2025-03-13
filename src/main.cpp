#include <iostream>

std::string BUILTINS[] = {"echo", "exit", "type"};

int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true)
  {
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);

    if (input.starts_with("exit"))
    {
      break;
    }

    if (input.starts_with("echo"))
    {
      std::cout << input.substr(5) << std::endl;
      continue;
    }

    if (input.starts_with("type"))
    {
      std::string command = input.substr(5);
      bool found = false;

      for (std::string builtin : BUILTINS)
      {
        if (command == builtin)
        {
          std::cout << command << " is a shell builtin" << std::endl;
          found = true;
        }
      }

      if (found)
      {
        continue;
      }
      std::cout << command << ": not found" << std::endl;
      continue;
    }

    std::cout << input << ": command not found" << std::endl;
  }
}
