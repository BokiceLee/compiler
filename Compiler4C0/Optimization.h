#include "GenQuaternary.h"
#ifndef OPTIMIZATION_H_INCLUDED
#define OPTIMIZATION_H_INCLUDED
#define NEXT_BLOCK_NUM 100
#define BLOCK_NUM 1000
#define FUNC_NUM 100
struct basic_block{
    int beginx;
    int endx;
    int suffixBlock[NEXT_BLOCK_NUM];
    int suffix_num=0;
};
struct basic_block basic_blocks[FUNC_NUM][BLOCK_NUM];
int blockx=0;
int funcx=0;
void add_block(int beginx,int endx);
void divide_block();
void build_dag();
void export_code_from_dag();
void enter_dag_node();

void add_block(int func_x,int beginx,int endx){
    basic_blocks[func_x][blockx].beginx=beginx;
    basic_blocks[func_x][blockx].endx=endx;
    blockx++;
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
void divide_block(){
    int i=0;
    int j;
    int k;
    int funct_begin;
    int entryment[BLOCK_NUM];
    int en_index=0;
    while(i<=quat_index){
        while(i<=quat_index&&quat_table[i].op!=op_func&&quat_table[i].op!=op_main){
            i++;
        }
        if(i>quat_index){
            break;
        }
        if(quat_table[i].op==op_func||quat_table[i].op==op_main){
            funct_begin=i;
            entryment[en_index++]=i;
            i++;
            while(quat_table[i].op!=op_efunc&&quat_table[i].op!=op_emain){
                if(quat_table[i].op<=16 &&quat_table[i].op>=10){//分支指令
                    for(j=funct_begin;;j++){
                        if(quat_table[j].op==op_efunc||quat_table[j].op==op_emain){
                            break;
                        }
                        if(quat_table[j].op==op_set_label){
                            if(strcmp(quat_table[j].dest,quat_table[i].dest)==0){
                                entryment[en_index++]=j;
                                break;
                            }
                        }
                    }
                    //目标
                    entryment[en_index++]=i+1;
                    //后继
                }else if(quat_table[i].op==op_ret_value||quat_table[i].op==op_ret_void){//返回指令后继
                    entryment[en_index++]=i+1;
                }
                i++;
            }
        //登进基本块并清空
            sort_entryment(entryment,en_index);
            for(j=0;j<en_index-1;j++){
                add_block(entryment[j],entryment[j+1]-1);
            }
            for(j=0;j<en_index;j++){
                if(quat_table[entryment[j]].op<=15&&quat_table[entryment[j]].op>=10){

                }else{

                }
            }
            //清空
            en_index=0;
            funcx++;
        }
    }
}
#endif // OPTIMIZATION_H_INCLUDED
