
#include "evl.h"
evl phi_evl = NULL;

evl unite_evl(evl s1, evl s2) {
    evl s;
    enode *p;

    s = p = (enode *) malloc(sizeof(enode));

    s1 = s1->next;
    s2 = s2->next;

    /* skip head node of s1 and s2 */

    while (s1 && s2) {
        p->next = (enode *) malloc(sizeof(enode));
        p = p->next;
        if (s1->elem < s2->elem) {
            p->elem = s1->elem;
            p->cnt = s1->cnt;
            p->lpl = s1->lpl;
            s1 = s1->next;
        } else {
            p->elem = s2->elem;
            p->cnt = s2->cnt;
            p->lpl = s2->lpl;
            s2 = s2->next;
        }
    }

    while (s1) {

        p->next = (enode *) malloc(sizeof(enode));
        p = p->next;
        p->elem = s1->elem;
        p->cnt = s1->cnt;
        p->lpl = s1->lpl;
        s1 = s1->next;

    }

    while (s2) {

        p->next = (enode *) malloc(sizeof(enode));
        p = p->next;
        p->elem = s2->elem;
        p->cnt = s2->cnt;
        p->lpl = s2->lpl;
        s2 = s2->next;
    }

    p->next = NULL;

    return s;
} // unite_evl

void insert_evl(evl s, int elem, int cnt, int lpl) {
    enode *p = s;
    enode *q;

    while (p->next && p->next->elem < elem) {
        p = p->next;
    }

    q = (enode *) malloc(sizeof(enode));
    q->elem = elem;
    q->cnt = cnt;
    q->lpl = lpl;
    q->next = p->next;
    p->next = q;
} // insert_evl

evl create_evl(int data, .../* SYM_NULL */) {
    va_list list;
    evl s;
    int cnt;
    int lpl;

    s = (enode *) malloc(sizeof(enode));
    s->elem = -1;
    s->cnt = 0;
    s->lpl = 0;
    s->next = NULL;

    va_start(list, data);
    cnt = va_arg(list, int);
    lpl = va_arg(list, int);
    while (data) {
        insert_evl(s, data, cnt, lpl);
        data = va_arg(list, int);
        cnt = va_arg(list, int);
        lpl = va_arg(list, int);
    }
    va_end(list);
    return s;
} // create_evl

void destroy_evl(evl s) {
    enode *p;

    while (s) {
        p = s;
        p->elem = -1000000; // added by alex, 2015-12-25
        s = s->next;
        p->next = NULL;
        p->cnt = 0;
        p->lpl = 0;
        free(p);
    }
} // destroy_evl

int is_in_evl(int elem, evl s) {
    s = s->next;
    while (s && s->elem < elem)   // modified by alex, 2015-12-25,  s->elem < elem
        s = s->next;

    if (s && s->elem == elem)
        return 1;
    else
        return 0;
} // is_in_evl

// EOF set.c
