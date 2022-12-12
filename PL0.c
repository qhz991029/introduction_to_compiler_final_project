// pl0 compiler source code
//for Visual Studio
#pragma warning(disable:4996)
#include "PL0.h"
#include "set.h"

int data_alloc_index[MAXLEVEL]; // data allocation index
int zx[MAXLEVEL];

int jdgok(int curBlkNum, int saveBlkNum)  //if saveBlkNum is the prefix of the curBlkNum it's ok
{
    while (curBlkNum > saveBlkNum) {
        curBlkNum /= 10;
    }
    return curBlkNum == saveBlkNum;
}

void print_table();

void in_block() {
    block_num = block_num * 10 + block_level_count[block_level];
    /*
    if (block_num==101111) {
        int kk;
        kk=1;
        print_table();
    }
    */

//    printf("blk_num=%d\n",block_num);
    block_level_count[block_level]++;
    block_level++;
    block_level_count[block_level] = 0;
}

void out_block(int saveBlkNum, int saveBlkLvl) {
    block_level = saveBlkLvl;
    block_num = saveBlkNum;
//	printf("blk_num=%d\n",block_num);
}

void error(int n) {

//    color(12); //red
    int i;
    printf("      ");
    for (i = 1; i <= char_count - 1; i++)
        printf(" ");
    printf("^\n");
    printf("Error %3d: %s\n", n, err_msg[n]);
    err++;
//    color(7);//black
} // error

void getch(void) {
    if (char_count == line_length) {
        if (feof(infile)) {
            printf("\nPROGRAM INCOMPLETE\n");
            exit(1);
        }
        line_length = char_count = 0;
        printf("%5d  ", current_instruction_index);
        while ((!feof(infile)) // added & modified by alex 01-02-09
               && ((last_char_read = getc(infile)) != '\n')) {
            printf("%c", last_char_read);
            line[++line_length] = last_char_read;
        } // while
        printf("\n");
        line[++line_length] = ' ';
    }
    last_char_read = line[++char_count];
} // getch

void get_next_symbol(void) {
    int i, k;
    char a[MAXIDLEN + 1];

    while (last_char_read == ' ' || last_char_read == '\t')
        getch();

    if (isalpha(last_char_read))   // symbol is a reserved word or an identifier.
    {
        k = 0;
        do {
            if (k < MAXIDLEN)
                a[k++] = last_char_read;
            getch();
        } while (isalpha(last_char_read) || isdigit(last_char_read));   //超出部分不报错自动截断？
        a[k] = 0;
        strcpy(id, a);
        word[0] = id;
        i = NRW;
        while (strcmp(id, word[i--]));
        if (++i)
            last_sym_read = wsym[i]; // symbol is a reserved word
        else
            last_sym_read = SYM_IDENTIFIER; // symbol is an identifier
    } else if (isdigit(last_char_read))     // symbol is a number.
    {
        k = last_num_read = 0;
        last_sym_read = SYM_NUMBER;
        do {
            last_num_read = last_num_read * 10 + last_char_read - '0';
            k++;
            getch();
        } while (isdigit(last_char_read));
        if (k > MAXNUMLEN)
            error(25); // The number is too great.
    } else if (last_char_read == ':') {
        getch();
        if (last_char_read == '=') {
            last_sym_read = SYM_BECOMES; // :=
            getch();
        } else {
            last_sym_read = SYM_NULL; // illegal?
        }
    } else if (last_char_read == '>') {
        getch();
        if (last_char_read == '=') {
            last_sym_read = SYM_GEQ; // >=
            getch();
        } else {
            last_sym_read = SYM_GTR; // >
        }
    } else if (last_char_read == '<') {
        getch();
        if (last_char_read == '=') {
            last_sym_read = SYM_LEQ; // <=
            getch();
        } else if (last_char_read == '>') {
            last_sym_read = SYM_NEQ; // <>
            getch();
        } else {
            last_sym_read = SYM_LES; // <
        }
    } else if (last_char_read == '/')   //����ע�͹���
    {
        getch();
        if (last_char_read == '/') {
            //sym = SYM_LINECOMM;
            while (line_length - char_count) {
                getch();
            }
            get_next_symbol();
        } else if (last_char_read == '*') {
            //sym = SYM_BLOCKCOMM;
            int end = 1;
            char b, a = 0;
            getch();
            while (end) {
                if (a != 0)
                    a = b;
                else
                    a = last_char_read;
                getch();
                b = last_char_read;
                if (a == '*' && b == '/')
                    end = 0; //��δ��Ӵ������?
            }
            getch();
            get_next_symbol();
        } else if (last_char_read == '=') {
            last_sym_read = SYM_DIVEQU,
                    getch();
        } else {
            last_sym_read = SYM_SLASH;
        }
    } else if (last_char_read == '%') {
        getch();
        {
            if (last_char_read == '=') {
                last_sym_read = SYM_MODEQU;
                getch();
            } else error(38);//////�¼�
        }
    } else     // other tokens
    {
        i = NSYM;
        csym[0] = last_char_read;
        while (csym[i--] != last_char_read);
        if (++i) {
            if (last_char_read == '+') {
                getch();
                if (last_char_read == '=') {
                    last_sym_read = SYM_ADDEQU;
                    getch();
                } else if (last_char_read == '+') {
                    last_sym_read = SYM_ADDADD;
                    getch();
                } else {
                    last_sym_read = SYM_PLUS;
                }
            } else if (last_char_read == '-') {
                getch();
                if (last_char_read == '=') {
                    last_sym_read = SYM_SUBEQU;
                    getch();
                } else if (last_char_read == '-') {
                    last_sym_read = SYM_SUBSUB;
                    getch();
                } else last_sym_read = SYM_MINUS;
            } else if (last_char_read == '*') {
                getch();
                {
                    if (last_char_read == '=') {
                        last_sym_read = SYM_MULEQU;
                        getch();
                    } else last_sym_read = SYM_TIMES;
                }
            } else {
                last_sym_read = ssym[i];
                getch();
            }
        } else {
//            color(12);
            printf("Fatal Error: Unknown character.\n");
//            color(7);
            exit(1);
        }
    }
    //printf("zq: sym=%d\n",sym);
} // getsym

void gen_instruction(int x, int y, int z) {
    if (current_instruction_index > CXMAX) {
        printf("Fatal Error: Program too long.\n");
        exit(1);
    }
    code[current_instruction_index].f = x;
    code[current_instruction_index].l = y;
    code[current_instruction_index++].a = z;
} // gen

// enter_obj_2_table object(constant, variable or procedre) into table.
void enter_obj_2_table(int kind)
{
    mask *mk;
    array *ar;
    tx++;
    //search the name before entering
    int i;
    for (i = tx_[level]; i < tx; i++) {
        if (!strcmp(id, table[i].name)) {
            error(36);
            goto l1;
        }
    }

    strcpy(table[tx].name, id);
    table[tx].kind = kind;
    table[tx].cnt = 0;  //zq
    table[tx].lpl = loop_level; //
    table[tx].blkNum = block_num;
    table[tx].evl = create_evl(0, 0);
    table[tx].quote = 0; //cy_quote
    switch (kind) {
        case ID_CONSTANT:
            if (last_num_read > MAXADDRESS) {
                error(25); // The number is too great.
                last_num_read = 0;
            }
            table[tx].value = last_num_read;
            break;
        case ID_VARIABLE:
            mk = (mask *) &table[tx];
            mk->level = level;
            mk->address = data_alloc_index[level]++;
            break;
        case ID_PROCEDURE:
            mk = (mask *) &table[tx];
            mk->level = level;
            break;
        case ID_ARRAY:
            ar = (array *) &table[tx];
            ar->dim_n = dim;
            ar->level = level;
            ar->address = data_alloc_index[level];
            ar->kind = (short) kind;
            data_alloc_index[level] += array_size;
            p_dim *p = NULL;
            int i;
            for (i = dim - 1; i >= 0; i--) {
                p_dim *q = (p_dim *) malloc(sizeof(p_dim));
                q->dim_len = latit[i];
                q->next = p;
                p = q;
            }
            ar->next = p;
            break;
    } // switch
    l1:;
} // enter_obj_2_table

void test(symset s1, symset s2, int n) {
    symset s;

    if (!inset(last_sym_read, s1)) {
        if (n) {
            error(n);
        }
        s = uniteset(s1, s2);
        while (!inset(last_sym_read, s))
            get_next_symbol();
        destroyset(s);
    }

} // test

void enter_par() {
    mask *mk;
    tx++;
    strcpy(table[tx].name, id);
    table[tx].kind = ID_VARIABLE;
    mk = (mask *) &table[tx];
    mk->level = level + 1; //differences
    mk->address = zx[level]++; //differences
    mk->quote = 1;
}

