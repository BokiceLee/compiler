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
enum regs{//�Ĵ���0-2ȫ�ּĴ�����3-5��ʱ�Ĵ���
    ebx,edi,esi,eax,ecx,edx,no_reg
};
struct opt_quat_struct{//�Ż�ʱ���õ���Ԫʽ�ṹ
    int quat_id;
    enum op_code op;
    char dest[VAR_LEN];
    char src1[VAR_LEN];
    char src2[VAR_LEN];
    struct opt_quat_struct* next;
};
struct var_reg_struct{//����-�Ĵ�����Ӧ��
    char var[VAR_LEN];
    int varx;//������all_out�е��±�
    enum regs reg;
};
struct block_struct{//������ṹ
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
struct funct_struct{//�����ṹ
    struct block_struct blocks[B_NUM];
    int block_num;
    struct opt_quat_struct* funct_begin;
    //struct quat_struct* funct_end;Ӧ���ò���
    struct var_reg_struct var_reg[VAR_NUM];
    int var_reg_len;
};
struct dag_map_struct{//dagͼ�ṹ
    int node_id;
    int lchild_id;
    int rchild_id;
    enum op_code op;
    int is_assigned;
};
struct dag_table_struct{//dag��ṹ
    char name[VAR_LEN];
    int node_id[N_NUM];
    int id_num;
    int has_init;
};

int entryment[F_NUM][E_NUM];//ÿ��������������
int entry_num[F_NUM];//ÿ���������������
struct opt_quat_struct *data;
struct funct_struct functs[F_NUM];//����
int g_funct_num;
struct opt_quat_struct *seq_begin,*after_seq;//�����ֲ������ӱ��ʽ��������Ԫʽ�ĵ�һ��ʽ�Ӻ���һ��ʽ��
struct dag_map_struct dag_map[DAG_NUM];//dagͼ
struct dag_table_struct dag_table[DAG_NUM];//dag��
int father_num[DAG_NUM];//ÿ��dagͼ�ڵ�ĸ��ڵ����
int export_queue[DAG_NUM];//����dagͼ�ڵ�����
char node_name[DAG_NUM][VAR_LEN];//ÿ��dagͼ�ڵ������
int dag_table_x=-1;//dag���±�
int dag_map_x=-1;//dagͼ�±�
int export_x=0;//���������±�
int rep_name_x=0;//��������node_name
int current_funct;//��ǰ����
int current_block;//��ǰ��
struct opt_quat_struct *export_head,*export_tail;//�����ĵ�һ����Ԫʽ�����һ����Ԫʽ
char all_out[VAR_NUM][VAR_LEN];//��������������out�Ĳ�
int all_out_len=0;
int conflict_matrix[VAR_NUM][VAR_NUM];//��ͻ����

int find_entries(struct quat_struct quat_table[],int quat_num){//�ҵ����е������䣬���غ�������
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
            funct_begin=i;//�������
            entryment[funcx][en_index++]=i;
            i++;
            while(quat_table[i].op!=op_efunc&&quat_table[i].op!=op_emain){
                if(quat_table[i].op<=16 &&quat_table[i].op>=10){//��ָ֧��
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
                    //Ŀ��
                    entryment[funcx][en_index++]=i+1;
                    //���
                }else if(quat_table[i].op==op_ret_value||quat_table[i].op==op_ret_void){//����ָ����
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
    return funcx;//���غ�������
}
int sort_entryment(int ent[],int ent_num){//����������
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

//ת����ԪʽΪ����洢
void mov_struct(struct opt_quat_struct* next_quat,struct quat_struct src){//��������
    strcpy(next_quat->src1,src.src1);
    strcpy(next_quat->src2,src.src2);
    strcpy(next_quat->dest,src.dest);
    next_quat->op=src.op;
    next_quat->next=NULL;
}
struct opt_quat_struct* convert_quat_table(struct quat_struct src[],int src_len){//ת��������
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
//���ֻ�����
struct opt_quat_struct* get_quat_p(int quat_id,struct opt_quat_struct* p){//������Ԫʽ��Ż�ȡ���Ӧ��ָ��
    while(p->next!=NULL){
        if(p->quat_id==quat_id){
            return p;
        }
        p=p->next;
    }
    return NULL;
}
int jumpbefore(int funcx,char label[]){//�����Ƿ������ת����ǰlabel����ת��䣬���򷵻�1�����򷵻�0
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
void divide_blocks(struct opt_quat_struct* code_head,struct quat_struct src[]){//����������ӵ�һ�б�ת�������νṹ
    int i,valid,canjump;
    struct opt_quat_struct* p;
    struct opt_quat_struct* p_next;
    int funcx=0;
    p=code_head;
    while(p!=NULL){
        if(p->op==op_func||p->op==op_main){
            functs[funcx].funct_begin=p;//������ʼ���
            valid=1;
            //ÿ����������д���
            for(i=0;i<entry_num[funcx]-1;i++){//ÿ��������
                if(valid==0){//�ж��Ƿ�����Ч����е��������ת�������
                    if(src[entryment[funcx][i]].op==op_set_label){
                        //��ǰ�������
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
                    valid=0;//ret����������Ч
                }//��ʱ��ȷ��������֮��Ĺ�ϵ����Ϊ������δȷ��
                functs[funcx].block_num++;

                p_next=p->next;
                p->next=NULL;//����������һ�����ָ���
                p=p_next;
            }
            if(p->op==op_efunc||p->op==op_emain){//���һ��������Ϊ�����������
                functs[funcx].blocks[functs[funcx].block_num].block_begin=p;
                functs[funcx].block_num++;
                funcx++;

                p_next=p->next;
                p->next=NULL;
                p=p_next;
            }else{
                printf("error:326,���һ��������Ӧ��efunc or emain\n");
            }
        }else{
            p=p->next;
            printf("error:338,��������֮���в���ʶ�����\n");
        }
    }
}
//�����������Ż���Ԫʽ
int is_available_op(int op){//�ж�op�����Ƿ���Ż����Ƿ���1�����򷵻�0
    switch(op){
        case op_mov://ע�⣡
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
int find_continues_exp(struct opt_quat_struct* find_begin){//�����������Ż���Ԫʽ
    struct opt_quat_struct *p,*p_last;
    int con_num=0;
    seq_begin=NULL;
    after_seq=NULL;//�ÿ�
    for(p=find_begin;p!=NULL;p=p->next){//���ҵ�һ������ʽ
        if(is_available_op(p->op)==1){
            seq_begin=p;
            break;
        }
    }
    while(p!=NULL){//������������ʽ
        if(is_available_op(p->op)==1){
            p_last=p;
            p=p->next;
            con_num++;
        }else{
            p_last->next=NULL;//���һ������ʽ����һ������ÿ�
            after_seq=p;//����ʽ֮��ĵ�һ�����
            break;
        }
    }
    return con_num;
}
//����dagͼ��dag��
int get_child_id(char var_name[]){//��ȡdagͼ�ڵ�id
    int i;
    for(i=dag_table_x;i>=0;i--){
        if(strcmp(var_name,dag_table[i].name)==0){
            return dag_table[i].node_id[dag_table[i].id_num-1];
        }
    }
    if(i==-1){
        dag_map_x++;
        dag_map[dag_map_x].node_id=dag_map_x;//��dagͼ�в����½ڵ㣬����ýڵ�id
        dag_map[dag_map_x].op=op_leaf;//��ʾ�ýڵ�ΪҶ�ڵ�
        dag_map[dag_map_x].is_assigned=0;//
        dag_map[dag_map_x].lchild_id=-1;
        dag_map[dag_map_x].rchild_id=-1;
        father_num[dag_map_x]=0;

        dag_table_x++;
        dag_table[dag_table_x].id_num=1;//��dag���в����¶�Ӧ��ϵ
        dag_table[dag_table_x].node_id[0]=dag_map_x;
        strcpy(dag_table[dag_table_x].name,var_name);
        dag_table[dag_table_x].has_init=1;
    }
    return dag_map_x;
}
int get_parent_id(int op,int src1_id,int src2_id){//��ȡdagͼ�ڵ�id
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
void update_dag_table(struct opt_quat_struct* quat,int dest_id,char dest_name[]){//����dag��
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
void build_dag(int con_num){//����dagͼ��dag��
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
            dag_map_x++;//ֱ�Ӳ����µĽڵ�
            dag_map[dag_map_x].op=p->op;
            dag_map[dag_map_x].node_id=dag_map_x;
            dag_map[dag_map_x].lchild_id=src1_id;
            dag_map[dag_map_x].rchild_id=-1;
            father_num[dag_map_x]=0;
            father_num[src1_id]++;
        }else{
            src1_id=get_child_id(p->src1);
            src2_id=get_child_id(p->src2);
            dest_id=get_parent_id(p->op,src1_id,src2_id);//���¸��ڵ������ʲô���ڵ�
            father_num[src1_id]++;
            father_num[src2_id]++;
            update_dag_table(p,dest_id,p->dest);
        }
        p=p->next;
    }
}
//���µ����м����
int is_exportable(int node_id){//�ж�dagͼ�ڵ��Ƿ�ɵ���
    return father_num[node_id]==0;
}
int get_max_node(){//�ҵ��ɵ������ڵ�
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
int is_used_later(char var_name[]){//�ж���ʱ�����Ƿ��ں��ʹ��
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
void set_node_name(int node_id,char init_name[]){//���ýڵ���
    if(strcmp(init_name,"")==0){
        sprintf(node_name[node_id],"&r%03d",rep_name_x);
        rep_name_x++;
    }else{
        sprintf(node_name[node_id],"%s",init_name);
    }
}
void re_gen_quat(int op,char dest[],char src1[],char src2[]){//����������Ԫʽ
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
void finish_export(){//������β����
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
int get_var_on_node(int var_on_node_x[],int node_id){//��ȡͼ�ڵ��ϵı����±�
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
        }else{//�ҵ���ֵҶ�ڵ�
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
void export_node(int node_id){//�����ڵ�
    int i;
    int var_on_node_x[dag_table_x];
    int var_num=0;
    if(dag_map[node_id].op==op_printc || dag_map[node_id].op==op_printi || dag_map[node_id].op==op_prints){
        re_gen_quat(dag_map[node_id].op,node_name[dag_map[node_id].lchild_id],"","");
    }else{
        var_num=get_var_on_node(var_on_node_x,node_id);//ͬʱ����node_name
        for(i=0;i<var_num;i++){
            re_gen_quat(op_mov,dag_table[var_on_node_x[i]].name,node_name[node_id],"");
        }
    }
}
void export_dag(){//��dagͼ���µ����м����
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
//dagͼ���������ӱ��ʽ
void exec_dag(int funct_num){//dag���ƺ���
    int i,j,k;
    struct opt_quat_struct *find_begin;
    struct opt_quat_struct *p;
    int con_num;
    for(i=0;i<funct_num;i++){
        current_funct=i;
        for(j=0;j<functs[i].block_num;j++){
            current_block=j;
            find_begin=functs[i].blocks[j].block_begin;
            while(find_begin!=NULL){//�����û�����
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
                if(con_num==0){//δ�ҵ��������㣬������һ��������
                    break;
                }else{
                    find_begin=after_seq;
                }
            }
        }
    }
}
//��������
int n_in_set(char ele[],char eles[][VAR_LEN],int set_len){//�ж�Ԫ���Ƿ��ڼ�����
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
int merge_set(char dest_set[][VAR_LEN],char set1[][VAR_LEN],int set1_len,char set2[][VAR_LEN],int set2_len){//���ϲ�����
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
int sub_set(char dest_set[][VAR_LEN],char set1[][VAR_LEN],int set1_len,char set2[][VAR_LEN],int set2_len){//���ϲ�����
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
int insert_ele(char ele[],int ele_x,char eles[][VAR_LEN],int set_len){//�������Ԫ��
    int i=set_len;
    while(i>ele_x){
        strcpy(eles[i],eles[i-1]);
        i--;
    }
    strcpy(eles[ele_x],ele);
    return set_len++;
}
int eql_set(char set1[][VAR_LEN],int set1_len,char set2[][VAR_LEN],int set2_len){//�жϽ���Ƿ����
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
//����������use,def,in,out����
int is_available_var(char var_name[]){//�ж��Ƿ��ǿɷ���ȫ�ּĴ����ı���
    if(var_name[0]=='&'){
        return 1;
    }
    if(var_name[0]=='@'){
        return 1;
    }
    return 0;
}
void add_ele(int use_def,char var_name[],struct block_struct *p_block){//����������ӵ��������use��def�����У�use:0,def:1
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
void get_use_def(struct block_struct *p_block){//��ÿ���������use��def��
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
struct block_struct* find_block(char label_name[],int funcx){//�ҵ���label��Ӧ�Ļ�����
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
void get_suffix(int funcx,int blocki){//Ϊÿ���������ҵ���Ӧ�ĺ�׺
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
int cal_in(char d_set[][VAR_LEN],struct block_struct *p_block){//���ÿ���������in���ϣ�������out
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
void cal_in_out(int funcx){//�ж�in�Ƿ�ı䣬������£��������
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
//ȫ�ּĴ�������
void convert2x(char vars[][VAR_LEN],int varx[],int v_len){//������ת��Ϊ����all_out�е��±�
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
void build_conflit_matrix(int funcx){//������ͻ����
    int i,j,k;
    int var_xs[VAR_NUM];
    char block_var[VAR_NUM][VAR_LEN];
    int bvar_len;
    all_out_len=0;
    for(i=0;i<functs[funcx].block_num;i++){//�ϲ�out���õ��������ı���
        all_out_len=merge_set(all_out,all_out,all_out_len,functs[funcx].blocks[i].out,functs[funcx].blocks[i].out_len);
    }
    for(j=0;j<all_out_len;j++){//����֮���ʼ��Ϊ�޳�ͻ
        for(k=0;k<all_out_len;k++){
            conflict_matrix[j][k]=0;
        }
    }
    for(i=0;i<functs[funcx].block_num;i++){//ͬ��һ��������in��out�ı���֮����ڳ�ͻ,�Ƿ��б���������in�е���������out��
        bvar_len=merge_set(block_var,functs[funcx].blocks[i].in,functs[funcx].blocks[i].in_len,functs[funcx].blocks[i].out,functs[funcx].blocks[i].out_len);
        convert2x(block_var,var_xs,bvar_len);//��������ת��Ϊ����out�е��±�
        for(j=0;j<bvar_len;j++){//��ʶ�г�ͻ����
            for(k=j+1;k<bvar_len;k++){
                conflict_matrix[var_xs[j]][var_xs[k]]=1;
                conflict_matrix[var_xs[k]][var_xs[j]]=1;
            }
        }
    }
}
void alloct_reg(int funcx){//���ճ�ͻ�������ͼ��ɫ�㷨���мĴ�������
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
    }//���ÿ���Ĵ����ĳ�ͻ��
    for(i=0;i<all_out_len;i++){
        if(conflict_num[i]>=GLOBAL_REG_NUM){//�����ͻ�����ڵ��ڼĴ����������Ƴ�
            //��ÿ�����������ı�������ͻ����һ,���³�ͻ��
            for(j=i+1;j<all_out_len;j++){
                if(conflict_matrix[i][j]==1){
                    conflict_num[j]--;
                }
            }
        }else{//������ջ
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
        for(i=0;i<functs[funcx].var_reg_len;i++){//�ҵ�����ñ��������ӱߵ��ѷ���ļĴ����ı�����Ӧ�ļĴ���
            if(conflict_matrix[pop_x][functs[funcx].var_reg[i].varx]==0){
                distributed_reg=functs[funcx].var_reg[i].reg;//���������ñ���
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
            printf("998:�޷�����\n");
        }
        functs[funcx].var_reg[functs[funcx].var_reg_len].reg=distributed_reg;
        functs[funcx].var_reg[functs[funcx].var_reg_len].varx=pop_x;
        strcpy(functs[funcx].var_reg[functs[funcx].var_reg_len].var,all_out[pop_x]);
        functs[funcx].var_reg_len++;
    }
}

//�Ż��ܿ��Ƴ���
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
    //��������Ϣ��ʼת��Ϊ����洢
    p=head;
    if(p->op!=op_func&&p->op!=op_main){//��data��code����
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
