%require "3.2"
%language "c++"
%define api.value.type variant
%define api.token.constructor

%code requires {
    #include <string>
    #include <cstdint>
    #include <string>
    #include <sstream>
    #include <parser/ParserProcessor.hpp>
}
%code {
    extern size_t yylineno;
    extern yy::parser::symbol_type yylex(void);
    extern int yyparse(void);
    extern void yyset_istream(std::istream *in);
    ParserProcessor *m_pproc = nullptr;
}

%token<std::string> Instruction
%token<std::string> Identifier
%token<std::string> Directive
%token<std::string> Label
%token<std::string> Number
%token Comma LeftParen RightParen NewLine SyntaxError

%start program

%%

program
    : /* empty */
    | program line { std::cout << "parsed line: " << yylineno << "\n";}
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
        { std::cout << "Instruction: " << $1 << "\n"; m_pproc->push_instruction($1, yylineno); }
    | Identifier opt_operand_list
        { std::cout << "Instruction: " << $1 << "\n"; m_pproc->push_instruction($1, yylineno); }
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
        { std::cout << "  Number: " << $1 << "\n"; m_pproc->push_param($1); }
    | Identifier
        { std::cout << "  Identifier operand: " << $1 << "\n"; m_pproc->push_param($1); }
    | LeftParen Identifier RightParen
        { std::cout << "  Parenthesized: (" << $2 << ")\n"; }
    ;

%%

void yy::parser::error(const std::string& msg){
    std::cerr << "Parsing error (line:" << yylineno << "): " << msg << '\n';
}

int asm_parse(ParserProcessor &pproc, const std::string &str){
    m_pproc = &pproc;
    std::stringstream ss(str);
    yyset_istream(&ss);

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
