#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "parser.h"
#include "lexer.h"
#include "inputbuf.h"

using namespace std;

Token t;

void syntax_error(){
  std::cout << "Syntax Error" << '\n';
  exit(1);
}

// Basic Functions

void Parser::addList(std::string name, int line, int type){
  if(symbolTable == NULL)
  {
    sTable* newEntry = new sTable;
    sTableEntry* newItem = new sTableEntry();

    // Add item to the node
    newItem->name = name;
    newItem->lineNo = t.line_no;
    newItem->type = type;
    newItem->printed = 0;

    // Add node as newEntry
    newEntry->item = newItem;
    newEntry->next = NULL;
    newEntry->prev = NULL;

    symbolTable = newEntry;
  }
  else
  {
    // Retrieve old symbolTable
    sTable* temp = symbolTable;
    while(temp->next != NULL)
    {
      temp = temp->next;
    }

    sTable* newEntry = new sTable();
    sTableEntry* newItem = new sTableEntry();

    newItem->name = name;
    newItem->lineNo = t.line_no;
    newItem->type = type;
    newItem->printed = 0;

    newEntry->item = newItem;
    newEntry->next = NULL;
    newEntry->prev = temp;
    temp->next = newEntry;
  }
}

int Parser::searchList(std::string n){
  sTable* temp = symbolTable;
  bool found = false;
  if(symbolTable == NULL)
  {
    addList(n, t.line_no, enumCount);
    enumCount++;
    return 4;
  }
  else
  {
    while(temp->next != NULL)
    {
      if(strcmp(temp->item->name.c_str(), n.c_str()) == 0)
      {
        found = true;
        return(temp->item->type);
      }
      else
      {
        temp = temp->next;
      }
    }

    if(strcmp(temp->item->name.c_str(), n.c_str()) == 0)
    {
      found = true;
      return(temp->item->type);
    }
    if(!found)
    {
      addList(n, t.line_no, enumCount);
      enumCount++;
      return(enumCount-1)
    }
  }
}


// Parser Functions Begins
void Parser::parse_program(){
  t = input.GetToken();
  if(t.token_type == ID)
  {
    input.UngetToken(t);
    parse_global_vars();
    parse_body();
  }
  else if(t.token_type == LBRACE)
  {
    input.UngetToken(t);
    parse_body();
  }
  else
  {
    syntax_error();
    return;
  }
}

void Parser::parse_global_vars(){
  t = input.GetToken();
  if(t.token_type == ID)
  {
    input.UngetToken(t);
    parse_var_decl_list();
  }
  else
  {
    syntax_error();
    return;
  }
}

void Parser::parse_var_decl_list(){
  t = input.GetToken();
  if(t.token_type == ID)
  {
    input.UngetToken(t);
    parse_var_decl();

    t = input.GetToken();
    if(t.token_type == ID)
    {
      input.UngetToken(t);
      parse_var_decl_list();
    }
    else
    {
      input.UngetToken(t);
    }
  }
  else
  {
    syntax_error();
    return;
  }
}

void Parser::parse_var_decl(){
  t = input.GetToken();
  if(t.token_type == ID)
  {
    input.UngetToken(t);
    parse_var_list();

    t = input.GetToken();
    if(t.token_type == COLON)
    {
      parse_type_name();

      t1 = input.GetToken();
      if(t1.token_type == SEMICOLON)
      {
        // parsed successfully
      }
      else
      {
        syntax_error();
        return;
      }
    }
    else
    {
      syntax_error();
      return;
    }
  }
  else
  {
    input.UngetToken(t);
  }
}

int Parser::parse_var_list(){
  t = input.GetToken();
  int var;
  addList(t.lexeme, t.line_no, 0);

  if(t.token_type == ID)
  {
    //parsed and store
    //look for more
    t = input.GetToken();
    if(t.token_type == COMMA)
    {
      var = parse_var_list();
    }
    else if(t.token_type == COLON)
    {
      input.UngetToken(t);
    }
    else
    {
      syntax_error();
      return;
    }
  }
  else
  {
    syntax_error();
    return;
  }
  return 0;
}

