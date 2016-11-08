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
FILE *fsource;//源代码文件指针
FILE *ftarget;//目标代码输出文件指针
char fsourcename[MAX_LEN_OF_FILE];//源代码文件名称
char ftargetname[MAX_LEN_OF_FILE];//目标代码文件名称
//存储用的数据结构
char stringtable[LEN_OF_STRING_TABLE][LEN_OF_STRING];
enum symbol{
    beginsy=0,casesy,charsy,constsy,defaultsy,elsesy,endsy,
    ifsy,intsy,mainsy,printfsy,returnsy,scanfsy,
    switchsy,voidsy,whilesy,
    eql,neq,gtr,geq,lss,leq,
    plus,minus,times,idiv,
    lparent,rparent,lbrack,rbrack,lfbrace,rfbrace,
    comma,semicolon,colon,becomes,//colon:冒号,semicolon:分号
    ident,intcon,charcon,stringcon
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
//int cc;//char counter
//int ll;// line length
char ch;//读到的字符
//char line[MAX_LEN_OF_LINE];//读取一行字符
enum symbol sym;
char token[MAX_LEN_OF_TOKEN];
int num_read=0;
int read_ch_len=0;
int string_table_index=0;
int string_index=0;
int i_temp;
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
    printf("please input the absolute path of your source file(space is not available):\n");
    scanf("%s",fsourcename);
    fsource=fopen(fsourcename,"r");
    if(NULL==fsource){
        printf("no such file\n");
        return;
    }
    printf("please input the absolute path of target file you expect:\n");
    scanf("%s",ftargetname);
    ftarget=fopen(ftargetname,"w");
    if(NULL==ftarget){
        printf("please input the correct absolute path of target file\n");
        return;
    }
}
void outPut2File(char s[]){
    fprintf(ftarget,"%s",s);
}
void out2Screen(char s[]){
    printf("%s",s);
}
void getNextCh(){
    if((ch=fgetc(fsource))==EOF){
        printf("incomplete");
        exit(0);
    }
    ch=tolower(ch);
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
                printf("num begin with zero\n");
                read_ch_len--;
            }
            token[read_ch_len++]=ch;
            getNextCh();
        }while(is_digital(ch));
        if(is_letter(ch)){
            printf("error number\n");
        }else{
            token[read_ch_len]='\0';
            num_read=atoi(token);
        }
        if(read_ch_len>MAX_LEN_OF_NUM || num_read>MAX_NUM){
            printf("error too big num\n");
            read_ch_len=0;
            num_read=0;
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
                printf("wrong char\n");
            }
            token[0]=ch;
            getNextCh();
            if(ch!='\''){
                printf("error of char\n");
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
                        printf("wrong char in string\n");
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
                printf("error '!'");
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
            printf("illegal character\n");
            getNextCh();
        }
    }
}
int main()
{
    setInputOutput();
    ch=' ';
    printf("lexical analysis result:");
    while(1){
    //for (i=0;i<500;i++){
        printf("\n");
        getNextSym();
        if(0<=sym && sym<=15){
            printf(reversed_table[sym]);
            printf(" ");
        }else if(16<=sym&&sym<=35){
            printf(special_symbol[sym-16]);
            printf(" ");
        }else{
            switch(sym){
            case ident:
                printf("ident:");
                printf("%s ",token);
                break;
            case intcon:
                printf("num:");
                printf("%d ",num_read);
                break;
            case charcon:
                printf("char:");
                printf("%c ",token[0]);
                break;
            case stringcon:
                printf("string:");
                printf("%s ",stringtable[string_table_index-1]);
                break;
            default:
                ;
            }
        }
    }
    fclose(fsource);
    return 0;
}
