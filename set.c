

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "set.h"

symset uniteset(symset s1, symset s2) {
	symset s;
	snode *p;

	s = p = (snode *) malloc(sizeof(snode));

	s1 = s1->next;
	s2 = s2->next;

	/* skip head node of s1 and s2 */

	while (s1 && s2) {
		p->next = (snode *) malloc(sizeof(snode));
		p = p->next;
		if (s1->elem < s2->elem) {
			p->elem = s1->elem;
			s1 = s1->next;
		} else {
			p->elem = s2->elem;
			s2 = s2->next;
		}
	}

	while (s1) {
		p->next = (snode *) malloc(sizeof(snode));
		p = p->next;
		p->elem = s1->elem;
		s1 = s1->next;

	}

	while (s2) {
		p->next = (snode *) malloc(sizeof(snode));
		p = p->next;
		p->elem = s2->elem;
		s2 = s2->next;
	}

	p->next = NULL;

	return s;
} // uniteset

void setinsert(symset s, int elem) {
	snode *p = s;
	snode *q;

	while (p->next && p->next->elem < elem) {
		p = p->next;
	}

	q = (snode *) malloc(sizeof(snode));
	q->elem = elem;
	q->next = p->next;
	p->next = q;
} // setinsert

symset createset(int elem, .../* SYM_NULL */) {
	va_list list;
	symset s;

	s = (snode *) malloc(sizeof(snode));
	s->elem = -1;
	s->next = NULL;

	va_start(list, elem);
	while (elem) {
		setinsert(s, elem);
		elem = va_arg(list, int);
	}
	va_end(list);
	return s;
} // createset

void destroyset(symset s) {
	snode *p;

	while (s) {
		p = s;
		p->elem = -1000000; // added by alex, 2015-12-25
		s = s->next;
		p->next = NULL;
		free(p);
	}
} // destroyset

int inset(int elem, symset s) {
	s = s->next;
	while (s && s->elem < elem)   // modified by alex, 2015-12-25,  s->elem < elem
		s = s->next;

	if (s && s->elem == elem)
		return 1;
	else
		return 0;
} // inset

// EOF set.c
