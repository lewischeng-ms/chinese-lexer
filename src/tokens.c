// 文件: tokens.c
// 作者: 成立
// 日期: 2010/4/3
// 描述: 汉语词法记号相关的信息定义

#include "global.h"
#include "tokens.h"

#define TOK(TID, DESC, ABBR) { TID, DESC, ABBR } 

struct TokenInfo tokens[] = {
  TOK(TID_INVALID, "无效", "invalid"),
  TOK(TID_NOUN, "名词", "n"),
  TOK(TID_PRONOUN, "代词", "pron"),
  TOK(TID_VERB, "动词", "v"),
  TOK(TID_QUALIFIER, "修饰词", "qual"),
  TOK(TID_NUMERIAL, "数词", "numrl"),
  TOK(TID_QUANTIFIER, "量词", "quan"),
  TOK(TID_PREPOSITION, "介词", "prep"),
  TOK(TID_AUXILIARY, "助词", "aux"),
  TOK(TID_CONJUNCT, "连词", "conj"),
  TOK(TID_IDIOM, "习语", "idiom"),
  TOK(TID_PUNCTUATION, "标点", "punct"),
  TOK(TID_NUMBER, "数字", "num"),
  TOK(TID_ENGLISH, "英语单词", "eng"),
  TOK(TID_OTHER, "其它", "o")
};
