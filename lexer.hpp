#ifndef LEXER_HPP
#define LEXER_HPP
#include "token.hpp"
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include "symbol_table.hpp"

class Lexer {
public:
  Lexer(std::istream &is) : is(is) { advance(); }
  Token nextToken() {
    clearWord();
    advance();
    // omit spaces
    while (isspace(ch)) {
      advance();
    }
    loadToWord();
    if (islower(ch) || isupper(ch) || ch == '_') {
      while (islower(lookahead) || isupper(lookahead) || lookahead == '_' ||
             isdigit(lookahead)) {
        advance();
        loadToWord();
      }
      bool is_keyword = false;
      for (int i = 0; i < KEYWORDS; ++i) {
        if (current_word == keywords[i]) {
          token.type = (TokenType)(i + 1);
          is_keyword = true;
          break;
        }
      }
      if (!is_keyword) {
        token.type = ID;
        // add to symbol table
        // st.emplace(current_word, Symbol(current_word));
      }
    } else if (isdigit(ch)) {
      while (isdigit(lookahead)) {
        advance();
        loadToWord();
      }
      token.type = INT;
    } else if (ch == '<') {
      token.type = OP;
    } else if (ch == '>') {
      token.type = OP;
    } else if (ch == '=') {
      token.type = OP;
    } else if (ch == '<') {
      token.type = OP;
    } else if (ch == '#') {
      token.type = OP;
    } else if (ch == ';') {
      token.type = SEMI;
    } else if (ch == ':') {
      if (lookahead == '=') {
        advance();
        loadToWord();
        token.type = OP;
      } else {
        token.type = ERROR;
      }
    } else if (ch == '+') {
      token.type = OP;
    } else if (ch == '-') {
      token.type = OP;
    } else if (ch == '*') {
      token.type = OP;
    } else if (ch == '/') {
      token.type = OP;
    } else if (ch == ',') {
      token.type = COMMA;
    } else if (ch == EOF) {
      token.type = TERMINATE;
      current_word = "terminate";
    } else if (ch == '(') {
      token.type = OP;
    } else if (ch == ')') {
      token.type = OP;
    } else {
      token.type = ERROR;
    }
    token.content = current_word;
    return token;
  }

  int getPos() const { return pos; }
  int getLine() const { return line; }
  // SymbolTable &getSymbolTable() const {
  //   return st;
  // }

private:
  Token token;
  // SymbolTable &st;
  char lookahead;
  char ch;
  std::string current_word;
  std::istream &is;
  int line = 1;
  int pos = 0;

  void advance() {
    ch = lookahead;
    is.read(&lookahead, sizeof(char));
    if (is.eof()) {
      lookahead = EOF;
    }
    if (ch == '\n') {
      pos = 0;
      ++line;
    } else {
      ++pos;
    }
  }

  void loadToWord() { current_word += ch; }

  void clearWord() { current_word = ""; }
};

#endif