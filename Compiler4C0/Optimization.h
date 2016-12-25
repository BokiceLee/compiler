#include "GenQuaternary.h"
#ifndef OPTIMIZATION_H_INCLUDED
#define OPTIMIZATION_H_INCLUDED
#define QUAT_LEN 1000
#define VAR_LEN 20
#define OP_LEN 31
#define F_NUM 100
#define B_NUM 100
#define E_NUM 100
#define N_NUM 50
#define DAG_NUM 1000
#define VAR_NUM 100
#define GLOBAL_REG_NUM 3
#define MAX_SUFFIX 2
enum regs{//寄存器0-2全局寄存器，3-5临时寄存器
    ebx,edi,esi,eax,ecx,edx,no_reg
};
struct opt_quat_struct{//优化时所用的四元式结构
    int quat_id;
    enum op_code op;
    char dest[VAR_LEN];
    char src1[VAR_LEN];
    char src2[VAR_LEN];
    struct opt_quat_struct* next;
};
struct var_reg_struct{//变量-寄存器对应表
    char var[VAR_LEN];
    int varx;//变量在all_out中的下标
    enum regs reg;
};
struct block_struct{//基本块结构
    struct opt_quat_struct* block_begin;
    struct block_struct* suffix[MAX_SUFFIX];
    int suffix_num;
    char def[VAR_NUM][VAR_LEN];
    char use[VAR_NUM][VAR_LEN];
    char in[VAR_NUM][VAR_LEN];
    char out[VAR_NUM][VAR_LEN];
    int def_len;
    int use_len;
    int in_len;
    int out_len;
};
struct funct_struct{//函数结构
    struct block_struct blocks[B_NUM];
    int block_num;
    struct opt_quat_struct* funct_begin;
    //struct quat_struct* funct_end;应该用不上
    struct var_reg_struct var_reg[VAR_NUM];
    int var_reg_len;
};
struct dag_map_struct{//dag图结构
    int node_id;
    int lchild_id;
    int rchild_id;
    enum op_code op;
    int is_assigned;
};
struct dag_table_struct{//dag表结构
    char name[VAR_LEN];
    int node_id[N_NUM];
    int id_num;
    int has_init;
};

int entryment[F_NUM][E_NUM];//每个函数的入口语句
int entry_num[F_NUM];//每个函数入口语句个数
struct opt_quat_struct *data;
struct funct_struct functs[F_NUM];//函数
int g_funct_num;
struct opt_quat_struct *seq_begin,*after_seq;//消除局部公共子表达式的连续四元式的第一个式子和下一个式子
struct dag_map_struct dag_map[DAG_NUM];//dag图
struct dag_table_struct dag_table[DAG_NUM];//dag表
int father_num[DAG_NUM];//每个dag图节点的父节点个数
int export_queue[DAG_NUM];//导出dag图节点序列
char node_name[DAG_NUM][VAR_LEN];//每个dag图节点的名称
int dag_table_x=-1;//dag表下标
int dag_map_x=-1;//dag图下标
int export_x=0;//导出序列下标
int rep_name_x=0;//用于生成node_name
int current_funct;//当前函数
int current_block;//当前块
struct opt_quat_struct *export_head,*export_tail;//导出的第一个四元式和最后一个四元式
char all_out[VAR_NUM][VAR_LEN];//单个函数中所有out的并
int all_out_len=0;
int conflict_matrix[VAR_NUM][VAR_NUM];//冲突矩阵

int find_entries(struct quat_struct quat_table[],int quat_num){//找到所有的入口语句，返回函数个数
    int i=0;
    int j;
    int en_index;
    int funcx=0;
    int funct_begin;
    while(i<=quat_num){
        while(i<=quat_num&&quat_table[i].op!=op_func&&quat_table[i].op!=op_main){
            i++;
        }
        if(i>quat_num){
            break;
        }
        en_index=0;
        if(quat_table[i].op==op_func||quat_table[i].op==op_main){
            funct_begin=i;//函数入口
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
            entryment[funcx][en_index++]=i;
            entry_num[funcx]=en_index;
            en_index=0;
            funcx++;
        }
    }
    return funcx;//返回函数个数
}
int sort_entryment(int ent[],int ent_num){//整理入口语句
    int i,j;
    int tmp;
    int new_num;
    int cpy[E_NUM];
    for(i=0;i<ent_num;i++){
        for(j=i;j<ent_num;j++){
            if(ent[i]>ent[j]){
                tmp=ent[i];
                ent[i]=ent[j];
                ent[j]=tmp;
            }
        }
    }
    for(i=0;i<ent_num;i++){
        cpy[i]=ent[i];
    }
    ent[0]=cpy[0];
    i=1;
    new_num=1;
    while(i<ent_num){
        if(cpy[i]>cpy[i-1]){
            ent[new_num++]=cpy[i];
        }
        i++;
    }
    return new_num;
}

