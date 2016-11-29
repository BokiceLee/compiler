#ifndef GRAMMARANALYSIS_H_INCLUDED
#define GRAMMARANALYSIS_H_INCLUDED
void program();

void constdeclaraction(int fsys[],int fsys_len,int is_global);
void constdefinition(int fsys[],int fsys_len,int is_global);
void vardeclaraction(int fsys[],int fsys_len,int is_global);
void vardefinition(int fsys[],int fsys_len,int is_global);

void parameterlist(int fsys[],int fsys_len);
void funct_ret_declaraction(int fsys[],int fsys_len);
void funct_void_declaraction(int fsys[],int fsys_len);
void funct_main_declaraction(int fsys[],int fsys_len);

void compound_statement(int fsys[],int fsys_len);
void statements(int fsys[],int fsys_len);
void statement(int fsys[],int fsys_len);

int factor(int fsys[],int fsys_len,int* const ftype,char fname[]);
int term(int fsys[],int fsys_len,int* const ttype,char tname[]);
int simpleexpression(int fsys[],int fsys_len,int* const stype,char sname[]);

void assignment(int fsys[],int fsys_len,char tmp_token[]);
void if_statement(int fsys[],int fsys_len);
void while_statement(int fsys[],int fsys_len);
int condition(int fsys[],int fsys_len);

void switch_statement(int fsys[],int fsys_len);
void caselabel(int fsys[],int fsys_len,int* const stype,char sname[]);
void onecase(int fsys[],int fsys_len);
void defaultcase(int fsys[],int fsys_len);

void scanf_statement(int fsys[],int fsys_len);
void printf_statement(int fsys[],int fsys_len);
void return_statement(int fsys[],int fsys_len);

void funct_call(int fsys[],int fsys_len,char funct_name[],int* const funct_type);
void selector(int fsys[],int fsys_len,int* const seltype,char selname[]);

void program(){
    int stop_set[SET_LEN]={intsy,charsy,voidsy};
    int stop_set_len=3;
    if(sym==constsy){
        constdeclaraction(stop_set,stop_set_len,1);
    }
    if(sym==intsy||sym==charsy){
        vardeclaraction(stop_set,stop_set_len,1);
    }
    if(sym==lparent){
        funct_ret_declaraction(stop_set,stop_set_len);
    }
    while(sym==intsy||sym==charsy||sym==voidsy){
        if(sym==voidsy){
            getNextSym();
            funct_void_declaraction(stop_set,stop_set_len);
        }else{
            funct_ret_declaraction(stop_set,stop_set_len);
        }
        if(sym!=mainsy){
            test(stop_set,stop_set_len,NULL,0,31);
        }
    }
    funct_main_declaraction(NULL,0);
    //printf("程序\n");
    if(sym==-1 && errors==0){
        printf("程序正确结束\n");
    }else{
        printf("程序未正确结束");
    }
}

