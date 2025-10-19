%require "3.2"
%language "c++"
%define api.value.type variant
%define api.token.constructor

%code requires {
    #include <string>
    #include <cstdint>
}
%code {
    extern size_t yylineno;
    extern yy::parser::symbol_type yylex(void);
    extern int yyparse(void);
}

%token<std::string> Instruction
%token<std::string> Identifier
%token<std::string> Directive
%token<std::string> Label
%token<int64_t> Number
%token Comma LeftParen RightParen NewLine SyntaxError

%start program

%%

program
    : /* empty */
    | program line
    ;

line
    : NewLine
    | statement NewLine
    | Label statement_opt NewLine
        { std::cout << "Label: " << $1 << "\n"; }
    ;

statement_opt
    : /* empty */
    | statement
    ;

statement
    : Directive opt_operand_list
        { std::cout << "Directive: " << $1 << "\n"; }
    | Instruction opt_operand_list
        { std::cout << "Instruction: " << $1 << "\n"; }
    | Identifier opt_operand_list
        { std::cout << "Instruction: " << $1 << "\n"; }
    ;

opt_operand_list
    : /* empty */
    | operand_list
    ;

operand_list
    : operand
    | operand Comma operand_list
    ;

operand
    : Number
        { std::cout << "  Number: " << $1 << "\n"; }
    | Identifier
        { std::cout << "  Identifier operand: " << $1 << "\n"; }
    | LeftParen Identifier RightParen
        { std::cout << "  Parenthesized: (" << $2 << ")\n"; }
    ;

%%

void yy::parser::error(const std::string& msg){
    std::cerr << "Parsing error (line:" << yylineno << "): " << msg << '\n';
}

int main() {
    yy::parser parser;  
    std::cout << "Starting parser\n";
    yylineno = 1;
    if (parser.parse() == 0) {
        std::cout << "Parse succeeded\n";
    } else {
        std::cout << "Parse failed\n";
    }
    return 0;
}
