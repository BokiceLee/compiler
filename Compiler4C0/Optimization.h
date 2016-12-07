#include "GenQuaternary.h"
#ifndef OPTIMIZATION_H_INCLUDED
#define OPTIMIZATION_H_INCLUDED
#define NEXT_BLOCK_NUM 2
#define BLOCK_NUM 1000
#define FUNC_NUM 100
#define N_NUM 20
#define NODE_NUM 100
#define FATHER_NUM 20
struct basic_block{
    int b_num;
    int beginx;
    int endx;
    int suffixBlock[NEXT_BLOCK_NUM];
    int suffix_num=0;
};
struct dag_map_node{
    enum op_code op;
    int dest;//整数位该节点在节点表中登记所用
    int src1;
    int src2;
    int father_n;//父节点个数
    int fathers[FATHER_NUM];
};
struct dag_list_node{
    int node_n[N_NUM];//同名节点在不同时间修改,怎么处理
    int node_x=-1;
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
struct dag_map_node dag_map[NODE_NUM];
struct dag_list_node dag_list[NODE_NUM];
int dag_map_x=-1;
int dag_list_x=-1;
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
int insert2dag_map(int op,int src1_n,int src2_n){
    dag_map_x++;
    dag_map[dag_map_x].dest=dag_map_x;
    dag_map[dag_map_x].op=op;
    dag_map[dag_map_x].src1=src1_n;
    dag_map[dag_map_x].src2=src2_n;
    if(op==-1){
        dag_map[dag_map_x].father_n=0;
    }else{
        dag_map[src1_n].fathers[dag_map[src1_n].father_n++]=dag_map_x;
        if(op!=op_mov){
            dag_map[src2_n].fathers[dag_map[src2_n].father_n++]=dag_map_x;
        }
    }
    return dag_map_x;
}
void insert2dag_list(int node_n,char node_name){
    dag_list_x++;
    strcpy(dag_list[dag_list_x].name,node_name);
    dag_list[dag_list_x].node_x++;
    dag_list[dag_list_x].node_n[node_x]=node_n;
}
void handle_node(int op,char dest[],char src1[],char src2[]){
    int i;
    int find_x,find_y,find_z,find_mid;
    int src1_n,src2_n,dest_n;
    find_x=0;
    find_y=0;
    find_z=0;
    find_mid=0;
    //查找源操作数1
    for(i=0;i<=dag_list_x;i++){
        if(strcmp(dag_list[i].name,src1)==0){
            src1_n=dag_list[i].node_n;
            find_x=1;
        }
    }
    if(find_x==0){
    //如果未找到，插入节点到dag图和dag表
        src1_n=insert2dag_map(-1,-1,-1);
        insert2dag_list(src1_n,src1);
    }
    //查找源操作数2
    for(i=0;i<=dag_list_x;i++){
        if(strcmp(dag_list[i].name,src1)==0){
            src2_n=dag_list[i].node_n;
            find_y=1;
        }
    }
    if(find_y==0){
        src2_n=insert2dag_map(-1,-1,-1);
        insert2dag_list(src2_n,src2);
    }
    if(find_x==1&&op==op_mov){
        find_y=1;//mov只有一个操作数
    }
    //在图中查找与目标操作数等价的中间节点，获取其编号
    if(find_x==1&&find_y==1){
        for(i=0;i<=dag_map_x;i++){
            if(dag_map[i].op==op){
                switch(op){
                case op_add:
                case op_mul:
                    if(dag_map[i].src1==src1_n&&dag_map[i].src2==src2_n){
                        dest_n=dag_map[i].dest;
                        find_mid=1;
                    }else if(dag_map[i].src2==src1_n&&dag_map[i].src1==src2_n){
                        dest_n=dag_map[i].dest;
                        find_mid=1;
                    }
                    break;
                }
                case op_idiv:
                case op_sub:
                case op_arr_assign:
                case op_arr_get:
                    if(dag_map[i].src1==src1_n&&dag_map[i].src2==src2_n){
                        dest_n=dag_map[i].dest;
                        find_mid=1;
                    }
                    break;
                case op_mov:
                    if(dag_map[i].src1==src1_n){
                        dest_n=dag_map[i].dest;
                        find_mid=1;
                    }

            }
        }
    }
    if(find_mid==0){//未找到中间节点，向dag图中插入新的节点
        dest_n=insert2dag_map(op,src1_n,src2_n);
    }
    //在表中查找目标操作数
    for(i=0;i<list_len;i++){
        if(strcmp(dag_list[i].name,dest)==0){//在表中找到目标操作数，修改节点编号
            dag_list[i].node_x++;
            dag_list[i].node_n[node_x]=dest_n;
            find_k=1;
        }
    }
    if(find_k==0){//未找到目标操作数，插入
        insert2dag_list(dest_n,dest);
    }
}
void build_dag(struct basic_block block){
    int beginx=block.beginx;
    int endx=block.endx;
    for(i=beginx;i<=endx;i++){
        switch(quat_table[i].op){
        case op_add:
        case op_mul:
        case op_sub:
        case op_idiv:
        case op_arr_assign:
        case op_arr_get:
        case op_mov:
            handle_node(quat_table[i].op,quat_table[i].dest,quat_table[i].src1,quat_table[i].src2);
            break;
        defautl:
            ;
        }
    }
}
void export_one_node(int node_x,int father_in,int export_queue,int* const export_n){
    if(dag_map[node_x].father_n==0||dag_map[node_x].father_n==father_in[node_x]){
        (*export_n)++;
        export_queue[*export_n]=node_x;
        father_in[dag_map[node_x].src1]++;
        if(dag_map[node_x].op!=op_mov){
            father_in[dag_map[node_x].src2]++;
        }
        export_one_node(dag_map[node_x].src1,father_in,export_queue,export_n);
    }
}
void reverse_queue(int export_queue[],int export_n){
    int i=0;
    int tmp;
    while(i<export_n/2){
        tmp=export_queue[i];
        export_queue[i]=export_queue[export_n-i];
        export_queue[export_n-i]=tmp;
        i++;
    }
}
void export_dag(){
    int i;
    int export_n=-1;
    int export_queue[dag_map_x+1];
    int father_in[dag_map_x+1][dag_map_x+1];
    char src1[VAR_LEN];
    char src2[VAR_LEN];
    char dest[VAR_LEN];
    for(i=0;i<=dag_map_x;i++){
        father_in[i]=0;
    }
    while(export_n<dag_map_x){
        for(i=0;i<=dag_map_x;i++){
            export_one_node(i,father_in,export_queue,&export_n);
        }
    }
    reverse_queue(export_queue,export_n);
    for(i=0;i<=export_n;i++){
        if(dag_map[export_queue[i].op!=-1){

            for(j=0;j<dag_list_x;j++){
                for(k=0;k<dag_list[j].node_x;k++){
                    if(export_queue[i]==dag_list[j].node_n[k]){
                        //输出对应的节点名称
                    }
                }
            }
        }
    }
}
#endif // OPTIMIZATION_H_INCLUDED