//转换四元式为链表存储
void mov_struct(struct opt_quat_struct* next_quat,struct quat_struct src){//拷贝数据
    strcpy(next_quat->src1,src.src1);
    strcpy(next_quat->src2,src.src2);
    strcpy(next_quat->dest,src.dest);
    next_quat->op=src.op;
    next_quat->next=NULL;
}
struct opt_quat_struct* convert_quat_table(struct quat_struct src[],int src_len){//转换成链表
    int i;
    struct opt_quat_struct *head,*tail,*p;
    head=NULL;
    tail=NULL;
    p=NULL;
    for(i=0;i<src_len;i++){
        p=(struct opt_quat_struct*)malloc(sizeof(struct opt_quat_struct));
        mov_struct(p,src[i]);
        p->quat_id=i;
        if(head==NULL){
            head=p;
        }else{
            tail->next=p;
        }
        tail=p;
    }
    return head;
}
//划分基本块
struct opt_quat_struct* get_quat_p(int quat_id,struct opt_quat_struct* p){//根据四元式编号获取其对应的指针
    while(p->next!=NULL){
        if(p->quat_id==quat_id){
            return p;
        }
        p=p->next;
    }
    return NULL;
}
int jumpbefore(int funcx,char label[]){//查找是否存在跳转到当前label的跳转语句，是则返回1，否则返回0
    int i;
    struct opt_quat_struct* p;
    for(i=0;i<functs[funcx].block_num;i++){
        p=functs[funcx].blocks[i].block_begin;
        while(p!=NULL){
            if(p->op>=op_beq&&p->op<=op_jump){
                if(strcmp(p->dest,label)==0){
                    return 1;
                }
            }
            p=p->next;
        }
    }
    return 0;
}
void divide_blocks(struct opt_quat_struct* code_head,struct quat_struct src[]){//将整个程序从单一列表转化成树形结构
    int i,valid,canjump;
    struct opt_quat_struct* p;
    struct opt_quat_struct* p_next;
    int funcx=0;
    p=code_head;
    while(p!=NULL){
        if(p->op==op_func||p->op==op_main){
            functs[funcx].funct_begin=p;//函数开始语句
            valid=1;
            //每个基本块进行处理
            for(i=0;i<entry_num[funcx]-1;i++){//每个基本块
                if(valid==0){//判断是否有有效语句中的语句能跳转到该语句
                    if(src[entryment[funcx][i]].op==op_set_label){
                        //在前面找语句
                        canjump=jumpbefore(funcx,src[entryment[funcx][i]].dest);
                        if(canjump){
                            valid=1;
                        }
                    }
                    if(valid==0){
                        p=get_quat_p(entryment[funcx][i+1],p);
                        continue;
                    }
                }
                p=get_quat_p(entryment[funcx][i],p);
                functs[funcx].blocks[functs[funcx].block_num].block_begin=p;
                p=get_quat_p(entryment[funcx][i+1]-1,p);

                if(p->op==op_ret_value||p->op==op_ret_void){
                    valid=0;//ret语句后的语句无效
                }//暂时不确定基本块之间的关系，因为基本块未确定
                functs[funcx].block_num++;

                p_next=p->next;
                p->next=NULL;//基本块的最后一个语句指向空
                p=p_next;
            }
            if(p->op==op_efunc||p->op==op_emain){//最后一个基本块为函数结束语句
                functs[funcx].blocks[functs[funcx].block_num].block_begin=p;
                functs[funcx].block_num++;
                funcx++;

                p_next=p->next;
                p->next=NULL;
                p=p_next;
            }else{
                printf("error:326,最后一个入口语句应是efunc or emain\n");
            }
        }else{
            p=p->next;
            printf("error:338,函数定义之间有不可识别语句\n");
        }
    }
}
//查找连续可优化四元式
int is_available_op(int op){//判断op运算是否可优化，是返回1，否则返回0
    switch(op){
        case op_mov://注意！
        case op_add:
        case op_sub:
        case op_mul:
        case op_idiv:
        case op_arr_assign:
        case op_arr_get:
        case op_printc:
        case op_printi:
        case op_prints:
            return 1;
        default:
            return 0;
    }
}
int find_continues_exp(struct opt_quat_struct* find_begin){//查找连续可优化四元式
    struct opt_quat_struct *p,*p_last;
    int con_num=0;
    seq_begin=NULL;
    after_seq=NULL;//置空
    for(p=find_begin;p!=NULL;p=p->next){//查找第一个运算式
        if(is_available_op(p->op)==1){
            seq_begin=p;
            break;
        }
    }
    while(p!=NULL){//查找所有运算式
        if(is_available_op(p->op)==1){
            p_last=p;
            p=p->next;
            con_num++;
        }else{
            p_last->next=NULL;//最后一个运算式的下一个语句置空
            after_seq=p;//运算式之后的第一个语句
            break;
        }
    }
    return con_num;
}
//建立dag图和dag表
int get_child_id(char var_name[]){//获取dag图节点id
    int i;
    for(i=dag_table_x;i>=0;i--){
        if(strcmp(var_name,dag_table[i].name)==0){
            return dag_table[i].node_id[dag_table[i].id_num-1];
        }
    }
    if(i==-1){
        dag_map_x++;
        dag_map[dag_map_x].node_id=dag_map_x;//向dag图中插入新节点，并获得节点id
        dag_map[dag_map_x].op=op_leaf;//表示该节点为叶节点
        dag_map[dag_map_x].is_assigned=0;//
        dag_map[dag_map_x].lchild_id=-1;
        dag_map[dag_map_x].rchild_id=-1;
        father_num[dag_map_x]=0;

        dag_table_x++;
        dag_table[dag_table_x].id_num=1;//向dag表中插入新对应关系
        dag_table[dag_table_x].node_id[0]=dag_map_x;
        strcpy(dag_table[dag_table_x].name,var_name);
        dag_table[dag_table_x].has_init=1;
    }
    return dag_map_x;
}
int get_parent_id(int op,int src1_id,int src2_id){//获取dag图节点id
    int i;
    for(i=dag_map_x;i>=0;i--){
        if(dag_map[dag_map_x].op==op){
            if(op==op_add||op==op_mul){
                if(dag_map[dag_map_x].lchild_id==src1_id && dag_map[dag_map_x].rchild_id==src2_id){
                    return dag_map_x;
                }else if(dag_map[dag_map_x].lchild_id==src2_id && dag_map[dag_map_x].rchild_id==src1_id){
                    return dag_map_x;
                }
            }else{
                if(dag_map[dag_map_x].lchild_id==src1_id && dag_map[dag_map_x].rchild_id==src2_id){
                    return dag_map_x;
                }
            }
        }
    }
    if(i==-1){
        dag_map_x++;
        dag_map[dag_map_x].node_id=dag_map_x;
        dag_map[dag_map_x].lchild_id=src1_id;
        dag_map[dag_map_x].rchild_id=src2_id;
        dag_map[dag_map_x].op=op;
        dag_map[dag_map_x].is_assigned=1;
        father_num[dag_map_x]=0;
    }
    return dag_map_x;
}
void update_dag_table(struct opt_quat_struct* quat,int dest_id,char dest_name[]){//更新dag表
    int i;
    for(i=dag_table_x;i>=0;i--){
        if(strcmp(dest_name,dag_table[i].name)==0){
            dag_table[i].node_id[dag_table[i].id_num]=dest_id;
            dag_table[i].id_num++;
            return;
        }
    }
    if(i==-1){
        dag_table_x++;
        strcpy(dag_table[dag_table_x].name,dest_name);
        dag_table[dag_table_x].id_num=1;
        dag_table[dag_table_x].node_id[0]=dest_id;
        dag_table[dag_table_x].has_init=0;
    }

}
void build_dag(int con_num){//建立dag图和dag表
    int i;
    struct opt_quat_struct* p;
    int dest_id,src1_id,src2_id;
    p=seq_begin;
    dag_map_x=-1;
    dag_table_x=-1;
    for(i=0;i<con_num;i++){
        if(p->op==op_mov){
            src1_id=get_child_id(p->src1);
            dag_map[src1_id].is_assigned=1;
            update_dag_table(p,src1_id,p->dest);
        }else if(p->op==op_printc || p->op==op_printi || p->op==op_prints){
            src1_id=get_child_id(p->dest);
            dag_map_x++;//直接插入新的节点
            dag_map[dag_map_x].op=p->op;
            dag_map[dag_map_x].node_id=dag_map_x;
            dag_map[dag_map_x].lchild_id=src1_id;
            dag_map[dag_map_x].rchild_id=-1;
            father_num[dag_map_x]=0;
            father_num[src1_id]++;
        }else{
            src1_id=get_child_id(p->src1);
            src2_id=get_child_id(p->src2);
            dest_id=get_parent_id(p->op,src1_id,src2_id);//更新父节点个数，什么父节点
            father_num[src1_id]++;
            father_num[src2_id]++;
            update_dag_table(p,dest_id,p->dest);
        }
        p=p->next;
    }
}
//重新导出中间代码
int is_exportable(int node_id){//判断dag图节点是否可导出
    return father_num[node_id]==0;
}
int get_max_node(){//找到可导出最大节点
    int i;
    for(i=dag_map_x;i>=0;i--){
        if(is_exportable(i)){
            father_num[i]=-1;
            if(dag_map[i].lchild_id!=-1){
                father_num[dag_map[i].lchild_id]--;
            }
            if(dag_map[i].rchild_id!=-1){
                father_num[dag_map[i].rchild_id]--;
            }
            return i;
        }
    }
    return i;
}
int is_used_later(char var_name[]){//判断临时变量是否在后继使用
    struct opt_quat_struct* p;
    int i;
    p=after_seq;
    while(p!=NULL){
        if(strcmp(var_name,p->src1)==0){
            return 1;
        }
        if(strcmp(var_name,p->src2)==0){
            return 1;
        }
        if(strcmp(var_name,p->dest)==0){
            return 1;
        }
        p=p->next;
    }
    for(i=current_block+1;i<functs[current_funct].block_num;i++){
        p=functs[current_funct].blocks[i].block_begin;
        while(p!=NULL){
            if(strcmp(var_name,p->src1)==0){
                return 1;
            }
            if(strcmp(var_name,p->src2)==0){
                return 1;
            }
            if(strcmp(var_name,p->dest)==0){
                return 1;
            }
            p=p->next;
        }
    }
    return 0;
}
void set_node_name(int node_id,char init_name[]){//设置节点名
    if(strcmp(init_name,"")==0){
        sprintf(node_name[node_id],"&r%03d",rep_name_x);
        rep_name_x++;
    }else{
        sprintf(node_name[node_id],"%s",init_name);
    }
}
void re_gen_quat(int op,char dest[],char src1[],char src2[]){//重新生成四元式
    struct opt_quat_struct* p;
    p=(struct opt_quat_struct*)malloc(sizeof(struct opt_quat_struct));
    if(export_head==NULL){
        export_head=p;
        export_tail=p;
    }else{
        export_tail->next=p;
        export_tail=p;
    }
    p->op=op;
    strcpy(p->dest,dest);
    strcpy(p->src1,src1);
    strcpy(p->src2,src2);
    p->next=NULL;
}
void finish_export(){//导出结尾处理
    struct opt_quat_struct *p,*tmp;
    p=seq_begin->next;
    seq_begin->op=export_head->op;
    strcpy(seq_begin->dest,export_head->dest);
    strcpy(seq_begin->src1,export_head->src1);
    strcpy(seq_begin->src2,export_head->src2);
    seq_begin->next=export_head->next;
    while(p!=NULL){
        tmp=p;
        p=p->next;
        free(tmp);
    }
    if(export_head==export_tail){
        seq_begin->next=after_seq;
    }else{
        export_tail->next=after_seq;
    }
}
int get_var_on_node(int var_on_node_x[],int node_id){//获取图节点上的变量下标
    int i,j;
    int v_num=0;
    if(dag_map[node_id].op==op_leaf){
        if(dag_map[node_id].is_assigned==0){
            for(i=0;i<=dag_table_x;i++){
                for(j=0;j<dag_table[i].id_num;i++){
                    if(dag_table[i].node_id[j]==node_id){
                        set_node_name(node_id,dag_table[i].name);
                        return 0;
                    }
                }
            }
        }else{//找到初值叶节点
            for(i=0;i<=dag_table_x;i++){
                for(j=0;j<dag_table[i].id_num;j++){
                    if(dag_table[i].node_id[j]==node_id){
                        if(dag_table[i].has_init==1){
                            set_node_name(node_id,dag_table[i].name);
                        }else{
                            if(dag_table[i].name[0]!='&'){
                                ;
                            }else if(is_used_later(dag_table[i].name)){
                                ;
                            }else{
                                continue;
                            }
                            var_on_node_x[v_num++]=i;
                        }
                    }
                }
            }
            return v_num;
        }
    }else{
        for(i=0;i<=dag_table_x;i++){
            if(dag_table[i].name[0]!='&'){
                ;
            }else if(is_used_later(dag_table[i].name)){
                ;
            }else{
                continue;
            }
            for(j=0;j<dag_table[i].id_num;j++){
                if(dag_table[i].node_id[j]==node_id){
                    var_on_node_x[v_num++]=i;
                }
            }
        }
        if(v_num==1){
            set_node_name(node_id,dag_table[var_on_node_x[0]].name);
            v_num=0;
        }else{
            set_node_name(node_id,"");
        }
        re_gen_quat(dag_map[node_id].op,node_name[node_id],node_name[dag_map[node_id].lchild_id],node_name[dag_map[node_id].rchild_id]);
        return v_num;
    }
}
void export_node(int node_id){//导出节点
    int i;
    int var_on_node_x[dag_table_x];
    int var_num=0;
    if(dag_map[node_id].op==op_printc || dag_map[node_id].op==op_printi || dag_map[node_id].op==op_prints){
        re_gen_quat(dag_map[node_id].op,node_name[dag_map[node_id].lchild_id],"","");
    }else{
        var_num=get_var_on_node(var_on_node_x,node_id);//同时设置node_name
        for(i=0;i<var_num;i++){
            re_gen_quat(op_mov,dag_table[var_on_node_x[i]].name,node_name[node_id],"");
        }
    }
}
void export_dag(){//从dag图重新导出中间代码
    int node_num=dag_map_x+1;
    int i;
    int export_id;
    int export_x=0;
    for(i=0;i<node_num;i++){
        strcpy(node_name[i],"");
    }
    while((export_id=get_max_node())>=0){
        export_queue[export_x++]=export_id;
        if(dag_map[export_id].op==op_leaf){
            continue;
        }
        while(1){
            if(is_exportable(dag_map[export_id].lchild_id)){
                export_id=dag_map[export_id].lchild_id;
                export_queue[export_x++]=export_id;
                father_num[export_id]=-1;
                if(dag_map[export_id].op==op_leaf){
                    break;
                }else{
                    if(dag_map[export_id].lchild_id!=-1){
                        father_num[dag_map[export_id].lchild_id]--;
                    }
                    if(dag_map[export_id].rchild_id!=-1){
                        father_num[dag_map[export_id].rchild_id]--;
                    }
                }
            }else{
                break;
            }
        }
    }
    export_head=NULL;
    export_tail=NULL;
    for(i=export_x-1;i>=0;i--){
        export_node(export_queue[i]);
    }
    finish_export();
}
//dag图消除公共子表达式
void exec_dag(int funct_num){//dag控制函数
    int i,j,k;
    struct opt_quat_struct *find_begin;
    struct opt_quat_struct *p;
    int con_num;
    for(i=0;i<funct_num;i++){
        current_funct=i;
        for(j=0;j<functs[i].block_num;j++){
            current_block=j;
            find_begin=functs[i].blocks[j].block_begin;
            while(find_begin!=NULL){//遍历该基本块
                con_num=find_continues_exp(find_begin);
                p=seq_begin;
                for(k=0;k<con_num;k++){
                    //printf("%d %s %s %s %s\n",p->quat_id,op_name[p->op],p->dest,p->src1,p->src2);
                    p=p->next;
                }
                //printf("\n");
                if(con_num>=1){
                    build_dag(con_num);
                    export_dag();
                    for(p=seq_begin;p!=after_seq;p=p->next){
                        ;//printf("%s %s %s %s\n",op_name[p->op],p->dest,p->src1,p->src2);
                    }
                }
                if(con_num==0){//未找到连续运算，进行下一个基本块
                    break;
                }else{
                    find_begin=after_seq;
                }
            }
        }
    }
}
//集合运算
int n_in_set(char ele[],char eles[][VAR_LEN],int set_len){//判断元素是否在集合中
    int i;
    int r;
    for(i=0;i<set_len;i++){
        r=strcmp(ele,eles[i]);
        if(r>0){
            continue;
        }else if(r==0){
            return -1;
        }else{
            return i;
        }
    }
    return i;
}
int merge_set(char dest_set[][VAR_LEN],char set1[][VAR_LEN],int set1_len,char set2[][VAR_LEN],int set2_len){//集合并运算
    char d_set[VAR_NUM][VAR_LEN];
    int d_x=0;
    int set1_x=0;
    int set2_x=0;
    int i;
    while(set1_x<set1_len&&set2_x<set2_len){
        if(strcmp(set1[set1_x],set2[set2_x])==-1){
            strcpy(d_set[d_x],set2[set2_x]);
            set2_x++;
            d_x++;
        }else{
            strcpy(d_set[d_x],set1[set1_x]);
            if(strcmp(set1[set1_x],set2[set2_x])==0){
                set2_x++;
            }
            set1_x++;
            d_x++;
        }
    }
    if(set1_x==set1_len){
        while(set2_x<set2_len){
            strcpy(d_set[d_x],set2[set2_x]);
            set2_x++;
            d_x++;
        }
    }else{
        while(set1_x<set1_len){
            strcpy(d_set[d_x],set1[set1_x]);
            set1_x++;
            d_x++;
        }
    }
    for(i=0;i<d_x;i++){
        strcpy(dest_set[i],d_set[i]);
    }
    return d_x;
}
int sub_set(char dest_set[][VAR_LEN],char set1[][VAR_LEN],int set1_len,char set2[][VAR_LEN],int set2_len){//集合差运算
    int i;
    char d_set[VAR_NUM][VAR_LEN];
    int d_set_x=0;
    for(i=0;i<set1_len;i++){
        if(n_in_set(set1[i],set2,set2_len)==-1){
            strcpy(d_set[d_set_x],set1[i]);
            d_set_x++;
        }
    }
    if(dest_set!=NULL){
        for(i=0;i<d_set_x;i++){
            strcpy(dest_set[i],d_set[i]);
        }
    }
    return d_set_x;
}
int insert_ele(char ele[],int ele_x,char eles[][VAR_LEN],int set_len){//集合添加元素
    int i=set_len;
    while(i>ele_x){
        strcpy(eles[i],eles[i-1]);
        i--;
    }
    strcpy(eles[ele_x],ele);
    return set_len++;
}
int eql_set(char set1[][VAR_LEN],int set1_len,char set2[][VAR_LEN],int set2_len){//判断结合是否相等
    int i;
    if(set1_len!=set2_len){
        return 0;
    }
    for(i=0;i<set1_len;i++){
        if(strcmp(set1[i],set2[i])!=0){
            return 0;
        }
    }
    return 1;
}
//计算基本块的use,def,in,out集合
int is_available_var(char var_name[]){//判断是否是可分配全局寄存器的变量
    if(var_name[0]=='&'){
        return 1;
    }
    if(var_name[0]=='@'){
        return 1;
    }
    return 0;
}
void add_ele(int use_def,char var_name[],struct block_struct *p_block){//将变量名添加到基本块的use或def集合中，use:0,def:1
    int ndefed;
    int nused;
    if(!is_available_var(var_name)){
        return;
    }
    if(use_def==0){
        ndefed=n_in_set(var_name,p_block->def,p_block->def_len);
        if(ndefed!=-1){
            nused=n_in_set(var_name,p_block->use,p_block->use_len);
            if(nused!=-1){
                insert_ele(var_name,nused,p_block->use,p_block->use_len);
                p_block->use_len++;
            }
        }
    }else{
        nused=n_in_set(var_name,p_block->use,p_block->use_len);
        if(nused!=-1){
            ndefed=n_in_set(var_name,p_block->def,p_block->def_len);
            if(ndefed!=-1){
                insert_ele(var_name,ndefed,p_block->def,p_block->def_len);
                p_block->def_len++;
            }
        }
    }
}
void get_use_def(struct block_struct *p_block){//求每个基本块的use和def集
    struct opt_quat_struct *p;
    p_block->def_len=0;
    p_block->use_len=0;
    p=p_block->block_begin;
    while(p!=NULL){
        switch(p->op){
        case op_printc:
        case op_printi:
        case op_ret_value:
        case op_para:
            add_ele(0,p->dest,p_block);
            break;
        case op_add:
        case op_sub:
        case op_mul:
        case op_idiv:
            add_ele(0,p->src1,p_block);
            add_ele(0,p->src2,p_block);
            add_ele(1,p->dest,p_block);
            break;
        case op_arr_assign:
        case op_bne:
        case op_beq:
        case op_bge:
        case op_bgt:
        case op_ble:
        case op_bls:
            add_ele(0,p->src1,p_block);
            add_ele(0,p->src2,p_block);
            break;
        case op_arr_get:
            add_ele(0,p->src2,p_block);
            add_ele(1,p->dest,p_block);
            break;
        case op_scanfi:
        case op_scanfc:
            add_ele(1,p->dest,p_block);
            break;
        case op_mov:
            add_ele(0,p->src1,p_block);
            add_ele(1,p->dest,p_block);
        default:
            ;
        }
        p=p->next;
    }
}
struct block_struct* find_block(char label_name[],int funcx){//找到以label对应的基本块
    int i;
    for(i=0;i<functs[funcx].block_num;i++){
        if(functs[funcx].blocks[i].block_begin->op==op_set_label){
            if(strcmp(functs[funcx].blocks[i].block_begin->dest,label_name)==0){
                return &(functs[funcx].blocks[i]);
            }
        }
    }
    return NULL;
}
void get_suffix(int funcx,int blocki){//为每个基本块找到对应的后缀
    struct opt_quat_struct* p;
    struct block_struct* p_block;
    functs[funcx].blocks[blocki].suffix_num=0;
    p=functs[funcx].blocks[blocki].block_begin;
    while(p->next!=NULL){
        p=p->next;
    }
    switch(p->op){
    case op_jump:
        p_block=find_block(p->dest,funcx);
        functs[funcx].blocks[blocki].suffix[functs[funcx].blocks[blocki].suffix_num++]=p_block;
        break;
    case op_beq:
    case op_bne:
    case op_bge:
    case op_bgt:
    case op_ble:
    case op_bls:
        p_block=find_block(p->dest,funcx);
        functs[funcx].blocks[blocki].suffix[functs[funcx].blocks[blocki].suffix_num++]=p_block;
        p_block=&(functs[funcx].blocks[blocki+1]);
        functs[funcx].blocks[blocki].suffix[functs[funcx].blocks[blocki].suffix_num++]=p_block;
        break;
    case op_ret_value:
    case op_ret_void:
        break;
    default:
        p_block=&(functs[funcx].blocks[blocki+1]);
        functs[funcx].blocks[blocki].suffix[functs[funcx].blocks[blocki].suffix_num++]=p_block;
        break;
    }
}
int cal_in(char d_set[][VAR_LEN],struct block_struct *p_block){//求解每个基本块的in集合，并更新out
    int i;
    char new_out[VAR_NUM][VAR_LEN];
    int new_out_len=0;
    int d_set_len=0;
    char sub_res[VAR_NUM][VAR_LEN];
    int sub_res_len=0;
    for(i=0;i<p_block->suffix_num;i++){
        new_out_len=merge_set(new_out,new_out,new_out_len,p_block->suffix[i]->in,p_block->suffix[i]->in_len);
    }
    for(i=0;i<new_out_len;i++){
        strcpy(p_block->out[i],new_out[i]);
    }
    p_block->out_len=new_out_len;
    sub_res_len=sub_set(sub_res,new_out,new_out_len,p_block->def,p_block->def_len);
    d_set_len=merge_set(d_set,p_block->use,p_block->use_len,sub_res,sub_res_len);
    return d_set_len;
}
void cal_in_out(int funcx){//判断in是否改变，是则更新，否则完成
    int haschanged=0;
    int i,j;
    struct block_struct *p_block;
    char new_in[VAR_NUM][VAR_LEN];
    int new_in_len=0;
    for(i=functs[funcx].block_num-1;i>=0;i--){
         functs[funcx].blocks[i].in_len=0;
         functs[funcx].blocks[i].out_len=0;
    }
    while(1){
        for(i=functs[funcx].block_num-2;i>=0;i--){
            p_block=&(functs[funcx].blocks[i]);
            haschanged=0;
            new_in_len=cal_in(new_in,p_block);
            if(eql_set(p_block->in,p_block->in_len,new_in,new_in_len)==0){
                for(j=0;j<new_in_len;j++){
                    strcpy(p_block->in[j],new_in[j]);
                }
                p_block->in_len=new_in_len;
                haschanged=1;
            }
        }
        if(haschanged==0){
            break;
        }
    }
}
//全局寄存器分配
void convert2x(char vars[][VAR_LEN],int varx[],int v_len){//将变量转化为其在all_out中的下标
    int i,j;
    for(i=0;i<v_len;i++){
        for(j=0;j<all_out_len;j++){
            if(strcmp(vars[i],all_out[j])==0){
                varx[i]=j;
                break;
            }
        }
    }
}
void build_conflit_matrix(int funcx){//建立冲突矩阵
    int i,j,k;
    int var_xs[VAR_NUM];
    char block_var[VAR_NUM][VAR_LEN];
    int bvar_len;
    all_out_len=0;
    for(i=0;i<functs[funcx].block_num;i++){//合并out，得到跨基本块的变量
        all_out_len=merge_set(all_out,all_out,all_out_len,functs[funcx].blocks[i].out,functs[funcx].blocks[i].out_len);
    }
    for(j=0;j<all_out_len;j++){//变量之间初始化为无冲突
        for(k=0;k<all_out_len;k++){
            conflict_matrix[j][k]=0;
        }
    }
    for(i=0;i<functs[funcx].block_num;i++){//同处一个基本跨in或out的变量之间存在冲突,是否有变量存在于in中但不存在于out中
        bvar_len=merge_set(block_var,functs[funcx].blocks[i].in,functs[funcx].blocks[i].in_len,functs[funcx].blocks[i].out,functs[funcx].blocks[i].out_len);
        convert2x(block_var,var_xs,bvar_len);//将变量名转化为其中out中的下表
        for(j=0;j<bvar_len;j++){//标识有冲突变量
            for(k=j+1;k<bvar_len;k++){
                conflict_matrix[var_xs[j]][var_xs[k]]=1;
                conflict_matrix[var_xs[k]][var_xs[j]]=1;
            }
        }
    }
}
void alloct_reg(int funcx){//按照冲突矩阵根据图着色算法进行寄存器分配
    int conflict_num[all_out_len];
    int regs_used[GLOBAL_REG_NUM]={0,0,0};
    enum regs distributed_reg;
    int varx_stack[VAR_NUM];
    int stack_x=0;
    int pop_x;
    int i,j;
    for(i=0;i<all_out_len;i++){
        conflict_num[i]=0;
        for(j=0;j<all_out_len;j++){
            conflict_num[i]+=conflict_matrix[i][j];
        }
    }//获得每个寄存器的冲突数
    for(i=0;i<all_out_len;i++){
        if(conflict_num[i]>=GLOBAL_REG_NUM){//如果冲突数大于等于寄存器个数，移除
            //对每个与其相连的变量，冲突数减一,更新冲突数
            for(j=i+1;j<all_out_len;j++){
                if(conflict_matrix[i][j]==1){
                    conflict_num[j]--;
                }
            }
        }else{//否则入栈
            varx_stack[stack_x++]=i;
            for(j=i+1;j<all_out_len;j++){
                if(conflict_matrix[i][j]==1){
                    conflict_num[j]--;
                }
            }
        }
    }
    while(stack_x>0){
        pop_x=varx_stack[--stack_x];
        distributed_reg=no_reg;
        for(i=0;i<functs[funcx].var_reg_len;i++){//找到不与该变量有连接边的已分配的寄存器的变量对应的寄存器
            if(conflict_matrix[pop_x][functs[funcx].var_reg[i].varx]==0){
                distributed_reg=functs[funcx].var_reg[i].reg;//将其分配给该变量
                break;
            }
        }
        if(distributed_reg==no_reg){
            for(i=0;i<GLOBAL_REG_NUM;i++){
                if(regs_used[i]==0){
                    distributed_reg=i;
                    regs_used[i]=1;
                    break;
                }
            }
        }
        if(distributed_reg==no_reg){
            printf("998:无法分配\n");
        }
        functs[funcx].var_reg[functs[funcx].var_reg_len].reg=distributed_reg;
        functs[funcx].var_reg[functs[funcx].var_reg_len].varx=pop_x;
        strcpy(functs[funcx].var_reg[functs[funcx].var_reg_len].var,all_out[pop_x]);
        functs[funcx].var_reg_len++;
    }
}