void modify_table(int numOfPar) {
    int i;
    mask *mk;
    for (i = 0; i < numOfPar; i++) {
        mk = (mask *) &table[tx - i];
        mk->address = mk->address - numOfPar;
    }
}

void print_table() {
    int i;
    printf("=========================print Table=========================\n");
    printf("%10s%10s%10s%10s%10s%10s%10s\n", "No.", "name", "level", "address",
           "numOfPar", "quote", "dim");
    for (i = 1; i <= tx; i++) {
        array *ar = (array *) &table[i];
        if (table[i].kind == ID_CONSTANT)
            printf("%10d:%10s%10d\n", i, table[i].name, table[i].value);
        else if (ar->kind == ID_ARRAY) {
            printf("%10d:%10s%10d%10d%10d%10d%10d\n", i, table[i].name,
                   ar->level, ar->address, ar->numOfPar, ar->quote, ar->dim_n);
            /*	p_dim *p=ar->next;
             while(p){
             printf("%10d",p->dim_len);
             p=p->next;
             }
             printf("\n");*/
        } else {
            mask *mk = (mask *) &table[i];
            printf("%10d:%10s%10d%10d%10d%10d\n", i, table[i].name, mk->level,
                   mk->address, mk->numOfPar, mk->quote);
        }
    }
}
// locates identifier in symbol table.
int get_identifier_id(char *identifier) {
    int i;
    strcpy(table[0].name, identifier);
    i = tx + 1;
    while (strcmp(table[--i].name, identifier) != 0);
    mask *mk = (mask *) &table[i];
    array *ar = (array *) &table[i];
    if (mk->level == level)   //cy_quote
    {
        table[i].quote = 1;
    } else
        table[i].quote = 2;

    return i;
} // position

void const_declaration(symset fsys) {
    int constexpre(symset fsys);
    if (last_sym_read == SYM_IDENTIFIER) {
        char idsaved[MAXIDLEN + 1];
        strcpy(idsaved, id);
        get_next_symbol();
        if (last_sym_read == SYM_EQU || last_sym_read == SYM_BECOMES) {
            if (last_sym_read == SYM_BECOMES)
                error(1); // Found ':=' when expecting '='.
            get_next_symbol();
            last_num_read = const_expression(
                    uniteset(fsys, createset(SYM_COMMA, SYM_SEMICOLON, SYM_NULL)));
            strcpy(id, idsaved);
            enter_obj_2_table(ID_CONSTANT);
        } else {
            error(3); // There must be an '=' to follow the identifier.
        }
    } else
        error(4);
    // There must be an identifier to follow 'const', 'var', or 'procedure'.
} // const_declaration

void var_declaration(void) {
    if (last_sym_read == SYM_IDENTIFIER) {
        char idsaved[MAXIDLEN + 1] = {'\0'};
        strcpy(idsaved, id);
        get_next_symbol();
        if (last_sym_read == SYM_LSQUARE) {
            strcpy(id, idsaved);
            dim_declaration();
        } else {
            char idsaved2[MAXIDLEN + 1] = {'\0'};
            strcpy(idsaved2, id);
            strcpy(id, idsaved);
            enter_obj_2_table(ID_VARIABLE);
            strcpy(id, idsaved2);
        }
    } else {
        error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
    }
} // var_declaration

void list_code(int from, int to) {
    int i;

    printf("\n");
    for (i = from; i < to; i++) {
        printf("%5d %s\t%d\t%d\n", i, mnemonic[code[i].f], code[i].l,
               code[i].a);
    }
    printf("\n");
} // list_code

int const_factor(symset fsys) {
    int i;
    int n;
    symset set;
    test(factor_begin_sys, fsys, 24); // The symbol can not be as the beginning of an expression.
    while (inset(last_sym_read, factor_begin_sys)) {
        if (last_sym_read == SYM_IDENTIFIER) {
            if ((i = get_identifier_id(id)) == 0) {
                error(11); // Undeclared identifier.
                get_next_symbol();
            } else {
                array *ar = (array *) &table[i];

                switch (table[i].kind) {
                    mask *mk;
                    case ID_CONSTANT:
                        n = table[i].value;
                        get_next_symbol();
                        break;
                    case ID_VARIABLE:
                        error(37);
                        get_next_symbol();
                        break;
                    case ID_PROCEDURE:
                        error(21); // Procedure identifier can not be in an expression.
                        get_next_symbol();
                        break;
                    default:
                        error(37);
                        break;
                } // switch
            }
        } else if (last_sym_read == SYM_NUMBER) {
            if (last_num_read > MAXADDRESS) {
                error(25); // The number is too great.
                last_num_read = 0; //number̫����ֱ�Ӹ�ֵΪ0
            }
            n = last_num_read;
            get_next_symbol();
        } else if (last_sym_read == SYM_LPAREN){
            get_next_symbol();
            set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
            n = const_expression(set);
            destroyset(set);
            if (last_sym_read == SYM_RPAREN) {
                get_next_symbol();
            } else {
                error(22); // Missing ')'.
            }
        }
    } // while
    return n;
} // factor

int const_term(symset fsys) {
    int mulop;
    symset set;

    set = uniteset(fsys, createset(SYM_TIMES, SYM_SLASH, SYM_NULL));
    int n1 = const_factor(set);
    int n = n1;
    while (last_sym_read == SYM_TIMES || last_sym_read == SYM_SLASH) {
        mulop = last_sym_read;
        get_next_symbol();
        int n2 = const_factor(set);
        if (mulop == SYM_TIMES) {
            n1 = n1 * n2;
        } else {
            n1 = n1 / n2;
        }
    } // while
    destroyset(set);
    return n1;
} // term

int const_expression(symset fsys) {
    int addop;
    symset set;
    int n = 0;
    set = uniteset(fsys, createset(SYM_PLUS, SYM_MINUS, SYM_NULL));
    if (last_sym_read == SYM_PLUS || last_sym_read == SYM_MINUS) {
        addop = last_sym_read;
        get_next_symbol();
        n = const_term(set);
        if (addop == SYM_MINUS) {
            n = -n;
        }
    } else {
        n = const_term(set);
    }

    while (last_sym_read == SYM_PLUS || last_sym_read == SYM_MINUS) {
        addop = last_sym_read;
        get_next_symbol();
        int n1 = const_term(set);
        if (addop == SYM_PLUS) {
            n = n + n1;
        } else {
            n = n - n1;
        }
    } // while

    destroyset(set);
    return n;
} // expression
int sx = 0;
void dim_declaration(void) {
    char idsaved[MAXIDLEN + 1] = {'\0'};
    int constexpre(symset fsys);
    strcpy(idsaved, id);
    dim = 0; //��ʼ��
    array_size = 1; //��ʼ��
    while (last_sym_read == SYM_LSQUARE)   //'['
    {
        get_next_symbol();
//        if (sym == SYM_NUMBER) {
//         latit[dim++] = last_num_read;
//         array_size *= last_num_read;
//         getsym();
//         } else {
//         error(26); //lack last_num_read
//         }
        symset set = createset(SYM_RSQUARE, SYM_NULL);
        int nn = const_expression(set);
        latit[dim++] = nn;
        destroyset(set);
        array_size *= nn;
        if (last_sym_read == SYM_RSQUARE)   //']'
        {
            get_next_symbol();
        } else {
            error(27); //lack ']'
        }
    }
    strcpy(id, idsaved);
    enter_obj_2_table(ID_ARRAY);
//    printf("ID_ARRAY:%d\n",ID_ARRAY);
//     array * mk=(array*)&table[tx];
//     printf("name:%s\n",mk->name);
//     printf("dim:%d\n",mk->dim_n);
//     printf("kind:%d\n",mk->kind);
//     p_dim* p=mk->next;
//     for(int b=0;b<mk->dim_n;b++){
//     printf("%d,",p->dim_len);
//     p=p->next;
//     }
//     printf("\n");
}
// tests if error occurs and skips all symbols that do not belongs to s1 or s2.
void optimize(mask *mk, int saveCx) {

    //printf("cx:	%d,	loop_level:	%d,	name:	%s\n",cx,loop_level,mk->name);

    //if (loop_level>0) return;
    if (!OPTM) return;
    evl ee = mk->evl;
    ee = ee->next;
    int cnt = 0;
    while (ee) {
        ee = ee->next;
        cnt++;
    }
    strcpy(id, mk->name);
    if (cnt > 1) {

        int i = get_identifier_id(id);
        if (!i) {
            enter_obj_2_table(ID_VARIABLE);
            mask *tp = (mask *) &table[tx];
            tp->quote = 1;
            table[tx].evl = mk->evl;
            gen_instruction(STO, level - tp->level, tp->address);
            tp->cnt++;
            gen_instruction(LOD, level - tp->level, tp->address);

        } else {
            int flag = 1;
            evl e1 = mk->evl;
            evl e2 = table[i].evl;

            //is_break_appear=jdgok(table[i].blkNum,block_num);
            flag = jdgok(block_num, table[i].blkNum);
            table[i].blkNum = block_num;


            e1 = e1->next;
            e2 = e2->next;
            while (e1) {
                if (e1->cnt > e2->cnt || e1->lpl > e2->lpl) {
                    e2->cnt = e1->cnt;
                    e2->lpl = e1->lpl;
                    flag = 0;
                }
                e1 = e1->next;
                e2 = e2->next;
            }
            if (flag) {
                printf("===============Improved in expression %s==============\n", table[i].name);
                if (sx) //term update
                {
                    current_instruction_index = sx;
                    printf("cx=%d\n", current_instruction_index);
                    sx = 0;
                } else {
                    current_instruction_index = saveCx;
                    mask *tp = (mask *) &table[i];
                    gen_instruction(LOD, level - tp->level, tp->address);
                }
            }


        }
    }

}

