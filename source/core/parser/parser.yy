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
    #include <ui.hpp>
    #include <format>
}
%code {
    extern size_t yylineno;
    extern yy::parser::symbol_type yylex(void);
    extern int yyparse(void);
    extern void yyset_istream(std::istream *in);
    static ParserProcessor m_pproc{};
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
    | program line
    | program final_line
    ;

final_line
    : statement
    | Label statement_opt
        { m_pproc.add_label($1); }
    ;

line
    : NewLine
    | statement NewLine
    | Label statement_opt NewLine
        { m_pproc.add_label($1); }
    ;

statement_opt
    : /* empty */
    | statement
    ;

statement
    : Directive opt_operand_list
    | Instruction opt_operand_list
        { m_pproc.push_instruction($1, yylineno); }
    | Identifier opt_operand_list
        { m_pproc.push_instruction($1, yylineno); }
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
        { m_pproc.push_param($1); }
    | Identifier
        { m_pproc.push_param($1); }
    | LeftParen Identifier RightParen
        { m_pproc.push_param($2); }
    | Number LeftParen Identifier RightParen
        { m_pproc.push_param($3); m_pproc.push_param($1); }
    ;

%%

void yy::parser::error(const std::string& msg){
    ui::print_error(std::format("[parsing] line: {}:{}", yylineno, msg));
}

namespace asm_parsing {
    ParsingResult parse(const std::string &str) {
        m_pproc.reset();
        std::stringstream ss(str);
        yyset_istream(&ss);

        yy::parser parser;
        yylineno = 0;
        auto parsing_error = parser.parse();
        auto result = m_pproc.get_parsing_result();
        result.error_code = parsing_error;
        return result;
    }
}