void constdeclaraction(int fsys[],int fsys_len,int is_global){
    int stop_set[SET_LEN]={semicolon};
    int stop_set_len=1;
    int stop_set2[SET_LEN]={semicolon,constsy};
    int stop_set2_len=2;
    do{
        if(sym==constsy){//可以不要
            getNextSym();
        }
        if(sym==intsy||sym==charsy){
            stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
            constdefinition(stop_set,stop_set_len,is_global);
            needsym(semicolon);
            stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
            test(stop_set2,stop_set2_len,NULL,0,31);
        }else{
            error(14);
            stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
            test(stop_set,stop_set_len,fsys,fsys_len,-1);
            needsym(semicolon);
        }
        //printf("常量说明\n");
    }while(sym==constsy);
}
void constdefinition(int fsys[],int fsys_len,int is_global){
    enum types ident_typ;
    int redcl_flag=0;
    char tmp_token[LEN_OF_NAME];
    int value=0;
    int positive;
    int stop_set[SET_LEN]={comma,semicolon};
    int stop_set_len=1;
    if(sym==intsy){//进来肯定是常量声明，sym肯定是int或char
        ident_typ=ints;
    }else{
        ident_typ=chars;
    }
    do{
        getNextSym();
        if(sym==ident){
            redcl_flag=check_redeclaraction(is_global,token);
            if(redcl_flag){
                error(36);//跳到下一个comma
                test(stop_set,stop_set_len,fsys,fsys_len,-1);
            }else{
                strcpy(tmp_token,token);
                getNextSym();
                if(sym==becomes){
                    positive=1;
                    getNextSym();
                    if(sym==pluss||sym==minuss){
                        if(sym==minuss){
                            positive=-1;
                        }
                        getNextSym();
                        if(sym!=intcon){
                            error(39);//跳到comma
                            test(stop_set,stop_set_len,fsys,fsys_len,-1);
                        }else if(num_read==0){
                            error(16);//不管
                        }
                    }
                    if(sym!=intcon&&sym!=charcon){
                        error(23);//跳到comma
                        test(stop_set,stop_set_len,fsys,fsys_len,-1);
                    }
                    if(sym==intcon && ident_typ==ints){
                        enter_ident(is_global,tmp_token,con,ints,0,positive*num_read);
                        getNextSym();
                    }else if(sym==charcon && ident_typ==chars){
                        value=token[0];
                        enter_ident(is_global,tmp_token,con,chars,0,value);
                        getNextSym();
                    }else if(sym==charcon && ident_typ==ints){
                        error(26);//不管
                        value=token[0];
                        enter_ident(is_global,tmp_token,con,ints,0,value);
                        getNextSym();
                    }else {
                        error(11);//comma
                        test(stop_set,stop_set_len,fsys,fsys_len,-1);
                    }
                    test(stop_set,stop_set_len,fsys,fsys_len,-1);
                }else{
                    error(12);//comma
                    test(stop_set,stop_set_len,fsys,fsys_len,-1);
                }
            }
        }else{
            error(13);//comma
            test(stop_set,stop_set_len,fsys,fsys_len,-1);
        }
        //printf("常量定义\n");
    }while(sym==comma);
}
void vardeclaraction(int fsys[],int fsys_len,int is_global){
    int stop_set[SET_LEN]={semicolon,lparent};
    int stop_set_len=2;
    if(is_global){
        stop_set[stop_set_len++]=intsy;
        stop_set[stop_set_len++]=charsy;
        stop_set[stop_set_len++]=voidsy;
    }
    do{
        stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        vardefinition(stop_set,stop_set_len,is_global);
        if(sym==lparent){
            break;
        }else{
            needsym(semicolon);
        }
        if(is_global){
            test(stop_set,stop_set_len,fsys,fsys_len,31);
        }
    }while(sym==intsy||sym==charsy);
}
void vardefinition(int fsys[],int fsys_len,int is_global){
    enum types ident_typs;
    int redcl_flag=0;
    char tmp_token[LEN_OF_NAME];
    int stop_set[SET_LEN]={comma,semicolon};
    int stop_set_len=2;
    int stop_set2[SET_LEN]={intcon};
    int stop_set2_len=1;
    char iname[VAR_LEN];
    char array_len[VAR_LEN];
    if(sym==intsy){
        ident_typs=ints;
    }else{
        ident_typs=chars;
    }
    //读到标识符时循环
    do{
        getNextSym();
        if(sym==ident){
            redcl_flag=check_redeclaraction(is_global,token);
            if(redcl_flag){
                error(36);
                test(stop_set,stop_set_len,fsys,fsys_len,-1);
            }else{
                strcpy(tmp_token,token);
                getNextSym();
                if(sym==lparent){
                    typ_var_funct=ident_typs;
                    strcpy(ident_name_var_funct,tmp_token);
                    break;
                }
                if(sym!=lbrack){
                    enter_ident(is_global,tmp_token,var,ident_typs,0,0);
                    convert_name(iname,tmp_token,is_global);
                    gen_quaternary(op_var_dcl,iname,"","");
                    test(stop_set,stop_set_len,fsys,fsys_len,45);
                }else{
                    getNextSym();
                    if(sym!=intcon){
                        error(20);
                        test(stop_set2,stop_set2_len,fsys,fsys_len,-1);
                        if(sym==intcon){
                            goto int_label;
                        }
                    }else{
                    int_label:
                        if(num_read==0){
                            error(20);//不管
                        }
                        enter_ident(is_global,tmp_token,arrays,ident_typs,array_tab_index,0);
                        enter_array(num_read);//？
                        convert_name(iname,tmp_token,is_global);
                        insert2name(array_len,num_read,ints);
                        gen_quaternary(op_array_dcl,iname,array_len,"");
                        getNextSym();
                        needsym(rbrack);
                        test(stop_set,stop_set_len,fsys,fsys_len,45);
                    }
                }
            }
        }
        //printf("变量定义\n");
    }while(sym==comma);
}