void optimize_term(mask *mk, int saveCx) {
    //if (loop_level>0) return;\OPTM
    if (!OPTM) return;
    evl ee = mk->evl;
    ee = ee->next;
    int cnt = 0;
    while (ee) {
        ee = ee->next;
        cnt++;
    }
    strcpy(id, mk->name);
    if (cnt > 1) {

        int i = get_identifier_id(id);
        if (!i) {
            //do nothing

            enter_obj_2_table(ID_VARIABLE);
            mask *tp = (mask *) &table[tx];
            tp->quote = 1;
            table[tx].evl = mk->evl;
            gen_instruction(STO, level - tp->level, tp->address);
            tp->cnt++;
            gen_instruction(LOD, level - tp->level, tp->address);
            sx = current_instruction_index;
        } else {
            sx = 0;
            int flag = 1;
            evl e1 = mk->evl;//next,next->next
            evl e2 = table[i].evl;//next,next->next




            e1 = e1->next;
            e2 = e2->next;
            while (e1) {
                if (e1->cnt > e2->cnt || e1->lpl > e2->lpl) {
                    //e2->cnt = e1->cnt;
                    //e2->lpl	= e1->lpl;
                    flag = 0;
                }
                e1 = e1->next;
                e2 = e2->next;
            }
            if (flag) {
                printf("===============Improved in term==============\n");
                current_instruction_index = saveCx;
                mask *tp = (mask *) &table[i];
                gen_instruction(LOD, level - tp->level, tp->address);
            }
        }
    }

}

mask *factor(symset fsys) {
    mask *mk = (mask *) malloc(sizeof(mask));
    mk->evl = create_evl(0, 0);
    mask *expression(symset fsys);
    int i;
    symset set;

    test(factor_begin_sys, fsys, 24); // The symbol can not be as the beginning of an expression.

    while (inset(last_sym_read, factor_begin_sys)) {
        if (last_sym_read == SYM_IDENTIFIER) {
            if ((i = get_identifier_id(id)) == 0) {
                error(11); // Undeclared identifier.
                get_next_symbol();
            } else {
                //printf("table[i].name=%s\n",table[i].name);
                strcpy(mk->name, table[i].name); //zq
                //printf("mk->name=%s\n",mk->name);
                mk->evl = create_evl(i, table[i].cnt, table[i].lpl, 0, 0, 0);  //0 -> SYM_NULL

                array *ar = (array *) &table[i];
                mask *tmk;
                switch (table[i].kind) {
                    case ID_CONSTANT:

                        gen_instruction(LIT, 0, table[i].value); //????
                        get_next_symbol();
                        break;
                    case ID_VARIABLE:
                        tmk = (mask *) &table[i];
                        gen_instruction(LOD, level - tmk->level, tmk->address);
                        get_next_symbol();
                        break;
                    case ID_PROCEDURE:;
                        mask *tp;
                        tp = (mask *) &table[i];

                        get_next_symbol();
                        if (last_sym_read == SYM_LPAREN) {
                            get_next_symbol();
                            if (last_sym_read == SYM_RPAREN) {
                                if (tp->numOfPar == 0)
                                    gen_instruction(CAL, level - tp->level, tp->address);
                                else
                                    error(34); // the number of segement doesn't match
                                get_next_symbol();
                                goto ff2;
                            } else {
                                int k = 0;
                                do {

                                    if (last_sym_read == SYM_COMMA)
                                        get_next_symbol();
                                    k++;
                                    symset set1 = createset(SYM_RPAREN, SYM_NULL);
                                    set = uniteset(set1, fsys);
                                    expression(set);
                                    destroyset(set);
                                    destroyset(set1);
                                    if (k > tp->numOfPar) {
                                        error(34); //too many segments
                                        break;
                                    }
                                } while (last_sym_read == SYM_COMMA);

                                if (last_sym_read == SYM_RPAREN) {
                                    if (tp->numOfPar == k)
                                        gen_instruction(CAL, level - tp->level, tp->address);
                                    else
                                        error(34); //error "didn't match"
                                    get_next_symbol();
                                    goto ff2;
                                } else {
                                    error(22); //')' missing
                                    goto ff2;
                                }
                            }
                        } else {
                            if (tp->numOfPar == 0)
                                gen_instruction(CAL, level - tp->level, tp->address);
                            else
                                error(34); //didn't match
                            goto ff2;

                        }
                        get_next_symbol();
                    ff2:;

                        gen_instruction(INT, 0, -tp->numOfPar + 1);
                        //gen(INT,0,1);
                        break;
                    default:
                        if ((ar->kind == ID_ARRAY)) { //??��?????m??
                            get_next_symbol();
                            int dl = 0;
                            p_dim *p = ar->next;
                            if (last_sym_read == SYM_LSQUARE) {
                                gen_instruction(LIT, 0, 0);
                                gen_instruction(LIT, 0, p->dim_len);
                                while (p && last_sym_read == SYM_LSQUARE) {
                                    p = p->next;
                                    gen_instruction(OPR, 0, OPR_MUL);
                                    get_next_symbol();
                                    expression(uniteset(fsys, createset(SYM_COMMA, SYM_NULL)));
                                    if (last_sym_read == SYM_RSQUARE) {
                                        get_next_symbol();
                                    } else {
                                        error(27);
                                    }
                                    gen_instruction(OPR, 0, OPR_ADD);
                                    if (p) {
                                        gen_instruction(LIT, 0, p->dim_len);
                                    }
                                }
                                gen_instruction(LAD, level - ar->level, ar->address);
                            } else {
                                error(28); //need'['
                            }
                        }
                        break;
                }// switch
            }
        } else if (last_sym_read == SYM_NUMBER) {
            if (last_num_read > MAXADDRESS) {
                error(25); // The number is too great.
                last_num_read = 0; //number????????0
            }
            sprintf(mk->name, "%d", last_num_read);
            mk->evl = create_evl(0, 0);
            gen_instruction(LIT, 0, last_num_read);
            get_next_symbol();
        } else if (last_sym_read == SYM_LPAREN) //(expression)
        {
            get_next_symbol();
            set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
            //expression(set);
            strcpy(mk->name, "("); //zq
            mask *mk2 = expression(set);
            strcat(mk->name, mk2->name);
            mk->evl = unite_evl(mk->evl, mk2->evl);
            strcat(mk->name, ")");
            destroyset(set);
            if (last_sym_read == SYM_RPAREN) {
                get_next_symbol();
            } else {
                error(22); // Missing ')'.
            }
        }
        //test(fsys, createset(SYM_LPAREN, SYM_NULL), 23);
    } // while

    return mk;
} // factor

mask *term(symset fsys) {
    int saveCx = current_instruction_index;
    mask *mk = (mask *) malloc(sizeof(mask));
    mk->evl = create_evl(0, 0);
    int mulop;
    symset set;

    set = uniteset(fsys, createset(SYM_TIMES, SYM_SLASH, SYM_NULL));

    mask *mk2 = factor(set);
    strcpy(mk->name, mk2->name);
    mk->evl = unite_evl(mk->evl, mk2->evl);
    while (last_sym_read == SYM_TIMES || last_sym_read == SYM_SLASH) {
        mulop = last_sym_read;
        if (last_sym_read == SYM_TIMES)
            strcat(mk->name, "*");
        else
            strcat(mk->name, "/");

        get_next_symbol();
        mask *mk2 = factor(set);
        strcat(mk->name, mk2->name);
        mk->evl = unite_evl(mk->evl, mk2->evl);
        if (mulop == SYM_TIMES) {
            gen_instruction(OPR, 0, OPR_MUL);
        } else {
            gen_instruction(OPR, 0, OPR_DIV);
        }
    } // while
    destroyset(set);
    if (OPTM)
        optimize_term(mk, saveCx);
    return mk;
} // term

