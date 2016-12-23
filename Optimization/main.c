#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
int entryment[F_NUM][E_NUM];
int entry_num[F_NUM];
enum regs{
    eax,ecx,edx,ebx,edi,esi,no_reg
};
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
    op_arr_get,//30
    op_leaf//31 叶节点
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
struct src_quat{
    enum op_code op;
    char dest[VAR_LEN];
    char src1[VAR_LEN];
    char src2[VAR_LEN];
};
struct quat_struct{
    int quat_id;
    enum op_code op;
    char dest[VAR_LEN];
    char src1[VAR_LEN];
    char src2[VAR_LEN];
    struct quat_struct* next;
};
void mov_struct(struct quat_struct* next_quat,struct src_quat src){
    strcpy(next_quat->src1,src.src1);
    strcpy(next_quat->src2,src.src2);
    strcpy(next_quat->dest,src.dest);
    next_quat->op=src.op;
    next_quat->next=NULL;
}
struct var_reg_struct{
    char var[VAR_LEN];
    int varx;
    enum regs reg;
};
struct quat_struct* convert_quat_table(struct src_quat src[],int src_len){
    int i;
    struct quat_struct *head,*tail,*p;
    head=NULL;
    tail=NULL;
    p=NULL;
    for(i=0;i<src_len;i++){
        p=(struct quat_struct*)malloc(sizeof(struct quat_struct));
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
struct block_struct{
    struct quat_struct* block_begin;
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
struct funct_struct{
    struct block_struct blocks[B_NUM];
    int block_num;
    struct quat_struct* funct_begin;
    struct quat_struct* funct_end;
    struct var_reg_struct var_reg[VAR_NUM];
    int var_reg_len;
};
struct dag_map_struct{
    int node_id;
    int lchild_id;
    int rchild_id;
    enum op_code op;
    int is_assigned;
};
struct dag_table_struct{
    char name[VAR_LEN];
    int node_id[N_NUM];
    int id_num;
    int has_init;
};
struct funct_struct funct_stru[F_NUM];
struct quat_struct *seq_begin,*after_seq;
struct dag_map_struct dag_map[DAG_NUM];
struct dag_table_struct dag_table[DAG_NUM];
int father_num[DAG_NUM];
int father_in[DAG_NUM];
int export_queue[DAG_NUM];
char node_name[DAG_NUM][VAR_LEN];
int dag_table_x=-1;
int dag_map_x=-1;
int export_x=0;
int current_funct;
int current_block;
struct quat_struct *export_head,*export_tail;
int rep_name_x=0;

int get_op(char op_n[]){
    int i;
    for(i=0;i<OP_LEN;i++){
        if(strcmp(op_n,op_name[i])==0){
            return i;
        }
    }
    return -1;
}
int get_squat(struct src_quat src[]){
    int i,r;
    int line_no;
    char op[20],dest[20],src1[20],src2[20];
    FILE* fquat;
    char fname[40]="C:\\Users\\24745\\Desktop\\quat.txt";
    fquat=fopen(fname,"r");
    i=0;
    while(1){
        r=fscanf(fquat,"%d",&line_no);
        if(r<0){
            break;
        }
        fscanf(fquat,"%s",op);
        fscanf(fquat,"%s",dest);
        fscanf(fquat,"%s",src1);
        fscanf(fquat,"%s",src2);
        src[i].op=get_op(op);
        if(strcmp(dest,"__")==0){
            strcpy(dest,"");
        }
        if(strcmp(src1,"__")==0){
            strcpy(src1,"");
        }
        if(strcmp(src2,"__")==0){
            strcpy(src2,"");
        }
        strcpy(src[i].dest,dest);
        strcpy(src[i].src1,src1);
        strcpy(src[i].src2,src2);
//        printf("%d %d %s %s %s\n",line_no,src[i].op,src[i].dest,src[i].src1,src[i].src2);
        i++;
    }
    return i;
}
int find_entries(int quat_num,struct src_quat quat_table[]){
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
            entryment[funcx][en_index++]=i;
            entry_num[funcx]=en_index;
            en_index=0;
            funcx++;
        }
    }
    return funcx;
}
int sort_entryment(int ent[],int ent_num){
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
struct quat_struct* get_quat_p(int quat_id,struct quat_struct* p){
    while(p->next!=NULL){
        if(p->quat_id==quat_id){
            return p;
        }
        p=p->next;
    }
    return NULL;
}
int jumpbefore(int funcx,char label[]){
    int i;
    struct quat_struct* p;
    for(i=0;i<funct_stru[funcx].block_num;i++){
        p=funct_stru[funcx].blocks[i].block_begin;
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
void divide_blocks(struct quat_struct* head,struct src_quat src[],int funct_num){
    int i,valid,canjump;
    struct quat_struct* p;
    struct quat_struct* p_next;
    int funcx=0;
    p=head;
    while(p!=NULL){
        if(p->op==op_func||p->op==op_main){
            funct_stru[funcx].funct_begin=p;//函数开始语句
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
                funct_stru[funcx].blocks[funct_stru[funcx].block_num].block_begin=p;
                p=get_quat_p(entryment[funcx][i+1]-1,p);

                if(p->op==op_ret_value||p->op==op_ret_void){
                    valid=0;//ret语句后的语句无效
                }//暂时不确定基本块之间的关系，因为基本块未确定
                funct_stru[funcx].block_num++;

                p_next=p->next;
                p->next=NULL;//基本块的最后一个语句指向空
                p=p_next;
            }
            if(p->op==op_efunc||p->op==op_emain){//最后一个基本块为函数结束语句
                funct_stru[funcx].blocks[funct_stru[funcx].block_num].block_begin=p;
                funct_stru[funcx].block_num++;
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
int is_available_op(int op){
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
int find_continues_exp(struct quat_struct* find_begin){
    struct quat_struct *p,*p_last;
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
int get_child_id(char var_name[]){
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
int get_parent_id(int op,int src1_id,int src2_id){
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
void update_dag_table(struct quat_struct* quat,int dest_id,char dest_name[]){
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
void build_dag(int con_num){
    int i;
    struct quat_struct* p;
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
int is_exportable(int node_id){
    return father_in[node_id]==0;
}
int get_max_node(){
    int i;
    for(i=dag_map_x;i>=0;i--){
        if(is_exportable(i)){
            father_in[i]=-1;
            if(dag_map[i].lchild_id!=-1){
                father_in[dag_map[i].lchild_id]--;
            }
            if(dag_map[i].rchild_id!=-1){
                father_in[dag_map[i].rchild_id]--;
            }
            return i;
        }
    }
    return i;
}
int is_used_later(char var_name[]){
    struct quat_struct* p;
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
    for(i=current_block+1;i<funct_stru[current_funct].block_num;i++){
        p=funct_stru[current_funct].blocks[i].block_begin;
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
void set_node_name(int node_id,char init_name[]){
    if(strcmp(init_name,"")==0){
        sprintf(node_name[node_id],"&r%03d",rep_name_x);
        rep_name_x++;
    }else{
        sprintf(node_name[node_id],"%s",init_name);
    }
}
void re_gen_quat(int op,char dest[],char src1[],char src2[]){
    struct quat_struct* p;
    p=(struct quat_struct*)malloc(sizeof(struct quat_struct));
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
void finish_export(){
    struct quat_struct *p,*tmp;
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
int get_var_on_node(int var_on_node_x[],int node_id){
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
void export_node(int node_id){
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
void export_dag(){
    int node_num=dag_map_x+1;
    int i;
    int export_id;
    int export_x=0;
    for(i=0;i<node_num;i++){
        father_in[i]=father_num[i];
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
                father_in[export_id]=-1;
                if(dag_map[export_id].op==op_leaf){
                    break;
                }else{
                    if(dag_map[export_id].lchild_id!=-1){
                        father_in[dag_map[export_id].lchild_id]--;
                    }
                    if(dag_map[export_id].rchild_id!=-1){
                        father_in[dag_map[export_id].rchild_id]--;
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
//    for(i=0;i<=dag_map_x;i++){
//        printf("%d %s\n",i,node_name[i]);
//    }
    finish_export();
}
void exec_dag(int funct_num){
    int i,j,k;
    struct quat_struct *find_begin;
    struct quat_struct *p;
    int con_num;
    for(i=0;i<funct_num;i++){
        current_funct=i;
        for(j=0;j<funct_stru[i].block_num;j++){
            current_block=j;
            find_begin=funct_stru[i].blocks[j].block_begin;
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

int is_available_var(char name[]){
    if(name[0]=='&'){
        return 1;
    }
    if(name[0]=='@'){
        return 1;
    }
    return 0;
}
int n_in_set(char ele[],char set[][VAR_LEN],int set_len){//传递下标
    int i;
    int r;
    for(i=0;i<set_len;i++){
        r=strcmp(ele,set[i]);
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
int merge_set(char dest_set[][VAR_LEN],char set1[][VAR_LEN],int set1_len,char set2[][VAR_LEN],int set2_len){
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
int sub_set(char dest_set[][VAR_LEN],char set1[][VAR_LEN],int set1_len,char set2[][VAR_LEN],int set2_len){
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
void insert_ele(char ele[],int ele_x,char set[][VAR_LEN],int set_len){
    int i=set_len;
    while(i>ele_x){
        strcpy(set[i],set[i-1]);
        i--;
    }
    strcpy(set[ele_x],ele);
}

void add_ele(int use_def,char name[],struct block_struct *p_block){//use:0,def:1
    int ndefed;
    int nused;
    if(!is_available_var(name)){
        return;
    }
    if(use_def==0){
        ndefed=n_in_set(name,p_block->def,p_block->def_len);
        if(ndefed!=-1){
            nused=n_in_set(name,p_block->use,p_block->use_len);
            if(nused!=-1){
                insert_ele(name,nused,p_block->use,p_block->use_len);
                p_block->use_len++;
            }
        }
    }else{
        nused=n_in_set(name,p_block->use,p_block->use_len);
        if(nused!=-1){
            ndefed=n_in_set(name,p_block->def,p_block->def_len);
            if(ndefed!=-1){
                insert_ele(name,ndefed,p_block->def,p_block->def_len);
                p_block->def_len++;
            }
        }
    }
}
int eql_set(char set1[][VAR_LEN],int set1_len,char set2[][VAR_LEN],int set2_len){
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
void get_use_def(struct block_struct *p_block){
    struct quat_struct *p;
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
struct block_struct* find_block(char name[],int funcx){
    int i;
    for(i=0;i<funct_stru[funcx].block_num;i++){
        if(funct_stru[funcx].blocks[i].block_begin->op==op_set_label){
            if(strcmp(funct_stru[funcx].blocks[i].block_begin->dest,name)==0){
                return &(funct_stru[funcx].blocks[i]);
            }
        }
    }
    return NULL;
}
void set_suffix(int funcx,int blocki){
    struct quat_struct* p;
    struct block_struct* p_block;
    funct_stru[funcx].blocks[blocki].suffix_num=0;
    p=funct_stru[funcx].blocks[blocki].block_begin;
    while(p->next!=NULL){
        p=p->next;
    }
    switch(p->op){
    case op_jump:
        p_block=find_block(p->dest,funcx);
        funct_stru[funcx].blocks[blocki].suffix[funct_stru[funcx].blocks[blocki].suffix_num++]=p_block;
        break;
    case op_beq:
    case op_bne:
    case op_bge:
    case op_bgt:
    case op_ble:
    case op_bls:
        p_block=find_block(p->dest,funcx);
        funct_stru[funcx].blocks[blocki].suffix[funct_stru[funcx].blocks[blocki].suffix_num++]=p_block;
        p_block=&(funct_stru[funcx].blocks[blocki+1]);
        funct_stru[funcx].blocks[blocki].suffix[funct_stru[funcx].blocks[blocki].suffix_num++]=p_block;
        break;
    case op_ret_value:
    case op_ret_void:
        break;
    default:
        p_block=&(funct_stru[funcx].blocks[blocki+1]);
        funct_stru[funcx].blocks[blocki].suffix[funct_stru[funcx].blocks[blocki].suffix_num++]=p_block;
        break;
    }
}
char all_out[VAR_NUM][VAR_LEN];
int all_out_len=0;
int conflict_matrix[VAR_NUM][VAR_NUM];//对每个函数进行
int cal_in(char d_set[][VAR_LEN],struct block_struct *p_block){
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
void cal_in_out(int funcx){
    int haschanged=0;
    int i,j;
    struct block_struct *p_block;
    char new_in[VAR_NUM][VAR_LEN];
    int new_in_len=0;
    for(i=funct_stru[funcx].block_num-1;i>=0;i--){
         funct_stru[funcx].blocks[i].in_len=0;
         funct_stru[funcx].blocks[i].out_len=0;
    }
    while(1){
        for(i=funct_stru[funcx].block_num-2;i>=0;i--){
            p_block=&(funct_stru[funcx].blocks[i]);
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
void convert2x(char vars[][VAR_LEN],int varx[],int v_len){
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
void build_conflit_matrix(int funcx){
    int i,j,k;
    int var_xs[VAR_NUM];
    char block_var[VAR_NUM][VAR_LEN];
    int bvar_len;
    all_out_len=0;
    for(i=0;i<funct_stru[funcx].block_num;i++){//合并out，得到跨基本块的变量
        all_out_len=merge_set(all_out,all_out,all_out_len,funct_stru[funcx].blocks[i].out,funct_stru[funcx].blocks[i].out_len);
    }
    for(j=0;j<all_out_len;j++){//变量之间初始化为无冲突
        for(k=0;k<all_out_len;k++){
            conflict_matrix[j][k]=0;
        }
    }
    for(i=0;i<funct_stru[funcx].block_num;i++){//同处一个基本跨in或out的变量之间存在冲突,是否有变量存在于in中但不存在于out中
        bvar_len=merge_set(block_var,funct_stru[funcx].blocks[i].in,funct_stru[funcx].blocks[i].in_len,funct_stru[funcx].blocks[i].out,funct_stru[funcx].blocks[i].out_len);
        convert2x(block_var,var_xs,bvar_len);//将变量名转化为其中out中的下表
        for(j=0;j<bvar_len;j++){//标识有冲突变量
            for(k=j+1;k<bvar_len;k++){
                conflict_matrix[var_xs[j]][var_xs[k]]=1;
                conflict_matrix[var_xs[k]][var_xs[j]]=1;
            }
        }
    }
}
void alloct_reg(int funcx){
    int conflict_num[all_out_len];
    int regs_used[GLOBAL_REG_NUM]={0,0,0};
    enum regs distributed_reg;
    int stack[VAR_NUM];
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
            stack[stack_x++]=i;
            for(j=i+1;j<all_out_len;j++){
                if(conflict_matrix[i][j]==1){
                    conflict_num[j]--;
                }
            }
        }
    }
    while(stack_x>0){
        pop_x=stack[--stack_x];
        distributed_reg=no_reg;
        for(i=0;i<funct_stru[funcx].var_reg_len;i++){//找到不与该变量有连接边的已分配的寄存器的变量对应的寄存器
            if(conflict_matrix[pop_x][funct_stru[funcx].var_reg[i].varx]==0){
                distributed_reg=funct_stru[funcx].var_reg[i].reg;//将其分配给该变量
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
        funct_stru[funcx].var_reg[funct_stru[funcx].var_reg_len].reg=distributed_reg;
        funct_stru[funcx].var_reg[funct_stru[funcx].var_reg_len].varx=pop_x;
        strcpy(funct_stru[funcx].var_reg[funct_stru[funcx].var_reg_len].var,all_out[pop_x]);
        funct_stru[funcx].var_reg_len++;
    }
}

int main()
{
    struct quat_struct *head,*code,*p,*data;
    struct src_quat src[600];
    int i,j,k;
    int quat_num;
    int funct_num;
    head=NULL;
    p=NULL;
    quat_num=get_squat(src);
    head=convert_quat_table(src,quat_num);
    p=head;
    i=0;
    funct_num=find_entries(quat_num,src);
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
    divide_blocks(code,src,funct_num);
    exec_dag(funct_num);
    for(i=0;i<funct_num;i++){
        for(j=0;j<funct_stru[i].block_num;j++){
            get_use_def(&(funct_stru[i].blocks[j]));
            set_suffix(i,j);
            printf("\nfunct %d block %d \nuse: ",i,j);
            for(k=0;k<funct_stru[i].blocks[j].use_len;k++){
                printf("%s ",funct_stru[i].blocks[j].use[k]);
            }
            printf("\ndef: ");
            for(k=0;k<funct_stru[i].blocks[j].def_len;k++){
                printf("%s ",funct_stru[i].blocks[j].def[k]);
            }
        }
    }
    for(i=0;i<funct_num;i++){
        cal_in_out(i);
        for(j=0;j<funct_stru[i].block_num;j++){
            printf("\nfunct %d block %d \nin: ",i,j);
            for(k=0;k<funct_stru[i].blocks[j].in_len;k++){
                printf("%s ",funct_stru[i].blocks[j].in[k]);
            }
            printf("\nout: ");
            for(k=0;k<funct_stru[i].blocks[j].out_len;k++){
                printf("%s ",funct_stru[i].blocks[j].out[k]);
            }
        }
    }
    for(i=0;i<funct_num;i++){
        build_conflit_matrix(i);
        alloct_reg(i);
        printf("\nfunct %d",i);
        for(j=0;j<funct_stru[i].var_reg_len;j++){
            printf("\n%s,%d",funct_stru[i].var_reg[j].var,funct_stru[i].var_reg[j].reg);
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
//        for(j=0;j<funct_stru[i].block_num;j++){
//            p=funct_stru[i].blocks[j].block_begin;
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
    return 0;
}
