#include "SymbolManagement.h"
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
    comma,semicolon,colon,becomes,//colon:冒号,semicolon:分号
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

int row_in_source_file=1;
int column_in_source_file=0;

char ch;//读到的字符
enum symbol sym;
char token[LEN_OF_NAME];
int num_read=0;
int read_ch_len=0;

int end_flag=0;
int return_temp=0;

extern void error(int err_code);

void clearTmpToken(char tmp_token[]);
void clearToken();
int is_reversed(char s[]);
int is_empty_char(char c);
int is_add_op_char(char c);
int is_mul_op_char(char c);
int is_letter(char c);
int is_not_zero_digital(char c);
int is_right_char_in_string(char c);
int is_digital(char c);
void getNextCh();
void getNextSym();

void clearTmpToken(char tmp_Token[]){
    int i;
    for(i=0;i<LEN_OF_NAME;i++){
        tmp_Token[i]='\0';
    }
}
void clearToken(){
    while(read_ch_len>=0){
        token[read_ch_len--]='\0';
    }
    read_ch_len=0;
}
int is_reversed(char s[]){
    int left=0;
    int right=REVERSED_NUM-1;
    int mid;
    int compare_res;
    char tmp_s[LEN_OF_NAME];
    strcpy(tmp_s,s);
    strlwr(s);
    do{
        mid=(left+right)/2;
        compare_res=strcmp(s,reversed_table[mid]);
        switch(compare_res){
        case -1:
            right=mid-1;
            break;
        case 0:
            if(strcmp(tmp_s,s)!=0){
                error(52);
            }
            return mid;
        case 1:
            left=mid+1;
            break;
        }
    }while(left<=right);
    return 0;
}
int is_empty_char(char c){
    if(c==' '||c=='\n'||c=='\t'){
        return 1;
    }
    return 0;
}
int is_add_op_char(char c){
    if('+'==c||'-'==c){
        return 1;
    }
    return 0;
}
int is_mul_op_char(char c){
    if('/'==c||'*'==c){
        return 1;
    }
    return 0;
}
int is_letter(char c){
    if(c=='_'){
        return 1;
    }
    if('a'<=c&&c<='z'){
        return 1;
    }
    if('A'<=c&&c<='Z'){
        return 1;
    }
    return 0;
}
int is_not_zero_digital(char c){
    if('1'<=c && c<='9'){
        return 1;
    }
    return 0;
}
int is_right_char_in_string(char c){
    if(35<=c && c<=126){
        return 1;
    }
    if(c==32 || c==33){
        return 1;
    }
    return 0;
}
int is_digital(char c){
    if(is_not_zero_digital(c)){
        return 1;
    }
    if(c=='0'){
        return 1;
    }
    return 0;
}
void getNextCh(){
    if(end_flag){
        return;
    }
    if(return_temp==1){
        return_temp=0;
        row_in_source_file++;
        column_in_source_file=0;
    }
    if((ch=fgetc(fsource))==EOF){
        //end_flag=1;
        column_in_source_file++;
    }else if(ch=='\n'){
        return_temp=1;
        column_in_source_file++;
    }else if(ch=='\t'){
        column_in_source_file+=4;
    }else{
        column_in_source_file++;
    }
    //system("pause");
}
void getNextSym(){
    int reversed_id;
    while(is_empty_char(ch)){
        getNextCh();
    }
    clearToken();
    if(is_letter(ch)){
        //开始读token
        do{
            token[read_ch_len++]=ch;
            getNextCh();
        }while(is_letter(ch)||is_digital(ch));
        token[read_ch_len]='\0';
        reversed_id=is_reversed(token);
        if(reversed_id==0){
            sym=ident;
        }else{
            sym=reversed_id;
        }
    }else if(is_digital(ch)){
        sym=intcon;
        num_read=0;
        //开始读数字
        do{
            if(read_ch_len==1&&token[0]=='0'){
                error(0);
            }
            token[read_ch_len++]=ch;
            getNextCh();
        }while(is_digital(ch));
        if(is_letter(ch)){
            error(1);
        }
        token[read_ch_len]='\0';
        num_read=atoi(token);
        if(read_ch_len>MAX_LEN_OF_NUM || num_read>MAX_NUM){
            error(2);
        }
    }else{
        switch(ch){
        case '+':
            sym=pluss;
            getNextCh();
            break;
        case '-':
            sym=minuss;
            getNextCh();
            break;
        case '*':
            sym=times;
            getNextCh();
            break;
        case '/':
            sym=idiv;
            getNextCh();
            break;
        case ',':
            sym=comma;
            getNextCh();
            break;
        case ':':
            sym=colon;
            getNextCh();
            break;
        case ';':
            sym=semicolon;
            getNextCh();
            break;
        case '(':
            sym=lparent;
            getNextCh();
            break;
        case ')':
            sym=rparent;
            getNextCh();
            break;
        case '[':
            sym=lbrack;
            getNextCh();
            break;
        case ']':
            sym=rbrack;
            getNextCh();
            break;
        case '{':
            sym=lquote;
            getNextCh();
            break;
        case '}':
            sym=rquote;
            getNextCh();
            break;
        case '\''://??????????
            sym=charcon;
            getNextCh();
            if(!(is_add_op_char(ch)||is_mul_op_char(ch)||is_digital(ch)||is_letter(ch))){
                if(ch=='\''){
                    error(9);
                    token[0]='\0';
                    read_ch_len++;
                    getNextCh();
                    break;
                }else{
                    error(3);
                }
            }
            token[0]=ch;
            do{
                getNextCh();
                read_ch_len++;
            }while(ch!='\'');
            if(read_ch_len>1){
                error(4);
            }
            getNextCh();
            break;
        case '"':
            string_index=0;
            ++string_table_index;
            do{
                getNextCh();
                if(ch!='"'){
                    if(is_right_char_in_string(ch)){
                        string_tab[string_table_index][string_index++]=ch;
                    }else{
                        error(5);
                    }
                }else{
                    string_tab[string_table_index][string_index]='\0';
                }
            }while(ch!='"');
            getNextCh();
            sym=stringcon;
            break;
        case '!':
            getNextCh();
            if(ch=='='){
                sym=neq;
                getNextCh();
            }else{
                error(6);
            }
            break;
        case '=':
            getNextCh();
            if(ch=='='){
                sym=eql;
                getNextCh();
            }else{
                sym=becomes;
            }
            break;
        case '<':
            getNextCh();
            if(ch=='='){
                sym=leq;
                getNextCh();
            }else{
                sym=lss;
            }
            break;
        case '>':
            getNextCh();
            if(ch=='='){
                sym=geq;
                getNextCh();
            }else{
                sym=gtr;
            }
            break;
        case -1:
            sym=-1;
            if(end_flag){
                error(51);
            }
            end_flag=1;
            break;
        default:
            error(7);
            sym=illegalcon;
            token[read_ch_len++]=ch;
            getNextCh();
        }
    }
}

#endif // LEXICALANALYSIS_H_INCLUDED