mask *expression(symset fsys) {
    int saveCx = current_instruction_index;
    mask *mk = (mask *) malloc(sizeof(mask));
    mk->evl = create_evl(0, 0);
    int addop;
    symset set;

    set = uniteset(fsys, createset(SYM_PLUS, SYM_MINUS, SYM_NULL));
    if (last_sym_read == SYM_PLUS || last_sym_read == SYM_MINUS) {
        if (last_sym_read == SYM_PLUS)
            strcpy(mk->name, "+");
        else
            strcpy(mk->name, "-");
        addop = last_sym_read;
        get_next_symbol();
        mask *mk2 = term(set);
        strcat(mk->name, mk2->name);
        mk->evl = unite_evl(mk->evl, mk2->evl);
        if (addop == SYM_MINUS) {
            gen_instruction(OPR, 0, OPR_NEG);
        }
    } else {
        mask *mk2 = term(set);
        strcpy(mk->name, mk2->name);
        mk->evl = unite_evl(mk->evl, mk2->evl);
    }

    while (last_sym_read == SYM_PLUS || last_sym_read == SYM_MINUS) {
        optimize(mk, saveCx);
        if (last_sym_read == SYM_PLUS)
            strcat(mk->name, "+");
        else
            strcat(mk->name, "-");
        addop = last_sym_read;
        get_next_symbol();
        mask *mk2 = term(set);
        strcat(mk->name, mk2->name);
        mk->evl = unite_evl(mk->evl, mk2->evl);
        if (addop == SYM_PLUS) {
            gen_instruction(OPR, 0, OPR_ADD);
        } else {
            gen_instruction(OPR, 0, OPR_MIN);
        }
    } // while

    destroyset(set);
    //printf("mk->name: %s\n",mk->name);
    //printevl(mk->evl);
    optimize(mk, saveCx);


    return mk;
}

void condition_factor(symset fsys) {
    void condition(symset fsys);
    int i;
    symset set, set1;


    symset con_facbegsys = uniteset(factor_begin_sys, createset(SYM_ODD, SYM_NOT, SYM_LPAREN, SYM_NULL));
    test(con_facbegsys, fsys, 29); // the symbol can not be as the beginning of the condition

    while (inset(last_sym_read, con_facbegsys)) //conbeg
    {
        if (last_sym_read == SYM_LPAREN) {
            get_next_symbol();
            set1 = createset(SYM_RPAREN, SYM_NULL);
            set = uniteset(set1, fsys);
            condition(set);
            destroyset(set);
            destroyset(set1);
            if (last_sym_read == SYM_RPAREN) {
                get_next_symbol();
            } else {
                error(22); // Missing ')'
            }
        } else if (last_sym_read == SYM_ODD || last_sym_read == SYM_NOT) {
            int saveSym = last_sym_read;
            get_next_symbol();
            condition(fsys);
            if (saveSym == SYM_ODD)
                gen_instruction(OPR, 0, OPR_ODD);
            else
                gen_instruction(OPR, 0, OPR_NOT);
        } else {
            set = uniteset(rel_set, fsys);
            expression(fsys);
            destroyset(set);
            if (!inset(last_sym_read, rel_set)) {
                //error(20);
                test(fsys, phi, 20);
            } else {
                int relop = last_sym_read;
                get_next_symbol();
                expression(fsys);
                switch (relop) {
                    case SYM_EQU:
                        gen_instruction(OPR, 0, OPR_EQU);
                        break;
                    case SYM_NEQ:
                        gen_instruction(OPR, 0, OPR_NEQ);
                        break;
                    case SYM_LES:
                        gen_instruction(OPR, 0, OPR_LES);
                        break;
                    case SYM_GEQ:
                        gen_instruction(OPR, 0, OPR_GEQ);
                        break;
                    case SYM_GTR:
                        gen_instruction(OPR, 0, OPR_GTR);
                        break;
                    case SYM_LEQ:
                        gen_instruction(OPR, 0, OPR_LEQ);
                        break;
                } // switch
            }

        }
    }

}

void condition_term(symset fsys) {
    int saveCx[MAXLEVEL];
    int k = 0;
    symset set, set1;
    set1 = createset(SYM_AND, SYM_NULL);
    set = uniteset(fsys, set1);
    condition_factor(set);
    saveCx[k++] = current_instruction_index;
    gen_instruction(JPC, 0, 0);
    while (last_sym_read == SYM_AND) {
//		in_block();
        get_next_symbol();
        condition_factor(set);
        saveCx[k++] = current_instruction_index;
        gen_instruction(JPC, 0, 0);
    }
    destroyset(set);
    destroyset(set1);
    gen_instruction(LIT, 0, 1);
    int cx0 = current_instruction_index;
    gen_instruction(JMP, 0, 0);
    while (k > 0)
        code[saveCx[--k]].a = current_instruction_index;
    gen_instruction(LIT, 0, 0);
    code[cx0].a = current_instruction_index;
}

void condition(symset fsys) {
    int saveCx[MAXLEVEL];
    int k = 0;
    symset set, set1;
    set1 = createset(SYM_OR, SYM_NULL);
    set = uniteset(set1, fsys);
    condition_term(set);
    gen_instruction(OPR, 0, OPR_NOT);
    saveCx[k++] = current_instruction_index;
    gen_instruction(JPC, 0, 0);
    while (last_sym_read == SYM_OR) {
        in_block();//
        get_next_symbol();
        condition_term(set);
        gen_instruction(OPR, 0, OPR_NOT);
        saveCx[k++] = current_instruction_index;
        gen_instruction(JPC, 0, 0);
    }
    destroyset(set1);
    destroyset(set);
    gen_instruction(LIT, 0, 0);
    int cx0 = current_instruction_index;
    gen_instruction(JMP, 0, 0);
    while (k > 0)
        code[saveCx[--k]].a = current_instruction_index;
    gen_instruction(LIT, 0, 1);
    code[cx0].a = current_instruction_index;
}

