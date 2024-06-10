# 编译原理

- 项目语言：C++

## 词法分析

### 符号

|   符号    | 编号 |   符号   | 编号 |
| :-------: | :--: | :------: | :--: |
| undefined |  0   |    <     |  25  |
|   bool    |  1   |    >     |  26  |
|   break   |  2   |    <=    |  27  |
|   case    |  3   |    >=    |  28  |
|   char    |  4   |    &&    |  29  |
| continue  |  5   |   \|\|   |  30  |
|    do     |  6   |    !     |  31  |
|   else    |  7   |    &     |  32  |
|   float   |  8   |    \|    |  33  |
|    for    |  9   |    =     |  34  |
|    if     |  10  |    ++    |  35  |
|    int    |  11  |    --    |  36  |
|  return   |  12  |    .     |  37  |
|  switch   |  13  |    (     |  38  |
|   void    |  14  |    )     |  39  |
|   while   |  15  |    [     |  40  |
|   true    |  16  |    ]     |  41  |
|   false   |  17  |    {     |  42  |
|     +     |  18  |    }     |  43  |
|     -     |  19  |    ,     |  44  |
|     *     |  20  |    ;     |  45  |
|     /     |  21  | 数字常量 |  46  |
|     %     |  22  | 浮点常量 |  47  |
|    ==     |  23  |  标识符  |  48  |
|    !=     |  24  |   结束   |  49  |

在这里，结束（49）不是词法单元，仅用来标识读取结束



### 有穷自动机

你们看一下在`Lexer.cpp`的`Lexer::Scan()`里的一堆switch case，还有下面的单词和数字（这部分跟书上差不多）



### 算法说明

主要就是Scan，书上差不多

```
Token* Scan() {
	// 1. 跳过空白符，处理注释
	
	// 2. 处理运算符、分隔符
	
	// 3. 处理数字，整型与浮点
	
	// 4. 处理保留字以及标识符
	
	// 如果到这还没处理完则发生词法错误，返回错误，并读取下一个字符。
}
```



### 类

---

#### Tag

##### 描述

是一个枚举。来存储符号的编号。

数值在上面。

---

#### Token

##### 描述

基础的token。

##### 变量

| tag  | 描述Token的类型 | int（Tag） |
| ---- | --------------- | ---------- |

##### 方法

| ToString | 一个虚函数，给子类实现 |
| -------- | ---------------------- |



---

#### Num

##### 描述

继承自`Token`，描述整型的标识符。

##### 变量

| value | 保存数值 | int  |
| ----- | -------- | ---- |

##### 方法

| ToString | 把内部value的值变成字符串输出 |
| -------- | ----------------------------- |



---

#### Real

##### 描述

继承自`Token`，描述浮点的标识符。

##### 变量

| value | 保存数值 | double |
| ----- | -------- | ------ |

##### 方法

| ToString | 把内部value的值变成字符串输出 |
| -------- | ----------------------------- |



---

#### Word

##### 描述

继承自`Token`，描述关键字、标识符、运算符等等之类的标识符。

##### 变量

| lexeme | 对应的标识符的书写形式 | string |
| ------ | ---------------------- | ------ |

##### 方法

| ToString | 把内部lexeme的值输出 |
| -------- | -------------------- |



---

#### Type

##### 描述

继承自`Word`，描述变量类型，如`int`，`float`，`char`等。

##### 变量

| width | 保存该类型占宽 | int  |
| ----- | -------------- | ---- |



---

#### Lexer

##### 描述

最主要的类，用来进行词法分析

##### 变量

| line     | 保存数值                                     | int           |
| -------- | -------------------------------------------- | ------------- |
| peek     | 当前读取的字符                               | char          |
| words    | 存储关键字以及后续的变量标识符的表。方便识别 | unordered_map |
| fp       | 所读取的文件的指针                           | FILE*         |
| instance | 单例实现                                     | Lexer         |

##### 方法

| Instance       | 获取单例                                                     |
| -------------- | ------------------------------------------------------------ |
| SetFilePointer | 设置文件指针，在运行`Scan`前先调用                           |
| Reverse        | 把输入的关键字插入`words`表                                  |
| Readch         | 重载1：按照`fp`读取下一个字符<br />重载2：读取下一个字符，并返回与传入的字符的比较结果，一致返回`true` |
| Scan           | 最主要的函数，用来进行词法分析                               |



### 语法分析

TBD。。。



### 语义分析

#### 属性

`nextlist`：指向一个包含跳转指令的列表，这些指令最终获得的目标标号就是按照运行顺序紧跟在该属性所属的变量代码之后的指令的标号

`breaklist`：指向一个包含跳转指令的列表，这些指令最终获得的目标标号就是包含其的最近的`while`循环语句代码之后的指令标号

`continueList`：指向一个包含跳转指令的列表，这些指令最终获得的目标标号就是包含其的最近的`while`循环语句代码开始的指令标号