void parameterlist(int fsys[],int fsys_len){
    enum types ident_typ;
    int redcl_flag=0;
    char tmp_token[LEN_OF_NAME];
    int stop_set[SET_LEN]={rparent,comma};
    int stop_set_len=2;
    int i=0;
    do{
        if(sym==comma){
            getNextSym();
        }
        if(sym!=intsy&&sym!=charsy&&sym!=rparent){
            error(22);
            getNextSym();
            test(stop_set,stop_set_len,fsys,fsys_len,-1);
        }else if(sym==intsy||sym==charsy){
            if(sym==intsy){
                ident_typ=ints;
            }else{
                ident_typ=chars;
            }
            getNextSym();
            if(sym==ident){
                redcl_flag=check_redeclaraction(0,token);
                if(redcl_flag){
                    error(36);
                    getNextSym();
                    test(stop_set,stop_set_len,fsys,fsys_len,-1);
                }else{
                    strcpy(tmp_token,token);
                    enter_ident(0,tmp_token,paras,ident_typ,0,++i);
                    getNextSym();
                }
            }else{
                error(13);
                test(stop_set,stop_set_len,fsys,fsys_len,-1);
            }
        }
    }while(sym==comma);
    //printf("参数表\n");
}
void funct_void_declaraction(int fsys[],int fsys_len){
    int refer=0;
    int adr=0;
    int redcl_flag=0;
    int lastpar;
    char tmp_token[LEN_OF_NAME];
    int stop_set2[SET_LEN]={rquote};
    int stop_set2_len=1;
    int stop_set3[SET_LEN]={rparent};
    int stop_set3_len=1;
    if(sym==mainsy){
        return;
    }
    if(sym==ident){
        redcl_flag=check_redeclaraction(1,token);
        if(redcl_flag){
            error(36);//避免跳过太多
        }
//            getNextSym();
//            test(fsys,fsys_len,NULL,0,-1);
//            return;
//        }else{
            strcpy(tmp_token,token);
            getNextSym();
            if(sym!=lparent){
                error(22);//跳到下一个函数
                test(fsys,fsys_len,NULL,0,-1);
                return;
            }else{
                gen_quaternary(op_func,tmp_token,"","");
                last_local_ident_index=-1;
                stop_set3_len=merge_sym_set(stop_set3,stop_set3_len,fsys,fsys_len);
                getNextSym();
                parameterlist(stop_set3,stop_set3_len);
                needsym(rparent);
                lastpar=last_local_ident_index;
                refer=enter_funct(0,lastpar,0,0);
                enter_ident(1,tmp_token,func,notyp,refer,adr);
                if(sym!=lquote){
                    error(24);//不管
                }else{
                    getNextSym();
                }
                stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
                compound_statement(stop_set2,stop_set2_len);
                needsym(rquote);//不管,应该不会出现
                funct_tab[refer].last=last_local_ident_index;
                gen_quaternary(op_ret_void,"","","");
                gen_quaternary(op_efunc,tmp_token,"","");
            }
        //}
        //printf("无返回值函数定义\n");
    }else{//非标识符
        error(13);//跳到下一个函数
        test(fsys,fsys_len,NULL,0,-1);
    }
}
void funct_ret_declaraction(int fsys[],int fsys_len){
    enum types typ;
    int refer;
    int adr=0;
    int redcl_flag=0;
    int lastpar;
    char tmp_token[LEN_OF_NAME];
    int stop_set2[SET_LEN]={rquote};
    int stop_set2_len=1;
    int stop_set3[SET_LEN]={lquote};
    int stop_set3_len=1;
    if(sym==intsy){
        typ=ints;
        getNextSym();
    }else if(sym==charsy){
        typ=chars;
        getNextSym();
    }
    //如果是main，报错
    if(sym==ident){
        redcl_flag=check_redeclaraction(1,token);
        if(redcl_flag){
            error(36);//避免跳过太多
        }
//            getNextSym();
//            test(fsys,fsys_len,NULL,0,-1);
//            return;
//      }else{
        strcpy(tmp_token,token);
//       }
        getNextSym();
    }else if(sym==lparent){
        if(ident_name_var_funct[0]=='\0'){
            error(28);
            test(fsys,fsys_len,NULL,0,-1);
        }else{
            strcpy(tmp_token,ident_name_var_funct);
            typ=typ_var_funct;
            clearTmpToken(ident_name_var_funct);
        }
    }else{
        if(sym==mainsy){
            error(44);
            getNextSym();
        }else{
            error(13);
            test(fsys,fsys_len,NULL,0,-1);
            return;
        }
    }
    if(sym!=lparent){
        error(22);
        test(fsys,fsys_len,NULL,0,-1);
    }else{
        gen_quaternary(op_func,tmp_token,"","");
        last_local_ident_index=-1;
        stop_set3_len=merge_sym_set(stop_set3,stop_set3_len,fsys,fsys_len);
        getNextSym();
        parameterlist(stop_set3,stop_set3_len);
        lastpar=last_local_ident_index;
        needsym(rparent);
        refer=enter_funct(0,lastpar,0,0);
        enter_ident(1,tmp_token,func,typ,refer,adr);
        if(sym!=lquote){
            error(24);//不管
        }else{
            getNextSym();
        }
        stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
        compound_statement(stop_set2,stop_set2_len);
        needsym(rquote);
        funct_tab[refer].last=last_local_ident_index;
        gen_quaternary(op_ret_value,"$0","","");
        gen_quaternary(op_efunc,tmp_token,"","");
        //printf("有返回值函数定义\n");
    }
}
void funct_main_declaraction(int fsys[],int fsys_len){
    int refer;
    int adr=0;
    int stop_set[SET_LEN]={rquote};
    int stop_set_len=1;
    int stop_set3[SET_LEN]={lquote};
    int stop_set3_len=1;
    if(sym==mainsy){
        getNextSym();
        if(sym!=lparent){
            error(22);//不管
        }else{
            getNextSym();
        }
        gen_quaternary(op_main,"","","");
        last_local_ident_index=-1;
        stop_set3_len=merge_sym_set(stop_set3,stop_set3_len,fsys,fsys_len);
        parameterlist(stop_set3,stop_set3_len);
        if(last_local_ident_index!=-1){
            error(22);
        }
        needsym(rparent);
        if(sym!=lquote){
            error(24);
        }else{
            getNextSym();
        }
        refer=enter_funct(0,-1,0,0);
        merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        compound_statement(stop_set,stop_set_len);
        enter_ident(1,"main",func,notyp,refer,adr);
        funct_tab[refer].last=last_local_ident_index-1;
        //enter_ident(token,is_global,func,notyp,refer,adr);
        needsym(rquote);
        gen_quaternary(op_emain,"","","");
    }else{
        error(43);//不管
    }
    //printf("主函数定义\n");
}