void statement(symset fsys) {
    int i, cx1, cx2;
    symset set1, set;
    if (last_sym_read == SYM_IDENTIFIER){   // variable assignment
        mask *mk;
        i = get_identifier_id(id);
        array *ar = (array *) &table[i];
        if (!i) {
            error(11); // Undeclared identifier.
            get_next_symbol();
        } else if ((table[i].kind != ID_VARIABLE) && (ar->kind != ID_ARRAY)) {
            error(12); // Illegal assignment.
            i = 0;
        }
        get_next_symbol();
        if ((ar->kind == ID_ARRAY)) {
            int dl = 0;
            p_dim *p = ar->next;
            if (last_sym_read == SYM_LSQUARE) {
                gen_instruction(LIT, 0, 0);
                gen_instruction(LIT, 0, p->dim_len);
                while (last_sym_read == SYM_LSQUARE)   //cy_array
                {
                    if (p)   //cy_array
                    {
                        p = p->next;
                    }
                    gen_instruction(OPR, 0, OPR_MUL);
                    get_next_symbol();
                    expression(uniteset(fsys, createset(SYM_COMMA, SYM_NULL)));
                    dl++; //cy_array
                    if (last_sym_read == SYM_RSQUARE) {
                        get_next_symbol();
                    } else {
                        error(27);
                    }
                    gen_instruction(OPR, 0, OPR_ADD);
                    if (p) {
                        gen_instruction(LIT, 0, p->dim_len);
                    }
                }
                if (last_sym_read != SYM_BECOMES)   //cy_array
                {
                    set1 = createset(SYM_BECOMES, SYM_NULL);
                    test(set1, fsys, 0);
                    destroyset(set1);
                }
                if (dl != ar->dim_n)   //cy_array
                {
                    error(26);
                }
            } else {
                error(28); //need'['
            }
        }
        if (i) {
            if ((table[i].kind == ID_VARIABLE)) {
                if (last_sym_read == SYM_BECOMES) {
                    get_next_symbol();
                    expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
                    mk = (mask *) &table[i];
                    gen_instruction(STO, level - mk->level, mk->address);
                } else if (last_sym_read == SYM_ADDEQU) {

                    get_next_symbol();
                    expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
                    mk = (mask *) &table[i];
                    gen_instruction(LOD, level - mk->level, mk->address);
                    gen_instruction(OPR, 0, OPR_ADD);
                    gen_instruction(STO, level - mk->level, mk->address);
                } else if (last_sym_read == SYM_MULEQU) {
                    get_next_symbol();
                    expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
                    mk = (mask *) &table[i];
                    gen_instruction(LOD, level - mk->level, mk->address);
                    gen_instruction(OPR, 0, OPR_MUL);
                    gen_instruction(STO, level - mk->level, mk->address);
                } else if (last_sym_read == SYM_SUBEQU) {
                    get_next_symbol();
                    expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
                    mk = (mask *) &table[i];
                    gen_instruction(LOD, level - mk->level, mk->address);
                    gen_instruction(OPR, 0, OPR_MIN);
                    gen_instruction(OPR, 0, OPR_NEG);
                    gen_instruction(STO, level - mk->level, mk->address);
                } else if (last_sym_read == SYM_DIVEQU) {
                    get_next_symbol();
                    mk = (mask *) &table[i];
                    gen_instruction(LOD, level - mk->level, mk->address);
                    expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
                    gen_instruction(OPR, 0, OPR_DIV);
                    gen_instruction(STO, level - mk->level, mk->address);
                } else if (last_sym_read == SYM_MODEQU) {
                    get_next_symbol();
                    mk = (mask *) &table[i];
                    gen_instruction(LOD, level - mk->level, mk->address);
                    expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
                    gen_instruction(OPR, 0, OPR_MOD);
                    gen_instruction(STO, level - mk->level, mk->address);
                } else if (last_sym_read == SYM_ADDADD) {
                    get_next_symbol();
                    mk = (mask *) &table[i];
                    gen_instruction(LOD, level - mk->level, mk->address);
                    gen_instruction(LIT, 0, 1);
                    gen_instruction(OPR, 0, OPR_ADD);
                    gen_instruction(STO, level - mk->level, mk->address);
                } else if (last_sym_read == SYM_SUBSUB) {
                    get_next_symbol();
                    mk = (mask *) &table[i];
                    gen_instruction(LOD, level - mk->level, mk->address);
                    gen_instruction(LIT, 0, 1);
                    gen_instruction(OPR, 0, OPR_MIN);
                    gen_instruction(STO, level - mk->level, mk->address);

                } else error(13);

            } else {
                if (last_sym_read == SYM_BECOMES) {
                    get_next_symbol();
                    expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
                    mk = (mask *) &table[i];
                    gen_instruction(STA, level - mk->level, mk->address);
                } else error(13); // ':=' expected.
                // 
            }
        }
    } // if sym==SYM_IDENTIFIER
    else if (last_sym_read == SYM_EXIT) {
        get_next_symbol();
        if (last_sym_read == SYM_LPAREN) {
            get_next_symbol();
            expression(uniteset(fsys, createset(SYM_RPAREN, SYM_NULL)));
            //gen(STO,1,dx[level-1]-1);
            gen_instruction(STO, 0, -zx[level - 1]); //////////////////
            if (last_sym_read == SYM_RPAREN) {
                get_next_symbol();
            } else {
                error(22);
            }
        }
        gen_instruction(OPR, 0, OPR_RET); // return
    }
    else if (last_sym_read == SYM_BREAK) {
        if (break_code_index.is_in_loop_sign) {
            break_code_index_list pp;
            if (break_code_index.is_break_appear) {
                pp = break_code_index.then;
                while (pp->next) {
                    pp = pp->next;
                }
                pp->next = (break_code_index_list) malloc(sizeof(struct cxlink));
                pp = pp->next;
                pp->next = NULL;
            } else {
                break_code_index.is_break_appear = 1;
                pp = (break_code_index_list) malloc(sizeof(struct cxlink));
                break_code_index.then = pp;
                pp->next = NULL;
            }
            pp->break_code_index = current_instruction_index;
            gen_instruction(JMP, 0, 0);
            get_next_symbol();
        } else {
            error(35);
            if (last_sym_read == SYM_SEMICOLON) {
                get_next_symbol();
            }
        }
    }  //cy
    else if (last_sym_read == SYM_CALL) {
        get_next_symbol();
        if (last_sym_read != SYM_IDENTIFIER) {
            error(14); // There must be an identifier to follow the 'call'.
        } else {
            if (!(i = get_identifier_id(id))) {
                error(11); // Undeclared identifier.
                get_next_symbol();
            } else if (table[i].kind == ID_PROCEDURE) {
                mask *mk;
                mk = (mask *) &table[i];

                get_next_symbol();
                if (last_sym_read == SYM_LPAREN) {
                    get_next_symbol();
                    if (last_sym_read == SYM_RPAREN) {
                        if (mk->numOfPar == 0)
                            gen_instruction(CAL, level - mk->level, mk->address);
                        else
                            error(34); // the number of segement doesn't match
                        get_next_symbol();
                        goto ff2;
                    } else {
                        int k = 0;
                        do {

                            if (last_sym_read == SYM_COMMA)
                                get_next_symbol();
                            k++;
                            set1 = createset(SYM_RPAREN, SYM_NULL);
                            set = uniteset(set1, fsys);
                            expression(set);
                            destroyset(set);
                            destroyset(set1);
                            if (k > mk->numOfPar) {
                                error(34); //too many segments
                                break;
                            }
                        } while (last_sym_read == SYM_COMMA);

                        if (last_sym_read == SYM_RPAREN) {
                            if (mk->numOfPar == k)
                                gen_instruction(CAL, level - mk->level, mk->address);
                            else
                                error(34); //error "didn't match"
                            get_next_symbol();
                            goto ff2;
                        } else {
                            error(22); //')' missing
                            goto ff2;
                        }
                    }
                } else {
                    if (mk->numOfPar == 0)
                        gen_instruction(CAL, level - mk->level, mk->address);
                    else
                        error(34); //didn't match
                    goto ff2;

                }
            } else {
                error(15); // A constant or variable can not be called.
            }
            get_next_symbol();
            ff2:;
            mask *mk;
            mk = (mask *) &table[i];
            gen_instruction(INT, 0, -mk->numOfPar);
        }

    }  // procedure call
    else if (last_sym_read == SYM_REPEAT) {
        int saveBlkNum = block_num;
        int saveBlkLvl = block_level;
        set1 = createset(SYM_UNTIL, SYM_SEMICOLON, SYM_NULL);
        set = uniteset(set1, fsys);
        cx1 = current_instruction_index;
        get_next_symbol();

        break_code_block cxbsaved = break_code_index; //cy
        break_code_index.is_break_appear = 0; //cy
        break_code_index.is_in_loop_sign = 1; //cy
        break_code_index.then = NULL; //cy
        statement(set1);
        if (last_sym_read != SYM_SEMICOLON) {
            error(10); // "';' expected."
        } else {
            get_next_symbol();
            if (last_sym_read == SYM_UNTIL) {
                get_next_symbol();

                condition(set);
                gen_instruction(JPC, 0, cx1);

                if (break_code_index.is_break_appear)   //cy
                {
                    break_code_index_list p = break_code_index.then;
                    while (p) {
                        code[p->break_code_index].a = current_instruction_index;
                        break_code_index_list q = p;
                        free(p);
                        p = q->next;
                    }

                }
                break_code_index.is_break_appear = cxbsaved.is_break_appear; //cy
                break_code_index.then = cxbsaved.then; //cy
                break_code_index.is_in_loop_sign = cxbsaved.is_in_loop_sign; //cy

            } else
                error(31); //missing repeat

            out_block(saveBlkNum, saveBlkLvl);
        }
    }
    else if (last_sym_read == SYM_IF) {
        int saveBlkNum = block_num;
        int saveBlkLvl = block_level;
        get_next_symbol();
        set1 = createset(SYM_THEN, SYM_NULL); //modified by ZQ
        set = uniteset(set1, fsys);
        condition(set);
        in_block();
        destroyset(set1);
        destroyset(set);
        if (last_sym_read == SYM_THEN)
            get_next_symbol();
        else
            error(16); // 'then' expected.
        cx1 = current_instruction_index;
        gen_instruction(JPC, 0, 0);
        statement(uniteset(fsys, createset(SYM_ELSE, SYM_NULL)));

        if (last_sym_read == SYM_ELSE) {
            cx2 = current_instruction_index;
            gen_instruction(JMP, 0, 0);
            get_next_symbol();
            code[cx1].a = current_instruction_index;
            statement(fsys);
            code[cx2].a = current_instruction_index;
        } else {
            code[cx1].a = current_instruction_index;
        }

        out_block(saveBlkNum, saveBlkLvl);
    }
    else if (last_sym_read == SYM_BEGIN)
    {
        get_next_symbol();
        if (last_sym_read == SYM_VAR) {
            get_next_symbol();
            do {
                //varnum++;
                var_declaration();
                while (last_sym_read == SYM_COMMA) {
                    get_next_symbol();
                    //varnum++;
                    var_declaration();
                }
                if (last_sym_read == SYM_SEMICOLON) {
                    get_next_symbol();
                } else {
                    error(5); // Missing ',' or ';'.
                }
            } while (last_sym_read == SYM_IDENTIFIER);

        };

        set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
        set = uniteset(set1, fsys);
        statement(set);
        while (last_sym_read == SYM_SEMICOLON || inset(last_sym_read, statement_begin_sys)) {
            if (last_sym_read == SYM_SEMICOLON) {
                get_next_symbol();
            } else {
                error(10);
            }
            statement(set);
        } // while
        destroyset(set1);
        destroyset(set);
        if (last_sym_read == SYM_END) {
            get_next_symbol();
        } else {
            error(17); // ';' or 'end' expected.
        }
    } // block
    else if (last_sym_read == SYM_WHILE)
    {
        int saveBlkNum = block_num;
        int saveBlkLvl = block_level;
        cx1 = current_instruction_index;
        get_next_symbol();
        set1 = createset(SYM_DO, SYM_NULL);
        set = uniteset(set1, fsys);
        condition(set);
        in_block();
        destroyset(set1);
        destroyset(set);
        cx2 = current_instruction_index;
        gen_instruction(JPC, 0, 0);
        if (last_sym_read == SYM_DO) {
            get_next_symbol();
        } else {
            error(18); // 'do' expected.
        }

        break_code_block cxbsaved = break_code_index; //cy
        break_code_index.is_break_appear = 0; //cy
        break_code_index.is_in_loop_sign = 1; //cy
        break_code_index.then = NULL; //cy
        statement(fsys);
        gen_instruction(JMP, 0, cx1);
        code[cx2].a = current_instruction_index;
        out_block(saveBlkNum, saveBlkLvl);
        if (break_code_index.is_break_appear)   //cy
        {
            break_code_index_list p = break_code_index.then;
            while (p) {
                code[p->break_code_index].a = current_instruction_index;
                break_code_index_list q = p;
                //free(p);
                p = q->next;
            }
            break_code_index.then = NULL;
        }
        break_code_index.is_break_appear = cxbsaved.is_break_appear; //cy
        break_code_index.then = cxbsaved.then; //cy
        break_code_index.is_in_loop_sign = cxbsaved.is_in_loop_sign; //cy
    } // while statement
    else if (last_sym_read == SYM_WRITE || last_sym_read == SYM_WRITELN){
        int saveSym = last_sym_read;
        get_next_symbol();

        if (last_sym_read == SYM_LPAREN) {
            get_next_symbol();
        } else {
            error(33); //'(' expected
        }

        if (last_sym_read != SYM_RPAREN) {


            set1 = createset(SYM_COMMA, SYM_RPAREN, SYM_NULL);
            set = uniteset(set1, fsys);
            expression(set);
            destroyset(set1);
            destroyset(set);
            gen_instruction(OPR, 0, OPR_PRT); //what about the previous expression didn't make a propriate value

            while (last_sym_read == SYM_COMMA) {
                get_next_symbol();
                set1 = createset(SYM_RPAREN, SYM_NULL);
                set = uniteset(set1, fsys);
                expression(set);
                destroyset(set1);
                destroyset(set);
                gen_instruction(OPR, 0, OPR_PRT);
            }
        }
        if (last_sym_read == SYM_RPAREN) {
            get_next_symbol();
            if (saveSym == SYM_WRITELN) {
                gen_instruction(OPR, 0, OPR_WTL); // new line
            }
        } else {
            error(22); //"Missing ')'."
        }

    } //added by zq
    else if (last_sym_read == SYM_READ){
        get_next_symbol();
        if (last_sym_read == SYM_LPAREN) {
            get_next_symbol();
        } else {
            error(33); //'(' expected
        }
        //set1=createset(SYM_COMMA,SYM_PAREN,SYM_NULL);
        //set=uniteset(set1,fsys);
        if (last_sym_read == SYM_IDENTIFIER) {
            if ((i = get_identifier_id(id)) == 0) {
                error(11); //Undeclared identifier
                get_next_symbol();
            } else {
                switch (table[i].kind) {
                    mask *mk;
                    case ID_CONSTANT:
                        error(12); //Illegal assignment
                        break;
                    case ID_PROCEDURE:
                        error(12);
                        break;
                    case ID_VARIABLE:
                        mk = (mask *) &table[i];
                        gen_instruction(OPR, 0, OPR_RED);
                        gen_instruction(STO, level - mk->level, mk->address);
                        mk->cnt++;
                        break;
                }
            }
        } else {
            error(19); //incorrect symbol
        }
        get_next_symbol();
        while (last_sym_read == SYM_COMMA) {
            get_next_symbol();
            if (last_sym_read == SYM_IDENTIFIER) {
                if ((i = get_identifier_id(id)) == 0) {
                    error(11);
                    get_next_symbol();
                } else {
                    switch (table[i].kind) {
                        mask *mk;
                        case ID_CONSTANT:
                            error(12);
                            break;
                        case ID_PROCEDURE:
                            error(12);
                            break;
                        case ID_VARIABLE:
                            mk = (mask *) &table[i];
                            gen_instruction(OPR, 0, OPR_RED);
                            gen_instruction(STO, level - mk->level, mk->address);
                            mk->cnt++;
                            break;

                    }
                }
            } else {
                error(19);
            }
            get_next_symbol();
        }

        if (last_sym_read == SYM_RPAREN) {
            get_next_symbol();
        } else {
            error(22); //"Missing ')'."
        }

    }
    else if (last_sym_read == SYM_FOR) {

        int saveBlkNum = block_num;
        int saveBlkLvl = block_level;
        in_block();
        get_next_symbol();
        mask *mk;
        if (last_sym_read != SYM_IDENTIFIER)
            error(4);
        i = get_identifier_id(id);
        mk = (mask *) &table[i];
        if (i == 0)
            error(11);
        else if (table[i].kind != ID_VARIABLE) //ASSIGNMENT TO NON-VARIABLE
            error(12);
        get_next_symbol();
        if (last_sym_read != SYM_BECOMES) //:=
            error(13);
        get_next_symbol();
        set1 = createset(SYM_DOWNTO, SYM_DO, SYM_TO, SYM_NULL);
        set = uniteset(fsys, set1);
        expression(set);
        destroyset(set1);
        destroyset(set);
        if (last_sym_read == SYM_DOWNTO) {
            get_next_symbol();
            cx1 = current_instruction_index;
            gen_instruction(STO, level - mk->level, mk->address);
            mk->cnt++;
            gen_instruction(LOD, level - mk->level, mk->address);
            set1 = createset(SYM_DO, SYM_NULL);
            set = uniteset(fsys, set1);
            expression(set);
            destroyset(set1);
            destroyset(set);

            gen_instruction(OPR, 0, OPR_GEQ);
            cx2 = current_instruction_index;
            gen_instruction(JPC, 0, 0);
            if (last_sym_read == SYM_DO) {
                get_next_symbol();
                break_code_block cxbsaved = break_code_index; //cy
                break_code_index.is_break_appear = 0;
                break_code_index.is_in_loop_sign = 1;
                break_code_index.then = NULL;;

                set1 = createset(SYM_SEMICOLON, SYM_NULL);
                set = uniteset(set1, fsys);
                statement(set);
                destroyset(set1);
                destroyset(set);
                gen_instruction(LOD, level - mk->level, mk->address);
                gen_instruction(LIT, 0, STEP);
                gen_instruction(OPR, 0, OPR_MIN);
                gen_instruction(JMP, 0, cx1);
                code[cx2].a = current_instruction_index;
                if (break_code_index.is_break_appear) {
                    break_code_index_list p = break_code_index.then;
                    while (p) {
                        code[p->break_code_index].a = current_instruction_index;
                        break_code_index_list q = p;
                        free(p);
                        p = q->next;
                    }

                }
                break_code_index.is_break_appear = cxbsaved.is_break_appear; //cy
                break_code_index.then = cxbsaved.then; //cy
                break_code_index.is_in_loop_sign = cxbsaved.is_in_loop_sign; //cy

            } else
                error(18); //do expected
        }
        else if (last_sym_read == SYM_TO) {
            get_next_symbol();
            break_code_block cxbsaved = break_code_index;
            break_code_index.is_break_appear = 0;
            break_code_index.is_in_loop_sign = 1; //cy
            break_code_index.then = NULL;; //cy
            cx1 = current_instruction_index;
            gen_instruction(STO, level - mk->level, mk->address);
            mk->cnt++;
            gen_instruction(LOD, level - mk->level, mk->address);
            set1 = createset(SYM_DO, SYM_NULL);
            set = uniteset(fsys, set1);
            expression(set);
            destroyset(set1);
            destroyset(set);
            gen_instruction(OPR, 0, OPR_LEQ);
            cx2 = current_instruction_index;
            gen_instruction(JPC, 0, 0);
            if (last_sym_read == SYM_DO) {
                get_next_symbol();
                set1 = createset(SYM_SEMICOLON, SYM_NULL);
                set = uniteset(set1, fsys);
                statement(set);
                destroyset(set1);
                destroyset(set);
                gen_instruction(LOD, level - mk->level, mk->address);
                gen_instruction(LIT, 0, STEP);
                gen_instruction(OPR, 0, OPR_ADD);
                gen_instruction(JMP, 0, cx1);
                code[cx2].a = current_instruction_index;
                if (break_code_index.is_break_appear)   //cy
                {
                    break_code_index_list p = break_code_index.then;
                    while (p) {//一层for中可能有多个break，用list串起来,遍历list将其逐个回填
                        code[p->break_code_index].a = current_instruction_index;
                        break_code_index_list q = p;
                        free(p);
                        p = q->next;
                    }

                }
                break_code_index.is_break_appear = cxbsaved.is_break_appear; //cy
                break_code_index.then = cxbsaved.then; //cy
                break_code_index.is_in_loop_sign = cxbsaved.is_in_loop_sign; //cy

            } else
                error(18); //do expected
        } else
            error(30); //to or downto expected

        out_block(saveBlkNum, saveBlkLvl);
    }
    test(fsys, phi, 19);
} // statement

