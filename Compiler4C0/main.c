#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#define MAX_LEN_OF_FILE 1024
#define MAX_LEN_OF_LINE 1024
FILE *fsource;//Դ�����ļ�ָ��
FILE *ftarget;//Ŀ���������ļ�ָ��
char fsourcename[MAX_LEN_OF_FILE];//Դ�����ļ�����
char ftargetname[MAX_LEN_OF_FILE];//Ŀ������ļ�����
int cc;//char counter
int ll;// line length
char ch;//�������ַ�
char line[MAX_LEN_OF_LINE];//��ȡһ���ַ�
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
    if(cc==ll){
        if(feof(fsource)==NULL){
            out2Screen("source file incomplete\n");
        }else{
            fgets(line,MAX_LEN_OF_LINE,fsource);
            cc=0;
            ll=strlen(line);
            line[++ll]=' ';
        }
    }
    ch=line[cc++];
}
void getNextSym(){

}
int main()
{
    setInputOutput();
    fclose(fsource);
    return 0;
}
