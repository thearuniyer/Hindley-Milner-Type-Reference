/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

// ------- token types -------------------

typedef enum { END_OF_FILE = 0,
    IF, WHILE, SWITCH, CASE,
    INT, REAL, BOOL, TRUE, FALSE,
    PLUS, MINUS, DIV, MULT, NOT,
    EQUAL, COLON, COMMA, SEMICOLON,
    LBRACE, RBRACE, LPAREN, RPAREN,
    NOTEQUAL, GREATER, LESS, LTEQ, GTEQ,
    NUM, ID, ERROR, REALNUM, PUBLIC, PRIVATE // TODO: Add labels for new token types here
} TokenType;

class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

class LexicalAnalyzer {
  public:
    Token GetToken();
    TokenType UngetToken(Token);
    LexicalAnalyzer();

  private:
    std::vector<Token> tokens;
    int line_no;
    Token tmp;
    InputBuffer input;

    bool SkipSpace();
    bool IsKeyword(std::string);
    TokenType FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
    Token ScanNumber();
};

#endif  //__LEXER__H__
