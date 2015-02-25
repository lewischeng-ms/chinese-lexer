// 文件: global.h
// 作者: 成立
// 日期: 2010/4/3
// 描述: 全局参数和变量的定义

#ifndef CHS_GLOBAL_H_
#define CHS_GLOBAL_H_

#include <assert.h>
#include <ctype.h>
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 是否关闭调试模式
#define NDEBUG

// 记号种类个数
#define MAX_TOKEN_COUNT 15

// 记号词语长度
#define MAX_TOKEN_WORD_LEN 32

// 记号缩写长度
#define MAX_TOKEN_ABBR_LEN 10

// 字典行缓冲大小
#define MAX_DICT_BUF_SIZE 100

// 打印trie时缩进宽度
#define TRIE_INDENT_WIDTH 4

// 可以保存ansi扩展字符的类型
typedef unsigned short wchar;

// 文件流
extern FILE* text;

// 日志，记录分析信息
extern FILE* logging;

// 以详细模式打印记号。
extern bool human_readable;

#endif // CHS_GLOBAL_H_