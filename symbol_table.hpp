#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP
#include <vector>
#include <map>
#include <string>

// TODO more types
enum Type {
  VAL,
  PROC,
};

struct Symbol {
  Symbol(std::string name, Type type = VAL) : name(name), type(type), is_const(false) {
    
  }

  void markConst() {
    is_const = true;
  }

  std::string name;
  Type type;
  // size_t size;
  bool is_const;
  // size_t address;
  std::string value;
};

using SymbolTable = std::map<std::string, Symbol>;

#endif