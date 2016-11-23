#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<ctype.h>
#include "Global.h"
#include "Error.h"
#include "GenAsm.h"
#include "LexicalAnalysis.h"
#include "Optimization.h"
#include "SemanticAnalysis.h"
#include "SymbolManagement.h"
////global////
int output2where_console_0_file_1=0;
int optimization=0;
int return_temp=0;
int end_flag=0;
////词法分析////
int row_in_source_file=1;
int column_in_source_file=0;
////输入输出////
FILE *fsource;//源代码文件指针
FILE *ftarget;//目标代码输出文件指针
char fsourcename[MAX_LEN_OF_FILE];//源代码文件名称
char ftargetname[MAX_LEN_OF_FILE];//目标代码文件名称
////词法分析////
char ch;//读到的字符
enum symbol sym;
char token[LEN_OF_NAME];
int num_read=0;
int read_ch_len=0;
////lobal////
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
////符号管理////
int i_temp;
enum types typ_var_funct;
char ident_name_var_funct[LEN_OF_NAME];

int ident_index=0;

char string_tab[LEN_OF_STRING_TAB][LEN_OF_STRING];
int string_table_index=0;
int string_index=0;

struct global_ident_tab_item global_ident_tab[IDENT_TAB_LEN];
struct local_ident_tab_item local_ident_tab[IDENT_TAB_LEN];
int global_ident_index=0;
int local_ident_index=0;
int last_local_ident_index=-1;
struct funct_tab_item funct_tab[LEN_OF_FUNC_TAB];
int funct_tab_index=0;
int funct_index=-1;//记录当前函数在函数表中的索引
int array_tab[LEN_OF_ARRAY_TAB];
int array_tab_index=0;
int position_res_global_flag;
////符号表管理////
int position(char tmp_token[]){
    int i;
    for(i=local_ident_index-1;i>=0;i--){
        if(strcmp(tmp_token,local_ident_tab[i].name)==0){
            position_res_global_flag=0;
            return i;
        }
    }
    for(i=global_ident_index-1;i>=0;i--){
        if(strcmp(tmp_token,global_ident_tab[i].name)==0){
            position_res_global_flag=1;
            return i;
        }
    }
    return i;
}
void enter_ident(int is_global,char name[],int obj,int typ,int refer,int adr){
    if(ident_index>=IDENT_TAB_LEN){
        fatal();
    }else{
        if(is_global){
            strcpy(global_ident_tab[global_ident_index].name,name);
            global_ident_tab[global_ident_index].obj=obj;
            global_ident_tab[global_ident_index].typ=typ;
            global_ident_tab[global_ident_index].refer=refer;
            global_ident_tab[global_ident_index].adr=adr;
            global_ident_index++;
        }else{
            strcpy(local_ident_tab[local_ident_index].name,name);
            local_ident_tab[local_ident_index].obj=obj;
            local_ident_tab[local_ident_index].typ=typ;
            local_ident_tab[local_ident_index].refer=refer;
            local_ident_tab[local_ident_index].adr=adr;
            local_ident_tab[local_ident_index].link=last_local_ident_index;
            last_local_ident_index=local_ident_index;
            local_ident_index++;
        }
    }
}
void enter_array(int high){
    array_tab[array_tab_index++]=high;
}
int enter_funct(int last,int lastpar,int psize,int vsize){
    funct_tab[funct_tab_index].last=last;
    funct_tab[funct_tab_index].lastpar=lastpar;
    funct_tab[funct_tab_index].psize=psize;
    funct_tab[funct_tab_index].vsize=vsize;
    return funct_tab_index++;
}
////出错处理////
int errors=0;
int end_right=0;
////出错处理////
void error(int error_code){
    switch(error_code){
    case -1:
        return;
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
int search_sym_in_set(int sym_set[],int set_len){
    int i;
    int flag=0;
    for(i=0;i<set_len;i++){
        if(sym_set[i]==sym){
            flag=1;
            break;
        }
    }
    return flag;
}
void skip(int sym_set[],int set_len,int err_code){
    error(err_code);
    while(!search_sym_in_set(sym_set,set_len)){
        getNextSym();
    }
}
int merge_sym_set(int s1[],int s1_len,int s2[],int s2_len){
    int i;
    int j;
    int res=s1_len;
    if(s2_len==0){
        return s1_len;
    }
    for(j=0;j<s2_len;j++){
        for(i=0;i<s1_len;i++){
            if(s1[i]==s2[j]){
                break;
            }
        }
        if(i>=s1_len){
            s1[res++]=s2[j];
        }
    }
    return res;
}
void test(int legal_set[],int legal_set_len,int stop_set[],int stop_set_len,int err_code){
    int new_set[SET_LEN];
    int new_set_len;
    if(!search_sym_in_set(legal_set,legal_set_len)){
        new_set_len=merge_sym_set(new_set,0,legal_set,legal_set_len);
        new_set_len=merge_sym_set(new_set,new_set_len,stop_set,stop_set_len);
        skip(new_set,new_set_len,err_code);
    }
}
void needsym(int sym_need){
    if(sym!=sym_need){
        switch(sym_need){
            case semicolon:
                error(15);
                break;
            case rparent:
                error(17);
                break;
            case rbrack:
                error(18);
                break;
            case rquote:
                error(19);
                break;
            default:
                ;
        }
    }else{
        getNextSym();
    }
}
void fatal(){
    ;
}
////词法分析////
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
    strlwr(s);
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
        end_flag=1;
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
            do{
                getNextCh();
                if(ch!='"'){
                    if(is_right_char_in_string(ch)){
                        string_tab[string_table_index][string_index++]=ch;
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
        case -1:
            sym=-1;
            break;
        default:
            error(7);
            sym=illegalcon;
            token[read_ch_len++]=ch;
            getNextCh();
        }
    }
}
////语法分析////
void program(){
    int stop_set[SET_LEN]={intsy,charsy,voidsy};
    int stop_set_len=3;
    if(sym==constsy){
        constdeclaraction(stop_set,stop_set_len,1);
    }
    if(sym==intsy||sym==charsy){
        vardeclaraction(stop_set,stop_set_len,1);
    }
    if(sym==lparent){
        funct_ret_declaraction(stop_set,stop_set_len,1);
    }
    while(sym==intsy||sym==charsy||sym==voidsy){
        if(sym==voidsy){
            getNextSym();
            funct_void_declaraction(stop_set,stop_set_len,1);
        }else{
            funct_ret_declaraction(stop_set,stop_set_len,1);
        }
        if(sym!=mainsy){
            test(stop_set,stop_set_len,NULL,0,31);
        }
    }
    funct_main_declaraction(1,NULL,0);
    printf("程序\n");
    if(sym==-1){
        printf("程序正确结束\n");
    }else{
        printf("程序未正确结束");
    }
}
void constdeclaraction(int fsys[],int fsys_len,int is_global){
    int stop_set[SET_LEN]={semicolon};
    int stop_set_len=1;
    int stop_set2[SET_LEN]={semicolon,constsy};
    int stop_set2_len=2;
    do{
        if(sym==constsy){//可以不要
            getNextSym();
        }
        if(sym==intsy||sym==charsy){
            stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
            constdefinition(stop_set,stop_set_len,is_global);
            needsym(semicolon);
            stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
            test(stop_set2,stop_set2_len,NULL,0,31);
        }else{
            error(14);
            stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
            test(stop_set,stop_set_len,fsys,fsys_len,-1);
            needsym(semicolon);
        }
        printf("常量说明\n");
    }while(sym==constsy);
}
void constdefinition(int fsys[],int fsys_len,int is_global){
    enum types ident_typ;
    int redcl_flag=0;
    char tmp_token[LEN_OF_NAME];
    int value=0;
    int positive;
    int stop_set[SET_LEN]={comma,semicolon};
    int stop_set_len=1;
    if(sym==intsy){//进来肯定是常量声明，sym肯定是int或char
        ident_typ=ints;
    }else{
        ident_typ=chars;
    }
    do{
        getNextSym();
        if(sym==ident){
            redcl_flag=check_redeclaraction(is_global,token);
            if(redcl_flag){
                error(36);//跳到下一个comma
                test(stop_set,stop_set_len,fsys,fsys_len,-1);
            }else{
                strcpy(tmp_token,token);
                getNextSym();
                if(sym==becomes){
                    positive=1;
                    getNextSym();
                    if(sym==pluss||sym==minuss){
                        if(sym==minuss){
                            positive=-1;
                        }
                        getNextSym();
                        if(sym!=intcon){
                            error(39);//跳到comma
                            test(stop_set,stop_set_len,fsys,fsys_len,-1);
                        }else if(num_read==0){
                            error(16);//不管
                        }
                    }
                    if(sym!=intcon&&sym!=charcon){
                        error(23);//跳到comma
                        test(stop_set,stop_set_len,fsys,fsys_len,-1);
                    }
                    if(sym==intcon && ident_typ==ints){
                        enter_ident(is_global,tmp_token,con,ints,0,num_read);
                        getNextSym();
                    }else if(sym==charcon && ident_typ==chars){
                        value=token[0];
                        enter_ident(is_global,tmp_token,con,chars,0,value);
                        getNextSym();
                    }else if(sym==chars && ident_typ==ints){
                        error(26);//不管
                        value=token[0];
                        enter_ident(is_global,tmp_token,con,ints,0,value);
                        getNextSym();
                    }else {
                        error(11);//comma
                        test(stop_set,stop_set_len,fsys,fsys_len,-1);
                    }
                    test(stop_set,stop_set_len,fsys,fsys_len,-1);
                }else{
                    error(12);//comma
                    test(stop_set,stop_set_len,fsys,fsys_len,-1);
                }
            }
        }else{
            error(13);//comma
            test(stop_set,stop_set_len,fsys,fsys_len,-1);
        }
        printf("常量定义\n");
    }while(sym==comma);
}
void vardeclaraction(int fsys[],int fsys_len,int is_global){
    enum types ident_typs;
    int stop_set[SET_LEN]={semicolon,lparent};
    int stop_set_len=2;
    if(is_global){
        stop_set[stop_set_len++]=intsy;
        stop_set[stop_set_len++]=charsy;
        stop_set[stop_set_len++]=voidsy;
    }
    do{
        if(sym==intsy){
            ident_typs=ints;
        }else{
            ident_typs=chars;
        }
        stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        vardefinition(stop_set,stop_set_len,is_global);
        if(sym==lparent){
            break;
        }else{
            needsym(semicolon);
        }
        if(is_global){
            test(stop_set,stop_set_len,fsys,fsys_len,31);
        }
    }while(sym==intsy||sym==charsy);
}
void vardefinition(int fsys[],int fsys_len,int is_global){
    enum types ident_typs;
    int redcl_flag=0;
    char tmp_token[LEN_OF_NAME];
    int stop_set[SET_LEN]={comma,semicolon};
    int stop_set_len=2;
    int stop_set2[SET_LEN]={intcon};
    int stop_set2_len=1;
    if(sym==intsy){
        ident_typs=ints;
    }else{
        ident_typs=chars;
    }
    //读到标识符时循环
    do{
        getNextSym();
        if(sym==ident){
            redcl_flag=check_redeclaraction(is_global,token);
            if(redcl_flag){
                error(36);
                test(stop_set,stop_set_len,fsys,fsys_len,-1);
            }else{
                strcpy(tmp_token,token);
                getNextSym();
                if(sym==lparent){
                    typ_var_funct=ident_typs;
                    strcpy(ident_name_var_funct,tmp_token);
                    break;
                }
                if(sym!=lbrack){
                    enter_ident(is_global,tmp_token,var,ident_typs,0,0);
                    test(stop_set,stop_set_len,fsys,fsys_len,45);
                }else{
                    getNextSym();
                    if(sym!=intcon){
                        error(20);
                        test(stop_set2,stop_set2_len,fsys,fsys_len,-1);
                        if(sym==intcon){
                            goto int_label;
                        }
                    }else{
                    int_label:
                        if(num_read==0){
                            error(20);//不管
                        }
                        enter_ident(is_global,tmp_token,arrays,ident_typs,array_tab_index,0);
                        enter_array(num_read);
                        getNextSym();
                        needsym(rbrack);
                        test(stop_set,stop_set_len,fsys,fsys_len,45);
                    }
                }
            }
        }
        printf("变量定义\n");
    }while(sym==comma);
}
void funct_void_declaraction(int fsys[],int fsys_len){
    int refer=0;
    int adr=0;
    int redcl_flag=0;
    int lastpar;
    char tmp_token[LEN_OF_NAME];
    int stop_set2[SET_LEN]={rquote};
    int stop_set2_len=1;
    int stop_set3[SET_LEN]={rparent};
    int stop_set3_len=1;
    if(sym==mainsy){
        return;
    }
    if(sym==ident){
        redcl_flag=check_redeclaraction(1,token);
        if(redcl_flag){
            error(36);//跳到下一个函数声明
            getNextSym();
            test(fsys,fsys_len,NULL,0,-1);
            return;
        }else{
            strcpy(tmp_token,token);
            getNextSym();
            if(sym!=lparent){
                error(22);//跳到下一个函数
                test(fsys,fsys_len,NULL,0,-1);
                return;
            }else{
                last_local_ident_index=-1;
                stop_set3_len=merge_sym_set(stop_set3,stop_set3_len,fsys,fsys_len);
                getNextSym();
                parameterlist(stop_set3,stop_set3_len);
                needsym(rparent);
                lastpar=local_ident_index;
                if(sym!=lquote){
                    error(24);//不管
                }else{
                    getNextSym();
                }
                stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
                compound_statement(stop_set2,stop_set2_len);
                needsym(rquote);//不管,应该不会出现
                refer=enter_funct(last_local_ident_index,lastpar,0,0);
                enter_ident(1,tmp_token,func,notyp,refer,adr);
            }
        }
        printf("无返回值函数定义\n");
    }else{//非标识符
        error(13);//跳到下一个函数
        test(fsys,fsys_len,NULL,0,-1);
    }
}
void funct_ret_declaraction(int fsys[],int fsys_len){
    enum types typ;
    int refer;
    int adr=0;
    int redcl_flag=0;
    int lastpar;
    char tmp_token[LEN_OF_NAME];
    int stop_set2[SET_LEN]={rquote};
    int stop_set2_len=1;
    int stop_set3[SET_LEN]={lquote};
    int stop_set3_len=1;
    if(sym==intsy){
        typ=ints;
        getNextSym();
    }else if(sym==chars){
        typ=chars;
        getNextSym();
    }
    //如果是main，报错
    if(sym==ident){
        redcl_flag=check_redeclaraction(1,token);
        if(redcl_flag){
            error(36);
            getNextSym();
            test(fsys,fsys_len,NULL,0,-1);
            return;
        }else{
            strcpy(tmp_token,token);
        }
        getNextSym();
    }else if(sym==lparent){
        if(ident_name_var_funct[0]=='\0'){
            error(28);
            test(fsys,fsys_len,NULL,0,-1);
        }else{
            strcpy(tmp_token,ident_name_var_funct);
            clearTmpToken(ident_name_var_funct);
        }
    }else{
        if(sym==mainsy){
            error(44);
        }else{
            error(13);
        }
        test(fsys,fsys_len,NULL,0,-1);
        return;
    }
    if(sym!=lparent){
        error(22);
        test(fsys,fsys_len,NULL,0,-1);
    }else{
        last_local_ident_index=-1;
        stop_set3_len=merge_sym_set(stop_set3,stop_set3_len,fsys,fsys_len);
        getNextSym();
        parameterlist(stop_set3,stop_set3_len);
        lastpar=last_local_ident_index;
        needsym(rparent);
        if(sym!=lquote){
            error(24);//不管
        }else{
            getNextSym();
        }
        stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
        compound_statement(stop_set2,stop_set2_len);
        needsym(rquote);
        refer=enter_funct(last_local_ident_index,lastpar,0,0);
        enter_ident(1,tmp_token,func,typ,refer,adr);
        printf("有返回值函数定义\n");
    }
}
void funct_main_declaraction(int fsys[],int fsys_len){
    int refer;
    int adr=0;
    int stop_set[SET_LEN]={rquote};
    int stop_set_len=1;
    int stop_set3[SET_LEN]={lquote};
    int stop_set3_len=1;
    if(sym==mainsy){
        getNextSym();
        if(sym!=lparent){
            error(22);//不管
        }else{
            getNextSym();
        }
        last_local_ident_index=-1;
        stop_set3_len=merge_sym_set(stop_set3,stop_set3_len,fsys,fsys_len);
        parameterlist(stop_set3,stop_set3_len);
        if(last_local_ident_index!=-1){
            error(22);
        }
        needsym(rparent);
        if(sym!=lquote){
            error(24);
        }else{
            getNextSym();
        }
        merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        compound_statement(stop_set,stop_set_len);
        refer=enter_funct(last_local_ident_index,-1,0,0);
        enter_ident(1,"main",func,notyp,refer,adr);
        //enter_ident(token,is_global,func,notyp,refer,adr);
        needsym(rquote);
    }else{
        error(43);//不管
    }
    printf("主函数定义\n");
}
void parameterlist(int fsys[],int fsys_len){
    enum types ident_typ;
    int redcl_flag=0;
    char tmp_token[LEN_OF_NAME];
    int stop_set[SET_LEN]={rparent,comma};
    int stop_set_len=2;
    do{
        if(sym==comma){
            getNextSym();
        }
        if(sym!=intsy&&sym!=charsy&&sym!=rparent){
            error(22);
            getNextSym();
            test(stop_set,stop_set_len,fsys,fsys_len,-1);
        }else if(sym==intsy||sym==charsy){
            if(sym==intsy){
                ident_typ=ints;
            }else{
                ident_typ=chars;
            }
            getNextSym();
            if(sym==ident){
                redcl_flag=check_redeclaraction(0,token);
                if(redcl_flag){
                    error(36);
                    getNextSym();
                    test(stop_set,stop_set_len,fsys,fsys_len,-1);
                }else{
                    strcpy(tmp_token,token);
                    enter_ident(0,tmp_token,paras,ident_typ,0,0);
                    getNextSym();
                }
            }else{
                error(13);
                test(stop_set,stop_set_len,fsys,fsys_len,-1);
            }
        }
    }while(sym==comma);
    printf("参数表\n");
}
void compound_statement(int fsys[],int fsys_len){
    if(sym==constsy){
        constdeclaraction(fsys,fsys_len,0);
    }
    if(sym==intsy||sym==charsy){
        vardeclaraction(fsys,fsys_len,0);
    }
    statements(fsys,fsys_len);
   printf("复合语句\n");
}
void statement(int fsys[],int fsys_len){
    char tmp_token[LEN_OF_NAME];
    int stop_set[SET_LEN]={rquote};
    int stop_set_len=1;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    //test(fsys,fsys_len,NULL,0,-1);
    switch(sym){
        case ifsy:
            getNextSym();
            if_statement(fsys,fsys_len);
            break;
        case whilesy:
            getNextSym();
            while_statement(fsys,fsys_len);
            break;
        case lquote:
            getNextSym();
            statements(stop_set,stop_set_len);
            needsym(rquote);
            break;
        case ident:
            strcpy(tmp_token,token);
            getNextSym();
            if(sym==lbrack){
                getNextSym();
                selector(fsys,fsys_len);
            }
            if(sym==becomes){
                assignment(fsys,fsys_len,tmp_token);
            }else if(sym==lparent){
                getNextSym();
                funct_call(fsys,fsys_len,tmp_token);
            }else{
                error(31);
                break;
            }
            needsym(semicolon);
            break;
        case scanfsy:
            getNextSym();
            scanf_statement(fsys,fsys_len);
            needsym(semicolon);
            break;
        case printfsy:
            getNextSym();
            printf_statement(fsys,fsys_len);
            needsym(semicolon);
            break;
        case switchsy:
            getNextSym();
            switch_statement(fsys,fsys_len);
            break;
        case returnsy:
            getNextSym();
            return_statement(fsys,fsys_len);
            needsym(semicolon);
            break;
        case semicolon:
            getNextSym();
            break;
        default:
            break;
    }
    test(fsys,fsys_len,NULL,0,-1);
printf("语句\n");
}
void statements(int fsys[],int fsys_len){
    char tmp_token[LEN_OF_NAME];
    int stop_set[SET_LEN]={ident,ifsy,whilesy,lquote,scanfsy,printfsy,switchsy,returnsy,semicolon,rquote};
    int stop_set_len=10;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    while(sym!=rquote){
        if(sym==semicolon){
            getNextSym();
        }
        statement(stop_set,stop_set_len);
    }
printf("语句列\n");
}
void if_statement(int fsys[],int fsys_len){
    int stop_set1[SET_LEN]={rparent};
    int stop_set1_len=1;
    int stop_set2[SET_LEN]={elsesy};
    int stop_set2_len=1;
    if(sym!=lparent){
        error(25);
    }else{
        getNextSym();
    }
    stop_set1_len=merge_sym_set(stop_set1,stop_set1_len,fsys,fsys_len);
    condition(stop_set1,stop_set1_len);
    needsym(rparent);
    stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
    statement(stop_set2,stop_set2_len);
    if(sym==elsesy){
        getNextSym();
        statement(fsys,fsys_len);
printf("if 中 else\n");
    }
printf("if 语句\n");
}
void while_statement(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    if(sym!=lparent){
        error(25);
    }else{
        getNextSym();
    }
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    condition(stop_set,stop_set_len);
    needsym(rparent);
    statement(fsys,fsys_len);
    printf("while 语句\n");
}
void condition(int fsys[],int fsys_len){
    enum symbol tmp_sym;
    int stop_set[SET_LEN]={eql,neq,gtr,geq,lss,leq};
    int stop_set_len=6;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    simpleexpression(stop_set,stop_set_len);
    if(sym==eql||sym==neq||sym==gtr||sym==geq||sym==lss||sym==leq){
        tmp_sym=sym;
        getNextSym();
        simpleexpression(fsys,fsys_len);
        switch(tmp_sym){
        case eql://根据不同情况生成指令
            break;
        default:
            break;
        }
    }
    //打标签
    printf("if或while判断条件 \n");
}
void assignment(int fsys[],int fsys_len,char tmp_token[]){
    enum types ret_typ;
    int stop_set[SET_LEN]={becomes};
    int stop_set_len=1;
    int res_position=position(tmp_token);
    //tmp_token可能是数组
    if(sym==lbrack){
        getNextSym();
        stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        selector(stop_set,stop_set_len);
    }
    if(sym!=becomes){
        error(12);
        test(fsys,fsys_len,NULL,0,-1);
    }else{
        if(res_position==-1){
            error(32);
            test(fsys,fsys_len,NULL,0,-1);
        }else{
            getNextSym();
            ret_typ=simpleexpression(fsys,fsys_len);
            //插入运算指令
    //        if(ret_typ==ints && ident_tab[res_position].typ==ints){
    //        }else if(ret_typ==chars && ident_tab[res_position].typ==chars){
    //        }else if(ret_typ==ints && ident_tab[res_position].typ==ints){
    //        }else{
    //            error(11);//报错之后继续插入
    //        }
        }
    }
    printf("赋值语句\n");
}
void switch_statement(int fsys[],int fsys_len){
    enum types condition_typ;
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    int stop_set2[SET_LEN]={rquote};
    int stop_set2_len=1;
    if(sym!=lparent){
        error(25);
    }else{
        getNextSym();
    }
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    condition_typ=simpleexpression(stop_set,stop_set_len);
    needsym(rparent);
    if(sym!=lquote){
        error(24);
    }else{
        getNextSym();
    }
    stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
    caselabel(stop_set2,stop_set2_len);//onst?
    needsym(rquote);
    printf("情况语句\n");
}
void caselabel(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={casesy,defaultsy};
    int stop_set_len=2;
    test(stop_set,stop_set_len,fsys,fsys_len,27);
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    if(sym==casesy){
        do{
            getNextSym();
            if(sym!=intcon&&sym!=charcon){
                error(40);//重标签同处理
                test(stop_set,2,fsys,fsys_len,-1);
            }else{
                //分号
                getNextSym();
                onecase(stop_set,stop_set_len);
                //打标签
            }
        }while(sym==casesy);
    }else if(sym==defaultsy){
        error(27);
    }
    if(sym==defaultsy){
        getNextSym();
        defaultcase(fsys,fsys_len);
    }
    printf("情况表\n");
}
void onecase(int fsys,int fsys_len){
    if(sym!=colon){
        error(41);
    }else{
        getNextSym();
    }
    statement(fsys,fsys_len);
    printf("情况子语句\n");
}
void defaultcase(int fsys[],int fsys_len){
    if(sym!=colon){
        error(42);
    }else{
        getNextSym();
    }
    statement(fsys,fsys_len);
    printf("缺省\n");
}
void scanf_statement(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={comma,rparent};
    int stop_set_len=2;
    int res_position;
    if(sym!=lparent){
        error(29);
    }else{
        getNextSym();
    }
    if(sym!=ident){
        error(30);
        test(stop_set,stop_set_len,fsys,fsys_len,-1);
    }
    do{
        if(sym==comma){
            getNextSym();
        }
        if(sym!=ident){
            error(30);
            test(stop_set,stop_set_len,fsys,fsys_len,-1);
        }else{
            res_position=position(token);
            if(res_position==-1){
                error(32);
                test(stop_set,stop_set_len,fsys,fsys_len,-1);
            }else{
                //插入读指令
                getNextSym();
            }
        }
    }while(sym==comma);
    needsym(rparent);
printf("读语句\n");
}
void printf_statement(int fsys[],int fsys_len){
    int res_position;
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    if(sym!=lparent){
        error(29);
    }else{
        getNextSym();
    }
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    if(sym==stringcon){
        //处理字符串
        getNextSym();
        if(sym==comma){
            getNextSym();
            simpleexpression(stop_set,stop_set_len);
        }
        needsym(rparent);
    }else {
        simpleexpression(stop_set,stop_set_len);
        needsym(rparent);
    }
    printf("写语句\n");
}
int simpleexpression(int fsys[],int fsys_len){
    enum symbol positive;
    int stop_set[SET_LEN]={pluss,minuss};
    int stop_set_len=2;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    if(sym==pluss||sym==minuss){
        positive=sym;
        getNextSym();
        term(stop_set,stop_set_len);
        if(positive==minuss){
            //插入取反操作
        }
    }else{
        term(stop_set,stop_set_len);
    }
    while(sym==pluss||sym==minuss){
        positive=sym;
        getNextSym();
        term(stop_set,stop_set_len);
        if(positive==pluss){
            //插入加指令
        }else{
            //插入减指令
        }
    }
    printf("表达式\n");
    return 0;
}
int factor(int fsys[],int fsys_len){
    int res_position;
    char tmp_token[LEN_OF_NAME];
    int legal_set[SET_LEN]={lparent,ident,intcon,charcon};
    int legal_set_len=4;
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    //test
    test(legal_set,legal_set_len,fsys,fsys_len,31);
    if(sym==ident||sym==intcon||sym==charcon){
        if(sym==ident){
            strcpy(tmp_token,token);
            res_position=position(token);
            if(res_position==-1){
                error(32);
                getNextSym();
                test(fsys,fsys_len,NULL,0,-1);
            }else{
                getNextSym();
                if(sym==lbrack){
                    getNextSym();
                    selector(fsys,fsys_len);
                }else if(sym==lparent){
                    getNextSym();
                    funct_call(fsys,fsys_len,tmp_token);
                }else{
                }
            }
        }else{
            //插入加载常量操作
            getNextSym();
        }
    }else if(sym==lparent){
        getNextSym();
        stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        simpleexpression(stop_set,stop_set_len);
        needsym(rparent);
    }
    printf("因子\n");
    return 0;
}
int term(int fsys[],int fsys_len){
    enum symbol fac_op;
    int stop_set[SET_LEN]={times,idiv};
    int stop_set_len=2;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    factor(stop_set,stop_set_len);
    while(times==sym||idiv==sym){
        fac_op=sym;
        getNextSym();
        factor(stop_set,stop_set_len);
        //插入相应操作
        if(fac_op==times){
            ;
        }else{
            ;
        }
    }
printf("项\n");
    return 0;
}

void return_statement(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    if(sym==semicolon){
printf("返回语句\n");
        return;
    }
    if(sym!=lparent){
        error(29);
    }else{
        getNextSym();
    }
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    simpleexpression(stop_set,stop_set_len);
    needsym(rparent);
printf("返回语句\n");
}
void funct_call(int fsys[],int fsys_len,char funct_name[]){
    int stop_set[SET_LEN]={comma,rparent};
    int stop_set_len=2;
    if(sym!=rparent){
        stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        simpleexpression(stop_set,stop_set_len);
        if(sym==comma){
            do{
                getNextSym();
                simpleexpression(stop_set,stop_set_len);
            }while(sym==comma);
        }
        printf("值参数表\n");
    }else{
        //没有参数
        printf("参数表\n");
    }
    needsym(rparent);
    printf("函数调用语句\n");
}
void selector(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={rbrack};
    int stop_set_len=1;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    simpleexpression(stop_set,stop_set_len);
    needsym(rbrack);
    printf("数组选择器\n");
}

////语义分析////
int check_redeclaraction(int is_global,char ident_name[]){
    int check_index;
    if(is_global){
        check_index=global_ident_index-1;
        while(check_index>=0){
            if(strcmp(global_ident_tab[check_index].name,ident_name)==0){
                return 1;
            }
            check_index--;
        }
        return 0;
    }else{
        check_index=last_local_ident_index;
        while(check_index!=-1){
            if(strcmp(local_ident_tab[check_index].name,ident_name)==0){
                return 1;
            }else{
                check_index=local_ident_tab[check_index].link;
            }
        }
        return 0;
    }
    return 0;
}
////生成四元式////

////代码优化////

////生成汇编////
int main()
{
    setInputOutput();
    ch=' ';
    fprintf(ftarget,"grammar analysis result:");
    getNextSym();
    program();
    while(1){
        break;
//        fprintf(ftarget,"\n");
//        getNextSym();
//        if(0<=sym && sym<=15){
//            fprintf(ftarget,reversed_table[sym]);
//            fprintf(ftarget," ");
//        }else if(16<=sym&&sym<=35){
//            fprintf(ftarget,special_symbol[sym-16]);
//            fprintf(ftarget," ");
//        }else{
//            switch(sym){
//            case ident:
//                fprintf(ftarget,"ident:");
//                fprintf(ftarget,"%s ",token);
//                break;
//            case intcon:
//                fprintf(ftarget,"numcon:");
//                fprintf(ftarget,"%d ",num_read);
//                break;
//            case charcon:
//                fprintf(ftarget,"charcon:");
//                fprintf(ftarget,"%c",token[0]);
//                break;
//            case stringcon:
//                fprintf(ftarget,"stringcon:");
//                fprintf(ftarget,"%s",string_tab[string_table_index-1]);
//                break;
//            default:
//                fprintf(ftarget,"illegal:");
//                fprintf(ftarget,"%c",token[0]);
//            }
//        }
    }
    finish_compile();
    return 0;
}
