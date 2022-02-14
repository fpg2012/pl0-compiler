#ifndef TOKEN_HPP
#define TOKEN_HPP
#include <sstream>
#include <string>

#define KEYWORDS 11

static const char *keywords[KEYWORDS] = {"begin", "end",       "const", "var",
                                  "if",    "then",      "while", "do",
                                  "odd",   "procedure", "call"};

static const char *type_name[] = {"id", "int", "op", "comma", "semi"};

enum TokenType {
  UNKNOWN,
  BEGIN,
  END,
  CONST,
  VAR,
  IF,
  THEN,
  WHILE,
  DO,
  ODD,
  PROCEDURE,
  CALL,
  ID,
  INT,
  OP,
  COMMA,
  SEMI,
  NEWLINE,
  TERMINATE = -1,
  ERROR = -2,
};

struct Token {
  Token() = default;
  Token(TokenType tt, const std::string &cont) : type(tt), content(cont) {}
  TokenType type;
  std::string content;

  bool operator==(const Token &other) {
    return type == other.type && content == other.content;
  }

  void print(std::ostream &os, bool only_content = false) {
    if (content == "") {
      return;
    }
    if (only_content) {
      os << "(" << content << ")";
      return;
    }
    std::string desc;
    if (type >= 1 && type <= 11) {
      desc = keywords[type - 1];
    } else if (type >= 12 && type <= 16) {
      desc = std::string("\033[1;34m") + type_name[type - 12] + "\033[0m";
    } else if (type == TERMINATE) {
      desc = "$";
    } else {
      desc = "\033[1;31mERR\033[0m";
    }
    os << " <" << type << ":" << desc << ", \033[34m" << content << "\033[0m> ";
  }
};

#endif