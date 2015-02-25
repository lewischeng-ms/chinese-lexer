// 文件: rule.c
// 作者: 成立
// 日期: 2010/4/3
// 描述: 策略的实现

#include "global.h"
#include "tokens.h"
#include "rule.h"

// “匹配”链表的头结点。
struct Match match_head;

// 策略文件句柄。
static FILE* rule;

// 载入策略文件。
bool load_rule_file()
{
  if (rule) return true;
  rule = fopen("data/rule", "r");
  if (!rule)
  {
    fprintf(logging, "无法打开策略文件！\n");
    return false;
  }
  // Not implemented here!
  fclose(rule);
  return true;
}

// 从诸多匹配中挑选出最好的一个。
struct Match* select_best_match()
{
  struct Match* match = match_head.link;
  return match;
}