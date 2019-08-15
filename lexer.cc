
/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE", "INT", "REAL", "BOOL",
                      "TRUE", "FALSE", "IF", "WHILE", "SWITCH",
                      "CASE", "PUBLIC", "PRIVATE", "NUM",
                      "REALNUM", "NOT", "PLUS", "MINUS", "MULT",
                      "DIV", "GTEQ", "GREATER", "LTEQ", "NOTEQUAL",
                      "LESS", "LPAREN", "RPAREN", "EQUAL", "COLON",
                      "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "ID",
                      "ERROR" // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 11
string keyword[] = { "int", "real", "bool", "true",
                     "false", "if", "while", "switch",
                     "case", "public", "private" };

LexicalAnalyzer input;
Token token;
int enumCount = 4;

struct sTableEntry
{
    string name;
    int lineNo;
    int type;
    int printed;
};

struct sTable
{
    sTableEntry* item;
    sTable *prev;
    sTable *next;
};

sTable* symbolTable;
int line = 0;
string output = "";


void addList(std::string name, int line, int type)
{
    if(symbolTable == NULL)
    {
        sTable* newEntry = new sTable();
        sTableEntry* newItem = new sTableEntry();

        newItem->name = name;
        newItem->lineNo = token.line_no;
        newItem->type = type;
        newItem->printed = 0;

        newEntry->item = newItem;
        newEntry->next = NULL;
        newEntry->prev = NULL;

        symbolTable = newEntry;

    }
    else
    {
        sTable* temp = symbolTable;
        while(temp->next != NULL)
        {
          if (temp->item->name == name && temp->item->type == 0)
          {
            temp->item->type = type;
            return;
          }
            temp = temp->next;
        }

        sTable* newEntry = new sTable();
        sTableEntry* newItem = new sTableEntry();

        newItem->name = name;
        newItem->lineNo = token.line_no;
        newItem->type = type;
        newItem->printed = 0;

        newEntry->item = newItem;
        newEntry->next = NULL;
        newEntry->prev = temp;
        temp->next = newEntry;
    }
}

