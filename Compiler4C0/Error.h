#include "LexicalAnalysis.h"
#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED
#define ERROR_NUM 48
#define SET_LEN 30
char errormsg[ERROR_NUM][40]={
        "������0��ͷ"						  ,//0
        "��������ĸ��β"                      ,//1
        "����̫��"                            ,//2
        "Char���ͱ����ַ��Ƿ�"                ,//3
        "Char�����е��ַ��ж��"              ,//4
        "�ַ����е��ַ��Ƿ�"                  ,//5
        "!�Ƿ�ʹ��"                           ,//6
        "�Ƿ��ַ�"                            ,//7
        "Դ��������"                        ,//8
        "��char�����б���Ϊ��"                ,//9
        "��ʶ���Ƿ�"                          ,//10
        "���ʽ���Ͳ�ƥ��"                    ,//11
        "��ʶ���������ӦΪ="                 ,//12
        "int��char��ӦΪ��ʶ��"               ,//13
        "Const��ӦΪint��char"                ,//14
        "ȱ�ٷֺš�;��"                         ,//15
        "0ǰ����ַ���"                       ,//16
        "ȱ�١�)��"                             ,//17
        "ȱ�١�]��"                             ,//18
        "ȱ�١�}��"                             ,//19
        "���鳤��ӦΪ�޷�������"              ,//20
        "Main�������ж����ַ�����"          ,//21
        "�����������"                      ,//22
        "������ʼ������"                    ,//23
        "�����嶨��ȱ��{"                     ,//24
        "�������ȱ��("                       ,//25
        "��charֵ��ֵ��int���ͱ�ʶ��" 		  ,//26
        "Switch������"                      ,//27
        "���淶��ʶ��"                        ,//28
        "��д��䡢��������ʽ����ȱ�١�(��" ,//29
        "��д���ġ�,����ӦΪ��ʶ��"           ,//30
        "����ʶ�����"                        ,//31
        "δ�����ʶ��"                        ,//32
        "��������Խ��"                        ,//33
        "���淶�ı�ʾ������"                  ,//34
        "�������õĲ������������Ͳ�ƥ��"      ,//35
        "�ظ������ʶ��"                      ,//36
        "�޷���ֵ����������Ϊ����"            ,//37
        "�з���ֵ����û�кϷ��ķ������"      ,//38
        "+-���ź�ӦΪ����"                    ,//39
        "case��ӦΪ����"                      ,//40
        "case��ӦΪ:"                         ,//41
        "defaultӦΪ:"                        ,//42
        "δ����main����"                      ,//43
        "main������������Ϊvoid"              ,//44
        "����Ľ����ַ�"                      ,//45
        "return����뺯�����Ͳ�ƥ��"          ,//46
        "�ñ�ʶ��δ������߲��Ǻ���"          //47
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
