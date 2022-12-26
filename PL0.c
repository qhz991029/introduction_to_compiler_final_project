// pl0 compiler source code
//for Visual Studio
#pragma warning(disable:4996)
#include "PL0.h"
#include "set.h"

int data_alloc_index[MAXLEVEL]; // data allocation index
int zx[MAXLEVEL];

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
            line[++line_length] = last_char_read; // 从第二个位置开始存放
        } // while
        printf("\n");
        line[++line_length] = ' ';
        line[line_length + 1] = '\0';
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
            last_sym_read = SYM_COLON; // :

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
    if (current_instruction_index < 0 ){
        exit(0);
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
    if(last_sym_read == SYM_CALL){
        int i=1;
    }

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

        } else if (last_sym_read == SYM_MINUS) {
            get_next_symbol();
            n = const_factor(factor_begin_sys) * (-1);
            destroyset(set);

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
mask *factor(symset fsys) {
    int i;
    symset set;

    test(factor_begin_sys, fsys, 24);// The symbol can not be as the beginning of an expression.
    while (inset(last_sym_read, factor_begin_sys)) {
        if (last_sym_read == SYM_IDENTIFIER) {
            if ((i = get_identifier_id(id)) == 0) {
                error(11); // Undeclared identifier.
                get_next_symbol();
            }
            get_next_symbol();
            if(last_sym_read == SYM_BECOMES){
                 // variable assignment
                    mask *mk;
                    array *ar = (array *) &table[i];
                    if (!i) {
                        error(11); // Undeclared identifier.
                        get_next_symbol();
                    } else if ((table[i].kind != ID_VARIABLE) && (ar->kind != ID_ARRAY)) {
                        error(12); // Illegal assignment.
                        i = 0;
                    }
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
                                symset set1 = createset(SYM_BECOMES, SYM_NULL);
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
                                gen_instruction(LOD,level - mk->level, mk->address);
                            } else if (last_sym_read == SYM_ADDEQU) {
                                get_next_symbol();
                                expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
                                mk = (mask *) &table[i];
                                gen_instruction(LOD, level - mk->level, mk->address);
                                gen_instruction(OPR, 0, OPR_ADD);
                                gen_instruction(STO, level - mk->level, mk->address);
                                gen_instruction(LOD,level - mk->level, mk->address);
                            } else if (last_sym_read == SYM_MULEQU) {
                                get_next_symbol();
                                expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
                                mk = (mask *) &table[i];
                                gen_instruction(LOD, level - mk->level, mk->address);
                                gen_instruction(OPR, 0, OPR_MUL);
                                gen_instruction(STO, level - mk->level, mk->address);
                                gen_instruction(LOD,level - mk->level, mk->address);
                            } else if (last_sym_read == SYM_SUBEQU) {
                                get_next_symbol();
                                expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
                                mk = (mask *) &table[i];
                                gen_instruction(LOD, level - mk->level, mk->address);
                                gen_instruction(OPR, 0, OPR_MIN);
                                gen_instruction(OPR, 0, OPR_NEG);
                                gen_instruction(STO, level - mk->level, mk->address);
                                gen_instruction(LOD,level - mk->level, mk->address);
                            } else if (last_sym_read == SYM_DIVEQU) {
                                get_next_symbol();
                                mk = (mask *) &table[i];
                                gen_instruction(LOD, level - mk->level, mk->address);
                                expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
                                gen_instruction(OPR, 0, OPR_DIV);
                                gen_instruction(STO, level - mk->level, mk->address);
                                gen_instruction(LOD,level - mk->level, mk->address);
                            } else if (last_sym_read == SYM_MODEQU) {
                                get_next_symbol();
                                mk = (mask *) &table[i];
                                gen_instruction(LOD, level - mk->level, mk->address);
                                expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
                                gen_instruction(OPR, 0, OPR_MOD);
                                gen_instruction(STO, level - mk->level, mk->address);
                                gen_instruction(LOD,level - mk->level, mk->address);
                            } else if (last_sym_read == SYM_ADDADD) {
                                get_next_symbol();
                                mk = (mask *) &table[i];
                                gen_instruction(LOD, level - mk->level, mk->address);
                                gen_instruction(LIT, 0, 1);
                                gen_instruction(OPR, 0, OPR_ADD);
                                gen_instruction(STO, level - mk->level, mk->address);
                                gen_instruction(LOD,level - mk->level, mk->address);
                            } else if (last_sym_read == SYM_SUBSUB) {
                                get_next_symbol();
                                mk = (mask *) &table[i];
                                gen_instruction(LOD, level - mk->level, mk->address);
                                gen_instruction(LIT, 0, 1);
                                gen_instruction(OPR, 0, OPR_MIN);
                                gen_instruction(STO, level - mk->level, mk->address);
                                gen_instruction(LOD,level - mk->level, mk->address);
                            } else error(13);

                        } else {
                            if (last_sym_read == SYM_BECOMES) {
                                get_next_symbol();
                                expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
                                mk = (mask *) &table[i];
                                gen_instruction(LMT,0,0);
                                gen_instruction(LMT,0,0);
                                gen_instruction(STA, level - mk->level, mk->address);
                                gen_instruction(POP,0,0);
                                gen_instruction(LAD,level - mk->level, mk->address);
                            } else error(13); // ':=' expected.
                            //
                        }
                    }
            }else{
                array *ar = (array *) &table[i];
                mask *tmk;
                switch (table[i].kind) {
                    case ID_CONSTANT:
                        gen_instruction(LIT, 0, table[i].value); //????
                        break;
                    case ID_VARIABLE:
                        tmk = (mask *) &table[i];
                        gen_instruction(LOD, level - tmk->level, tmk->address);
                        break;
                    default:
                        if ((ar->kind == ID_ARRAY)) { //??��?????m??
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
        }
        else if (last_sym_read == SYM_NUMBER) {
            if (last_num_read > MAXADDRESS) {
                error(25); // The number is too great.
                last_num_read = 0; //number????????0
            }
            gen_instruction(LIT, 0, last_num_read);
            get_next_symbol();
        }
        else if (last_sym_read == SYM_LPAREN)
        {
            get_next_symbol();
            set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
            condition(set);
            destroyset(set);
            if (last_sym_read == SYM_RPAREN) {
                get_next_symbol();
            } else {
                error(22); // Missing ')'.
            }
        }
        else if(last_sym_read == SYM_SETJMP){
            get_next_symbol();
            if(last_sym_read != SYM_LPAREN){
                error(38);
            }
            get_next_symbol();
            symset set1 = createset(SYM_COMMA, SYM_RPAREN, SYM_NULL);
            set = uniteset(set1, fsys);
            int buf_idx = const_expression(set);
            jmp_table.buf_status[buf_idx] = allocated;
            setjmp_set[buf_idx].level = level;
            if (jmp_table.is_in_condition_block == inside){
                setjmp_set[buf_idx].stack_top_address = jmp_table.condition_stack_top_addr;
                setjmp_set[buf_idx].return_val_address = data_alloc_index[level];
                data_alloc_index[level]++;
                jmp_table.total_jump_buf_num += 1;
            }else{
                setjmp_set[buf_idx].stack_top_address = data_alloc_index[level];
                setjmp_set[buf_idx].return_val_address = data_alloc_index[level] + 2;
                data_alloc_index[level] += 3;
                jmp_table.total_jump_buf_num += 3;
                gen_instruction(STP,0,setjmp_set[buf_idx].stack_top_address);
            }
            setjmp_set[buf_idx].jmp_entry_pc = jmp_table.current_jmp_entry_pc;
            int load_ret_val_index = current_instruction_index;
            gen_instruction(LOD,0,0);
            code[load_ret_val_index].a = setjmp_set[buf_idx].return_val_address;

            destroyset(set1);
            destroyset(set);
            if(last_sym_read != SYM_RPAREN){
                error(22);
            }
            get_next_symbol();
        }
        else{
            break;
        }
        //test(fsys, createset(SYM_LPAREN, SYM_NULL), 23);
    } // while
} // factor

mask *term(symset fsys) {
    int saveCx = current_instruction_index;
    int mulop;
    symset set;

    set = uniteset(fsys, createset(SYM_TIMES, SYM_SLASH, SYM_NULL));

    mask *mk2 = factor(set);
    while (last_sym_read == SYM_TIMES || last_sym_read == SYM_SLASH) {
        mulop = last_sym_read;
        get_next_symbol();
        mask *mk2 = factor(set);
        if (mulop == SYM_TIMES) {
            gen_instruction(OPR, 0, OPR_MUL);
        } else {
            gen_instruction(OPR, 0, OPR_DIV);
        }
    } // while
    destroyset(set);
} // term

mask *expression(symset fsys) {
    int saveCx = current_instruction_index;
    int addop;
    symset set;

    set = uniteset(fsys, createset(SYM_PLUS, SYM_MINUS, SYM_NULL));
    if (last_sym_read == SYM_PLUS || last_sym_read == SYM_MINUS) {
        addop = last_sym_read;
        get_next_symbol();
        mask *mk2 = term(set);
        if (addop == SYM_MINUS) {
            gen_instruction(OPR, 0, OPR_NEG);
        }
    }
    else {
        mask *mk2 = term(set);
    }
    while (last_sym_read == SYM_PLUS || last_sym_read == SYM_MINUS) {
        addop = last_sym_read;
        get_next_symbol();
        mask *mk2 = term(set);
        if (addop == SYM_PLUS) {
            gen_instruction(OPR, 0, OPR_ADD);
        } else {
            gen_instruction(OPR, 0, OPR_MIN);
        }
    } // while
    destroyset(set);
}

void condition(symset fsys) {
    int i;
    symset set, set1;
    symset con_facbegsys = uniteset(factor_begin_sys, createset(SYM_ODD, SYM_NOT, SYM_LPAREN, SYM_NULL));
    test(con_facbegsys, fsys, 29); // the symbol can not be as the beginning of the condition

    while (inset(last_sym_read, con_facbegsys)) //conbeg
    {
        if (last_sym_read == SYM_ODD || last_sym_read == SYM_NOT){
            int saveSym = last_sym_read;
            get_next_symbol();
            condition(fsys);
            if (saveSym == SYM_ODD)
                gen_instruction(OPR, 0, OPR_ODD);
            else
                gen_instruction(OPR, 0, OPR_NOT);
        }
        else {
            set = uniteset(rel_set, fsys);
            expression(fsys);
            destroyset(set);
            if (!inset(last_sym_read, rel_set))
            {
                //error(20);
                test(fsys, phi, 20);
            }
            else {
                int relop = last_sym_read;
                get_next_symbol();
                expression(fsys);
                switch (relop)
                {
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

void statement(symset fsys) {
    int i, cx1, cx2;
    symset set1, set;
    if (last_sym_read == SYM_IDENTIFIER){   // variable assignment
        mask *mk;
        jmp_table.current_jmp_entry_pc = current_instruction_index;

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

        jmp_table.is_in_condition_block = inside;
        jmp_table.current_jmp_entry_pc = current_instruction_index;
        jmp_table.total_jump_buf_num += 2;//done
        jmp_table.condition_stack_top_addr = data_alloc_index[level];
        gen_instruction(STP,level,data_alloc_index[level]);
        data_alloc_index[level] += 2;

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
        jmp_table.is_in_condition_block = outside;
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

        jmp_table.is_in_condition_block = inside;
        jmp_table.current_jmp_entry_pc = current_instruction_index;
        jmp_table.total_jump_buf_num += 2;//done
        jmp_table.condition_stack_top_addr = data_alloc_index[level];
        gen_instruction(STP,level,data_alloc_index[level]);
        data_alloc_index[level] += 2;

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

        jmp_table.is_in_condition_block = inside;
    } // while statement
    else if (last_sym_read == SYM_PRINT){
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
        }else{
            gen_instruction(OPR, 0, OPR_WTL); // new line
        }

        if (last_sym_read == SYM_RPAREN) {
            get_next_symbol();
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
        break_code_block cxbsaved = break_code_index; //cy
        break_code_index.is_break_appear = 0;
        break_code_index.is_in_loop_sign = 1;
        break_code_index.then = NULL;
        // for ( var id:(
        get_next_symbol();
        mask *mk;
        if (last_sym_read != SYM_LPAREN){
            error(19);//missing (
        }
        get_next_symbol();
        if (last_sym_read != SYM_VAR){
            error(19);//missing VAR
        }
        get_next_symbol();
        if (last_sym_read != SYM_IDENTIFIER){
            error(4);// id
        }else{
            i = get_identifier_id(id);
        }
        if (i == 0) {
            tx++;
            strcpy(table[tx].name, id);
            table[tx].kind = ID_VARIABLE;
            mk = (mask *)&table[tx];
            mk->address = data_alloc_index[level]++;
            mk->level = level;
            jmp_table.total_jump_buf_num += 1;
        }else{
            mk = (mask *) &table[i];
            if (table[i].kind != ID_VARIABLE) //ASSIGNMENT TO NON-VARIABLE
                error(12);
        }
        get_next_symbol();
        if (last_sym_read != SYM_COLON) //:
            error(13);
        get_next_symbol();
        if (last_sym_read != SYM_LPAREN){
            error(19);// (
        }

        set1 = createset(SYM_COMMA,SYM_NULL);
        set = uniteset(fsys, set1);
        //start
        get_next_symbol();
        expression(set);

        destroyset(set1);
        destroyset(set);

        gen_instruction(STO, level - mk->level, mk->address);
        mk->cnt++;
        gen_instruction(LOD, level - mk->level, mk->address);

        // ", end )" or ", end, step )"
        if(last_sym_read == SYM_COMMA) {

            set1 = createset(SYM_COMMA,SYM_NULL);
            set = uniteset(set1, fsys);

            get_next_symbol();
            int copy_exp_start = current_instruction_index;
            expression(set);
            int copy_exp_end = current_instruction_index;

            destroyset(set1);
            destroyset(set);

            //start<end?以第一次进入for时两者大小关系为准，当某次循环后这种关系翻转时终止for loop
            gen_instruction(LMT,0,1);
            gen_instruction(LMT,0,1);
            gen_instruction(OPR,0,OPR_EQU);
            int first_equal_jmp = current_instruction_index;
            gen_instruction(JPC,0,0);
            gen_instruction(POP,0,0);
            int end_j = current_instruction_index;
            gen_instruction(JMP,0,0);
            code[first_equal_jmp].a = current_instruction_index;
            gen_instruction(OPR,0,OPR_LES);
            int loop_step;
            int is_step_expression = 0;
            //计算step
            int for_loop_entry = current_instruction_index;
            if(last_sym_read == SYM_RPAREN){//step为1
                loop_step = 1;
                gen_instruction(LIT, 0, loop_step);
                get_next_symbol();
                goto LOOP;
            } else if(last_sym_read == SYM_COMMA){//step为expression
                is_step_expression = 1;
                set1 = createset(SYM_COMMA, SYM_NULL);
                set = uniteset(set1, fsys);

                get_next_symbol();
                expression(set);

                destroyset(set1);
                destroyset(set);
                get_next_symbol();
                goto LOOP;
            } else{
                error(33);
            }
            //
            //
            //LOOP:
            //          step = 1 / step = expression
            //          statement
            //          var i = i + step
            //          end = expression(end)
            //          if i reach end ?
            //          Yes -> leave
            //          NO  -> goto LOOP
            LOOP:;
            if(last_sym_read == SYM_RPAREN){
                set1 = createset(SYM_SEMICOLON,SYM_NULL);
                set = uniteset(set1, fsys);
                get_next_symbol();
                statement(set);
                destroyset(set1);
                destroyset(set);

                gen_instruction(LOD, level - mk->level, mk->address);
                gen_instruction(OPR, 0, OPR_ADD);
                gen_instruction(STO, level - mk->level, mk->address);
                mk->cnt++;
                gen_instruction(LOD, level - mk->level, mk->address);

                int copy_temp = 0;
                while (copy_temp != copy_exp_end - copy_exp_start){
                    code[current_instruction_index + copy_temp] = code[copy_exp_start + copy_temp];
                    copy_temp++;
                }
                current_instruction_index += copy_temp;
                gen_instruction(LMT,0,1);
                gen_instruction(LMT,0,1);
                gen_instruction(OPR,0,OPR_EQU);
                int equal_jmp = current_instruction_index;
                gen_instruction(JPC,0,0);
                gen_instruction(POP,0,0);
                gen_instruction(POP,0,0);
                int end_jump = current_instruction_index;
                gen_instruction(JMP,0,0);
                code[equal_jmp].a = current_instruction_index;
                gen_instruction(OPR, 0, OPR_LEQ);//A<=B?
                gen_instruction(LMT,0,1);//Entry A<B?
                gen_instruction(OPR,0,OPR_NEQ);
                int final_jump_point = current_instruction_index;
                gen_instruction(JPC,0,0);
                int out_code_index = current_instruction_index;
                code[final_jump_point].a = for_loop_entry;
                code[end_jump].a = current_instruction_index;
                code[end_j].a = current_instruction_index;
                gen_instruction(POP,0,0);
            }else{
                error(0);//TODO:missing )
            }
        }else{
            error(10);//TODO:missing ,
        }
        out_block(saveBlkNum, saveBlkLvl);
    }
    else if (last_sym_read == SYM_SETJMP){
        int entry_pc = current_instruction_index;
        jmp_table.current_jmp_entry_pc = entry_pc;
        get_next_symbol();
        if(last_sym_read != SYM_LPAREN){
            error(38);
        }
        get_next_symbol();
        set1 = createset(SYM_COMMA, SYM_RPAREN, SYM_NULL);
        set = uniteset(set1, fsys);
        int buf_idx = const_expression(set);
        //为跳转分配的空间+2，用来储存该跳跃点的返回值栈顶
        jmp_table.total_jump_buf_num += 3;//done
        jmp_table.buf_status[buf_idx] = allocated;

        setjmp_point *new_jmp = &setjmp_set[buf_idx];
        new_jmp->level = level;
        new_jmp->return_val_address = data_alloc_index[level] + 2;
        new_jmp->stack_top_address = data_alloc_index[level];
        new_jmp->jmp_entry_pc = entry_pc;
        data_alloc_index[level] += 2;

        gen_instruction(STP,0,new_jmp->stack_top_address);
        gen_instruction(LOD,0,new_jmp->return_val_address);

        destroyset(set1);
        destroyset(set);
        if(last_sym_read != SYM_RPAREN){
            error(33);
        }
        get_next_symbol();
    }
    else if (last_sym_read == SYM_LONGJMP){
        get_next_symbol();
        if(last_sym_read != SYM_LPAREN){
            error(38);
        }
        get_next_symbol();
        set1 = createset(SYM_COMMA, SYM_RPAREN, SYM_NULL);
        set = uniteset(set1, fsys);
        int buf_idx = const_expression(set);
        destroyset(set1);
        destroyset(set);
        if(last_sym_read != SYM_COMMA){
            error(5);
        }
        get_next_symbol();
        set1 = createset(SYM_COMMA, SYM_RPAREN, SYM_NULL);
        set = uniteset(set1, fsys);
        int return_val = const_expression(set);
        destroyset(set1);
        destroyset(set);

        longjmp_point * new_longjmp = (longjmp_point *) malloc(sizeof (longjmp_point));
        new_longjmp->next = longjmp_set;
        longjmp_set = new_longjmp;
        new_longjmp->jmp_buf_id = buf_idx;
        new_longjmp->return_value = return_val;
        new_longjmp->jmp_level = level;
        gen_instruction(LIT,0,return_val);
        new_longjmp->save_return_value_pc = current_instruction_index;

        gen_instruction(STO,0,0);
        new_longjmp->load_stack_top_pc = current_instruction_index;
        gen_instruction(LTP,0,0);
        new_longjmp->jmp_pc = current_instruction_index;
        gen_instruction(JMP,0,0);

        if(last_sym_read != SYM_RPAREN){
            error(33);
        }
        get_next_symbol();

    }
    test(fsys, phi, 19);
} // statement

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

    jmp_table.current_level_alloc_pc = current_instruction_index;
    gen_instruction(INT, 0, data_alloc_index[level]);

    jmp_table.total_jump_buf_num = 0;

    set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
    set = uniteset(set1, fsys);
    statement(set);
    destroyset(set1);
    destroyset(set);
    code[cx0].a = data_alloc_index[level];
    code[jmp_table.current_level_alloc_pc].a += jmp_table.total_jump_buf_num;

    jmp_table.total_jump_buf_num = 0;
    gen_instruction(OPR, 0, OPR_RET); // return
    test(fsys, phi, 8); // test for error: Follow the statement is an incorrect symbol.
    //list_code(cx0, current_instruction_index);
    //print_table();

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
    int j=0;
    for(j=0;j<STACKSIZE;++j){
        stack[j] = 0;
    }
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
            case LMT:
                stack[top + 1] = stack[top - i.a];
                top = top + 1;
                break;
            case POP:
                top--;
                break;
            case LTP://加载栈顶
                top = stack[get_base_addr(stack, b, i.l) + i.a];
                b = stack[get_base_addr(stack, b, i.l) + i.a + 1];
                break;
            case STP://保存栈顶
                stack[get_base_addr(stack, b, i.l) + i.a] = top;
                stack[get_base_addr(stack, b, i.l) + i.a + 1] = b;
                break;
        } // switch
    } while (pc);

    printf("End executing PL/0 program.\n");
} // interpret

int main(int argc, char *argv[]) {
    FILE *hbin;
    FILE *hasm;
    char s[80];
    int i;
    symset set, set1, set2;

    if (argc == 1)

        strcpy(s, "../example/for.txt");

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
    statement_begin_sys = createset(SYM_SETJMP, SYM_BEGIN, SYM_CALL, SYM_IF, SYM_WHILE, SYM_NULL);
    factor_begin_sys = createset( SYM_IDENTIFIER, SYM_NUMBER, SYM_LPAREN, SYM_SETJMP, SYM_NULL);

    err = char_count = current_instruction_index = line_length = 0; // initialize global variables
    last_char_read = ' ';
    kk = MAXIDLEN;

    get_next_symbol();

    break_code_index.is_break_appear = 0; //cy
    break_code_index.is_in_loop_sign = 0; //cy

    int p=0;
    for(p=1;p<=MAX_JMP_BUFF;++p){
        jmp_table.buf_status[p] = unallocated;
    }
    jmp_table.is_in_condition_block = outside;
    jmp_table.total_jump_buf_num = 0;
    jmp_table.current_jmp_entry_pc = -1;
    longjmp_set = (longjmp_point *) malloc(sizeof (longjmp_point));
    longjmp_set->next = NULL;

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

    longjmp_point * j_point;
    j_point =longjmp_set;
    int buf_id;
    while(j_point->next != NULL){
        buf_id = j_point->jmp_buf_id;
        if(jmp_table.buf_status[buf_id] == unallocated){
            error(39);
        }else{
            code[j_point->save_return_value_pc].l = j_point->jmp_level - setjmp_set[buf_id].level;
            code[j_point->load_stack_top_pc].l = j_point->jmp_level - setjmp_set[buf_id].level;
            code[j_point->save_return_value_pc].a = setjmp_set[buf_id].return_val_address;
            code[j_point->load_stack_top_pc].a = setjmp_set[buf_id].stack_top_address;
            code[j_point->jmp_pc].a = setjmp_set[j_point->jmp_buf_id].jmp_entry_pc;
        }
        j_point = j_point->next;
    }

    if (last_sym_read != SYM_PERIOD)
        error(9); // '.' expected.
    if (err)
        printf("There are %d error(s) in PL/0 program.\n", err);
    else {
        printf("There is no error in PL/0 program.\n");
        list_code(0, current_instruction_index);
        interpret();
    }


}