void compound_statement(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={intsy,charsy,ident};
    int stop_set_len=3;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    if(sym==constsy){
        constdeclaraction(stop_set,stop_set_len,0);
    }
    if(sym==intsy||sym==charsy){
        vardeclaraction(fsys,fsys_len,0);
    }
    statements(fsys,fsys_len);
   //printf("复合语句\n");
}
void statements(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={ident,ifsy,whilesy,lquote,scanfsy,printfsy,switchsy,returnsy,semicolon,rquote};
    int stop_set_len=10;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    while(sym!=rquote){
        if(sym==semicolon){
            getNextSym();
        }
        statement(stop_set,stop_set_len);
    }
//printf("语句列\n");
}
void statement(int fsys[],int fsys_len){
    char tmp_token[LEN_OF_NAME];
    int stop_set[SET_LEN]={rquote};
    int stop_set_len=1;
    int fun_type;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    //test(fsys,fsys_len,NULL,0,-1);
    switch(sym){
        case ifsy:
            getNextSym();
            if_statement(fsys,fsys_len);
            break;
        case whilesy:
            getNextSym();
            while_statement(fsys,fsys_len);
            break;
        case lquote:
            getNextSym();
            statements(stop_set,stop_set_len);
            needsym(rquote);
            break;
        case ident:
            strcpy(tmp_token,token);
            getNextSym();
            if(sym==lbrack||sym==becomes){
                assignment(fsys,fsys_len,tmp_token);
            }else if(sym==lparent){
                getNextSym();
                funct_call(fsys,fsys_len,tmp_token,&fun_type);
            }else{
                error(31);
                break;
            }
            needsym(semicolon);
            break;
        case scanfsy:
            getNextSym();
            scanf_statement(fsys,fsys_len);
            needsym(semicolon);
            break;
        case printfsy:
            getNextSym();
            printf_statement(fsys,fsys_len);
            needsym(semicolon);
            break;
        case switchsy:
            getNextSym();
            switch_statement(fsys,fsys_len);
            break;
        case returnsy:
            getNextSym();
            return_statement(fsys,fsys_len);
            needsym(semicolon);
            break;
        case semicolon:
            getNextSym();
            break;
        default:
            break;
    }
    test(fsys,fsys_len,NULL,0,-1);
//printf("语句\n");
}

