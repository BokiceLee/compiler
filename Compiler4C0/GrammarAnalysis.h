#ifndef GRAMMARANALYSIS_H_INCLUDED
#define GRAMMARANALYSIS_H_INCLUDED
void program();
void constdeclaraction(int fsys[],int fsys_len,int is_global);
void constdefinition(int fsys[],int fsys_len,int is_global);
void vardeclaraction(int fsys[],int fsys_len,int is_global);
void vardefinition(int fsys[],int fsys_len,int is_global);
void parameterlist(int fsys[],int fsys_len);
void funct_ret_declaraction(int fsys[],int fsys_len);
void funct_void_declaraction(int fsys[],int fsys_len);
void funct_main_declaraction(int fsys[],int fsys_len);
void compound_statement(int fsys[],int fsys_len);
void statements(int fsys[],int fsys_len);
void statement(int fsys[],int fsys_len);
void selector(int fsys[],int fsys_len);
void resulttype();
void condition(int fsys[],int fsys_len);
void expression();
int simpleexpression(int fsys[],int fsys_len);
int term(int fsys[],int fsys_len);
int factor(int fsys[],int fsys_len);
void assignment(int fsys[],int fsys_len,char tmp_token[]);
void if_statement(int fsys[],int fsys_len);
void switch_statement(int fsys[],int fsys_len);
void caselabel(int fsys[],int fsys_len);
void onecase(int fsys[],int fsys_len);
void defaultcase(int fsys[],int fsys_len);
void while_statement(int fsys[],int fsys_len);
void scanf_statement(int fsys[],int fsys_len);
void printf_statement(int fsys[],int fsys_len);
void return_statement(int fsys[],int fsys_len);
void funct_call(int fsys[],int fsys_len,char funct_name[]);
void value_parameterlist();
void declaration_head();
#endif // GRAMMARANALYSIS_H_INCLUDED
