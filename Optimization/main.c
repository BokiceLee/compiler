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
int entryment[F_NUM][E_NUM];
int entry_num[F_NUM];
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
    struct quat_struct* block_end;
    struct block_struct* suffix[2];
    int suffix_num;
};
struct funct_struct{
    struct block_struct blocks[B_NUM];
    int block_num;
    struct quat_struct* funct_begin;
    struct quat_struct* funct_end;
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
    struct quat_struct* quats[N_NUM];
    int id_num;
    int has_init;
    int replacable;
    char replace_name[VAR_LEN];
};
struct funct_struct funct_stru[F_NUM];
struct quat_struct *seq_begin,*seq_end;
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
struct quat_struct *export_head,*export_tail;

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
        while(p!=funct_stru[funcx].blocks[i].block_end){
            if(p->op>=op_beq&&p->op<=op_jump){
                if(strcmp(p->dest,label)==0){
                    return 1;
                }
            }
            p=p->next;
        }
        if(p->op>=op_beq&&p->op<=op_jump){
            if(strcmp(p->dest,label)==0){
                return 1;
            }
        }
    }
    return 0;
}
void divide_blocks(struct quat_struct* head,struct src_quat src[],int funct_num){
    int i,valid,canjump;
    struct quat_struct *p;
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
                        p=get_quat_p(entryment[funcx][i+1]-1,p);
                        continue;
                    }
                }
                p=get_quat_p(entryment[funcx][i],p);
                funct_stru[funcx].blocks[funct_stru[funcx].block_num].block_begin=p;
                p=get_quat_p(entryment[funcx][i+1]-1,p);
                funct_stru[funcx].blocks[funct_stru[funcx].block_num].block_end=p;
                if(p->op==op_ret_value||p->op==op_ret_void){
                    valid=0;//ret语句后的语句无效
                }//暂时不确定基本块之间的关系，因为基本块未确定
                funct_stru[funcx].block_num++;
            }
            p=p->next;
            if(p->op==op_efunc||p->op==op_emain){
                funct_stru[funcx].funct_end=p;//函数结束语句
                p=p->next;
                funcx++;
            }else{
                printf("error:326,最后一个入口语句应是efunc or emain");
            }
        }else{
            p=p->next;
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
int find_continues_exp(struct quat_struct* find_begin,struct quat_struct* find_end){
    struct quat_struct* p;
    int con_num=0;
    p=find_begin;
    while(p->quat_id<=find_end->quat_id){
        if(is_available_op(p->op)==1){
            seq_begin=p;
            break;
        }else{
            p=p->next;
        }
    }
    while(p->quat_id<=find_end->quat_id){
        if(is_available_op(p->op)==1){
            seq_end=p;
            p=p->next;
            con_num++;
        }else{
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
        dag_table[dag_table_x].replacable=0;
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
            dag_table[i].quats[dag_table[i].id_num]=quat;
            dag_table[i].id_num++;
            return;
        }
    }
    if(i==-1){
        dag_table_x++;
        strcpy(dag_table[dag_table_x].name,dest_name);
        dag_table[dag_table_x].id_num=1;
        dag_table[dag_table_x].node_id[0]=dest_id;
        dag_table[dag_table_x].quats[0]=quat;
        dag_table[dag_table_x].replacable=0;
        dag_table[dag_table_x].has_init=0;
    }

}
void build_dag(int con_num){
    int i;
    struct quat_struct* p;
    int dest_id,src1_id,src2_id;
    p=seq_begin;
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
    p=seq_end->next;
    while(p!=funct_stru[current_funct].funct_end){
        if(strcmp(var_name,p->src1)==1){
            return 1;
        }
        if(strcmp(var_name,p->src2)==1){
            return 1;
        }
        if(strcmp(var_name,p->dest)==1){
            return 1;
        }
    }
    return 0;
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
    while(p!=seq_end){
        tmp=p;
        p=p->next;
        free(tmp);
    }
    export_tail->next=p->next;
    free(p);
}
void replace_child(char replaced[],char toreplace[]){
    int i;
    if(toreplace[0]!='&'){//非临时变量不用替换
        strcpy(replaced,toreplace);
        return;
    }else{
        for(i=0;i<=dag_table_x;i++){
            if(0==strcmp(toreplace,dag_table[i].name)){
                if(dag_table[i].replacable==1){
                    strcpy(replaced,dag_table[i].replace_name);
                    return;
                }
            }
        }
    }
}
void export_leaf_node(int node_id){
    int i,j;
    int n_has_init_var[dag_table_x];
    int n_x=0;
    char init_name[VAR_LEN];
    if(dag_map[node_id].is_assigned==1){//如果该叶子节点被赋值过
            for(i=0;i<=dag_table_x;i++){
                for(j=0;j<dag_table[i].id_num;j++){
                    if(dag_table[i].node_id[j]==node_id){
                        if(dag_table[i].has_init==1){//找到初值叶节点
                            strcpy(init_name,dag_table[i].name);
                            //strcpy(node_name[node_id],dag_table[i].name);
                        }else{
                            n_has_init_var[n_x++]=i;//非初值叶节点
                        }
                    }
                }
            }
            for(i=0;i<n_x;i++){//非初值叶节点的值都等于初值叶节点
                re_gen_quat(op_mov,dag_table[n_has_init_var[i]].name,init_name,"");
            }//不会出现临时变量等于一个值情况？
    }
//    }else{//叶节点未被赋值
//        for(i=0;i<=dag_table_x;i++){
//            for(j=0;j<dag_table[i].id_num;i++){
//                if(dag_table[i].node_id[j]==node_id){//将node_id与变量名对应
//                    strcpy(node_name[node_id],dag_table[i].name);
//                }
//            }
//        }
//    }
}

void export_midside_node(int node_id){
    int i,j,m,k,t;
    int has_expli_expo=0;
    char first_expo_name[VAR_LEN];
    char rep_src1[VAR_LEN];
    char rep_src2[VAR_LEN];
    int n_tmp_var[N_NUM];
    int n_exported_tmp_var[N_NUM];
    int n_t_x=0;
    int n_exported_t_x=0;
    int expli_export_tmp_id=-1;
    int replacable,has_next_node,next_node_id;
    for(i=0;i<=dag_table_x;i++){
        for(j=0;j<dag_table[i].id_num;j++){
            if(dag_table[i].node_id[j]==node_id){
                if(dag_table[i].name[0]!='&'){//非临时变量全部显示导出
                    n_tmp_var[n_t_x++]=i;
                    if(has_expli_expo==0){
                        strcpy(first_expo_name,dag_table[i].name);
                        has_expli_expo=1;//第一个导出的变量
                        //替换
                        replace_child(rep_src1,dag_table[i].quats[j]->src1);
                        replace_child(rep_src2,dag_table[i].quats[j]->src2);//临时变量不会被修改，因此只需要考虑一个就可以
                        re_gen_quat(dag_table[i].quats[j]->op,dag_table[i].name,rep_src1,rep_src2);
                    }else{//剩下的变量导出为赋值语句
                        re_gen_quat(op_mov,dag_table[i].name,first_expo_name,"");
                    }
                }else if(is_used_later(dag_table[i].name)==1){
                    expli_export_tmp_id=i;//由于后继使用而显式导出的临时变量
                    if(has_expli_expo==0){
                        strcpy(first_expo_name,dag_table[i].name);
                        has_expli_expo=1;//第一个导出的变量
                        replace_child(rep_src1,dag_table[i].quats[j]->src1);
                        replace_child(rep_src2,dag_table[i].quats[j]->src2);
                        re_gen_quat(dag_table[i].quats[j]->op,dag_table[i].name,rep_src1,rep_src2);
                    }else{//剩下的变量导出为赋值语句
                        re_gen_quat(op_mov,dag_table[i].name,first_expo_name,"");
                    }
                }else{
                    n_exported_tmp_var[n_exported_t_x++]=i;
                }
            }
        }
    }
    if(expli_export_tmp_id!=-1){//有显式导出的临时变量，则同id的临时变量都可以替换成该变量
        for(i=0;i<n_exported_t_x;i++){
            dag_table[i].replacable=1;
            strcpy(dag_table[n_exported_tmp_var[i]].replace_name,dag_table[expli_export_tmp_id].name);
        }
    }else{//否则，判断是否可替换
        for(i=0;i<n_exported_t_x;i++){//每个临时变量
            replacable=1;
            for(j=0;j<n_t_x;i++){//每个同id非临时变量
                has_next_node=0;
                for(t=0;t<dag_table[j].id_num-1;t++){//的下一个编号
                    if(dag_table[j].node_id[t]==node_id){
                        has_next_node=1;
                        break;
                    }
                }
                if(has_next_node==0){
                    continue;
                }
                next_node_id=dag_table[j].node_id[t+1];//下一个编号
                for(k=0;k<=dag_table_x;k++){//遍历所有变量
                    for(m=0;m<dag_table[k].id_num;m++){//的所有编号
                        if(dag_table[k].node_id[m]> next_node_id){
                            if(strcmp(dag_table[k].quats[m]->src1,dag_table[n_exported_tmp_var[i]].name)==0){
                                replacable=0;
                            }else if(strcmp(dag_table[k].quats[m]->src2,dag_table[n_exported_tmp_var[i]].name)==0){
                                replacable=0;
                            }
                        }
                    }
                }
                if(replacable==1){
                    dag_table[n_exported_tmp_var[i]].replacable=1;
                    strcpy(dag_table[n_exported_tmp_var[i]].replace_name,dag_table[n_tmp_var[j]].name);
                    break;
                }
            }
            if(replacable==0){
                if(has_expli_expo==0){
                    strcpy(first_expo_name,dag_table[i].name);
                    has_expli_expo=1;//第一个导出的变量
                    replace_child(rep_src1,dag_table[i].quats[j]->src1);
                    replace_child(rep_src2,dag_table[i].quats[j]->src2);
                    re_gen_quat(dag_table[i].quats[j]->op,dag_table[i].name,rep_src1,rep_src2);
                }else{//剩下的变量导出为赋值语句
                    re_gen_quat(op_mov,dag_table[i].name,first_expo_name,"");
                }
            }
        }
    }
}
void export_node(int node_id){
    int i;
    char printed_name[VAR_LEN];
    if(dag_map[node_id].op==op_printc || dag_map[node_id].op==op_printc || dag_map[node_id].op==op_printc){
        for(i=0;i<=dag_table_x;i++){//print也要插入到dag_table中
            if(dag_table[i].node_id[0]==node_id){
                replace_child(printed_name,dag_table[i].name);
                re_gen_quat(dag_map[node_id].op,printed_name,"","");
                break;
            }

        }
    }else if(dag_map[node_id].op!=op_leaf){//非叶节点
        export_midside_node(node_id);
    }else{//叶节点
        export_leaf_node(node_id);
    }
}
void export_dag(){
    int node_num=dag_map_x+1;
    int i;
    int export_id;
    int export_x=0;
    for(i=0;i<node_num;i++){
        father_in[i]=father_num[i];
    }
    while((export_id=get_max_node())>=0){
        export_queue[export_x++]=export_id;
        if(dag_map[export_id].op==op_leaf){
            continue;
        }
        while(1){
            if(is_exportable(dag_map[export_id].rchild_id)){
                export_id=dag_map[export_id].rchild_id;
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
    finish_export();
}
void exec_dag(int funct_num){
    int i,j,k;
//    struct quat_struct *seq_begin,*seq_end;
    struct quat_struct *find_begin,*find_end;
    struct quat_struct *p,*p_end;
    int con_num;
    for(i=0;i<funct_num;i++){
        current_funct=i;
        for(j=0;j<funct_stru[i].block_num;j++){
            find_begin=funct_stru[i].blocks[j].block_begin;
            find_end=funct_stru[i].blocks[j].block_end;
            while(find_begin->quat_id<=find_end->quat_id){//判断是否已经查过头了，find_end或者find_end后
                con_num=find_continues_exp(find_begin,find_end);
                p=seq_begin;
                for(k=0;k<con_num;k++){
                    printf("%d %s %s %s %s\n",p->quat_id,op_name[p->op],p->dest,p->src1,p->src2);
                    p=p->next;
                }
                printf("\n");
                p_end=seq_end->next;
                if(con_num>1){
                    build_dag(con_num);
                    export_dag();
                }
                p=seq_begin;
                printf("\n");
                while(p!=p_end){
                    printf("%s %s %s %s\n",op_name[p->op],p->dest,p->src1,p->src2);
                    p=p->next;
                }
                if(con_num==0){
                    find_begin=find_end->next;
                }else{
                    find_begin=p_end;
                }
            }
        }
    }
}
int main()
{
    struct quat_struct *head,*p;
    struct src_quat src[600];
    int i,j;
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
    divide_blocks(head,src,funct_num);
    exec_dag(funct_num);
    for(i=0;i<0;i++){
        printf("\nfunct %d----------------\n",i);
        for(j=0;j<funct_stru[i].block_num;j++){
            printf("block %d------\n",j);
            p=funct_stru[i].blocks[j].block_begin;
            while(p!=funct_stru[i].blocks[j].block_end){
                printf("%d %s %s %s %s\n",p->quat_id,op_name[p->op],p->dest,p->src1,p->src2);
                p=p->next;
            }
            printf("%d %s %s %s %s\n",p->quat_id,op_name[p->op],p->dest,p->src1,p->src2);
        }
        p=funct_stru[i].funct_end;
        printf("%d %s %s %s %s\n",p->quat_id,op_name[p->op],p->dest,p->src1,p->src2);
    }
    return 0;
}