int factor(int fsys[],int fsys_len,int* const ftype,char fname[]){
    int res_position;
    char tmp_token[LEN_OF_NAME];
    int legal_set[SET_LEN]={lparent,ident,intcon,charcon};
    int legal_set_len=4;
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    int is_global;
    int seltype;
    int* const p_seltype=&seltype;
    int selname[VAR_LEN];
    char basename[VAR_LEN];
    int para_n=0;
    //test
    test(legal_set,legal_set_len,fsys,fsys_len,31);
    if(sym==ident||sym==intcon||sym==charcon){
        if(sym==ident){
            strcpy(tmp_token,token);
            res_position=position(tmp_token,&is_global);
            if(res_position==-1){
                error(32);
                getNextSym();
                test(fsys,fsys_len,NULL,0,-1);
            }else{
                getNextSym();
                if(sym==lbrack){//数组
                    getNextSym();
                    selector(fsys,fsys_len,p_seltype,selname);
                    gen_name(fname);
                    convert_name(basename,tmp_token,is_global);
                    gen_quaternary(op_arr_get,fname,basename,selname);
                }else if(sym==lparent){//函数调用
                    getNextSym();
                    funct_call(fsys,fsys_len,tmp_token,ftype);
                    if(*ftype==notyp){
                        error(37);
                    }
                    gen_name(fname);
                    gen_quaternary(op_load_ret,fname,"","");
                }else{//
                    if(is_global){
                        *ftype=global_ident_tab[res_position].typ;
                        if(global_ident_tab[res_position].obj==con){
                            insert2name(fname,global_ident_tab[res_position].adr,*ftype);
                        }else{
                            convert_name(fname,tmp_token,1);
                        }
                    }else{
                        if(local_ident_tab[res_position].obj==paras){
                            para_n=local_ident_tab[res_position].adr;
                            para_name(fname,para_n);
                        }else if(local_ident_tab[res_position].obj==con){
                            insert2name(fname,local_ident_tab[res_position].adr,*ftype);
                        }else{
                            convert_name(fname,tmp_token,0);
                        }
                        *ftype=local_ident_tab[res_position].typ;
                    }
                }
            }
        }else{
            if(sym==charcon){
                insert2name(fname,token[0],chars);
                *ftype=chars;
            }else{
                insert2name(fname,num_read,ints);
                *ftype=ints;
            }
            getNextSym();
        }
    }else if(sym==lparent){
        getNextSym();
        stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        simpleexpression(stop_set,stop_set_len,ftype,fname);
        needsym(rparent);
    }
    //printf("因子\n");
    return 0;
}
int term(int fsys[],int fsys_len,int* const ttype,char tname[]){
    enum symbol fac_op;
    int stop_set[SET_LEN]={times,idiv};
    int stop_set_len=2;
    char fname1[VAR_LEN],fname2[VAR_LEN];
    int ftype1=-1;
    int ftype2=-1;
    int* const p_ftype1=&ftype1;
    int* const p_ftype2=&ftype2;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    factor(stop_set,stop_set_len,p_ftype1,fname1);
    *ttype=ftype1;
    while(times==sym||idiv==sym){
        if(*ttype==chars){
            error(11);
        }
        fac_op=sym;
        getNextSym();
        factor(stop_set,stop_set_len,p_ftype2,fname2);
        if(ftype2==chars){
            error(11);
        }
        gen_name(tname);
        if(fac_op==times){
            gen_quaternary(op_mul,tname,fname1,fname2);
        }else{
            gen_quaternary(op_idiv,tname,fname1,fname2);
        }
        strcpy(fname1,tname);
        *ttype=ints;
    }
    strcpy(tname,fname1);
    //printf("项\n");
    return 0;
}
int simpleexpression(int fsys[],int fsys_len,int* const stype,char sname[]){
    enum symbol positive;
    int stop_set[SET_LEN]={pluss,minuss};
    int stop_set_len=2;
    char tname1[VAR_LEN],tname2[VAR_LEN];
    int ttype1=-1;
    int ttype2=-1;
    int* const p_ttype1=&ttype1;
    int* const p_ttype2=&ttype2;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    if(sym==pluss||sym==minuss){
        positive=sym;
        getNextSym();
        term(stop_set,stop_set_len,p_ttype1,tname1);
        if(positive==minuss){
            //插入取反操作
            if(ttype1==chars){
                error(11);
            }
            gen_name(tname2);
            gen_quaternary(op_sub,tname2,"$0",tname1);
        }else{
            strcpy(tname2,tname1);
        }
        *stype=ints;
    }else{
        term(stop_set,stop_set_len,p_ttype2,tname2);
        *stype=ttype2;
    }
    while(sym==pluss||sym==minuss){
        gen_name(sname);
        positive=sym;
        getNextSym();
        term(stop_set,stop_set_len,p_ttype1,tname1);
        if(*stype!=ttype1){
            error(11);
        }
        if(positive==pluss){
            //插入加指令
            gen_quaternary(op_add,sname,tname2,tname1);
        }else{
            //插入减指令
            gen_quaternary(op_sub,sname,tname2,tname1);
        }
        strcpy(tname2,sname);
        *stype=ints;
    }
    strcpy(sname,tname2);
    //printf("表达式\n");
    return 0;
}

