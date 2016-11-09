#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<ctype.h>
#define MAX_LEN_OF_FILE 1024
#define MAX_LEN_OF_LINE 1024
#define MAX_LEN_OF_TOKEN 10
#define LEN_OF_STRING_TABLE 200
#define LEN_OF_STRING 200
#define MAX_LEN_OF_NUM 8
#define MAX_NUM 99999999
#define REVERSED_NUM 16
#define SPECIAL_SYM_NUM 20
#define ERROR_NUM 9
enum symbol{
    beginsy=0,casesy,charsy,constsy,defaultsy,elsesy,endsy,
    ifsy,intsy,mainsy,printfsy,returnsy,scanfsy,
    switchsy,voidsy,whilesy,
    eql,neq,gtr,geq,lss,leq,
    plus,minus,times,idiv,
    lparent,rparent,lbrack,rbrack,lfbrace,rfbrace,
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
    "lfbrace",//14
    "rfbrace",//15
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
    "incomplete"
};
int errors=0;
FILE *fsource;//源代码文件指针
FILE *ftarget;//目标代码输出文件指针
char fsourcename[MAX_LEN_OF_FILE];//源代码文件名称
char ftargetname[MAX_LEN_OF_FILE];//目标代码文件名称
//存储用的数据结构
char stringtable[LEN_OF_STRING_TABLE][LEN_OF_STRING];
char ch;//读到的字符
int row_in_source_file=1;
int column_in_source_file=0;
enum symbol sym;
char token[MAX_LEN_OF_TOKEN];
int num_read=0;
int read_ch_len=0;
int string_table_index=0;
int string_index=0;
int i_temp;
int output2where_console_0_file_1=0;
int optimization=0;
void error(int error_code){
    switch(error_code){
    case 0:
        token[--read_ch_len]='\0';
        break;
    case 1:
        break;
    case 2:
        num_read=0;
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        break;
    case 6:
        break;
    case 7:
        break;
    case 8:
        break;
    }
    printf("error at row %d,column %d:\n",row_in_source_file,column_in_source_file);
    printf("error:%d ~~~~~error message:%s\n",++errors,errormsg[error_code]);
}
void clearToken(){
    while(read_ch_len>=0){
        token[read_ch_len--]='\0';
    }
    read_ch_len=0;
}
int is_reversed(char* s){
    int left=0;
    int right=REVERSED_NUM-1;
    int mid;
    int compare_res;
    do{
        mid=(left+right)/2;
        compare_res=strcmp(s,reversed_table[mid]);
        switch(compare_res){
        case -1:
            right=mid-1;
            break;
        case 0:
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
void setInputOutput(){
    set_in:
    printf("please input the absolute path of your source file(space is not available):\n");
    scanf("%s",fsourcename);
    fsource=fopen(fsourcename,"r");
    if(NULL==fsource){
        printf("no such file\n");
        goto set_in;
    }
    set_out:
    printf("input 0/1,0 for output result to console,1 for output result to file and you will have to input a file path\n");
    scanf("%d",&output2where_console_0_file_1);
    if(output2where_console_0_file_1){
        printf("please input the absolute path of target file you expect:\n");
        scanf("%s",ftargetname);
        ftarget=fopen(ftargetname,"w");
        if(NULL==ftarget){
            printf("please input the correct absolute path of target file\n");
            goto set_out;
        }
    }else{
        ftarget=stdout;
    }
}
void finish_compile(){
    fclose(fsource);
    if(output2where_console_0_file_1){
        fclose(ftarget);
    }
}
void getNextCh(){
    if((ch=fgetc(fsource))==EOF){
        column_in_source_file++;
        error(8);
        exit(0);
    }
    ch=tolower(ch);
    if(ch=='\n'){
        column_in_source_file=0;
        row_in_source_file++;
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
            sym=plus;
            getNextCh();
            break;
        case '-':
            sym=minus;
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
            sym=lfbrace;
            getNextCh();
            break;
        case '}':
            sym=rfbrace;
            getNextCh();
            break;
        case '\''://??????????
            sym=charcon;
            getNextCh();
            if(!(is_add_op_char(ch)||is_mul_op_char(ch)||is_digital(ch)||is_letter(ch))){
                error(3);
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
            do{
                getNextCh();
                if(ch!='"'){
                    if(is_right_char_in_string(ch)){
                        stringtable[string_table_index][string_index++]=ch;
                    }else{
                        error(5);
                    }
                }else{
                    string_table_index++;
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
            if(ch=='='){
                sym=geq;
                getNextCh();
            }else{
                sym=gtr;
            }
            break;
        default:
            error(7);
            sym=illegalcon;
            token[read_ch_len++]=ch;
            getNextCh();
        }
    }
}
int main()
{
    setInputOutput();
    ch=' ';
    fprintf(ftarget,"lexical analysis result:");
    while(1){
    //for (i=0;i<500;i++){
        fprintf(ftarget,"\n");
        getNextSym();
        if(0<=sym && sym<=15){
            fprintf(ftarget,reversed_table[sym]);
            fprintf(ftarget," ");
        }else if(16<=sym&&sym<=35){
            fprintf(ftarget,special_symbol[sym-16]);
            fprintf(ftarget," ");
        }else{
            switch(sym){
            case ident:
                fprintf(ftarget,"ident:");
                fprintf(ftarget,"%s ",token);
                break;
            case intcon:
                fprintf(ftarget,"num:");
                fprintf(ftarget,"%d ",num_read);
                break;
            case charcon:
                fprintf(ftarget,"char:");
                fprintf(ftarget,"%c",token[0]);
                break;
            case stringcon:
                fprintf(ftarget,"string:");
                fprintf(ftarget,"%s",stringtable[string_table_index-1]);
                break;
            default:
                fprintf(ftarget,"illegal:");
                fprintf(ftarget,"%c",token[0]);
            }
        }
    }
    finish_compile();
    return 0;
}
