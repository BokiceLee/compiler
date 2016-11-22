#ifndef GRAMMARANALYSIS_H_INCLUDED
#define GRAMMARANALYSIS_H_INCLUDED
void program();
void constdeclaraction(int is_global,int fsys[],int fsys_len);
void constdefinition();
void vardeclaraction();
void vardefinition();
void parameterlist();
void funct_ret_declaraction();
void funct_void_declaraction();
void funct_main_declaraction();
void compound_statement();
void statements();
void statement();
void selector();
void resulttype();
void condition();
void expression();
void simpleexpression();
void term();
void factor();
void assignment();
void if_statement();
void switch_statement;
void caselabel();
void onecase();
void defaultcase();
void while_statement();
void scanf_statement();
void printf_statement();
void return_statement();
void funct_call(char funct_name[]);
void value_parameterlist();
void declaration_head();
#endif // GRAMMARANALYSIS_H_INCLUDED
