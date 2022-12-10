//
// Created by 35802 on 2022/12/10.
//

#ifndef INTRODUCTION_TO_COMPILER_FINAL_PROJECT_EVL_H
#define INTRODUCTION_TO_COMPILER_FINAL_PROJECT_EVL_H
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

typedef struct enode {
    int elem;
    int cnt;
    int lpl;
    struct enode *next;
} enode, *evl;
extern evl phi_evl;

evl create_evl(int data, .../* SYM_NULL */);

void destroy_evl(evl s);

evl unite_evl(evl s1, evl s2);

int is_in_evl(int elem, evl s);

void insert_evl(evl s, int elem, int cnt, int lpl);// insert_evl

#endif //INTRODUCTION_TO_COMPILER_FINAL_PROJECT_EVL_H
