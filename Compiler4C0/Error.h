#include "LexicalAnalysis.h"
#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED
#define ERROR_NUM 48
#define SET_LEN 30
char errormsg[ERROR_NUM][40]={
        "数字以0开头"						  ,//0
        "数字以字母结尾"                      ,//1
        "数字太大"                            ,//2
        "Char类型变量字符非法"                ,//3
        "Char类型中的字符有多个"              ,//4
        "字符串中的字符非法"                  ,//5
        "!非法使用"                           ,//6
        "非法字符"                            ,//7
        "源程序不完整"                        ,//8
        "在char类型中变量为空"                ,//9
        "标识符非法"                          ,//10
        "表达式类型不匹配"                    ,//11
        "标识符或变量后应为="                 ,//12
        "int，char后应为标识符"               ,//13
        "Const后应为int或char"                ,//14
        "缺少分号’;’"                         ,//15
        "0前面出现符号"                       ,//16
        "缺少’)’"                             ,//17
        "缺少’]’"                             ,//18
        "缺少’}’"                             ,//19
        "数组长度应为无符号整数"              ,//20
        "Main函数后有多余字符或定义"          ,//21
        "参数表定义出错"                      ,//22
        "常量初始化出错"                    ,//23
        "函数体定义缺少{"                     ,//24
        "条件语句缺少("                       ,//25
        "将char值赋值给int类型标识符" 		  ,//26
        "Switch语句错误"                      ,//27
        "不规范标识符"                        ,//28
        "读写语句、返回语句格式错误，缺少’(’" ,//29
        "读写语句的’,’后应为标识符"           ,//30
        "不可识别语句"                        ,//31
        "未定义标识符"                        ,//32
        "数组引用越界"                        ,//33
        "不规范的标示符类型"                  ,//34
        "函数调用的参数个数或类型不匹配"      ,//35
        "重复定义标识符"                      ,//36
        "无返回值函数不能作为因子"            ,//37
        "有返回值函数没有合法的返回语句"      ,//38
        "+-符号后应为数字"                    ,//39
        "case后应为常量"                      ,//40
        "case后应为:"                         ,//41
        "default应为:"                        ,//42
        "未定义main函数"                      ,//43
        "main函数返回类型为void"              ,//44
        "错误的结束字符"                      ,//45
        "return语句与函数类型不匹配"          ,//46
        "该标识符未定义或者不是函数"          //47
};

int errors=0;

void error(int err_code);
void fatal();
int search_sym_in_set(int sym_set[],int set_len);
void skip(int sym_set[],int set_len,int err_code);
int merge_sym_set(int s1[],int s1_len,int s2[],int s2_len);
void test(int legal_set[],int legal_set_len,int stop_set[],int stop_set_len,int err_code);
void needsym(int sym_need);

void fatal(){
    ;
}
void error(int err_code){
    switch(err_code){
    case -1:
        return;
    case 0:
        token[--read_ch_len]='\0';
        break;
    case 2:
        num_read=0;
        break;
    }
    printf("error near row %d,column %d(maybe not right):\n",row_in_source_file,column_in_source_file);
    printf("error:%d ~~~~~error message:%s\n",++errors,errormsg[err_code]);
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


#endif // ERROR_H_INCLUDED
