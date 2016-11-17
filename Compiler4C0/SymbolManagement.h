#ifndef SYMBOLMANAGEMENT_H_INCLUDED
#define SYMBOLMANAGEMENT_H_INCLUDED
#define LEN_OF_STRING_TAB 200
#define LEN_OF_STRING 200
#define LEN_OF_NAME 20
#define LEN_OF_FUNC 200
#define IDENT_TAB_LEN 200
enum objects{con,var,func,paras};
enum typs{ints,chars,arrays,notyp};
struct ident_tab_item{
    char* name[LEN_OF_NAME];
    int is_global;
    enum objects obj;
    enum typs typ;
    int refer;
    int adr;
};
struct funct_tab_item{
    int last;
    int lastpar;
    int psize;
    int vsize;
};
struct array_tab_item{
    enum typs eltyp;
    int high;
    int elsize;
    int totalsize;
};
int funct_index[LEN_OF_FUNC];
char string_tab[LEN_OF_STRING_TAB][LEN_OF_STRING];
void enter_ident(char token[],int is_global,enum objects obj,enum typs typ,int refer,int adr);
void enter_array();
void enter_string();
void enter_funct();
int position(char token[]);
#endif // SYMBOLMANAGEMENT_H_INCLUDED
