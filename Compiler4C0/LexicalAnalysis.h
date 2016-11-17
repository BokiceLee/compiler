#ifndef LEXICALANALYSIS_H_INCLUDED
#define LEXICALANALYSIS_H_INCLUDED
#define REVERSED_NUM 16
#define SPECIAL_SYM_NUM 20
enum symbol{
    beginsy=0,casesy,charsy,constsy,defaultsy,elsesy,endsy,
    ifsy,intsy,mainsy,printfsy,returnsy,scanfsy,
    switchsy,voidsy,whilesy,
    eql,neq,gtr,geq,lss,leq,
    pluss,minuss,times,idiv,
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
void clearTmpToken(char tmp_token[]);
void clearToken();
int is_reversed(char* s);
int is_empty_char(char c);
int is_add_op_char(char c);
int is_mul_op_char(char c);
int is_letter(char c);
int is_not_zero_digital(char c);
int is_right_char_in_string(char c);
int is_digital(char c);
void getNextCh();
void getNextSym();
#endif // LEXICALANALYSIS_H_INCLUDED