void assignment(int fsys[],int fsys_len,char tmp_token[]){
    int stop_set[SET_LEN]={becomes};
    int stop_set_len=1;
    int is_global;
    int res_position=position(tmp_token,&is_global);
    int stype;
    int* const p_stype=&stype;
    int target_type;
    char sname[VAR_LEN];
    int seltype;
    int* const p_seltype=&seltype;
    char selname[VAR_LEN];
    char target_name[VAR_LEN];
    char basename[VAR_LEN];
    int is_arr=0;
    //tmp_token可能是数组
    if(sym==lbrack){
        is_arr=1;
        getNextSym();
        stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        selector(stop_set,stop_set_len,p_seltype,selname);
        if(seltype==chars){
            error(11);
        }
        convert_name(basename,tmp_token,is_global);
    }
    if(sym!=becomes){
        error(12);
        test(fsys,fsys_len,NULL,0,-1);
    }else{
        if(res_position==-1){
            error(32);
            test(fsys,fsys_len,NULL,0,-1);
        }else{
            getNextSym();
            simpleexpression(fsys,fsys_len,p_stype,sname);
            if(is_global){
                target_type=global_ident_tab[res_position].typ;
            }else{
                target_type=local_ident_tab[res_position].typ;
            }
            if(stype==ints&&target_type==chars){
                error(11);
            }else if(stype==chars&&target_type==ints){
                error(11);
            }
            if(is_arr==0){
                if(is_global==0&&local_ident_tab[res_position].obj==paras){
                    para_name(target_name,local_ident_tab[res_position].adr);
                }else{
                    convert_name(target_name,tmp_token,is_global);
                }
                gen_quaternary(op_mov,target_name,sname,"");
            }else{
                gen_quaternary(op_arr_assign,basename,selname,sname);
            }
            //插入运算指令
    //        if(ret_typ==ints && ident_tab[res_position].typ==ints){
    //        }else if(ret_typ==chars && ident_tab[res_position].typ==chars){
    //        }else if(ret_typ==ints && ident_tab[res_position].typ==ints){
    //        }else{
    //            error(11);//报错之后继续插入
    //        }
        }
    }
    //printf("赋值语句\n");
}
void if_statement(int fsys[],int fsys_len){
    int stop_set1[SET_LEN]={rparent};
    int stop_set1_len=1;
    int stop_set2[SET_LEN]={elsesy};
    int stop_set2_len=1;
    int labx1,labx2;
    int code_x1,code_x2;
    if(sym!=lparent){
        error(25);
    }else{
        getNextSym();
    }
    stop_set1_len=merge_sym_set(stop_set1,stop_set1_len,fsys,fsys_len);
    code_x1=condition(stop_set1,stop_set1_len);
    needsym(rparent);
    stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
    statement(stop_set2,stop_set2_len);
    if(sym==elsesy){//对吗？
        code_x2=gen_quaternary(op_jump,"","","");
        labx2=gen_lab();
        gen_quaternary(op_set_label,label_table[labx2],"","");
        strcpy(quat_table[code_x1].dest,label_table[labx2]);
        getNextSym();
        statement(fsys,fsys_len);
        labx1=gen_lab();
        gen_quaternary(op_set_label,label_table[labx1],"","");
        strcpy(quat_table[code_x2].dest,label_table[labx1]);
        //printf("if 中 else\n");
    }else{
        labx1=gen_lab();
        gen_quaternary(op_set_label,label_table[labx1],"","");
        strcpy(quat_table[code_x1].dest,label_table[labx1]);
    }
//printf("if 语句\n");
}
void while_statement(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    int lab_x1,lab_x2;
    int codex;
    if(sym!=lparent){
        error(25);
    }else{
        getNextSym();
    }
    lab_x1=gen_lab();
    gen_quaternary(op_set_label,label_table[lab_x1],"","");
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    codex=condition(stop_set,stop_set_len);
    needsym(rparent);
    statement(fsys,fsys_len);
    gen_quaternary(op_jump,label_table[lab_x1],"","");
    lab_x2=gen_lab();
    gen_quaternary(op_set_label,label_table[lab_x2],"","");
    strcpy(quat_table[codex].dest,label_table[lab_x2]);
    //printf("while 语句\n");
}
int condition(int fsys[],int fsys_len){
    enum symbol tmp_sym;
    int stop_set[SET_LEN]={eql,neq,gtr,geq,lss,leq};
    int stop_set_len=6;
    char sname1[VAR_LEN],sname2[VAR_LEN];
    int stype1,stype2;
    int* const p_stype1=&stype1;
    int* const p_stype2=&stype2;
    int code_x;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    simpleexpression(stop_set,stop_set_len,p_stype1,sname1);
    if(sym==eql||sym==neq||sym==gtr||sym==geq||sym==lss||sym==leq){
        tmp_sym=sym;
        getNextSym();
        simpleexpression(fsys,fsys_len,p_stype2,sname2);
        switch(tmp_sym){
        case eql://根据不同情况生成指令
            code_x=gen_quaternary(op_bne,"",sname1,sname2);
            break;
        case neq:
            code_x=gen_quaternary(op_beq,"",sname1,sname2);
            break;
        case gtr:
            code_x=gen_quaternary(op_ble,"",sname1,sname2);
            break;
        case geq:
            code_x=gen_quaternary(op_bls,"",sname1,sname2);
            break;
        case lss:
            code_x=gen_quaternary(op_bge,"",sname1,sname2);
            break;
        case leq:
            code_x=gen_quaternary(op_bgt,"",sname1,sname2);
            break;
        default:
            printf("error compare operation!\n");
        }
    }else{
        code_x=gen_quaternary(op_ble,"",sname1,"$0");
    }
    //打标签
    //printf("if或while判断条件 \n");
    return code_x;
}

