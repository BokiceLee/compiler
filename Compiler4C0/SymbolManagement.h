#ifndef SYMBOLMANAGEMENT_H_INCLUDED
#define SYMBOLMANAGEMENT_H_INCLUDED
#define LEN_OF_STRING_TAB 200
#define LEN_OF_STRING 200
#define LEN_OF_NAME 20
#define LEN_OF_FUNC_TAB 200
#define IDENT_TAB_LEN 200
#define LEN_OF_ARRAY_TAB 200

enum objects{con,var,arrays,func,paras};
enum types{ints,chars,notyp};
struct global_ident_tab_item{
    char name[LEN_OF_NAME];
    enum objects obj;
    enum types typ;
    int refer;
    int adr;
};
struct local_ident_tab_item{
    char name[LEN_OF_NAME];
    enum objects obj;
    int link;
    enum types typ;
    int refer;
    int adr;
};
struct funct_tab_item{
    int last;
    int lastpar;
    int psize;
    int vsize;
};

int i_temp;
enum types typ_var_funct;
char ident_name_var_funct[LEN_OF_NAME];

char string_tab[LEN_OF_STRING_TAB][LEN_OF_STRING];
int string_table_index=-1;
int string_index=0;

struct global_ident_tab_item global_ident_tab[IDENT_TAB_LEN];
struct local_ident_tab_item local_ident_tab[IDENT_TAB_LEN];
int global_ident_index=0;
int local_ident_index=0;
int last_local_ident_index=-1;

struct funct_tab_item funct_tab[LEN_OF_FUNC_TAB];
int funct_tab_index=0;
int funct_index=-1;//记录当前函数在函数表中的索引

int array_tab[LEN_OF_ARRAY_TAB];//貌似不需要用到
int array_tab_index=0;

extern void fatal();

int position(char tmp_token[],int *is_global);
void enter_ident(int is_global,char name[],int obj,int typ,int refer,int adr);
void enter_array(int high);//好像不会用的
int enter_funct(int last,int lastpar,int psize,int vsize);
int check_redeclaraction(int is_global,char ident_name[]);

int position(char tmp_token[],int *is_global){
    int i;
    for(i=local_ident_index-1;i>=0;i=local_ident_tab[i].link){
        if(strcmp(tmp_token,local_ident_tab[i].name)==0){
            *is_global=0;
            return i;
        }
    }
    for(i=global_ident_index-1;i>=0;i--){
        if(strcmp(tmp_token,global_ident_tab[i].name)==0){
            *is_global=1;
            return i;
        }
    }
    return i;
}
void enter_ident(int is_global,char name[],int obj,int typ,int refer,int adr){
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
#endif // SYMBOLMANAGEMENT_H_INCLUDED
