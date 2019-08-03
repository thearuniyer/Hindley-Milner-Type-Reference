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

void Parser::parse_var_list(){
  t = input.GetToken();
  if(t.token_type == ID)
  {
    //parsed and store
    //look for more

    t1 = input.GetToken();
    if(t1.token_type == COMMA)
    {
      parse_var_list();
    }
    else
    {
      UngetToken(t1);
    }
  }
  else
  {
    syntax_error();
    return;
  }
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
