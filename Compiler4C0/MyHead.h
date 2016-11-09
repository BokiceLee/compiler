#ifndef MYHEAD_H_INCLUDED
#define MYHEAD_H_INCLUDED
#define MAX_LEN_OF_FILE 1024
#define MAX_LEN_OF_LINE 1024
#define MAX_LEN_OF_TOKEN 10
#define LEN_OF_STRING_TABLE 200
#define LEN_OF_STRING 200
#define MAX_LEN_OF_NUM 8
#define MAX_NUM 99999999
#define REVERSED_NUM 16
#define SPECIAL_SYM_NUM 20
#define ERROR_NUM 10
enum symbol{
    beginsy=0,casesy,charsy,constsy,defaultsy,elsesy,endsy,
    ifsy,intsy,mainsy,printfsy,returnsy,scanfsy,
    switchsy,voidsy,whilesy,
    eql,neq,gtr,geq,lss,leq,
    plus,minus,times,idiv,
    lparent,rparent,lbrack,rbrack,lquote,rquote,
    comma,semicolon,colon,becomes,//colon:Ã°ºÅ,semicolon:·ÖºÅ
    ident,intcon,charcon,stringcon,illegalcon
};
char* special_symbol[SPECIAL_SYM_NUM]={
    "eql",//0
    "neq",//1
    "gtr",//2
    "geq",//3
    "lss",//4
    "leq",//5
    "plus",//6
    "mius",//7
    "times",//8
    "idiv",//9
    "lparent",//10
    "rparent",//11
    "lbrack",//12
    "rbrack",//13
    "lquote",//14
    "rquote",//15
    "comma",//16
    "semicolon",//17
    "colon",//18
    "becomes"//19
};
char* reversed_table[REVERSED_NUM]={
    "begin",//0
    "case",//1
    "char",//2
    "const",//3
    "default",//4
    "else",//5
    "end",//6
    "if",//7
    "int",//8
    "main",//9
    "printf",//10
    "return",//11
    "scanf",//12
    "switch",//13
    "void",//14
    "while"//15
};
char* errormsg[ERROR_NUM]={
    "num begin with zero",
    "number should not end with letter or identity should not begin with number,maybe missing a space?",
    "error too big num",
    "wrong char",
    "error of char",
    "wrong char in string",
    "error '!'",
    "illegal character",
    "incomplete",
    "no char in define char"
};
void error(int error_code);
void clearToken();
int is_reversed(char* s);
int is_empty_char(char c);
int is_add_op_char(char c);
int is_mul_op_char(char c);
int is_letter(char c);
int is_not_zero_digital(char c);
int is_right_char_in_string(char c);
int is_digital(char c);
void setInputOutput();
void finish_compile();
void getNextCh();
void getNextSym();
#endif // MYHEAD_H_INCLUDED