void Parser::parse_type_name(){
  t = input.GetToken();
  if(t.token_type == INT ||
     t.token_type == REAL ||
     t.token_type == BOOL)
     {
       //something
     }
     else
     {
       syntax_error();
       return;
     }
}

void Parser::parse_body(){
  t = input.GetToken();

  if(t.token_type == LBRACE)
  {
    parse_stmt_list();
    t = input.GetToken();
    if(t.token_type == RBRACE)
    {
      // parsed successfully
      return 0;
    }
    else
    {
      syntax_error();
      return;
    }
  }
  else if(t.token_type == END_OF_FILE)
  {
    lexer.UngetToken(t);
    return;
  }
  else
  {
    syntax_error();
    return;
  }
}

void Parser::parse_stmt_list(){
  t = input.GetToken();
  if(t.token_type == ID ||
     t.token_type == IF ||
     t.token_type == WHILE ||
     t.token_type == SWITCH)
     {
       input.UngetToken(t);
       parse_stmt();

       t = input.GetToken();
       if(t.token_type == ID ||
          t.token_type == IF ||
          t.token_type == WHILE ||
          t.token_type == SWITCH)
          {
            input.UngetToken(t);
            parse_stmt_list();
          }
     }
     else
     {
       syntax_error();
       return;
     }
}

void Parser::parse_stmt(){
  t = input.GetToken();

  if(t.token_type == ID)
  {
    input.UngetToken(t);
    parse_assign_stmt();
  }
  else if(t.token_type == IF)
  {
    input.UngetToken(t);
    parse_if_stmt();
  }
  else if(t.token_type == WHILE)
  {
    input.UngetToken(t);
    parse_while_stmt();
  }
  else if(t.token_type == SWITCH)
  {
    input.UngetToken(t);
    parse_switch_stmt();
  }
  else
  {
    syntax_error();
    return;
  }
}

void Parser::parse_assign_stmt(){
  t = input.GetToken();
  if(t.token_type == ID)
  {
    //something
    t = input.GetToken();
    if(t.token_type == EQUAL)
    {
      parse_expr();

      t = input.GetToken();
      if(t.token_type == SEMICOLON)
      {
        // parsed successfully
      }
      else
      {
        syntax_error();
        return;
      }
    }
    else
    {
      syntax_error();
      return;
    }
    else
    {
      syntax_error();
      return;
    }
  }
  else
  {
    syntax_error();
    return;
  }
}

void Parser::parse_if_stmt(){
  t = input.GetToken();
  if(t.token_type == IF)
  {
    t1 = input.GetToken();
    if(t1.token_type == LPAREN)
    {
      parse_expr();

      t2 = input.GetToken();
      if(t2.token_type == RPAREN)
      {
        parse_body();
      }
      else
      {
        syntax_error();
        return;
      }
    }
    else
    {
      syntax_error();
    }
  }
  else
  {
    syntax_error();
    return;
  }
}

void Parser::parse_while_stmt(){
  t = input.GetToken();
  if(t.token_type == WHILE)
  {
    t1 = input.GetToken();
    if(t1.token_type == LPAREN)
    {
      parse_expr();

      t2 = input.GetToken();
      if(t2.token_type == RPAREN)
      {
        parse_body();
      }
      else
      {
        syntax_error();
        return;
      }
    }
    else
    {
      syntax_error();
      return;
    }
  }
  else
  {
    syntax_error();
  }
}

void Parser::parse_switch_stmt(){
  t = input.GetToken();
  if(t.token_type == SWITCH)
  {
    t1 = input.GetToken();
    if(t1.token_type == LPAREN)
    {
      parse_expr();

      t2 = input.GetToken();
      t3 = input.GetToken();
      if(t2.token_type == RPAREN && t3.token_type == LBRACE)
      {
        parse_case_list();

        t4 = input.GetToken();
        if(t4.token_type == RBRACE)
        {
          // parsed successfully
        }
        else
        {
          syntax_error();
          return;
        }
      }
      else
      {
        syntax_error();
        return;
      }
    }
    else
    {
      syntax_error();
      return;
    }
  }
  else
  {
    syntax_error();
    return;
  }
}

