#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#define MAX_LEN_OF_FILE 1024
FILE *fsource;
FILE *ftarget;
char fsourcename[MAX_LEN_OF_FILE];
char ftargetname[MAX_LEN_OF_FILE];
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
void output(char s[]){
    fprintf(ftarget,"%s",s);
}
int main()
{
    char line[1024];
    setInputOutput();
    while(fgets(line,1024,fsource)){
        output(line);
    }

    fclose(fsource);
    return 0;
}
