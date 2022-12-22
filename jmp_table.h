//
// Created by 35802 on 2022/12/17.
//

#ifndef INTRODUCTION_TO_COMPILER_FINAL_PROJECT_JMP_TABLE_H
#define INTRODUCTION_TO_COMPILER_FINAL_PROJECT_JMP_TABLE_H
#define MAX_JMP_BUFF 100
//setjmp_entry:<--------jump_entry            <-|
//      .......                                 |
//      LOD x x<---------return_code_index      |------>setjmp调用处:a condition block/an assignment block
//      .......                               <-|       只需要记录pc以及该块进入时的栈顶位置,longjump回来时将栈顶复位到该处
//                                                      则:若该块不在某个过程的调用/嵌套调用中，则一切正常,否则会有栈错误
//longjmp:                                              (也就是你不能在某个第三层嵌套调用中设置setjmp,然后从嵌套退出后试图通过longjmp准确跳回第三层嵌套,
//      STO val                                          setjmp并不保存整个栈,仅仅记录当前pc,以及能够保持程序正常运行的栈的最低限度信息
//      JMP setjmp_entry                                 如exp,eip,ebp,等等)
// addr+1 stack_top
// addr   return_val
typedef enum{
    unallocated,
    allocated,
}jmp_buf_status;
typedef enum {
    entry,
    inside,
    outside
}condition_status;
typedef struct setjmp_point{
    int jmp_buf_id;
    int level;
    int stack_top_address;
    int return_val_address;
    int jmp_entry_pc;
//    int save_stack_top_pc;
//    int load_return_value_pc;

}setjmp_point;

typedef struct longjmp_point{
    int jmp_buf_id;
    int return_value;
    int load_stack_top_pc;
    int save_return_value_pc;
    int jmp_pc;
    int jmp_level;
    struct longjmp_point* next;
}longjmp_point;

typedef struct jmp_state_table{
    int current_jmp_entry_pc;
    int condition_stack_top_addr;

    condition_status is_in_condition_block;
    jmp_buf_status buf_status[MAX_JMP_BUFF + 1];
    int total_jump_buf_num;
    int current_level_alloc_pc;
}jmp_state_table;

#endif //INTRODUCTION_TO_COMPILER_FINAL_PROJECT_JMP_TABLE_H
