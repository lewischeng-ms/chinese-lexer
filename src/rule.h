// 文件: rule.h
// 作者: 成立
// 日期: 2010/4/3
// 描述: 策略的接口

#ifndef CHS_RULE_H_
#define CHS_RULE_H_

// 匹配的结构。
struct Match
{
  wchar word[MAX_TOKEN_WORD_LEN];   // 匹配的词语字符串
  int tid;                          // 词语的类别
  int rule;                         // 策略编号
  unsigned next_pos;                // 指向该文件中词语之后的位置
  struct Match* link;               // 指向下一个匹配
};

// “匹配”链表的头结点。
extern struct Match match_head;

// 载入策略文件。
bool load_rule_file();

// 从诸多匹配中挑选出最好的一个。
struct Match* select_best_match();

#endif