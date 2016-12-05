#include "GenQuaternary.h"
#ifndef OPTIMIZATION_H_INCLUDED
#define OPTIMIZATION_H_INCLUDED
#define NEXT_BLOCK_NUM 2
#define BLOCK_NUM 1000
#define FUNC_NUM 100
#define N_NUM 20
#define NODE_NUM 100
struct basic_block{
    int b_num;
    int beginx;
    int endx;
    int suffixBlock[NEXT_BLOCK_NUM];
    int suffix_num=0;
};
struct dag_map_node{
    enum op_code op;
    int dest;
    int src1;
    int src2;
};
struct dag_list_node{
    int node_x[N_NUM];
    int node_num;
    char name[VAR_LEN];
};
struct basic_block basic_blocks[FUNC_NUM][BLOCK_NUM];
int block_num;
int entry[FUNC_NUM][BLOCK_NUM];
int entry_num[FUNC_NUM];
int blockx[FUNC_NUM];
int funcx=0;
void add_block(int beginx,int endx);
void divide_block();
void build_dag();
void export_code_from_dag();
void enter_dag_node();

int add_block(int func_x,int beginx,int endx){
    basic_blocks[func_x][blockx[funcx].beginx=beginx;
    basic_blocks[func_x][blockx[funcx].endx=endx;
    blockx[func_x]++;
    return block[func_x]-1;
}
void add_suffix(int fun_x,int block_i,int sf_i){
    basic_blocks[fun_x][block_i].suffixBlock[basic_blocks[fun_x][block_i].suffix_num++]=sf_i;
}
void sort_entryment(int ent[],int ent_num){
    int i,j;
    int tmp;
    for(i=0;i<ent_num;i++){
        for(j=i;j<ent_num;j++){
            if(ent[i]>ent[j]){
                tmp=ent[i];
                ent[i]=ent[j];
                ent[j]=tmp;
            }
        }
    }
}
void find_entries(){
    int i=0;
    int en_index;
    while(i<=quat_index){
        while(i<=quat_index&&quat_table[i].op!=op_func&&quat_table[i].op!=op_main){
            i++;
        }
        if(i>quat_index){
            break;
        }
        en_index=0;
        if(quat_table[i].op==op_func||quat_table[i].op==op_main){
            funct_begin=i;
            entryment[funcx][en_index++]=i;
            i++;
            while(quat_table[i].op!=op_efunc&&quat_table[i].op!=op_emain){
                if(quat_table[i].op<=16 &&quat_table[i].op>=10){//分支指令
                    for(j=funct_begin;;j++){
                        if(quat_table[j].op==op_efunc||quat_table[j].op==op_emain){
                            break;
                        }
                        if(quat_table[j].op==op_set_label){
                            if(strcmp(quat_table[j].dest,quat_table[i].dest)==0){
                                entryment[funcx][en_index++]=j;
                                break;
                            }
                        }
                    }
                    //目标
                    entryment[funcx][en_index++]=i+1;
                    //后继
                }else if(quat_table[i].op==op_ret_value||quat_table[i].op==op_ret_void){//返回指令后继
                    entryment[funcx][en_index++]=i+1;
                }
                i++;
            }
            entry_num[funcx]=en_index;
            en_index=0;
            funcx++;
        }
    }
}
void get_suffix(int func_i,int endx,int* const p_suffix1begin,int* const p_suffix2begin){
    int i;
    int func_begin=entry[func_i][0];
    i=0;
    if(quat_table[endx].op==op_jump){
        while(i<entry_num[func_i]){//跳转只有一种标签
            if(quat_table[i].op==op_set_label&&strcmp(quat_table[i].dest,quat_table[endx].dest)==0){
                break;
            }
        }
        *p_suffix1begin=i;
        //找到jmp目的标签所在
    }else if(quat_table[endx].op<=15 && quat_table[endx].op>=10){//比较语句有两种标签
        while(i<entry_num[func_i]){//跳转只有一种标签
            if(quat_table[i].op==op_set_label&&strcmp(quat_table[i].dest,quat_table[endx].dest)==0){
                break;
            }
        }
        *p_suffix1begin=i;
        *p_suffix2begin=endx+1;
    }else{//否则下一个就是后继
        *p_suffix2begin=endx+1;
    }
    while(quat_table[i].dest){
        if(quat_table[i].op==op_main||quat_table[i].op==op_func){
            break;
        }
        i--;
    }
}
void divide_block(){
    int i,j;
    int suffix1begin=0;
    int suffix2begin=0;
    int beginx,endx;
    int block_num;
    int func_begin;
    for(i=0;i<funcx;i++){
        for(j=0;j<BLOCK_NUM;j++){
            entered[BLOCK_NUM]=0;
        }
        for(j=0;j<entry_num[i];j++){
            beginx=entry[i][j];
            endx=entry[i][j+1]-1;
            block_num=add_block(i,beginx,endx);
            get_suffix(i,endx,&suffix1begin,&suffix2begin);
            if(suffix1begin!=0){
                add_suffix(i,j,suffix1begin);
            }
            if(suffix2begin!=0){
                add_suffix(i,j,suffix2begin);
            }
        }
    }
}
void enter_dag_node(int op_type,){

}
int find_dag(int op,int src1,int src2){

}
void build_dag(struct basic_block block){
    int beginx=block.beginx;
    int endx=block.endx;
    struct dag_map_node dag_map[NODE_NUM];
    struct dag_list_node dag_list[NODE_NUM];
    for(i=beginx;i<=endx;i++){
        switch(quat_table[i].op){
        case op_add:
        case op_mul:
            break;
        case op_sub:
        case op_idiv:
            break;
        case op_arr_assign:
            break;
        case op_arr_get:
            break;
        defautl:
            ;
        }
    }
}
#endif // OPTIMIZATION_H_INCLUDED
