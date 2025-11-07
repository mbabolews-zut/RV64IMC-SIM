%option noyywrap
%option c++

%{
#include "parser.tab.hh"
#include <string>
#include <iostream>
#include <cstdlib>

size_t yylineno = 0;
yyFlexLexer scanner;

enum FlexToken {
        InstructionToken = 1000,
        IdentifierToken,
        NumberToken,
        DirectiveToken,
        LabelToken,
        CommaToken,
        LeftParenToken,
        RightParenToken,
        NewLineToken,
        SyntaxErrorToken
    };


#ifdef FLEX_DEBUG
#define yydebug(msg) std::cerr << msg << std::endl
#else
#define yydebug(msg) {}
#endif

%}

%%

[a-zA-Z_]+(\.[a-zA-Z0-9_]*){1,2}                    { yydebug("flex: instruction");          return InstructionToken;    }
[a-zA-Z_][a-zA-Z0-9_]*                              { yydebug("flex: identifier");           return IdentifierToken;     }

0[bB][01]+                                          { yydebug("flex: binary number");        return NumberToken;         }
[+-]?((0x[0-9A-Fa-f]+)|(0[0-7]+)|([1-9][0-9]*)|0)     { yydebug("flex: number");               return NumberToken;         }

%[a-zA-Z_][a-zA-Z0-9_]*                             { yydebug("flex: directive");            return DirectiveToken;      }

\,                                                  { yydebug("flex: ,");                    return CommaToken;          }
\(                                                  { yydebug("flex: (");                    return LeftParenToken;      }
\)                                                  { yydebug("flex: )");                    return RightParenToken;     }

[a-zA-Z_][a-zA-Z0-9_]*:                             { yydebug("flex: label");                return LabelToken;          }

\n                                                  { yydebug("flex: new line"); return NewLineToken;        }

[ \t\r]+                                            { /* ignore whitespace */                                            }

.                                                   { yydebug("flex: syntax error");         return SyntaxErrorToken;    }

%%

void yyset_istream(std::istream *in){
    scanner.switch_streams(in);
}

yy::parser::symbol_type yylex() {
    int token = scanner.yylex();
    std::string text = scanner.YYText();

    if(token == NewLineToken) yylineno++;


    switch(token) {
        case 0:                return yy::parser::make_YYEOF();
        case InstructionToken: return yy::parser::make_Instruction(text);
        case IdentifierToken:  return yy::parser::make_Identifier(text);
        case NumberToken:      return yy::parser::make_Number(text);
        case DirectiveToken:   return yy::parser::make_Directive(text.substr(1));
        case LabelToken:       return yy::parser::make_Label(text.substr(0, text.size()-1));
        case CommaToken:       return yy::parser::token::Comma;
        case LeftParenToken:   return yy::parser::token::LeftParen;
        case RightParenToken:  return yy::parser::token::RightParen;
        case NewLineToken:     return yy::parser::token::NewLine;
        default:               return yy::parser::token::SyntaxError;
    }
}

