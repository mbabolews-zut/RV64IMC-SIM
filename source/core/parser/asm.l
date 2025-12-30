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
        SyntaxErrorToken,
    };

%}

%%


<<EOF>>                                            { return 0; }

[a-zA-Z_][a-zA-Z0-9_]*:                             { return LabelToken;          }
[a-zA-Z_][a-zA-Z0-9_]*                              { return IdentifierToken;     }

0[bB][01]+                                          { return NumberToken;         }
[+-]?((0x[0-9A-Fa-f]+)|(0[0-7]+)|([1-9][0-9]*)|0)   { return NumberToken;         }

%[a-zA-Z_][a-zA-Z0-9_]*                             { return DirectiveToken;      }

\,                                                  { return CommaToken;          }
\(                                                  { return LeftParenToken;      }
\)                                                  { return RightParenToken;     }

\n                                                  { return NewLineToken;        }

[#;].*                                              { /* ignore comments */       }
[ \t\r]+                                            { /* ignore whitespace */     }

.                                                   { return SyntaxErrorToken;    }

%%

void yyset_istream(std::istream *in){
    scanner.switch_streams(in);
}

yy::parser::symbol_type yylex() {
    int token = scanner.yylex();
    std::string text = scanner.YYText();

    if(token == NewLineToken || token == 0) yylineno++;


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