int get_array_size(int i){
    array *ar = (array *) &table[i];
    p_dim *p = ar->next;
    int c = 1;
    while (p) {
        c *= p->dim_len;
        p = p->next;
    }
    /*	for (int i = ar->dim_n; i; i--) {
     printf("i:%d",i);
     c *= p->dim_len;
     p=p->next;
     }*/
    return c;
}

int search_var(int len, int from)   //cy_quote
{
    int count = 0;
    int arsize(int i);
    int i;
    for (i = tx - from; len > 0; i--, len--) {
        array *ar = (array *) &table[i];
        if (table[i].quote == 0) {
            if (ar->kind == ID_ARRAY) {
                mask *mk = (mask *) &table[i];
                count += get_array_size(i);
            } else if (ar->kind == ID_VARIABLE) {
                count++;
            }
        }
    }
    return count;
}
int search_pro(int n)   //cy_quote
{
    int count = 0;
    int con = (tx - n + 1);
    for (int i = tx - n + 1; n; i++, n--) {
        if (table[i].kind == ID_PROCEDURE) {
            if (table[i].quote == 0) {
                proth[count] = i - con;
                count++;

            }
        }
    }
    return count;
}
void cut_code(int start, int end)   //cy_quote
{
    int len = end - start;
    for (int c = start; c + len < current_instruction_index; c++) {
        code[c].a = code[c + len].a;
        code[c].f = code[c + len].f;
        code[c].l = code[c + len].l;
        if ((code[c].f == JMP) || (code[c].f == JPC)) {
            code[c].a -= len;
        } else if (code[c].f == CAL) {
            if (code[c].a >= start) {
                code[c].a -= len;
            }
        }
    }
//printf("******************cx:%d\n", cx);
    current_instruction_index -= len;
}
int locate(int n, int star_tx, int end_tx, int f) {
    int c = 0;
    for (int i = star_tx; i < end_tx; i++) {
        mask *mk = (mask *) &table[i];
        if (mk->address == n) {
            break;
        }
        if ((mk->quote == 0) && (table[i].kind != ID_CONSTANT)) {
            if ((f == STA) || (f == LAD)) {
                c += get_array_size(i);
            } else if ((f == STO) || (f == LOD)) {
                c++;
            }
        }
    }
    return c;
}

