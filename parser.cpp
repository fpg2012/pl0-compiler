#include "parser.hpp"

#define ADD_VAR(name) pars.getST().emplace((name), Symbol(name))
#define ADD_PROC(name) pars.getST().emplace((name), Symbol((name), (PROC)))
#define GET_SYM(name) pars.getST().at((name))
#define WRITE(op, op1, op2, dest) pars.getTM().write((op), (op1), (op2), (dest))
#define TM pars.getTM()

Program *Parser::parse() {
    auto prog = new Program();
    prog->match_with_p(*this);
    std::cout << std::endl;
    prog->translate(*this);
    return prog;
}

void Condition::setElseLabel(MyLabel &lab) {
  else_label = lab;
  cond->else_label = lab;
}

// match

void Factor::match(Parser &pars) {
  if (pars.match(ID) || pars.match(INT)) {
    is_expr = false;
    tok = pars.getToken();
  } else {
    is_expr = true;
    pars.match_or_panic("(");
    expr = new Expression();
    expr->match_with_p(pars);
    pars.match_or_panic(")");
  }
}

void Program::match(Parser &pars) {
  block = new Block();
  block->match(pars);
}

void Block::match(Parser &pars) {
  if (pars.getLookahead().type == CONST) {
    match_const(pars);
  }
  std::cout << std::endl;
  if (pars.getLookahead().type == VAR) {
    match_var(pars);
  }
  std::cout << std::endl;
  if (pars.getLookahead().type == PROCEDURE) {
    match_proc(pars);
  }
  std::cout << std::endl;
  statement = new Statement();
  statement->match_with_p(pars);
}

void Block::match_const(Parser &pars) {
  pars.match_or_panic(CONST);
  do {
    pars.match_or_panic(ID);
    Token id = pars.getToken();
    pars.match_or_panic("=");
    pars.match_or_panic(INT);
    Token num = pars.getToken();
    const_s.emplace_back(id, num);
  } while (pars.match(COMMA));
  pars.match_or_panic(SEMI);
}

void Block::match_var(Parser &pars) {
  pars.match_or_panic(VAR);
  do {
    pars.match_or_panic(ID);
    Token id = pars.getToken();
    var_s.push_back(id);
  } while (pars.match(","));
  pars.match_or_panic(SEMI);
}

void Block::match_proc(Parser &pars) {
  do {
    pars.match_or_panic(PROCEDURE);
    pars.match_or_panic(ID);
    Token id = pars.getToken();
    pars.match_or_panic(SEMI);
    auto *bl = new Block();
    bl->match_with_p(pars);
    pars.match_or_panic(SEMI);
  } while (pars.match(PROCEDURE));
}

void Statement::match(Parser &pars) {
  auto lh = pars.getLookahead();
  if (lh.type == CALL) {
    type = CALL;
    auto *stcall = new StCall();
    stcall->match_with_p(pars);
    st = stcall;
  } else if (lh.type == BEGIN) {
    type = BEGIN;
    auto *stbegin = new StBegEnd();
    stbegin->match_with_p(pars);
    st = stbegin;
  } else if (lh.type == IF) {
    type = IF;
    StIf *stif = new StIf();
    stif->match_with_p(pars);
    st = stif;
  } else if (lh.type == WHILE) {
    type = WHILE;
    auto *stwhile = new StWhile();
    stwhile->match_with_p(pars);
    st = stwhile;
  } else if (lh.type == ID) {
    type = ID;
    auto *stdef = new StDef();
    stdef->match_with_p(pars);
    st = stdef;
  } else {
    // error
    pars.panic();
  }
}

void StDef::match(Parser &pars) {
  pars.match_or_panic(ID);
  id = pars.getToken();
  pars.match_or_panic(":=");
  expr = new Expression();
  expr->match_with_p(pars);
}

void StCall::match(Parser &pars) {
  pars.match_or_panic(CALL);
  pars.match_or_panic(ID);
  id = pars.getToken();
}

