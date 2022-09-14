/*FLEX CODE*/
%{

    #define YY_DECL int alpha_yylex(void* yylval)

    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <assert.h>
    #include <fcntl.h>

    struct alpha_token{
        unsigned int line_number;
        unsigned int token_number;
        char* value;
        char* category;
        struct alpha_token* next;
    };

    struct comment_stack_node{
        unsigned int start_line;
        struct comment_stack_node* next;
    };

    struct error_stack_node{
        char * text;
        struct error_stack_node* next;
    };

    typedef struct alpha_token *alpha_token_t;

    unsigned int token_number, comment_counter;
    int fd;

    struct comment_stack_node* comment_stack;

    struct error_stack_node* error_stack;

    int tmp_int;

    void Push_to_comment_stack(int line);
    int Pop_from_comment_stack();

    void Push_to_error_stack(char* text);
    char* Pop_from_error_stack();

    void create_and_fill_token(alpha_token_t node, unsigned int line_number, unsigned int token_number, char* value, char* category);
    char* filter_string(char* input_string);

    void Add_to_buffer();
    void Clear_buffer();

    char* buffer;
%}

/*FLEX OPTIONS*/
%option yylineno
%option header-file="./al.h"
%option outfile = "./al.c"
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

{IF} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "IF");}
{ELSE} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "ELSE");}
{WHILE} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "WHILE");}
{FOR} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "FOR");}
{FUNCTION} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "FUNCTION");}
{RETURN} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "RETURN");}
{BREAK} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "BREAK");}
{CONTINUE} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "CONTINUE");}
{AND} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "AND");}
{NOT} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "NOT");}
{OR} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "OR");}
{LOCAL} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "LOCAL");}
{TRUE} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "TRUE");}
{FALSE} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "FALSE");}
{NULL} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "KEYWORD"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-enumerated \n", yylineno, token_number, yytext, "KEYWORD", "NULL");}

{ID} { create_and_fill_token(yylval, yylineno, ++token_number, yytext, "ID"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-char* \n", yylineno, token_number, yytext, "ID", yytext); }
{STRING} {Clear_buffer(); tmp_int = yylineno; BEGIN(STRINGS);}
{INT} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "CONST_INT"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-integer \n", yylineno, token_number, yytext, "CONST_INT", yytext);}
{FLOAT} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "CONST_FLOAT"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s \"%s\" <-float \n", yylineno, token_number, yytext, "CONST_FLOAT", yytext);}
{SPACE} {}

{COMMENT_ONE_LINE} {fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, ++token_number, yytext, "COMMENT", "LINE_COMMENT"); }
{COMMENT_MULT_LINES} { BEGIN(MULT_LINES_COMMENTS); Push_to_comment_stack(yylineno); comment_counter++;}

{OPEN_SQUARE} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "PUNCTUATTION"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "PUNCTUATION", "OPEN_SQUARE");}
{CLOSE_SQUARE} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "PUNCTUATION"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "PUNCTUATION", "CLOSE_SQUARE");}
{OPEN_BRACETS} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "PUNCTUATION"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "PUNCTUATION", "LEFT_BRACETS");}
{CLOSE_BRACETS} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "PUNCTUATION"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "PUNCTUATION", "RIGHT_BRACETS");}
{LEFT_PARENTHESIS} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "PUNCTUATION"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "PUNCTUATION", "LEFT_PARENTHESIS");}
{RIGHT_PARENTHESIS} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "PUNCTUATION"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "PUNCTUATION", "RIGHT_PARENTHESIS");}
{SEMICOLON} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "PUNCTUATION"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "PUNCTUATION", "SEMICOLON");}
{COMMA} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "PUNCTUATION"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "PUNCTUATION", "COMMA");}
{COLON} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "PUNCTUATION"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "PUNCTUATION", "COLON");}
{DOUBLE_COLON} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "PUNCTUATION"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "PUNCTUATION", "DOUBLE_COLON");}
{DOT} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "PUNCTUATION"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "PUNCTUATION", "DOT");}
{DOUBLE_DOT} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "PUNCTUATION"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "PUNCTUATION", "DOUBLE_DOT");}

{ASSIGN} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "OPERATOR"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "OPERATOR", "ASSIGN");}
{PLUS} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "OPERATOR"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "OPERATOR", "PLUS");}
{MINUS} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "OPERATOR"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "OPERATOR", "MINUS");}
{ASTERISCUS} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "OPERATOR"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "OPERATOR", "ASTERISCUS");}
{SLASH} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "OPERATOR"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "OPERATOR", "SLASH");}
{PERCENT} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "OPERATOR"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "OPERATOR", "PERCENT");}
{EQUAL} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "OPERATOR"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "OPERATOR", "PERCENT");}
{NOT_EQUAL} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "OPERATOR"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "OPERATOR", "NOT_EQUAL");}
{PLUS_PLUS} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "OPERATOR"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "OPERATOR", "PLUS_PLUS");}
{MINUS_MINUS} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "OPERATOR"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "OPERATOR", "MINUS_MINUS");}
{SMALLER} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "OPERATOR"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "OPERATOR", "SMALLER");}
{BIGGER} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "OPERATOR"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "OPERATOR", "BIGGER");}
{BIGGER_EQUAL} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "OPERATOR"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "OPERATOR", "BIGGER_EQUAL");}
{SMALLER_EQUAL} {create_and_fill_token(yylval, yylineno, ++token_number, yytext, "OPERATOR"); fprintf(stdout,"%d:\t #%d\t \"%s\" %s %s <-enumerated \n", yylineno, token_number, yytext, "OPERATOR", "SMALLER_EQUAL");}