int Parser::parse_expr(){
  t = input.GetToken();
  int var;
  // check C2 errors
  if(t.token_type == ID ||
     t.token_type == NUM ||
     t.token_type == REALNUM ||
     t.token_type == TRUE ||
     t.token_type == FALSE)
  {
    input.UngetToken(t);
    var = parse_primary();
  }
  else if(t.token_type == PLUS ||
          t.token_type == MINUS ||
          t.token_type == MULT ||
          t.token_type == DIV ||
          t.token_type == GREATER ||
          t.token_type == GTEQ ||
          t.token_type == LESS ||
          t.token_type == LTEQ ||
          t.token_type == EQUAL ||
          t.token_type == NOTEQUAL)
  {
    input.UngetToken(t);
    var = parse_binary_operator();
    int lExpr = parse_expr();
    int rExpr = parse_expr();

    if((lExpr != rExpr) || isExpr(var))
    {
      if(var >= 15 && var <= 18)
      {
        if((lExpr <= 2 && rExpr > 3) || (leftExp > 3 && rightExp > 3))
        {
          updateTypes(lExpr, rExpr);
          rExpr = lExpr;
        }
        else if(lExpr > 3 && rExpr <= 2)
        {
          updateTypes(lExpr, rExpr);
          lExpr = rExpr;
        }
        else
        {
          cout << "TYPE MISMATCH " << t.line_no << " C2"<<endl;
          exit(1);
        }
      }
      else
      {
        cout << "TYPE MISMATCH " << t.line_no << " C2"<<endl;
        exit(1);
      }
    }
    if(var >= 19 && var <= 26)
    {
      var = 3;
    }
    else
    {
      var = rExpr;
    }
  }
  else if(t.token_type == NOT)
  {
    input.UngetToken(t);
    var = parse_unary_operator();
    var = parse_expr();

    if(var != 3)
    {
      cout << "TYPE MISMATCH " << t.line_no << " C2"<<endl;
      exit(1);
    }
  }
  else
  {
    syntax_error();
    return 0;
  }
  return var;
}

void Parser::parse_primary(){
  t = input.GetToken();
  if(t.token_type == ID)
  {
    //parsed successfully
    return(searchList(t.lexeme));
  }
  else if(t.token_type == NUM)
  {
    return 1;
  }
  else if(t.token_type == REALNUM)
  {
    return 2;
  }
  else if(t.token_type == TRUE)
  {
    return 3;
  }
  else if(t.token_type == FALSE)
  {
    return 4;
  }
  else
  {
    syntax_error();
    return 0;
  }
}

int Parser::parse_binary_operator(){
  t = input.GetToken();
  if(t.token_type == PLUS)
  {
    return 15;
  }
  else if(t.token_type == MINUS)
  {
    return 16;
  }
  else if(t.token_type == MULT)
  {
    return 17;
  }
  else if(t.token_type == DIV)
  {
    return 18;
  }
  else if(t.token_type == GREATER)
  {
    return 20;
  }
  else if(t.token_type == GTEQ)
  {
    return 23;
  }
  else if(t.token_type == LTEQ)
  {
    return 21;
  }
  else if(t.token_type == EQUAL)
  {
    return 26;
  }
  else if(t.token_type == NOTEQUAL)
  {
    return 22;
  }
  else
  {
    syntax_error();
    return -1;
  }
}

int Parser::parse_unary_operator(){
  t = input.GetToken();

  if(t.token_type == NOT)
  {
    // parsed successfully
    return 1;
  }
  else
  {
    syntax_error();
    return 0;
  }
}

void Parser::parse_case_list(){
  t = input.GetToken();
  if(t.token_type == CASE)
  {
    input.UngetToken(t);
    parse_case();

    t = input.GetToken();
    if(t.token_type == CASE)
    {
      input.UngetToken(t);
      parse_case_list();
    }
  }
  else
  {
    syntax_error();
    return;
  }
}

void Parser::parse_case(){
  t = input.GetToken();
  if(t.token_type == CASE)
  {
    t1 = input.GetToken();
    if(t1.token_type == NUM)
    {
      // take t1.lexeme and store

      t2 = input.GetToken();
      if(t2.token_type == COLON)
      {
        parse_body();
      }
      else
      {
        syntax_error();
        return;
      }
    }
    else
    {
      syntax_error();
      return;
    }
  }
  else
  {
    syntax_error();
    return;
  }
}



int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}