int searchList(std::string n)
{
    sTable* temp = symbolTable;
    bool found = false;
    if (symbolTable == NULL)
    {
        addList(n, token.line_no, enumCount);
        // std::cout << "addList = "<<n<<", "<<  enumCount << '\n';
        enumCount++;
        return (4);
    }
    else
    {
        while(temp->next != NULL)
        {
            if(strcmp(temp->item->name.c_str(), n.c_str()) == 0)
            {
                found = true;
                // std::cout << "while->type = "<<  temp->item->type << '\n';
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
            // std::cout << "if->type = "<<  temp->item->type << '\n';
            return(temp->item->type);
        }
        if(!found)
        {
            addList(n, token.line_no, enumCount);
            enumCount++;
            int t = enumCount-1;
            return(t);
        }
    }
    return 0;
}


void Token::Print()
{
    cout << "{" << this->lexeme << " , "
        << reserved[(int) this->token_type] << " , "
        << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    line = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');
    line = line_no;

    while (!input.EndOfInput() && isspace(c))
    {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
        line = line_no;
    }

    if (!input.EndOfInput())
    {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::SkipComments()
{
    bool comments = false;
    char c;
    if(input.EndOfInput() )
    {
        input.UngetChar(c);
        return comments;
    }

    input.GetChar(c);

    if(c == '/')
    {
        input.GetChar(c);
        if(c == '/')
        {
            comments = true;
            while(c != '\n')
            {
                comments = true;
                input.GetChar(c);
            }
            line_no++;
            line = line_no;
            SkipComments();
        }
        else
        {
            comments = false;
            cout << "Syntax Error\n";
            exit(0);
        }
    }
    else
    {
        input.UngetChar(c);
        return comments;
    }
    return true;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
        {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
        {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;
    bool realNUM = false;
    input.GetChar(c);
    if (isdigit(c))
    {
        if (c == '0')
        {
            tmp.lexeme = "0";
            input.GetChar(c);
            if(c == '.')
            {
                input.GetChar(c);

                if(!isdigit(c))
                {
                    input.UngetChar(c);
                }
                else
                {
                    while (!input.EndOfInput() && isdigit(c))
                    {
                        tmp.lexeme += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                    input.UngetChar(c);
                }
            }
            else
            {
                input.UngetChar(c);
            }
        }
        else
        {
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c))
            {
                tmp.lexeme += c;
                input.GetChar(c);
            }
            if(c == '.')
            {
                input.GetChar(c);

                if(!isdigit(c))
                {
                    input.UngetChar(c);
                }
                else
                {
                    while (!input.EndOfInput() && isdigit(c))
                    {
                        tmp.lexeme += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                }
            }
            if (!input.EndOfInput())
            {
                input.UngetChar(c);
            }
        }
        // TODO: You can check for REALNUM, BASE08NUM and BASE16NUM here!
        if(realNUM)
        {
            tmp.token_type = REALNUM;
        }
        else
        {
            tmp.token_type = NUM;
        }
        tmp.line_no = line_no;
        return tmp;
    }
    else
    {
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c))
    {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c))
        {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;

        if (IsKeyword(tmp.lexeme))
        {
             tmp.token_type = FindKeywordIndex(tmp.lexeme);
        }
        else
        {
            tmp.token_type = ID;
        }
    }
    else
    {
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}


TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty())
    {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    // SkipComments();
    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    //cout << "\n Char obtained " << c << "\n";
    switch (c)
    {
        case '!':
            tmp.token_type = NOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '>':
            input.GetChar(c);
            if(c == '=')
            {
                tmp.token_type = GTEQ;
            }
            else
            {
                input.UngetChar(c);
                tmp.token_type = GREATER;
            }
            return tmp;
        case '<':
            input.GetChar(c);
            if(c == '=')
            {
                tmp.token_type = LTEQ;
            }
            else if (c == '>')
            {
                tmp.token_type = NOTEQUAL;
            }
            else
            {
                input.UngetChar(c);
                tmp.token_type = LESS;
            }
            return tmp;
        case '(':
            //cout << "\n I am here" << c << " \n";
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '{':
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            return tmp;
        default:
            if (isdigit(c))
            {
                input.UngetChar(c);
                return ScanNumber();
            }
            else if (isalpha(c))
            {
                input.UngetChar(c);
                //cout << "\n ID scan " << c << " \n";
                return ScanIdOrKeyword();
            }
            else if (input.EndOfInput())
            {
                tmp.token_type = END_OF_FILE;
            }
            else
            {
                tmp.token_type = ERROR;
            }
            return tmp;
    }
}

int parse_var_list(void)
{
    token = input.GetToken();
    int var;
    addList(token.lexeme, token.line_no, 0);

    if(token.token_type == ID)
    {
        token = input.GetToken();
        if(token.token_type == COMMA)
        {
            var = parse_var_list();
        }
        else if(token.token_type == COLON)
        {
            input.UngetToken(token);
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_body(void);

int parse_unaryOperator(void)
{
    token = input.GetToken();

    if(token.token_type == NOT)
    {
        return(1);
    }
    else
    {
        cout << "\n Syntax Error\n";
        return(0);
    }
}

int parse_binaryOperator(void)
{
    token = input.GetToken();
    if(token.token_type == PLUS  )
    {
        return(15);
    }
    else if(token.token_type == MINUS )
    {
        return(16);
    }
    else if(token.token_type == MULT)
    {
        return(17);
    }
    else if(token.token_type == DIV )
    {
        return(18);
    }
    else if(token.token_type == GREATER)
    {
        return(20);
    }
    else if(token.token_type == LESS  )
    {
        return(23);
    }
    else if(token.token_type == GTEQ )
    {
        return(19);
    }
    else if(token.token_type == LTEQ)
    {
        return(21);
    }
    else if(token.token_type == EQUAL )
    {
        return(26);
    }
    else if(token.token_type == NOTEQUAL)
    {
        return(22);
    }
    else
    {
        cout << "\n Syntax Error \n";
        return(-1);
    }
}

int parse_primary(void)
{
    token = input.GetToken();
    if(token.token_type == ID )
    {
        return(searchList(token.lexeme));
    }
    else if(token.token_type == NUM )
    {
        return(1);
    }
    else if(token.token_type == REALNUM)
    {
        return(2);
    }
    else if(token.token_type == TRUE )
    {
        return(3);
    }
    else if(token.token_type == FALSE)
    {
        return(3);
    }
    else
    {
        cout << "\n Syntax Error \n";
        return(0);
    }
}


bool isExpress(int c){
    if(c != 15 && c != 16 && c != 17 && c != 18 && c != 19 && c != 20 && c != 21 && c != 22 && c != 23 && c != 26){
        return true;
    }
    else{
        return false;
    }
}

int parse_expression(void)
{
    //check for C2 error here
    int var;
    token = input.GetToken();
    if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TRUE || token.token_type == FALSE )
    {
        input.UngetToken(token);
        var = parse_primary();
    }
    else if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == GREATER || token.token_type == LESS || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL)
    {
        input.UngetToken(token);
        var = parse_binaryOperator();
        // std::cout << "var ="<< var << '\n';
        int lExpr = parse_expression();
        // std::cout << "lExpr ="<< lExpr << '\n';
        int rExpr = parse_expression();
        // std::cout << "rExpr ="<< rExpr << '\n';
        if((lExpr != rExpr) || isExpress(var))
        {
            if(var == 15 || var == 16 || var == 17 || var == 18)
            {
                if(lExpr <= 2 && rExpr > 3)
                {
                    updateTypes(rExpr, lExpr);
                    rExpr = lExpr;
                    // std::cout << "updated rExpr loop1 lExpr" <<lExpr<<"rExpr="<<rExpr <<'\n';
                }
                else if(lExpr > 3 && rExpr <= 2)
                {
                    updateTypes(rExpr, lExpr);
                    lExpr = rExpr;
                    // std::cout << "updated lExpr loop2 lExpr" <<lExpr<<"rExpr="<<rExpr <<'\n';
                }
                else if(lExpr > 3 && rExpr > 3)
                {
                    updateTypes(rExpr, lExpr);
                    rExpr = lExpr;
                    // std::cout << "updated rExpr loop3 lExpr" <<lExpr<<"rExpr="<<rExpr <<'\n';
                }
                else
                {
                    cout << "TYPE MISMATCH " << token.line_no << " C2"<<endl;
                    exit(1);
                }
            }
            else if(var == 19 || var == 20 || var == 21 || var == 22 || var == 23 || var == 26)
            {
                if(rExpr > 3 && lExpr > 3)
                {
                    updateTypes(rExpr, lExpr);
                    rExpr = lExpr;
                    return(3);
                }
                else
                {
                    cout << "TYPE MISMATCH " << token.line_no << " C2"<<endl;
                    exit(1);
                }
            }
            else
            {
                cout << "TYPE MISMATCH " << token.line_no << " C2"<<endl;
                exit(1);
            }
        }
        if(var == 19 || var == 20 || var == 21 || var == 23 || var == 26)
        {
            var = 3;
        }
        else
        {
            var = rExpr;
        }
    }
    else if(token.token_type == NOT)
    {
        input.UngetToken(token);
        var = parse_unaryOperator();
        var = parse_expression();
        if(var != 3)
        {
            cout << "TYPE MISMATCH " << token.line_no << " C3"<<endl;
            exit(1);
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
        return(0);
    }
    // std::cout << "var = "<<var << '\n';
    return var;
}

void compareLine(int line_No, int token_Type)
{
    sTable* temp = symbolTable;
    while(temp->next != NULL)
    {
        if(temp->item->lineNo == line_No)
        {
            temp->item->type = token_Type;
        }
        temp = temp->next;
    }
    if(temp->item->lineNo == line_No)
    {
        temp->item->type = token_Type;
    }
}

void updateTypes(int currentType, int newType)
{
    sTable* temp = symbolTable;

    while(temp->next != NULL)
    {
        if(temp->item->type == currentType)
        {
            temp->item->type = newType;
        }
        temp = temp->next;
    }
    if(temp->item->type == currentType)
    {
        temp->item->type = newType;
    }
}

int parse_assstmt(void)
{
    int var;
    string name;
    int LHS;
    int RHS;
    token = input.GetToken();
    if(token.token_type == ID)
    {
        LHS = searchList(token.lexeme);
        // std::cout << "LHS = "<<LHS << '\n';
        token = input.GetToken();
        if(token.token_type == EQUAL)
        {
            token = input.GetToken();
            if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TRUE || token.token_type == FALSE || token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == LESS || token.token_type == GREATER || token.token_type== GTEQ || token.token_type== LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL || token.token_type == NOT)
            {
                // std::cout << "token_type = "<< token.token_type << '\n';
                input.UngetToken(token);
                RHS = parse_expression();
                // std::cout << "RHS ="<<RHS << '\n';

                if(LHS <= 3)
                {
                    if(LHS == RHS)
                    {

                    }
                    else
                    {
                        if(LHS <= 3)
                        {
                            cout << "TYPE MISMATCH " << token.line_no << " C1" << endl;
                            exit(1);
                        }
                        else
                        {
                            updateTypes(RHS,LHS);
                            RHS = LHS;
                        }
                    }
                }
                else
                {
                    updateTypes(LHS,RHS);
                    LHS = RHS;
                }
                token = input.GetToken();
                if(token.token_type == SEMICOLON)
                {
                    //cout << "\n Rule parsed: assignment_stmt -> ID EQUAL expression SEMICOLON"<<endl;

                }
                else
                {
                    cout << "\n Syntax Error " << token.token_type << " \n";
                }
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_case(void)
{

    int var;
    token = input.GetToken();
    if(token.token_type == CASE )
    {
        token = input.GetToken();
        if(token.token_type == NUM)
        {
            token = input.GetToken();
            if(token.token_type == COLON)
            {
                var = parse_body();
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return 0;
}

int parse_caselist(void)
{
    int var;
    token = input.GetToken();
    if(token.token_type == CASE)
    {
        input.UngetToken(token);
        var = parse_case();
        token = input.GetToken();
        if(token.token_type == CASE)
        {
            input.UngetToken(token);
            var = parse_caselist();
        }
        else if(token.token_type == RBRACE)
        {
            input.UngetToken(token);
        }
    }
    return(0);
}

int parse_switchstmt(void)
{
    int var;
    token = input.GetToken();
    if(token.token_type == SWITCH)
    {
        token = input.GetToken();
        if(token.token_type == LPAREN)
        {
            var = parse_expression();
            if(var <= 3 && var != 1)
            {
                cout<< "TYPE MISMATCH " << token.line_no << " C5"<<endl;
                exit(1);
            }
            token = input.GetToken();
            if(token.token_type == RPAREN)
            {
                token = input.GetToken();
                if(token.token_type == LBRACE)
                {
                    var = parse_caselist();
                    token = input.GetToken();
                    if(token.token_type == RBRACE)
                    {
                        //Parsed successfully
                    }
                    else
                    {
                        cout << "\n Syntax Error \n";
                    }
                }
                else
                {
                    cout << "\n Syntax Error \n";
                }
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_whilestmt(void)
{
    int var;

    token = input.GetToken();
    if(token.token_type == WHILE)
    {
        token = input.GetToken();
        if(token.token_type == LPAREN)
        {
            var = parse_expression();
            if(var != 3)
            {
                cout<< "TYPE MISMATCH " << token.line_no << " C4" << endl;
                exit(1);
            }
            token = input.GetToken();
            if(token.token_type == RPAREN)
            {
                var = parse_body();
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_ifstmt(void)
{
    int var;
    token = input.GetToken();
    if(token.token_type == IF)
    {
        token = input.GetToken();
        if(token.token_type == LPAREN)
        {
            var = parse_expression();
            if(var != 3)
            {
                cout<< "TYPE MISMATCH " << token.line_no << " C4" << endl;
                exit(1);
            }
            token = input.GetToken();
            if(token.token_type == RPAREN)
            {
                var = parse_body();

            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_stmt(void)
{
    int var;
    token = input.GetToken();
    if(token.token_type == ID)
    {
        input.UngetToken(token);
        var = parse_assstmt();

    }
    else if(token.token_type == IF)
    {
        input.UngetToken(token);
        var = parse_ifstmt();
    }
    else if(token.token_type == WHILE)
    {
        input.UngetToken(token);
        var = parse_whilestmt();
    }
    else if(token.token_type == SWITCH)
    {
        input.UngetToken(token);
        var = parse_switchstmt();
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_stmtlist(void)
{
    token = input.GetToken();
    int var;
    if(token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)
    {
        input.UngetToken(token);
        var = parse_stmt();
        token = input.GetToken();
        if(token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)
        {
            input.UngetToken(token);
            var = parse_stmtlist();

        }
        else if (token.token_type == RBRACE)
        {
            input.UngetToken(token);
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_body(void)
{
    token = input.GetToken();
    int var;
    if(token.token_type == LBRACE)
    {
        var = parse_stmtlist();
        token = input.GetToken();
        if(token.token_type == RBRACE)
        {
            return(0);
        }
        else
        {
            cout << "\n Syntax Error\n ";
            return(0);
        }
    }
    else if(token.token_type == END_OF_FILE)
    {
        input.UngetToken(token);
        return(0);
    }
    else
    {
        cout << "\n Syntax Error \n ";
        return(0);
    }
}

int parse_typename(void)
{
    token = input.GetToken();
    if(token.token_type == INT || token.token_type == REAL || token.token_type == BOOLEAN)
    {
        // std::cout << "token_type = "<< token.token_type << '\n';
        // addList(token.lexeme, token.line_no,token.token_type);
        compareLine(token.line_no, token.token_type);
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(token.token_type);
}

int parse_vardecl(void)
{
    int var;
    token = input.GetToken();
    if(token.token_type == ID)
    {
        //
        input.UngetToken(token);
        var = parse_var_list();
        token = input.GetToken();
        if(token.token_type == COLON)
        {
            var = parse_typename();

            //addList(lexeme, token.line_no,var);
            // std::cout << "var = " << var << '\n';
            token = input.GetToken();
            if(token.token_type == SEMICOLON)
            {
                //Parsed successfully
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_vardecllist(void)
{
    int var;
    token = input.GetToken();
    while(token.token_type == ID)
    {
        input.UngetToken(token);
        var = parse_vardecl();
        token = input.GetToken();
        if(token.token_type != ID)
        {
          //Parsed successfully
        }
        else
        {
          //Parsed successfully
        }
    }
    input.UngetToken(token);
    return(0);
}

string global = "::";
int parse_globalVars(void)
{
    token = input.GetToken();
    int var;
    if(token.token_type == ID)
    {
        input.UngetToken(token);
        var = parse_vardecllist();
    }
    else
    {
        cout << "Syntax Error";
    }
    return(0);
}

int parse_program(void)
{
    token = input.GetToken();
    int var;
    while (token.token_type != END_OF_FILE)
    {
        if(token.token_type == ID)
        {
            input.UngetToken(token);
            var = parse_globalVars();
            var = parse_body();
        }
        else if(token.token_type == LBRACE)
        {
            input.UngetToken(token);
            var = parse_body();
        }
        else if(token.token_type == END_OF_FILE)
        {
            return(0);
        }
        else
        {
            cout << "\n Syntax Error\n";
            return(0);
        }
        token = input.GetToken();
    }
    return 0;
}



void printList(void)
{
    sTable* temp = symbolTable;
    int temp1;

    while(temp->next != NULL)
    {
       if(temp->item->type > 3 && temp->item->printed == 0)
        {
            temp1 = temp->item->type;
            output += temp->item->name;
            temp->item->printed = 1;
            while(temp->next != NULL)
            {
                temp = temp->next;
                if(temp->item->type == temp1)
                {
                    output += ", " + temp->item->name;
                    temp->item->printed = 1;
                }
                else
                {

                }
            }
            output += ": ? #";
            cout << output <<endl;
            temp->item->printed = 1;
            output = "";
            temp = symbolTable;
        }
        else if(temp->item->type < 4 && temp->item->printed == 0)
        {
            string lCase = keyword[(temp->item->type)-1 ];
            int temp1 = temp->item->type;
            output = temp->item->name + ": " + lCase + " #";
            cout << output <<endl;
            output = "";
            temp->item->printed = 1;

            while(temp->next != NULL  && temp->next->item->type == temp1)
            {
                temp = temp->next;
                string lCase2 = keyword[(temp->item->type)-1];
                output = temp->item->name + ": " + lCase2 + " #";
                cout << output <<endl;
                temp->item->printed = 1;
                output = "";
            }
        }
        else
        {
            temp = temp->next;
        }
    }
    if(temp->item->type <= 3 && temp->item->printed == 0)
    {
        string lCase3 = keyword[(temp->item->type)-1];
        output += temp->item->name + ": " + lCase3 + " #";
        cout << output <<endl;
        output = "";
    }
    else if (temp->item->type > 3 && temp->item->printed == 0)
    {
        output += temp->item->name + ":" + " ? " + "#";
        cout << output <<endl;
        output = "";
    }
    else
    {

    }
}

int main()
{
    int p;
    p = parse_program();
    printList();

}
