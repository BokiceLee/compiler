#ifndef GENQUATERNARY_H_INCLUDED
#define GENQUATERNARY_H_INCLUDED
#define LABEL_LEN 20
#define LABEL_TABLE_LEN 200
#define VAR_LEN 20
#define QUAT_TABLE_LEN 200
#define NAME_LEN 20
#define SW 20
#define MAX_PARA 20
void emit();
struct quat_struct{
    enum op_code op;
    char dest[VAR_LEN];
    char src1[VAR_LEN];
    char src2[VAR_LEN];
    char i1;
    char i2;
    int c1;
    int c2;
};
enum op_code{
    op_add,op_sub,op_mul,op_idiv,//3
    op_mov,//4
    op_push,op_pop,//6
    op_call,op_arr,op_ret_void,op_ret_value,//10
    op_beq,op_bne,op_ble,op_bls,op_bgt,op_bge//16
    op_jump,//17
    op_prints,op_printi,op_printc,op_scanfc,op_scanfi,//18
    op_set_label,//19
    op_var_dcl,op_array_dcl,//21
    op_load_ret,
    op_func,op_efunc,op_main,op_emain
};
//enum compare_type{
//    cmp_eql,cmp_neq,cmp_leq,cmp_lss,cmp_gtr,cmp_geq,cmp_bgz,cmp_blez
//};
int gen_quaternary(int op,char dest[],char src1[],char src2[]);
void gen_name(char name[]);
int gen_lab();
void insert2name(char name[],int c,int typ);
#endif // GENQUATERNARY_H_INCLUDED
