#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#define MAX_LEN_OF_FILE 1024
#define MAX_LEN_OF_LINE 1024
#define MAX_LEN_OF_NUM 8
#define MAX_NUM 99999999

FILE *fsource;//源代码文件指针
FILE *fquat;
FILE *fasm;
int is_optimize=0;
char fsourcename[MAX_LEN_OF_FILE];//源代码文件名称
char ftarget_dir[MAX_LEN_OF_FILE];//目标代码文件名称
char fasm_name[MAX_LEN_OF_FILE]="C:\\Users\\24745\\Desktop\\res.asm";
char fquat_name[MAX_LEN_OF_FILE]="C:\\Users\\24745\\Desktop\\quat.txt";

void setInputOutput();
void finish_compile();

void setInputOutput(){
    set_in:
    printf("输入源代码文件的绝对路径，按回车结束\n");
    scanf("%s",fsourcename);
    //strcpy(fsourcename,"C://Users//24745//Desktop//error_test//e5.txt");
    fsource=fopen(fsourcename,"r");
    if(NULL==fsource){
        printf("no such file\n");
        goto set_in;
    }
    printf("输入存储的四元式结果和汇编结果的目录的绝对路径，文件的名称默认为quat.txt和res.asm\n");
    scanf("%s",ftarget_dir);
//    strcpy(ftarget_dir,"C://Users//24745//Desktop//hh//");
    strcpy(fquat_name,ftarget_dir);
    strcpy(fasm_name,ftarget_dir);
    strcat(fquat_name,"/quat.txt");
    strcat(fasm_name,"/res.asm");
    printf("结果将存储在目录%s下\n%s\n",ftarget_dir,"错误信息将显示在控制台");
}
void finish_compile(){
    fclose(fsource);
    fclose(fasm);
    fclose(fquat);
    //fclose(fasm);
}

#endif // GLOBAL_H_INCLUDED
