#include <iostream>

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

    std::cout << input << ": command not found" << std::endl;
  }
}
