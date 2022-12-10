#ifndef SET_H
#define SET_H

typedef struct snode {
    int elem;
    struct snode *next;
} snode, *symset;

extern symset phi, decleration_begin_sys, statement_begin_sys, factor_begin_sys, rel_set;

symset createset(int data, .../* SYM_NULL */);

void destroyset(symset s);

symset uniteset(symset s1, symset s2);

int inset(int elem, symset s);

#endif
// EOF set.h
