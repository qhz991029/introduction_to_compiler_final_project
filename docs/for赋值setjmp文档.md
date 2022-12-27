## setjmp/longjmp

### 作用:

setjmp(buf_id)记录当前环境，并返回0，

longjmp(buf_id, val)跳转回setjmp(buf_id)处，并使setjmp返回值val

### 分析:

首先,setjmp的“记录环境”并不是记录整个堆栈，而是记录当前的程序计数器(pc)以及当前堆栈指针信息，使得这些信息在调用longjmp恢复时，程序恰好可以正常继续运行.所以longjmp只能在与setjmp同层次或更高层次的过程调用中被调用，否则堆栈已经被销毁,longjmp到原处无法正常运行.

如下图

longjmp调用限定在statement中,

而setjmp可以作为factor,

<img src="C:\Users\35802\Desktop\statement.png" alt="statement" style="zoom: 33%;" /><img src="C:\Users\35802\Desktop\factor.png" alt="factor" style="zoom: 33%;" />

### 核心思想

首先增加两条PL0指令,将栈顶(top)以及栈的基地址(base_reg)存入指定位置或从指定位置读取

```c
STP level addr
LTP level addr
```

setjmp可解析为:

```c
STP 0 buf_addr  //将当前的环境信息存入指定的buf_id对应的地址
LOD 0 data_addr //从当前buf_id指定的地址读取返回值(第一次为0)
```

longjmp可解析为:

```c
LTP level_diff buf_addr  //读取指定的buf_id对应的地址的环境信息
STO level_diff data_addr //将longjmp指定的返回值存入buf_id的返回值地址
JMP 0 target_pc //PC跳转到buf_id对应的setjmp处
```

每个level为当前level的所有setjmp额外开辟数据空间,用来存储setjmp调用时有效堆栈的栈顶(top)，有效堆栈的栈基地址(base_reg),以及返回值.由于要使setjmp和longjmp一一对应必须要解析完所有代码才可知.在解析过程中先记录下所有setjmp和longjmp的信息，并先产生代码，在最后再将所有setjmp和longjmp一一对应，并回填相关指令的操作数

```c
typedef struct setjmp_point{
    int jmp_buf_id;//setjmp指定的buf_id
    int level;//层次数
    int stack_top_address;//栈顶信息所在地址
    int return_val_address;//返回值的地址
    int jmp_entry_pc; //该setjmp点对应的pc
}setjmp_point;

typedef struct longjmp_point{
    int jmp_buf_id;//longjmp指定的buf_id
    int return_value;//返回值
    int load_stack_top_pc;//LTP指令所在位置(回填时用到)
    int save_return_value_pc;//STO指令所在位置(回填时用到)
    int jmp_pc;//JMP指令所在位置(回填时用到)
    int jmp_level;//longjmp所在level
    struct longjmp_point* next;
}longjmp_point;
```

注意到setjmp有两种情况

```c
id := xxxxx setjmp(1) xxxx ; //变量赋值语句

while( xxx setjmp(1) xxx )//条件语句中
if( xxx setjmp(1) xxx )
```

前者易于处理，遇到后者，通过longjmp跳回时必须从while()/if()开始，因此必须在每次调用while/if前记录进入时的栈顶以及是否在条件语句中的标识.

```c
typedef struct jmp_state_table{
    int current_jmp_entry_pc;//condition块进入前的pc
    int condition_stack_top_addr;//condition块进入前的栈顶位置
    condition_status is_in_condition_block;//是否在某个condition块中
	......
}jmp_state_table;
```

最后进行指令参数回填即可

```c
    longjmp_point * j_point;
    j_point =longjmp_set;
    int buf_id;
    while(j_point->next != NULL){
        buf_id = j_point->jmp_buf_id;
        if(jmp_table.buf_status[buf_id] == unallocated){
            error(39);//尝试跳转到一个未被分配的setjmp块
        }else{//参数回填
            code[j_point->save_return_value_pc].level = j_point->jmp_level - setjmp_set[buf_id].level;
            code[j_point->load_stack_top_pc].level = j_point->jmp_level - setjmp_set[buf_id].level;
            
            code[j_point->save_return_value_pc].addr = setjmp_set[buf_id].return_val_address;
            code[j_point->load_stack_top_pc].addr = setjmp_set[buf_id].stack_top_address;
            code[j_point->jmp_pc].addr = setjmp_set[j_point->jmp_buf_id].jmp_entry_pc;
        }
        j_point = j_point->next;
    }
```

## for语句

### 作用:

for (var i:(start, end, step) ) statement

令i(若未声明则就地声明)初值为start进行迭代,执行statement，每次迭代后i += step，超过end指定范围后停止迭代

具体的，我们的for语句设计成为半开半闭的，即范围为[start,end),此外start,end,step都支持表达式

<img src="C:\Users\35802\Desktop\statement.png" alt="statement" style="zoom: 80%;" />

### 分析:

由于有些栈数据需要多次用到，为更方便调整栈，增加PL0命令

```c
LMT 0 a//取栈顶往下偏移为a处数据到栈顶
POP 0 0//丢弃栈顶元素
```

for语句大致可拆解为:

- 1.声明i

- 2.解析初值

- 3.解析终值

- 4.得到初始大小关系(初值大于还是小于终值，若初值等于终值，则结束整个for语句),并保存在栈中

- 5.解析step(步长默认为1)

- 6.执行for内迭代语句

- 7.令i += step

- 8.解析终值

- 9.根据i与终值大小的关系判断是否到达退出迭代标准

- 10.是则结束，否则返回5

  ```c
  //LOOP:
  //          step = 1 / step = expression(step)
  //          statement
  //          var i = i + step
  //          end = expression(end)
  //          if i reach end ?
  //          Yes -> leave
  //          NO  -> goto LOOP
  ```

其中第9步判断是否达到迭代退出标准中.我们以第一次进入for时start和end的大小关系为准，

若第一次start<end，我们认为应该是递增迭代，终止当且仅当i>=end，

同理若start>end，我们认为应该是递减迭代，终止当且仅当i<=end,

通过start和end的大小关系的逆转我们可以知道何时终止for.

## 赋值表达式

<img src="C:\Users\35802\Desktop\factor.png" alt="factor" style="zoom: 33%;" />

### 作用:

赋值表达式是诸如 i:=j:=k:=10这样的语句,即赋值语句identifier := expression也作为一个表达式,其值为expression.

### 分析:

按如图方式拓展因子解析式.

可见遇见identifier时，往前读一个符号，若为:=，则证明为赋值表达式,否则该项解析结束.

解析完表达式的值后，将值赋给变量，然后再将变量值放回栈顶即可

## 负责人：黄鑫