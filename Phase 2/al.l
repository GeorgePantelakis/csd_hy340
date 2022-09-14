/*FLEX CODE*/
%{

    #include "parser.h"
    #include "Utilities.h"
%}

/*FLEX OPTIONS*/
%option yylineno
%option header-file="./analyzer.h"
%option outfile = "./analyzer.c"
%option noyywrap

/*FLEX MACROS*/
IF if
ELSE else 
WHILE while 
FOR for 
FUNCTION function 
RETURN return
BREAK break
CONTINUE continue
AND and 
NOT not 
OR or 
LOCAL local 
TRUE true 
FALSE false
NULL nil

ID [A-Za-z][A-Za-z0-9_]* 
SPACE [\r \t\n]
STRING \"
COMMENT_ONE_LINE "//".*
COMMENT_MULT_LINES "/*"
INT [0-9]+
FLOAT [0-9]+\.[0-9]+
OPEN_SQUARE \[
CLOSE_SQUARE \]
OPEN_BRACETS \{
CLOSE_BRACETS \}
LEFT_PARENTHESIS \(
RIGHT_PARENTHESIS \)
SEMICOLON \;
COMMA \,
COLON \:
DOUBLE_COLON "::"
DOT \.
DOUBLE_DOT ".."
ASSIGN \=
PLUS \+
MINUS \-
ASTERISCUS \*
SLASH \/
PERCENT \%
EQUAL "=="
NOT_EQUAL "!="
PLUS_PLUS "++"
MINUS_MINUS "--"
SMALLER \<
BIGGER \>
BIGGER_EQUAL ">="
SMALLER_EQUAL "<="

%x MULT_LINES_COMMENTS
%x STRINGS


%%

{IF} {return IF;}
{ELSE} {return ELSE;}
{WHILE} {return WHILE;}
{FOR} {return FOR;}
{FUNCTION} {return FUNCTION;}
{RETURN} {return RETURN;}
{BREAK} {return BREAK;}
{CONTINUE} {return CONTINUE;}
{AND} {return AND;}
{NOT} {return NOT;}
{OR} {return OR;}
{LOCAL} {return TOK_LOCAL;}
{TRUE} {yylval.boolValue = 1; return BOOL;}
{FALSE} {yylval.boolValue = 0; return BOOL;}
{NULL} {yylval.intValue = 0; return NIL;}

{ID} { yylval.stringValue = strdup(yytext); return ID; }
{STRING} {Clear_buffer(); tmp_int = yylineno; BEGIN(STRINGS);}
{INT} {yylval.intValue = atoi(yytext); return INT;}
{FLOAT} {yylval.floatValue = atof(yytext); return FLOAT;}
{SPACE} {}

{COMMENT_ONE_LINE} {}
{COMMENT_MULT_LINES} { BEGIN(MULT_LINES_COMMENTS); Push_to_comment_stack(yylineno); comment_counter++;}

{OPEN_SQUARE} {return '[';}
{CLOSE_SQUARE} {return ']';}
{OPEN_BRACETS} {return '{';}
{CLOSE_BRACETS} {return '}';}
{LEFT_PARENTHESIS} {return '(';}
{RIGHT_PARENTHESIS} {return ')';}
{SEMICOLON} {return ';';}
{COMMA} {return ',';}
{COLON} {return ':';}
{DOUBLE_COLON} {return DOUBLE_COLON;}
{DOT} {return '.';}
{DOUBLE_DOT} {return DOUBLE_DOT;}

{ASSIGN} {return '=';}
{PLUS} {return '+';}
{MINUS} {return '-';}
{ASTERISCUS} {return '*';}
{SLASH} {return '/';}
{PERCENT} {return '%';}
{EQUAL} {return EQUAL;}
{NOT_EQUAL} {return NOT_EQUAL;}
{PLUS_PLUS} {return PLUS_PLUS;}
{MINUS_MINUS} {return MINUS_MINUS;}
{SMALLER} {return '<';}
{BIGGER} {return '>';}
{BIGGER_EQUAL} {return BIGGER_EQUAL;}
{SMALLER_EQUAL} {return SMALLER_EQUAL;}

<MULT_LINES_COMMENTS>(.|\n) ;
<MULT_LINES_COMMENTS>("/*") {comment_counter++; Push_to_comment_stack(yylineno);}
<MULT_LINES_COMMENTS>("*/") { comment_counter--; tmp_int = Pop_from_comment_stack(); if(!(comment_counter)){ BEGIN(INITIAL);} }
<MULT_LINES_COMMENTS><<EOF>> {fprintf(stderr, "\033[1;31mERROR:\033[0m No terminated comment at line %d!", yylineno); exit(1);}

<STRINGS>(\\\") {Add_to_buffer();}
<STRINGS>(\") { Add_to_buffer(); yylval.stringValue = filter_string(buffer); BEGIN(INITIAL); return STRING;}
<STRINGS>(.) {Add_to_buffer();} 
<STRINGS>(\n) ;
<STRINGS><<EOF>> {fprintf(stderr, "\033[1;31mERROR:\033[0m No terminated \" to string at line %d!\n", tmp_int); exit(1);}

. {sprintf(error_buffer, "\033[1;33mWARNING\033[0m: UNKNOWN TOKEN \"%s\" at line %d!\n", yytext, yylineno); Push_to_error_stack(error_buffer); BEGIN(INITIAL);}

%%