void switch_statement(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    int stop_set2[SET_LEN]={rquote};
    int stop_set2_len=1;
    char sname[VAR_LEN];
    int stype;
    int* const p_stype=&stype;
    if(sym!=lparent){
        error(25);
    }else{
        getNextSym();
    }
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    simpleexpression(stop_set,stop_set_len,p_stype,sname);
    needsym(rparent);
    if(sym!=lquote){
        error(24);
    }else{
        getNextSym();
    }
    stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
    caselabel(stop_set2,stop_set2_len,p_stype,sname);//onst?
    needsym(rquote);
    //printf("情况语句\n");
}
void caselabel(int fsys[],int fsys_len,int* const stype,char sname[]){
    int stop_set[SET_LEN]={casesy,defaultsy};
    int stop_set_len=2;
    int con_name[VAR_LEN];
    int lab_finish;
    int labx[SW_BR_NUM];
    int positive=1;
    int i=0;
    int case_begin[SW_BR_NUM],case_end[SW_BR_NUM];
    int value=0;
    test(stop_set,stop_set_len,fsys,fsys_len,27);
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    if(sym==casesy){
        do{
            getNextSym();
            if(sym!=charcon){
                if(sym==pluss||sym==minuss){
                    if(sym==minuss){
                        positive=-1;
                    }
                    getNextSym();
                }
                if(sym==intcon){
                    if(num_read==0){
                        error(16);
                    }
                    value=positive*num_read;
                }else{
                    error(40);//重标签同处理
                    test(stop_set,2,fsys,fsys_len,-1);
                }
            }
            //分号
            if(sym==intcon){
                insert2name(con_name,value,ints);
            }else{
                insert2name(con_name,token[0],chars);
            }
            labx[i]=gen_lab();
            gen_quaternary(op_set_label,label_table[labx[i]],"","");
            case_begin[i]=gen_quaternary(op_bne,"",sname,con_name);
            getNextSym();
            onecase(stop_set,stop_set_len);
            case_end[i]=gen_quaternary(op_jump,"","","");
            i++;
            //打标签
        }while(sym==casesy);
    }else if(sym==defaultsy){
        error(27);
    }
    if(sym==defaultsy){
        labx[i]=gen_lab();
        gen_quaternary(op_set_label,label_table[labx[i]],"","");
        getNextSym();
        defaultcase(fsys,fsys_len);
        lab_finish=gen_lab();
        gen_quaternary(op_set_label,label_table[lab_finish],"","");
        while(i>0){
            strcpy(quat_table[case_begin[i-1]].dest,label_table[labx[i]]);
            strcpy(quat_table[case_end[i-1]].dest,label_table[lab_finish]);
            i--;
        }
    }else{
        lab_finish=gen_lab();
        labx[i]=lab_finish;
        gen_quaternary(op_set_label,label_table[lab_finish],"","");
        while(i>0){
            strcpy(quat_table[case_begin[i-1]].dest,label_table[labx[i]]);
            strcpy(quat_table[case_end[i-1]].dest,label_table[lab_finish]);
            i--;
        }
    }
    //printf("情况表\n");
}
void onecase(int fsys[],int fsys_len){
    if(sym!=colon){
        error(41);
    }else{
        getNextSym();
    }
    statement(fsys,fsys_len);
    //printf("情况子语句\n");
}
void defaultcase(int fsys[],int fsys_len){
    if(sym!=colon){
        error(42);
    }else{
        getNextSym();
    }
    statement(fsys,fsys_len);
    //printf("缺省\n");
}

