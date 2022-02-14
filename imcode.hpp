#ifndef IMCODE_HPP
#define IMCODE_HPP
#include <iostream>

using MyLabel = std::string;

enum OpCode {
  ADD,
  SUB,
  MUL,
  DIV,
  JEQ,
  JL,
  JG,
  JNE,
};

static const char *opcode_name[] = {
  "add", "sub", "mul", "div", "jeq", "jl", "jg", "jne"
};

struct TranslateManager {
  MyLabel createTempId() {
    char temp[10];
    sprintf(temp, "v%d", cnt_temp_id++);
    return temp;
  }
  MyLabel createLabel() {
    char temp[10];
    sprintf(temp, "l%d", cnt_label++);
    return temp;
  }
  MyLabel immediate(int imm) {
    char temp[100];
    sprintf(temp, "%d", imm);
    return temp;
  }
  void writeLabel(MyLabel label) {
    std::cout << label << ": ";
  }
  void write(OpCode opcode, MyLabel op1, MyLabel op2, MyLabel dest) {
    std::cout << opcode_name[(int) opcode] << " " << op1 << " " << op2 << " " << dest << std::endl;
  }
private:
  int cnt_temp_id = 0;
  int cnt_label = 0;
};

#endif