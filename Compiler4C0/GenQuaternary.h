#ifndef GENQUATERNARY_H_INCLUDED
#define GENQUATERNARY_H_INCLUDED
#define LABEL_LEN 20
#define LABEL_TABLE_LEN 1000
#define VAR_LEN 20
#define QUAT_TABLE_LEN 400
#define NAME_LEN 20
#define SW 20
#define MAX_PARA 20
#define OP_LEN 31
enum op_code{
    op_add=0,op_sub,op_mul,op_idiv,//3
    op_mov,//4
    op_para,//5
    op_call,op_arr_assign,op_ret_void,op_ret_value,//9
    op_beq,op_bne,op_ble,op_bls,op_bgt,op_bge,//15
    op_jump,//16
    op_prints,op_printi,op_printc,op_scanfc,op_scanfi,//21
    op_set_label,//22
    op_var_dcl,op_array_dcl,//24
    op_load_ret,//25
    op_func,op_efunc,op_main,op_emain,//29
    op_arr_get
};
char* op_name[OP_LEN]={
    "add",
    "sub",
    "mul",
    "idiv",
    "mov",
    "para",
    "call",
    "arr_assign",
    "ret_void",
    "ret_value",
    "beq",
    "bne",
    "ble",
    "bls",
    "bgt",
    "bge",
    "jump",
    "prints",
    "printi",
    "printc",
    "scanfc",
    "scanfi",
    "set_label",
    "var_dcl",
    "array_dcl",
    "load_ret",
    "func",
    "efunc",
    "main",
    "emain",
    "arr_get"
};
struct quat_struct{
    enum op_code op;
    char dest[VAR_LEN];
    char src1[VAR_LEN];
    char src2[VAR_LEN];
};
//enum compare_type{
//    cmp_eql,cmp_neq,cmp_leq,cmp_lss,cmp_gtr,cmp_geq,cmp_bgz,cmp_blez
//};
int gen_quaternary(int op,char dest[],char src1[],char src2[]);
void gen_name(char name[]);
int gen_lab();
void insert2name(char name[],int c,int typ);
void convert_name(char dest[],char src[],int is_global);
void para_name(char dest[],int n);
void gen_string_name(char name[]);
int gen_quaternary(int op,char dest[],char src1[],char src2[]);
int get_para_index(int pos);

#endif // GENQUATERNARY_H_INCLUDED
