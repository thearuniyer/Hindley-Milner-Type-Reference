#ifndef __PARSER__H__
#define __PARSER__H__

#include <vector>
#include <string>

#include "inputbuf.h"
#include "lexer.h"

struct sTableEntry
{
  string name;
  int line_no;
  int type;
  int printed;
};

struct sTable
{
  sTableEntry* item;
  sTable *prev;
  sTable *next;
};

class Parser{
  void parse_program();
  void parse_global_vars();
  void parse_var_decl_list();
  void parse_var_decl();
  void parse_var_list();
  void parse_type_name();
  void parse_body();
  void parse_stmt_list();
  void parse_stmt();
  void parse_assign_stmt();
  void parse_expr();
  void parse_unary_operator();
  void parse_binary_operator();
  void parse_primary();
  void parse_if_stmt();
  void parse_while_stmt();
  void parse_switch_stmt();
  void parse_case_list();
  void parse_case();
};

#endif  //__PARSER__H__