void cut_pro_var_code(int star_cx, int end_cx, int star_tx, int end_tx) {
    if (star_cx < end_cx) {
        int l = 0;
        for (int i = star_cx; i < end_cx; i++) {
            if (code[i].f == JMP) {
                if (code[code[i].a].f == INT) {
                    l++;
                }
            }
            if ((code[i].f == OPR) && (code[i].a == OPR_RET)) {
                if (code[i + 1].f == INT) {
                    l--;
                }
                if (code[i + 1].f == JMP) {
                    i++;
                }
            }
            if (code[i].l == l) {
                if ((code[i].f == LOD) || (code[i].f == LAD)
                    || (code[i].f == STA) || (code[i].f == STO)) {
                    //again locate the statue
                    code[i].a -= locate(code[i].a, star_tx, end_tx, code[i].f);
                }
            }
        }
    }
}

void block(symset fsys) {
    int saveBlkNum = block_num;
    int saveBlkLvl = block_level;
    in_block();
    int cx0; // initial code index
    mask *mk;
    int savedTx;
    int var_n = 0; //cy_quote
    int pro_n = 0; //cy_quote
    symset set1, set;
    data_alloc_index[level] = 3;
    if (level == 0)
        mk = (mask *) &table[tx]; //mk -> procedure in the table
    else
        mk = (mask *) &table[tx - zx[level - 1]];
    mk->address = current_instruction_index;
    int jmp_code_index = current_instruction_index; //cy_quote
    gen_instruction(JMP, 0, 0);
    int var_num = 0; //cy_quote
    int pro_num = 0; //cy_quote
    procedure_list *pro = NULL; //cy_quote
    tx_[level] = tx + 1;
    if (level > MAXLEVEL) {
        error(32); // There are too many levels.
    }
    do {
        if (last_sym_read == SYM_CONST)   // constant declarations
        {
            get_next_symbol();
            do {
                const_declaration(fsys);
                while (last_sym_read == SYM_COMMA) {
                    get_next_symbol();
                    const_declaration(fsys);
                    printf("OK\n");
                }
                if (last_sym_read == SYM_SEMICOLON) {
                    get_next_symbol();
                } else {
                    error(5); // Missing ',' or ';'.
                    //	printf("there\n");
                }
            } while (last_sym_read == SYM_IDENTIFIER);
        } // if
        if (last_sym_read == SYM_VAR)   // variable declarations
        {
            get_next_symbol();
            do {
                var_num++;
                var_declaration();
                while (last_sym_read == SYM_COMMA) {
                    get_next_symbol();
                    var_num++;
                    var_declaration();
                }
                if (last_sym_read == SYM_SEMICOLON) {
                    get_next_symbol();
                } else {
                    error(5); // Missing ',' or ';'.
                }
            } while (last_sym_read == SYM_IDENTIFIER);
//			block = dx;
        } // if
        procedure_list *head = NULL;
        while (last_sym_read == SYM_PROCEDURE)   // procedure declarations
        {
            pro_num++; //cy_quote
            procedure_list *p = (procedure_list *) malloc(sizeof(procedure_list)); //cy_quote
            p->next = NULL;
            if (pro == NULL)   //cy_quote
            {
                pro = p;
            } else {
                head->next = p;
            }
            head = p;
            zx[level] = 0; //�������?
            get_next_symbol();
            if (last_sym_read == SYM_IDENTIFIER) {
                enter_obj_2_table(ID_PROCEDURE);
                head->table_adr = tx;
                get_next_symbol();
            } else {
                error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
            }

            if (last_sym_read == SYM_LPAREN) //added by zq
            {
                do {
                    get_next_symbol();
                    if (last_sym_read == SYM_IDENTIFIER) {
                        //declare
                        enter_par();
                        get_next_symbol();
                    } else if (last_sym_read == SYM_RPAREN) {
                        /*break;*/
                    } else {
                        error(19); //incorrect symbol
                        get_next_symbol();
                    }
                } while (last_sym_read == SYM_COMMA);

                if (last_sym_read == SYM_RPAREN) {
                    modify_table(zx[level]);
                } else {
                    error(22); //')' missing
                }
                get_next_symbol();
            } else {
            }

            if (last_sym_read == SYM_SEMICOLON) {
                get_next_symbol();
            } else {
                error(5); // Missing ',' or ';'.
            }
            level++;
            savedTx = tx;
            set1 = createset(SYM_SEMICOLON, SYM_NULL);
            set = uniteset(set1, fsys);
            head->start = current_instruction_index; //cy_quote
            block(set);
            head->end = current_instruction_index; //cy_quote
            destroyset(set1);
            destroyset(set);
            level--;
            tx = savedTx - zx[level]; //modified by zq.  need to substract the parameters.

            if (last_sym_read == SYM_SEMICOLON) {
                get_next_symbol();
                set1 = createset(SYM_IDENTIFIER, SYM_PROCEDURE, SYM_NULL);
                set = uniteset(statement_begin_sys, set1);
                test(set, fsys, 6);
                destroyset(set1);
                destroyset(set);
            } else {
                error(5); // Missing ',' or ';'.
            }
        } // while
        set1 = createset(SYM_IDENTIFIER, SYM_NULL);
        set = uniteset(statement_begin_sys, set1);
        test(set, decleration_begin_sys, 7);
        destroyset(set1);
        destroyset(set);
    } while (inset(last_sym_read, decleration_begin_sys));

    code[mk->address].a = current_instruction_index;
    mk->address = current_instruction_index;
    if (level == 0)
        mk->numOfPar = zx[level]; //added by zq
    else
        mk->numOfPar = zx[level - 1];
    cx0 = current_instruction_index; //procedure enter_obj_2_table address
    gen_instruction(INT, 0, data_alloc_index[level]);
    set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
    set = uniteset(set1, fsys);
    statement(set);
    destroyset(set1);
    destroyset(set);

    code[cx0].a = data_alloc_index[level];

    if (OPTM_CY) {
        var_num = data_alloc_index[level] - 2;
        var_n = search_var(var_num, pro_num); //cy_quote
        //int var_n2 = search_var(var_num, pro_num, 2); //cy_quote
        pro_n = search_pro(pro_num); //cy_quote
        if (var_n) {
            cut_pro_var_code(cx0, current_instruction_index, tx_[level], tx - pro_n); //cy_quote
            cut_pro_var_code(jmp_code_index + 1, cx0, tx_[level], tx - pro_n); //cy_quote
        }

        code[cx0].a -= var_n; //cy_quote
        if (pro_n)   //如果有无用的过程
        {
            procedure_list *q = pro;
            int proth_formor = -1;
            for (int k = 0; k < pro_n; k++) {
                for (int n = proth[k] - proth_formor; n != 1; n--) {
                    q = q->next;
                }
                proth_formor = proth[k];
                int codelen = q->end - q->start; //对应过程的长�?
                for (procedure_list *qq = q->next; qq; qq = qq->next) {
                    qq->start -= codelen;
                    qq->end -= codelen;
                    //			mask* mk = (mask*) &table[qq->table_adr]; //已被调用过程的table
                    //			mk->address -= codelen;
                }
                cut_code(q->start, q->end); //剪掉代码段，改变jmp\jpc\call
                code[jmp_code_index].a -= codelen;
                cx0 -= codelen; //cx0为第一条INT指令，相当于本block的statement的首地址
            }
        }
    }

    gen_instruction(OPR, 0, OPR_RET); // return
    test(fsys, phi, 8); // test for error: Follow the statement is an incorrect symbol.
    list_code(cx0, current_instruction_index);
    print_table();

    out_block(saveBlkNum, saveBlkLvl);
} // block

