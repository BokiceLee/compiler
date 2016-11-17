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
int errors=0;
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
////符号管理////
int string_table_index=0;
int string_index=0;
struct ident_tab_item ident_tab[IDENT_TAB_LEN];
int ident_index=0;
int i_temp;
enum typs typ_var_funct;
char ident_name_var_funct[LEN_OF_NAME];
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

////出错处理////
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
    return;
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
    if(!search_sym_in_set(sym_set,set_len)){
        getNextSym();
    }
}
void merge_sym_set(int s1[],int s1_len,int s2[],int s2_len){
    int i;
    for(i=s1_len;i<s1_len+s2_len;i++){
        s1[i]=s2[i-s1_len];
    }
}
void test(int legal_set[],int legal_set_len,int stop_set[],int stop_set_len,int err_code){
    if(!search_sym_in_set(legal_set,legal_set_len)){
        merge_sym_set(legal_set,legal_set_len,stop_set,stop_set_len);
        skip(legal_set,legal_set_len+stop_set_len,err_code);
    }
}
void testsemicolon(){
    enum symbol tmp_legal_set[SET_LEN];
    int tmp_legal_set_len=0;
    if(sym==semicolon){
        getNextSym();
    }else{
        error(15);
        while(sym!=semicolon){
            getNextSym();
        }
        getNextSym();
    }
//    tmp_legal_set[tmp_legal_set_len++]=ident;
//    tmp_legal_set[tmp_legal_set_len++]=lquote;
//    if(sym==semicolon){
//        getNextSym();
//    }else{
//        error();
//        if(sym==comma || sym==colon){
//            getNextSym();
//        }
//    }
//    test(tmp_legal_set,tmp_legal_set_len,fsys,fsys_len,err_code);
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
void getNextCh(){
    if(end_flag){
        error(8);
        exit(0);
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
        default:
            error(7);
            sym=illegalcon;
            token[read_ch_len++]=ch;
            getNextCh();
        }
    }
}
////语法分析////
int check_redeclaraction(int is_global,char* ident_name){

    return 0;
}
void program(){
    if(sym==constsy){
        constdeclaraction(1);
    }
    if(sym==intsy||sym==charsy){
        vardeclaraction(1);
    }
    if(sym==lparent){
        funct_ret_declaraction(1);
    }
    while(sym==intsy||sym==charsy||sym==voidsy){
        if(sym==voidsy){
            getNextSym();
            funct_void_declaraction(1);
        }else{
            funct_ret_declaraction(1);
        }
    }
    funct_main_declaraction(1);
    while(ch!=-1){
        getNextCh();
        if(!is_empty_char(ch)){
            break;
        }
    }
    if(ch==-1){
        printf("程序正确结束\n");
    }else{
        printf("程序未正确结束");
    }
}
void constdeclaraction(int is_global){
    do{
        if(sym==constsy){
            getNextSym();
        }
        if(sym==intsy||sym==charsy){
            constdefinition(is_global);
        }else{
            error(14);
        }
        testsemicolon();
    }while(sym==constsy);
}
void constdefinition(int is_global){
    enum typs ident_typ;
    int redcl_flag=0;
    char tmp_token[LEN_OF_NAME];
    int value=0;
    if(sym==intsy){
        ident_typ=ints;
    }else{
        ident_typ=chars;
    }
    do{
        getNextSym();
        if(sym==ident){
            redcl_flag=check_redeclaraction(is_global,token);
            if(redcl_flag){
                error(36);
            }else{
                strcpy(tmp_token,token);
                getNextSym();
                if(sym==becomes){
                    getNextSym();
                    if(sym==intcon && ident_typ==ints){
                        enter_ident(tmp_token,is_global,con,ints,0,num_read);
                    }else if(sym==charcon && ident_typ==chars){
                        value=token[0];
                        enter_ident(tmp_token,is_global,con,chars,0,value);
                    }else if(sym==chars && ident_typ==ints){
                        error(26);
                        value=token[0];
                        enter_ident(tmp_token,is_global,con,ints,0,value);
                    }else {
                        error(11);
                    }
                }else{
                    error(12);
                }
            }
        }else{
            error(13);
        }
        getNextSym();
    }while(sym==comma);
}
void vardeclaraction(int is_global){
    enum typs ident_typs;
    while(sym==intsy || sym==charsy){
        if(sym==intsy){
            ident_typs=ints;
        }else{
            ident_typs=chars;
        }
        vardefinition(is_global);
        if(sym==semicolon){
            getNextSym();
        }else if(sym==lparent){
            break;
        }else{
            error(15);
        }
    }
}
void vardefinition(int is_global){
    enum typs ident_typs;
    int redcl_flag=0;
    char tmp_token[LEN_OF_NAME];
    if(sym==intsy){
        ident_typs=ints;
    }else{
        ident_typs=chars;
    }
    do{
        getNextSym();
        if(sym==ident){
            redcl_flag=check_redeclaraction(is_global,token);
            if(redcl_flag){
                error(36);
                getNextSym();
            }else{
                strcpy(tmp_token,token);
                getNextSym();
                if(sym==lparent){
                    typ_var_funct=ident_typs;
                    strcpy(ident_name_var_funct,tmp_token);
                    break;
                }
                if(sym!=lbrack){
                    enter_ident(tmp_token,is_global,var,ident_typs,0,0);
                }else{
                    getNextSym();
                    selector();
                    enter_array();
                    enter_ident(tmp_token,is_global,arrays,ident_typs,0,0);
                    getNextSym();
                }
            }
        }
    }while(sym==comma);
}
void funct_void_declaraction(int is_global){
    int refer=0;
    int adr=0;
    int redcl_flag=0;
    char tmp_token[LEN_OF_NAME];
    if(sym==mainsy){
        return;
    }
    if(sym==ident){
        //检查重命名
        getNextSym();
        if(sym!=lparent){
            error(22);
        }else{
            parameterlist();
            if(sym!=rparent){
                error(17);
            }else{
                getNextSym();
                if(sym!=lquote){
                    error(24);
                }else{
                    getNextSym();
                    compound_statement();
                    if(sym!=rquote){
                        error(19);
                    }else{
                        enter_funct();
                        enter_ident(token,is_global,func,notyp,refer,adr);
                        getNextSym();
                    }
                }
            }
        }
    }else{
        error(13);
    }
}
void funct_ret_declaraction(int is_global,enum typs ret_typ){
    enum typs typ;
    int refer;
    int adr;
    int redcl_flag=0;
    char tmp_token[LEN_OF_NAME];
    if(sym==intsy){
        typ=ints;
        getNextSym();
    }else if(sym==chars){
        typ=chars;
        getNextSym();
    }
    if(sym==ident){
        strcpy(tmp_token,token);
        getNextSym();
    }else if(sym==lparent){
        if(ident_name_var_funct[0]=='\0'){
            error(28);
        }else{
            strcpy(tmp_token,ident_name_var_funct);
            clearTmpToken(ident_name_var_funct);
        }
    }else{
        error(13);
    }
        //检查重命名
    if(sym!=lparent){
        error(22);
    }else{
        parameterlist();
        if(sym!=rparent){
            error(17);
        }else{
            getNextSym();
            if(sym!=lquote){
                error(24);
            }else{
                getNextSym();
                compound_statement();
                if(sym!=rquote){
                    error(19);
                }else{
                    getNextSym();
                    enter_funct();
                    enter_ident(tmp_token,is_global,func,typ,refer,adr);
                }
            }
        }
    }
}
void funct_main_declaraction(int is_global){
    int refer;
    int adr;
    if(sym==mainsy){
        getNextSym();
        if(sym!=lparent){
            error(21);
        }else{
            getNextSym();
            if(sym!=rparent){
                error(17);
            }else{
                getNextSym();
                if(sym!=lquote){
                    error(24);
                }else{
                    getNextSym();
                    compound_statement();
                    if(sym!=rquote){
                        error(19);
                    }else{
                        enter_funct();
                        //enter_ident(token,is_global,func,notyp,refer,adr);
                    }
                }
            }
        }
    }else{
        error(43);
    }
}
void parameterlist(){
    enum typs ident_typ;
    int redcl_flag=0;
    char tmp_token[LEN_OF_NAME];
    do{
        getNextSym();
        if(sym!=intsy&&sym!=charsy&&sym!=rparent){
            error(22);
        }else{
            if(sym==intsy||sym==charsy){
                if(sym==intsy){
                    ident_typ=ints;
                }else{
                    ident_typ=chars;
                }
                getNextSym();
                if(sym==ident){
                    //检查重命名;
                    if(redcl_flag){
                        error(36);
                    }else{
                        if(sym!=lbrack){
                            enter_ident(token,0,paras,ident_typ,0,0);
                        }else{
                            getNextSym();
                            if(sym!=intcon){
                                error(20);
                            }
                            getNextSym();
                            if(sym!=rbrack){
                                error(18);
                            }
                            enter_array();
                            enter_ident(token,0,paras,arrays,0,0);
                        }
                    }
                    getNextSym();
                }else{
                    error(13);
                }
            }else{
                break;
            }
        }
    }while(sym==comma);
}
void compound_statement(){
    if(sym==constsy){
        constdeclaraction(0);
    }
    if(sym==intsy||sym==charsy){
        vardeclaraction(0);
    }
    statements();
}
void statement(){
    char tmp_token[LEN_OF_NAME];
    switch(sym){
        case ifsy:
            getNextSym();
            if_statement();
            break;
        case whilesy:
            getNextSym();
            while_statement();
            break;
        case lquote:
            getNextSym();
            statements();
            getNextSym();
            break;
        case ident:
            strcpy(tmp_token,token);
            getNextSym();
            if(sym==lbrack){
                getNextSym();
                simpleexpression();
                if(sym!=rbrack){
                    error(18);
                }else{
                    getNextSym();
                }
            }
            if(sym==becomes){
                assignment(tmp_token);
            }else if(sym==lparent){
                funct_call(tmp_token);
            }else{
                error(32);
                //test
                break;
            }
            testsemicolon();
            break;
        case scanfsy:
            getNextSym();
            scanf_statement();
            testsemicolon();
            break;
        case printfsy:
            getNextSym();
            printf_statement();
            testsemicolon();
            break;
        case switchsy:
            getNextSym();
            switch_statement();
            break;
        case returnsy:
            getNextSym();
            return_statement();
            testsemicolon();
            break;
        case semicolon:
            getNextSym();
            break;
    }
}
void statements(){
    char tmp_token[LEN_OF_NAME];
    do{
        statement();
    }while(sym!=rquote);
}
void if_statement(){
    if(sym!=lparent){
        error(25);
    }else{
        getNextSym();
        condition();
    }
    if(sym!=rparent){
        error(17);
        //test
    }else{
        getNextSym();
        statement();
    }
}
void while_statement(){
    if(sym!=lparent){
        error(25);
    }else{
        getNextSym();
        condition();
    }
    if(sym!=rparent){
        error(17);
    }else{
        getNextSym();
        statement();
    }
}
void condition(){
    enum symbol tmp_sym;
    simpleexpression();
    if(sym==eql||sym==neq||sym==gtr||sym==geq||sym==lss||sym==leq){
        tmp_sym=sym;
        getNextSym();
        simpleexpression();
        switch(tmp_sym){
        case eql://根据不同情况生成指令
            break;
        default:
            break;
        }
    }else if(sym==rparent){
        //打标签
        return;
    }
}
void assignment(char tmp_token[]){
    enum typs ret_typ;
    int res_position=position(tmp_token);
    //tmp_token可能是数组
    if(sym==lbrack){
        getNextSym();
        selector();
    }
    if(sym!=becomes){
        error(12);
    }else{
        if(res_position==-1){
            error(32);
        }
        getNextSym();
        ret_typ=simpleexpression();
        if(ret_typ==ints && ident_tab[res_position].typ==ints){
            //修改符号表
        }else if(ret_typ==chars && ident_tab[res_position].typ==chars){
            //修改符号表
        }else if(ret_typ==ints && ident_tab[res_position].typ==ints){
            //修改符号表
        }else{
            error(11);
        }
    }
}
void switch_statement(){
    enum typs condition_typ;
    if(sym!=lparent){
        error(25);
    }else{
        getNextSym();
        condition_typ=simpleexpression();
        if(sym!=rparent){
            error(17);
        }else{
            getNextSym();
            if(sym!=lquote){
                error(24);
            }else{
                getNextSym();
                caselabel();
                if(sym!=rquote){
                    error(19);
                }else{
                    getNextSym();
                }
            }
        }
    }
}
void caselabel(){
    while(sym==casesy){
        //常量
        getNextSym();
        if(sym!=intcon&&sym!=charcon){
            error(41);
        }else{
            //分号
            getNextSym();
            onecase();
            //打标签
        }
    }
    if(sym==defaultsy){
        getNextSym();
        defaultcase();
    }
}
void onecase(){
    if(sym!=colon){
        error(40);
    }else{
        getNextSym();
        statement();
    }
}
void defaultcase(){
    if(sym!=colon){
        error(42);
    }else{
        getNextSym();
        statement();
    }
}
void scanf_statement(){
    int res_position;
    if(sym!=lparent){
        error(29);
    }else{
        getNextSym();
        do{
            if(sym!=ident){
                error(30);
            }else{
                res_position=position(token);
                if(res_position==-1){
                    error(32);
                }else{
                    //插入读指令
                    getNextSym();
                }
            }
        }while(sym==comma);
        if(sym!=rparent){
            error(17);
        }else{
            getNextSym();
        }
    }
}
void printf_statement(){
    int res_position;
    if(sym!=lparent){
        error(29);
    }else{
        getNextSym();
        if(sym==stringcon){
            //处理字符串
            getNextSym();
            if(sym==comma){
                getNextSym();
                simpleexpression();
            }
            if(sym!=rparent){
                error(17);
            }else{
                getNextSym();
            }
        }else {
            simpleexpression();
            if(sym!=rparent){
                error(17);
            }else{
                getNextSym();
            }
        }
    }
}
int factor(){
    int res_position;
    char tmp_token[LEN_OF_NAME];
    //test
    if(sym==ident||sym==intcon||sym==charcon){
        if(sym==ident){
            strcpy(tmp_token,token);
            //res_position=position(token);
            if(0){//res_position==-1){
                error(32);
                getNextSym();
            }else{
                getNextSym();
                if(sym==lbrack){
                    getNextSym();
                    selector();
                }else if(sym==lparent){
                    getNextSym();
                    funct_call(tmp_token);
                }
                else{
//                switch(ident_tab[res_position].typ){
//                case con://相应生成操作
//                    break;
//                case var:
//                    switch(ident_tab[res_position].obj){
//                    case ints:
//                        break;
//                    case chars:
//                        break;
//                    case arrays:
//                        break;
//                    }
//                    break;
//                case func:
//                    switch(ident_tab[res_position].obj){
//                    case notyp:
//                        error(37);
//                        break;
//                    case ints:
//                        break;
//                    case chars:
//                        break;
//                    }
//                    break;
//                 case array:
//                }
                }
            }
        }else{
            //插入加载常量操作
            getNextSym();
        }
    }else if(sym==lparent){
        getNextSym();
        simpleexpression();
        if(sym!=rparent){
            error(17);
        }else{
            getNextSym();
        }
    }
    return notyp;
}
int term(){
    enum symbol fac_op;
    factor();
    while(times==sym||sym==idiv){
        fac_op=sym;
        getNextSym();
        factor();
        //插入相应操作
        if(fac_op==times){
            ;
        }else{
            ;
        }
    }
    return notyp;
}
int simpleexpression(){
    enum symbol positive;
    if(sym==pluss||sym==minuss){
        positive=sym;
        getNextSym();
        term();
        if(positive==minuss){
            //插入取反操作
        }
    }else{
        term();
    }
    while(sym==pluss||sym==minuss){
        positive=sym;
        getNextSym();
        term();
        if(positive==pluss){
            //插入加指令
        }else{
            //插入减指令
        }
    }
    return notyp;
}
int expression(){
    enum symbol cmp_op;
    simpleexpression();
    if(eql<=sym&& sym<=leq){
        cmp_op=sym;
        getNextSym();
        simpleexpression();
        switch(cmp_op){//插入相应比较指令
        case eql:
            break;
        case neq:
            break;
        case gtr:
            break;
        case geq:
            break;
        case lss:
            break;
        case leq:
            break;
        }
    }
    return notyp;
}
void return_statement(){
    if(sym!=lparent){
        error(29);
    }else{
        getNextSym();
        simpleexpression();
    }
    if(sym!=rparent){
        error(17);
    }
    getNextSym();
}
void funct_call(char funct_name[]){
    if(sym!=rparent){
        value_parameterlist();
    }else{
        //没有参数
    }
    if(sym!=rparent){
        error(17);
    }else{
        getNextSym();
    }
}
void value_parameterlist(){
    simpleexpression();
    if(sym==comma){
        do{
            getNextSym();
            simpleexpression();
        }while(sym==comma);
    }
}
void selector(){
    simpleexpression();
    if(sym!=rbrack){
        error(18);
    }else{
        getNextSym();
    }
}
////符号表管理////
int position(char tmp_token[]){
    int i;
    int res=-1;
    for(i=ident_index-1;i>=0;i--){
        if(!strcmp(tmp_token,ident_tab[i].name)){
            res=i;
            break;
        }
    }
    if(res==-1){
        i=-1;
    }
    return i;
}
void enter_ident(char token[],int is_global,enum objects obj,enum typs typ,int refer,int adr){
    return ;
    if(ident_index>=IDENT_TAB_LEN){
        fatal();
    }else{
        strcpy(ident_tab[ident_index].name,token);
        ident_tab[ident_index].is_global=is_global;
        ident_tab[ident_index].obj=obj;
        ident_tab[ident_index].typ=typ;
        ident_tab[ident_index].refer=refer;
        ident_tab[ident_index].adr=adr;
    }

}
void enter_array(){
    ;
}
void enter_funct(){
    ;
}

////语义分析////

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