<MULT_LINES_COMMENTS>(.|\n) ;
<MULT_LINES_COMMENTS>("/*") {comment_counter++; Push_to_comment_stack(yylineno);}
<MULT_LINES_COMMENTS>("*/") { comment_counter--; tmp_int = Pop_from_comment_stack(); if(!(comment_counter)){ fprintf(stdout,"%d:\t #%d\t \"%d-%d\" %s %s <-enumerated \n", tmp_int, ++token_number, tmp_int, yylineno, "COMMENT", "MULT_LINE_COMMENT"); BEGIN(INITIAL);}else{fprintf(stdout,"%d:\t #%d\t \"%d-%d\" %s %s <-enumerated_nested \n", tmp_int, ++token_number, tmp_int, yylineno, "COMMENT", "MULT_LINE_COMMENT");} }
<MULT_LINES_COMMENTS><<EOF>> {fprintf(stderr, "[1;31mERROR:[0m No terminated comment at line %d!", yylineno); exit(1);}

<STRINGS>(\\\") {Add_to_buffer();}
<STRINGS>(\") { create_and_fill_token(yylval, yylineno, ++token_number, filter_string(buffer), "STRING"); fprintf(stdout,"%d:\t #%d\t %s %s \"%s\" <-char* \n", yylineno, token_number, buffer, "STRING", filter_string(buffer)); BEGIN(INITIAL);}
<STRINGS>(.) {Add_to_buffer();}
<STRINGS>(\n) ;
<STRINGS><<EOF>> {fprintf(stderr, "[1;31mERROR:[0m No terminated \" to string at line %d!\n", tmp_int); exit(1);}

. {sprintf(buffer, "[1;33mWARNING:[0m UNKNOWN TOKEN \"%s\" at line %d!\n", yytext, yylineno); Push_to_error_stack(buffer);}

%%

int main(int argc, char** argv){
    if(argc > 1){
        if(!(yyin = fopen(argv[1], "r"))){
            fprintf(stdout, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    }else{
        fprintf(stderr, "You haven\'t provide a file to translate!\n");
        exit(1);
    }

    if(argc == 3){
        fd  = open(argv[2], O_CREAT | O_RDWR, 0666);
        dup2(fd, 1);
    }

    if(argc > 3){
        fprintf(stderr, "TOO MANY ARGUMENTS!\n");
        exit(1);
    }

    alpha_token_t head = (alpha_token_t)malloc(sizeof(struct alpha_token));
    head->next = NULL;

    comment_stack = NULL;

    error_stack = NULL;

    token_number = 0;
    comment_counter = 0;

    buffer = (char*)malloc(sizeof(char) * 1024);

    alpha_yylex(head);

    while(error_stack){
        fprintf(stdout, Pop_from_error_stack());
    }

    return 0;
}

void create_and_fill_token(alpha_token_t head, unsigned int line_number, unsigned int token_number, char* value, char* category){

    assert(head);

    alpha_token_t node = (alpha_token_t)malloc(sizeof(struct alpha_token)) , dummy = head;


    while(dummy->next != NULL){
        dummy = dummy->next;
    }
    

    dummy->next = node;
    node->line_number = line_number;
    node->token_number = token_number;
    node->value = (char *)malloc(sizeof(value));
    strcpy(node->value, value);
    node->category = (char *)malloc(sizeof(category));
    strcpy(node->category, category);
    node->next = NULL;

}

char* filter_string(char* input_string){
    int i, output_string_index = 0;
    char * output_string = (char *)malloc(sizeof(char) * strlen(input_string));

    for(i = 0; i < (strlen(input_string)); i++){
        if(input_string[i] == '\\'){
            i++;
            if(input_string[i] == 'n'){
                output_string[output_string_index++] = '\n';
            }else if(input_string[i] == 't'){
                output_string[output_string_index++] = '\t';
            }else{
                output_string[output_string_index++] = input_string[i];
            }
        }else{
            output_string[output_string_index++] = input_string[i];
        }
    }

    return output_string;
}

void Push_to_comment_stack(int line){
    struct comment_stack_node* tmp;

    tmp = (struct comment_stack_node*)malloc(sizeof(struct comment_stack_node));
    tmp->start_line = line;
    tmp->next = comment_stack;
    comment_stack = tmp;
}

int Pop_from_comment_stack(){
    int result;
    struct comment_stack_node* tmp;

    result = comment_stack->start_line;
    tmp = comment_stack;
    comment_stack = comment_stack->next;
    free(tmp);

    return result;
}

void Push_to_error_stack(char* text){
    struct error_stack_node* tmp;
    struct error_stack_node* last;

    last = error_stack;
    tmp = (struct error_stack_node*)malloc(sizeof(struct error_stack_node));
    tmp->text = (char *)malloc(sizeof(char) * strlen(text));
    strcpy(tmp->text, text);
    tmp->next = NULL;

    if(last == NULL){
        error_stack = tmp;
        return;
    }

    while(last->next) last = last->next;

    last->next = tmp;
    
}

char* Pop_from_error_stack(){
    char* text;
    struct error_stack_node* tmp;

    text = (char*)malloc(sizeof(error_stack->text));
    strcpy(text, error_stack->text);
    tmp = error_stack;
    error_stack = error_stack->next;
    free(tmp->text);
    free(tmp);

    return text;
}

void Add_to_buffer(){
    int i = strlen(buffer);
    int j;

    for(j = 0; j < strlen(yytext); j++){
        buffer[i + j] = yytext[j];
    }
}

void Clear_buffer(){
    int i;

    for(i = 0; i < 1024; i++) buffer[i] = '\0';
}