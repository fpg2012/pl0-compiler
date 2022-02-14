#include "lexer.hpp"
#include "parser.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

bool test_lexer = false;
bool with_file = false;

int main(int argc, char **argv) {
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-lex") == 0) {
      test_lexer = true;
    } else {
      with_file = true;
    }
  }
  std::istream *is;
  if (!with_file) {
    std::cerr << "no file input, input from stdin!" << std::endl;
    is = &std::cin;
  } else {
    is = new std::ifstream(argv[argc - 1]);
  }

  SymbolTable sym_tab;

  Lexer lexer(*is);
  // test lexer
  if (test_lexer) {
    while (true) {
      Token token = lexer.nextToken();
      token.print(std::cout);
      if (token.type == TERMINATE) {
        std::cout << std::endl;
        break;
      }
    }
    exit(0);
  }

  Parser pars(lexer, sym_tab);
  auto *prog = pars.parse();
  std::cout << std::endl;
  delete is;
  return 0;
}