void StBegEnd::match(Parser &pars) {
  pars.match_or_panic(BEGIN);
  std::cout << std::endl;
  do {
    auto *st = new Statement();
    st->match_with_p(pars);
    st_s.push_back(st);
    pars.match(SEMI);
    std::cout << std::endl;
  } while (!pars.try_match(END));
  pars.match_or_panic(END);
  std::cout << std::endl;
}

void StIf::match(Parser &pars) {
  pars.match_or_panic(IF);
  cond = new Condition();
  cond->match_with_p(pars);
  pars.match_or_panic(THEN);
  st = new Statement();
  st->match(pars);
}

void StWhile::match(Parser &pars) {
  pars.match_or_panic(WHILE);
  cond = new Condition();
  cond->match_with_p(pars);
  std::cout << std::endl;
  pars.match_or_panic(DO);
  std::cout << std::endl;
  st = new Statement();
  st->match(pars);
}

void Condition::match(Parser &pars) {
  if (pars.getLookahead().type == ODD) {
    type = 1;
    auto *condodd = new CondOdd();
    condodd->match_with_p(pars);
    cond = condodd;
  } else {
    type = 0;
    auto *condbi = new CondBi();
    condbi->match_with_p(pars);
    cond = condbi;
  }
}

void CondBi::match(Parser &pars) {
  expr1 = new Expression();
  expr2 = new Expression();
  expr1->match_with_p(pars);
  if (pars.match("=") || pars.match("#") || pars.match("<") ||
      pars.match(">")) {
    op = pars.getToken();
  } else {
    // error
    pars.panic();
  }
  expr2->match_with_p(pars);
}

void CondOdd::match(Parser &pars) {
  pars.match_or_panic(ODD);
  expr = new Expression();
  expr->match_with_p(pars);
}

void Expression::match(Parser &pars) {
  if (pars.match("+") || pars.match("-")) {
    with_init = true;
    init_op = pars.getToken();
  } else {
    with_init = false;
  }

  first_term = new Term();
  first_term->match_with_p(pars);

  while (pars.match("+") || pars.match("-")) {
    auto op = pars.getToken();
    auto *term = new Term();
    term->match_with_p(pars);
    terms.emplace_back(op, term);
  }
}

void Term::match(Parser &pars) {
  first_fact = new Factor();
  first_fact->match_with_p(pars);
  while (pars.match("*") || pars.match("/")) {
    auto tok = pars.getToken();
    auto *fact = new Factor();
    fact->match_with_p(pars);
    facts.emplace_back(tok, fact);
  }
}

// translate

MyLabel Factor::translate(Parser &pars) {
  if (is_expr) {
    assert(expr);
    return expr->translate(pars);
  } else {
    MyLabel temp = TM.createTempId();
    if (tok.type == ID && GET_SYM(tok.content).is_const) {
      WRITE(ADD, GET_SYM(tok.content).value, "0", temp);
    } else {
      WRITE(ADD, tok.content, "0", temp);
    }
    return temp;
  }
}

MyLabel Program::translate(Parser &pars) { block->translate(pars); return ""; }

MyLabel Block::translate(Parser &pars) {
  for (const auto &const_pair : const_s) {
    auto const_name = const_pair.first.content;
    auto const_val = const_pair.second.content;
    ADD_VAR(const_name);
    GET_SYM(const_name).markConst();
    GET_SYM(const_name).value = const_val;
  }
  for (const auto &var_pair : var_s) {
    auto var_name = var_pair.content;
    ADD_VAR(var_name);
  }
  for (auto proc : proc_s) {
    auto proc_name = proc.first.content;
    ADD_PROC(proc_name);
    MyLabel proc_label = TM.createLabel();
    GET_SYM(proc_name).value = proc_label;
    TM.writeLabel(proc_label);
    proc.second->translate(pars);
  }
  statement->translate(pars);
  return "";
}

