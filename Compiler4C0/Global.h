#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#define MAX_LEN_OF_FILE 1024
#define MAX_LEN_OF_LINE 1024
#define MAX_LEN_OF_NUM 8
#define MAX_NUM 99999999

FILE *fsource;//Դ�����ļ�ָ��
FILE *fquat;
FILE *fasm;
int is_optimize=0;
char fsourcename[MAX_LEN_OF_FILE];//Դ�����ļ�����
char ftarget_dir[MAX_LEN_OF_FILE];//Ŀ������ļ�����
char fasm_name[MAX_LEN_OF_FILE]="C:\\Users\\24745\\Desktop\\res.asm";
char fquat_name[MAX_LEN_OF_FILE]="C:\\Users\\24745\\Desktop\\quat.txt";

void setInputOutput();
void finish_compile();

void setInputOutput(){
    set_in:
    printf("����Դ�����ļ��ľ���·�������س�����\n");
    scanf("%s",fsourcename);
    //strcpy(fsourcename,"C://Users//24745//Desktop//error_test//e5.txt");
    fsource=fopen(fsourcename,"r");
    if(NULL==fsource){
        printf("no such file\n");
        goto set_in;
    }
    printf("����洢����Ԫʽ����ͻ������Ŀ¼�ľ���·�����ļ�������Ĭ��Ϊquat.txt��res.asm\n");
    scanf("%s",ftarget_dir);
//    strcpy(ftarget_dir,"C://Users//24745//Desktop//hh//");
    strcpy(fquat_name,ftarget_dir);
    strcpy(fasm_name,ftarget_dir);
    strcat(fquat_name,"/quat.txt");
    strcat(fasm_name,"/res.asm");
    printf("������洢��Ŀ¼%s��\n%s\n",ftarget_dir,"������Ϣ����ʾ�ڿ���̨");
}
void finish_compile(){
    fclose(fsource);
    fclose(fasm);
    fclose(fquat);
    //fclose(fasm);
}

#endif // GLOBAL_H_INCLUDED