int get_base_addr(int stack[], int currentLevel, int levelDiff) {
    int base_reg = currentLevel;

    while (levelDiff--)
        base_reg = stack[base_reg];
    return base_reg;
} // get_base_addr

// interprets and executes codes.
void interpret() {
    int pc; // program counter
    int stack[STACKSIZE];
    int top; // top of stack
    int b; // program, get_base_addr, and top-stack register
    instruction i; // instruction register

    printf("Begin executing PL/0 program.\n");

    pc = 0;
    b = 1;
    top = 3;
    stack[1] = stack[2] = stack[3] = 0;
    do {
        i = code[pc++];
        switch (i.f) {
            case LIT:
                stack[++top] = i.a;
                break;
            case OPR:
                switch (i.a) // operator
                {
                    case OPR_RET:
                        top = b - 1;
                        pc = stack[top + 3];
                        b = stack[top + 2];
                        break;
                    case OPR_NEG:
                        stack[top] = -stack[top];
                        break;
                    case OPR_ADD:
                        top--;
                        stack[top] += stack[top + 1];
                        break;
                    case OPR_MIN:
                        top--;
                        stack[top] -= stack[top + 1];
                        break;
                    case OPR_MUL:
                        top--;
                        stack[top] *= stack[top + 1];
                        break;
                    case OPR_DIV:
                        top--;
                        if (stack[top + 1] == 0) {
                            fprintf(stderr, "Runtime Error: Divided by zero.\n");
                            fprintf(stderr, "Program terminated.\n");
                            continue;
                        }
                        stack[top] /= stack[top + 1];
                        break;
                    case OPR_MOD:
                        top--;
                        stack[top] %= stack[top + 1];
                    case OPR_ODD:
                        stack[top] %= 2;
                        break;
                    case OPR_NOT:
                        stack[top] = !stack[top];
                        break;
                    case OPR_EQU:
                        top--;
                        stack[top] = stack[top] == stack[top + 1];
                        break;
                    case OPR_NEQ:
                        top--;
                        stack[top] = stack[top] != stack[top + 1];
                        break;
                    case OPR_LES:
                        top--;
                        stack[top] = stack[top] < stack[top + 1];
                        break;
                    case OPR_GEQ:
                        top--;
                        stack[top] = stack[top] >= stack[top + 1];
                        break;
                    case OPR_GTR:
                        top--;
                        stack[top] = stack[top] > stack[top + 1];
                        break;
                    case OPR_LEQ:
                        top--;
                        stack[top] = stack[top] <= stack[top + 1];
                        break;
                    case OPR_AND:
                        top--;
                        stack[top] = stack[top] & stack[top + 1];
                        break;
                    case OPR_OR:
                        top--;
                        stack[top] = stack[top] | stack[top + 1];
                        break;
                    case OPR_PRT:
                        printf("%d ", stack[top]);
                        top--;;
                        break;
                    case OPR_WTL:
                        printf("\n");
                        break;
                    case OPR_RED:
                        scanf("%d", &stack[++top]);
                        break;

                    default:
                        fprintf(stderr, "No Such OPR_CODE=%d.\n", i.a);
                        break;
                } // switch
                break;
            case LOD:
                stack[++top] = stack[get_base_addr(stack, b, i.l) + i.a];
                break;
            case LAD:
                stack[top] = stack[get_base_addr(stack, b, i.l) + stack[top] + i.a];
                break;
            case STO:
                stack[get_base_addr(stack, b, i.l) + i.a] = stack[top];
                //printf("%d\n", stack[top]);
                top--;
                break;
            case STA: //ƫ����ȡ��ջ��Ԫ��
                stack[get_base_addr(stack, b, i.l) + stack[top - 1] + i.a] = stack[top];
                //printf("%d\n", stack[top]);
                top -= 2;
                break;
            case CAL:
                stack[top + 1] = get_base_addr(stack, b, i.l);
                // generate new block mark
                stack[top + 2] = b;
                stack[top + 3] = pc;
                b = top + 1;
                pc = i.a;
                break;
            case INT:
                top += i.a;
                break;
            case JMP:
                pc = i.a;
                break;
            case JPC:
                if (stack[top] == 0)
                    pc = i.a;
                top--;
                break;
        } // switch
    } while (pc);

    printf("End executing PL/0 program.\n");
} // interpret

void main(int argc, char *argv[]) {
    FILE *hbin;
    FILE *hasm;
    char s[80];
    int i;
    symset set, set1, set2;

    if (argc == 1)
        strcpy(s, "../example/array.txt");
    else
        strcpy(s, argv[1]);
    if ((infile = fopen(s, "r")) == NULL) {
        printf("File %s can't be opened.\n", s);
        exit(1);
    }
    phi = createset(SYM_NULL);
    rel_set = createset(SYM_EQU, SYM_NEQ, SYM_LES, SYM_LEQ, SYM_GTR, SYM_GEQ,
                        SYM_NULL);

    // create begin symbol sets
    decleration_begin_sys = createset(SYM_CONST, SYM_VAR, SYM_PROCEDURE, SYM_NULL);
    statement_begin_sys = createset(SYM_BEGIN, SYM_CALL, SYM_IF, SYM_WHILE, SYM_NULL);
    factor_begin_sys = createset(SYM_IDENTIFIER, SYM_NUMBER, SYM_LPAREN, SYM_NULL);

    err = char_count = current_instruction_index = line_length = 0; // initialize global variables
    last_char_read = ' ';
    kk = MAXIDLEN;

    get_next_symbol();
    break_code_index.is_break_appear = 0; //cy
    break_code_index.is_in_loop_sign = 0; //cy
    break_code_index.then = NULL; //cy
    set1 = createset(SYM_PERIOD, SYM_NULL);
    set2 = uniteset(decleration_begin_sys, statement_begin_sys);
    set = uniteset(set1, set2);
    block(set);
    destroyset(set1);
    destroyset(set2);
    destroyset(set);
    destroyset(phi);
    destroyset(rel_set);
    destroyset(decleration_begin_sys);
    destroyset(statement_begin_sys);
    destroyset(factor_begin_sys);

    if (last_sym_read != SYM_PERIOD)
        error(9); // '.' expected.
    if (err)
        printf("There are %d error(s) in PL/0 program.\n", err);
    else {
        printf("There is no error in PL/0 program.\n");
        list_code(0, current_instruction_index);
        interpret();
    }
    printf("OPTM=%d,	OPTM_CY=%d\n", OPTM, OPTM_CY);
}