//优化总控制程序
void optimize()
{
    struct opt_quat_struct *head,*code,*p;
    int i,j,k;
    int funct_num;
    int quat_num;
    head=NULL;
    p=NULL;
    quat_num=quat_index+1;
    head=convert_quat_table(quat_table,quat_num);
    p=head;
    i=0;
    funct_num=find_entries(quat_table,quat_num);
    g_funct_num=funct_num;
    for(i=0;i<funct_num;i++){
        entry_num[i]=sort_entryment(entryment[i],entry_num[i]);
    }
    //入口语句信息开始转化为链表存储
    p=head;
    if(p->op!=op_func&&p->op!=op_main){//将data和code分离
        data=p;
        while(p->next->op!=op_func && p->next->op!=op_main){
            p=p->next;
        }
        code=p->next;
        p->next=NULL;
    }else{
        data=NULL;
        code=p;
    }
    divide_blocks(code,quat_table);
    exec_dag(funct_num);
    for(i=0;i<funct_num;i++){
        for(j=0;j<functs[i].block_num;j++){
            get_use_def(&(functs[i].blocks[j]));
            get_suffix(i,j);
            printf("\nfunct %d block %d \nuse: ",i,j);
            for(k=0;k<functs[i].blocks[j].use_len;k++){
                printf("%s ",functs[i].blocks[j].use[k]);
            }
            printf("\ndef: ");
            for(k=0;k<functs[i].blocks[j].def_len;k++){
                printf("%s ",functs[i].blocks[j].def[k]);
            }
        }
    }
    for(i=0;i<funct_num;i++){
        cal_in_out(i);
        for(j=0;j<functs[i].block_num;j++){
            printf("\nfunct %d block %d \nin: ",i,j);
            for(k=0;k<functs[i].blocks[j].in_len;k++){
                printf("%s ",functs[i].blocks[j].in[k]);
            }
            printf("\nout: ");
            for(k=0;k<functs[i].blocks[j].out_len;k++){
                printf("%s ",functs[i].blocks[j].out[k]);
            }
        }
    }
    for(i=0;i<funct_num;i++){
        build_conflit_matrix(i);
        alloct_reg(i);
        printf("\nfunct %d",i);
        for(j=0;j<functs[i].var_reg_len;j++){
            printf("\n%s,%d",functs[i].var_reg[j].var,functs[i].var_reg[j].reg);
        }
    }
//    quat_num=0;
//    for(p=data;p!=NULL;p=p->next){
//        src[quat_num].op=p->op;
//        strcpy(src[quat_num].dest,p->dest);
//        strcpy(src[quat_num].src1,p->src1);
//        strcpy(src[quat_num].src2,p->src2);
//        quat_num++;
//    }
//    for(i=0;i<funct_num;i++){
//        for(j=0;j<functs[i].block_num;j++){
//            p=functs[i].blocks[j].block_begin;
//            while(p!=NULL){
//                src[quat_num].op=p->op;
//                strcpy(src[quat_num].dest,p->dest);
//                strcpy(src[quat_num].src1,p->src1);
//                strcpy(src[quat_num].src2,p->src2);
//                p=p->next;
//                quat_num++;
//            }
//        }
//    }
//    for(i=0;i<quat_num;i++){
//        printf("%d %s %s %s %s\n",i,op_name[src[i].op],src[i].dest,src[i].src1,src[i].src2);
//    }
    return;
}
void opt_outputquat(){
    int i,j;
    int m=0;
    int op_n;
    struct opt_quat_struct* p;
    fquat=fopen(fquat_name,"w");
    for(i=0;i<g_funct_num;i++){
        for(j=0;j<functs[i].block_num;j++){
            p=functs[i].blocks[j].block_begin;
            while(p!=NULL){
                fprintf(fquat,"%d  ",m);
                m++;
                op_n=p->op;
                fprintf(fquat,"%s  ",op_name[op_n]);
                convert_output(p->dest);
                convert_output(p->src1);
                convert_output(p->src2);
                fprintf(fquat,"\n");
            }
        }
    }
}
#endif // OPTIMIZATION_H_INCLUDED
