#ifndef PARSER_HPP
#define PARSER_HPP
#include "lexer.hpp"
#include "token.hpp"
#include "symbol_table.hpp"
#include "imcode.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <cassert>

struct Program;

class Parser {
public:
  Parser(Lexer &lexer, SymbolTable &sym_tab) : lexer(lexer), sym_tab(sym_tab) { advance(); }
  Program *parse();
  Token getToken() { return token; }
  Token getLookahead() { return lookahead; }
  bool match(Token tok) {
    if (lookahead == tok) {
      advance();
      return true;
    }
    return false;
  }
  bool match(TokenType tt) {
    if (lookahead.type == tt) {
      advance();
      return true;
    }
    return false;
  }
  bool match(std::string content) {
    if (lookahead.content == content) {
      advance();
      return true;
    }
    return false;
  }
  bool try_match(std::string content) {
    return lookahead.content == content;
  }
    bool try_match(TokenType tt) {
      return lookahead.type == tt;
    }
  void match_or_panic(Token tok) {
    if (!match(tok)) {
      std::cerr << "cannot match: " << tok.type << ", " << tok.content
                << std::endl;
      panic();
    }
  }
  void match_or_panic(TokenType tt) {
    if (!match(tt)) {
      std::cerr << "cannot match: " << tt << std::endl;
      panic();
    }
  }
  void match_or_panic(std::string content) {
    if (!match(content)) {
      std::cerr << "cannot match: " << content << std::endl;
      panic();
    }
  }
  void panic() {
    std::cerr << "panicked at " << lexer.getLine() << ":" << lexer.getPos()
              << std::endl;
    exit(-1);
  }
  void panic_err(std::string err_msg) {
    std::cerr << "panicked while translating! " << err_msg << std::endl;
    exit(-2);
  }
  void advance() {
    token = lookahead;
    lookahead = lexer.nextToken();
#ifdef WITH_OUTPUT
    token.print(std::cout);
#endif
  }

  SymbolTable &getST() {
    return sym_tab;
  }

  TranslateManager &getTM() {
    return trans_m;
  }

private:
  Lexer &lexer;
  SymbolTable &sym_tab;
  TranslateManager trans_m;
  Token token, lookahead;
};

struct Block;
struct Statement;
struct St;
struct Condition;
struct Cond;
struct Expression;
struct Term;
struct Factor;

struct Nonterminal {
  virtual void match(Parser &pars) {}
  virtual void match_with_p(Parser &pars) {
#ifdef WITH_OUTPUT
    std::cout << "(";
#endif
    match(pars);
#ifdef WITH_OUTPUT
    std::cout << ")";
#endif
  }
  virtual MyLabel translate(Parser &pars) {
    return "";
  }
};

struct Program : Nonterminal {
  Block *block;
  virtual void match(Parser &pars) override;
  virtual MyLabel translate(Parser &pars) override;
};

struct Block : Nonterminal {
  std::vector<std::pair<Token, Token>> const_s;
  std::vector<Token> var_s;
  std::vector<std::pair<Token, Block *>> proc_s;
  Statement *statement;
  virtual void match(Parser &pars) override;
  void match_const(Parser &pars);
  void match_var(Parser &pars);
  void match_proc(Parser &pars);
  virtual MyLabel translate(Parser &pars) override;
};

struct Statement : Nonterminal {
  int type;
  St *st;
  virtual void match(Parser &pars) override;
  virtual MyLabel translate(Parser &pars) override;
};

struct St : Nonterminal {};

struct StDef : St {
  Token id;
  Expression *expr = nullptr;
  virtual void match(Parser &pars) override;
  virtual MyLabel translate(Parser &pars) override;
};

struct StCall : St {
  Token id;
  virtual void match(Parser &pars) override;
  virtual MyLabel translate(Parser &pars) override;
};

struct StBegEnd : St {
  std::vector<Statement *> st_s;
  virtual void match(Parser &pars) override;
  virtual MyLabel translate(Parser &pars) override;
};

struct StIf : St {
  Condition *cond;
  Statement *st;
  virtual void match(Parser &pars) override;
  virtual MyLabel translate(Parser &pars) override;
};

struct StWhile : St {
  Condition *cond;
  Statement *st;
  virtual void match(Parser &pars) override;
  virtual MyLabel translate(Parser &pars) override;
};

struct Condition : Nonterminal {
  int type;
  Cond *cond;
  MyLabel else_label;
  virtual void match(Parser &pars) override;
  virtual MyLabel translate(Parser &pars) override;
  void setElseLabel(MyLabel &lab);
};

struct Cond : Nonterminal {
  MyLabel else_label;
};

struct CondOdd : Cond {
  Expression *expr;
  virtual void match(Parser &pars) override;
  virtual MyLabel translate(Parser &pars) override;
};

struct CondBi : Cond {
  Expression *expr1, *expr2;
  Token op;
  virtual void match(Parser &pars) override;
  virtual MyLabel translate(Parser &pars) override;
};

struct Expression : Nonterminal {
  bool with_init;
  Token init_op;
  Term *first_term;
  std::vector<std::pair<Token, Term *>> terms;
  virtual void match(Parser &pars) override;
  virtual MyLabel translate(Parser &pars) override;
};

struct Term : Nonterminal {
  Factor *first_fact;
  std::vector<std::pair<Token, Factor *>> facts;
  virtual void match(Parser &pars) override;
  virtual MyLabel translate(Parser &pars) override;
};

struct Factor : Nonterminal {
  bool is_expr;
  Token tok;
  Expression *expr;
  virtual void match(Parser &pars) override;
  virtual MyLabel translate(Parser &pars) override;
};

#endif