void scanf_statement(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={comma,rparent};
    int stop_set_len=2;
    int res_position;
    int is_global;
    int ce_name[VAR_LEN];
    if(sym!=lparent){
        error(29);
    }else{
        getNextSym();
    }
    if(sym!=ident){
        error(30);
        test(stop_set,stop_set_len,fsys,fsys_len,-1);
    }
    do{
        if(sym==comma){
            getNextSym();
        }
        if(sym!=ident){
            error(30);
            test(stop_set,stop_set_len,fsys,fsys_len,-1);
        }else{
            res_position=position(token,&is_global);
            if(res_position==-1){
                error(32);
                test(stop_set,stop_set_len,fsys,fsys_len,-1);
            }else{
                convert_name(ce_name,token,is_global);
                if(is_global){
                    if(global_ident_tab[res_position].typ==ints){
                        gen_quaternary(op_scanfi,ce_name,"","");
                    }else{
                        gen_quaternary(op_scanfc,ce_name,"","");
                    }
                }else{
                    if(local_ident_tab[res_position].typ==ints){
                        gen_quaternary(op_scanfi,ce_name,"","");
                    }else{
                        gen_quaternary(op_scanfc,ce_name,"","");
                    }
                }
                getNextSym();
            }
        }
    }while(sym==comma);
    needsym(rparent);
//printf("读语句\n");
}
void printf_statement(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    char string_name[VAR_LEN];
    char sname[VAR_LEN];
    int stype;
    int* const p_stype=&stype;
    if(sym!=lparent){
        error(29);
    }else{
        getNextSym();
    }
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    if(sym==stringcon){
        gen_string_name(string_name);
        gen_quaternary(op_prints,string_name,"","");
        getNextSym();
        if(sym==comma){
            getNextSym();
            simpleexpression(stop_set,stop_set_len,p_stype,sname);
            if(stype==ints){
                gen_quaternary(op_printi,sname,"","");
            }else{
                gen_quaternary(op_printc,sname,"","");
            }
        }
        needsym(rparent);
    }else {
        simpleexpression(stop_set,stop_set_len,p_stype,sname);
        needsym(rparent);
        if(stype==ints){
            gen_quaternary(op_printi,sname,"","");
        }else{
            gen_quaternary(op_printc,sname,"","");
        }
    }
    //printf("写语句\n");
}
void return_statement(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    int stype;
    int* const p_stype=&stype;
    char sname[VAR_LEN];
    if(sym==semicolon){
        if(global_ident_tab[global_ident_index-1].typ!=notyp){
            error(46);
        }
        gen_quaternary(op_ret_void,"","","");
        //printf("返回语句\n");
        return;
    }
    if(sym!=lparent){
        error(29);
    }else{
        getNextSym();
    }
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    simpleexpression(stop_set,stop_set_len,p_stype,sname);
    if(global_ident_tab[global_ident_index-1].typ!=stype){
        error(46);
    }
    needsym(rparent);
    gen_quaternary(op_ret_value,sname,"","");
    //printf("返回语句\n");
}



void funct_call(int fsys[],int fsys_len,char funct_name[],int* const funct_type){
    int stop_set[SET_LEN]={comma,rparent};
    int stop_set_len=2;
    int stop_set2[SET_LEN]={rparent};
    int stop_set2_len=1;
    int stype;
    int* const p_stype=&stype;
    char sname[MAX_PARA][VAR_LEN];
    int i=global_ident_index-1;
    int ref;
    int lastpar=-1;
    int par_x[MAX_PARA];
    int par_num=0;
    while(i>=0){
        if(strcmp(global_ident_tab[i].name,funct_name)==0 && global_ident_tab[i].obj==func){
            ref=global_ident_tab[i].refer;
            lastpar=funct_tab[ref].lastpar;
            *funct_type=global_ident_tab[i].typ;
            break;
        }
        i--;
    }
    if(i<0){
        error(47);
        stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
        test(stop_set2,stop_set2_len,NULL,0,-1);
        needsym(rparent);
        return;
    }
    i=0;
    while(lastpar!=-1){
        par_x[i++]=lastpar;
        lastpar=local_ident_tab[lastpar].link;
    }
    par_num=i;
    if(sym!=rparent && par_num>0){
        stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        simpleexpression(stop_set,stop_set_len,p_stype,sname[par_num-i]);
        if(local_ident_tab[par_x[i-1]].typ!=stype){
            printf("类型不匹配");
        }
        i--;
        if(sym==comma){
            do{
                if(i<0){
                    break;
                }
                getNextSym();
                simpleexpression(stop_set,stop_set_len,p_stype,sname[par_num-i]);
                i--;
            }while(sym==comma);
        }
        if(i==0&&sym==rparent){
            ;
        }else{
            error(35);
            test(stop_set2,stop_set2_len,NULL,0,-1);
           // needsym(rparent);
        }
        //printf("值参数表\n");
    }else{
        if(par_num>0&&sym==rparent){
            error(35);
            test(stop_set2,stop_set2_len,NULL,0,-1);
            needsym(rparent);
            return;
        }else if(par_num==0&&sym!=rparent){
            error(35);
            test(stop_set2,stop_set2_len,NULL,0,-1);
            needsym(rparent);
            return;
        }
        //没有参数
        //printf("空值参数表\n");
    }
    needsym(rparent);
    i=par_num;
    while(i>0){
        gen_quaternary(op_para,sname[i-1],"","");
        i--;
    }//倒着的参数para_n,para_n-1,para_n-2,..para_1
    gen_quaternary(op_call,funct_name,"","");
    //printf("函数调用语句\n");
}
void selector(int fsys[],int fsys_len,int* const seltype,char selname[]){
    int stop_set[SET_LEN]={rbrack};
    int stop_set_len=1;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    simpleexpression(stop_set,stop_set_len,seltype,selname);
    needsym(rbrack);
    //printf("数组选择器\n");
}

#endif // GRAMMARANALYSIS_H_INCLUDED
