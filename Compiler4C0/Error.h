#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED
#define ERROR_NUM 10
#define SET_LEN 20
char* errormsg[ERROR_NUM]={
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
        "���Ͳ�ƥ��"                          ,//11
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
        "main������������Ϊvoid"               //44
};
void error(int err_code);
void fatal();
int search_sym_in_set(int sym_set[],int set_len);
int merge_sym_set(int s1[],int s1_len,int s2[],int s2_len);
void skip(int sym_set[],int set_len,int err_code);
void test(int legal_set[],int legal_set_len,int stop_set[],int stop_set_len,int err_code);
void testsemicolon();
#endif // ERROR_H_INCLUDED
