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
void enter_ident(int is_global,char name[],int obj,int typ,int refer,int adr);
void enter_array(int high);
void enter_string();
int enter_funct(int last,int lastpar,int psize,int vsize);
int position(char token[]);
#endif // SYMBOLMANAGEMENT_H_INCLUDED