MyLabel Statement::translate(Parser &pars) {
  if (type == ID) {
    ((StDef *) st)->translate(pars);
  } else if (type == IF) {
    ((StIf *)st)->translate(pars);
  } else if (type == CALL) {
    ((StCall *)st)->translate(pars);
  } else if (type == WHILE) {
    ((StWhile *)st)->translate(pars);
  } else if (type == BEGIN) {
    ((StBegEnd *)st)->translate(pars);
  } else {
    std::cerr << this->type << std::endl;
    pars.panic_err("invalid statement type");
  }
  return "";
}

MyLabel StDef::translate(Parser &pars) {
  MyLabel temp = expr->translate(pars);
  WRITE(ADD, temp, "0", id.content);
  return "";
}

MyLabel StCall::translate(Parser &pars) {
  MyLabel dest = GET_SYM(id.content).value;
  WRITE(JEQ, "0", "0", dest);
  return "";
}

MyLabel StBegEnd::translate(Parser &pars) {
  for (auto st : st_s) {
    st->translate(pars);
  }
  return "";
}

MyLabel StIf::translate(Parser &pars) {
  MyLabel else_label = TM.createLabel();
  cond->setElseLabel(else_label);
  MyLabel temp = cond->translate(pars);
  st->translate(pars);
  TM.writeLabel(else_label);
  return "";
}

MyLabel StWhile::translate(Parser &pars) {
  MyLabel loop_label = TM.createLabel();
  MyLabel else_label = TM.createLabel();
  cond->setElseLabel(else_label);
  TM.writeLabel(loop_label);
  MyLabel temp = cond->translate(pars);
  st->translate(pars);
  WRITE(JEQ, "0", "0", loop_label);
  TM.writeLabel(else_label);
  return "";
}

MyLabel Condition::translate(Parser &pars) {
  if (type == 1) {
    // odd
    return ((CondOdd *) cond)->translate(pars);
  } else if (type == 0) {
    // binary
    return ((CondBi *) cond)->translate(pars);
  } else {
    pars.panic_err("invalid condition type");
  }
  return "";
}

MyLabel CondOdd::translate(Parser &pars) {
  MyLabel temp =  expr->translate(pars);
  WRITE(JEQ, temp, "0", else_label);
  return "";
}

MyLabel CondBi::translate(Parser &pars) {
  MyLabel temp = expr1->translate(pars);
  MyLabel temp2 = expr2->translate(pars);
  if (op.content == "=") {
    WRITE(JNE, temp, temp2, else_label);
  } else if (op.content == "#") {
    WRITE(JEQ, temp, temp2, else_label);
  } else if (op.content == "<") {
    MyLabel temp3 = TM.createTempId();
    WRITE(JL, temp, temp2, else_label);
  } else if (op.content == ">") {
    MyLabel temp3 = TM.createTempId();
    WRITE(JG, temp, temp2, else_label);
  } else {
    pars.panic_err("invalid op, expect =#<>");
  }
  return "";
}

MyLabel Expression::translate(Parser &pars) {
  MyLabel ft = first_term->translate(pars);
  if (with_init) {
    auto op = init_op.content;
    if (op == "-") {
      WRITE(SUB, "0", ft, ft);
    }
  }
  for (const auto &op_term : terms) {
    auto op = op_term.first.content;
    auto *trm = op_term.second;
    MyLabel term_temp = trm->translate(pars);
    if (op == "+") {
      WRITE(ADD, ft, term_temp, ft);
    } else if (op == "-") {
      WRITE(SUB, ft, term_temp, ft);
    } else {
      pars.panic_err("invalid op, expect +-");
    }
  }
  return ft;
}

MyLabel Term::translate(Parser &pars) {
  MyLabel ff = first_fact->translate(pars);
  for (const auto &op_fact : facts) {
    auto op = op_fact.first.content;
    auto *fact = op_fact.second;
    MyLabel fact_temp = fact->translate(pars);
    if (op == "*") {
      WRITE(MUL, ff, fact_temp, ff);
    } else if (op == "/") {
      WRITE(DIV, ff, fact_temp, ff);
    } else {
      pars.panic_err("invalid op, expect */");
    }
  }
  return ff;
}
