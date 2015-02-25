// 文件: tokens.h
// 作者: 成立
// 日期: 2010/4/3
// 描述: 汉语词法记号相关的信息声明

#ifndef CHS_TOKENS_H_
#define CHS_TOKENS_H_

enum TOKEN_TIDS {
  TID_INVALID,      // 无效记号
  TID_NOUN,         // 名词
  TID_PRONOUN,      // 代词
  TID_VERB,         // 动词
  TID_QUALIFIER,    // 修饰词（形容词或副词）
  TID_NUMERIAL,     // 数词（一，二……）
  TID_QUANTIFIER,   // 量词
  TID_PREPOSITION,  // 介词
  TID_AUXILIARY,    // 助词
  TID_CONJUNCT,	    // 连词
  TID_IDIOM,        // 习语（如成语等）
  TID_PUNCTUATION,  // 中文标点符号
  TID_NUMBER,       // 数字
  TID_ENGLISH,      // 英语单词
  TID_OTHER         // 其他未知的字符
};

struct TokenInfo
{
  int tid;    // 记号编号
  char* desc; // 记号描述
  char* abbr; // 记号缩写（用于显示记号及读取字典文件）
};

extern struct TokenInfo tokens[];

// 词法分析的记号结构
struct Token
{
  union {
    wchar word[MAX_TOKEN_WORD_LEN]; // 词语
    char str[MAX_TOKEN_WORD_LEN * 2]; // 英文单词
    int num;  //数字
  };
  int tid;
};

#endif // CHS_TOKENS_H_
