// 文件: chs.c
// 作者: 成立
// 日期: 2010/4/3
// 描述: 汉语词法分析器的主程序

#include "global.h"
#include "lex.h"
#include "trie.h"
#include "rule.h"
#include "util.h"

FILE* text = NULL;
FILE* logging = NULL;
bool human_readable = false;

int main(int argc, char * argv[])
{
  logging = stdout;
  if (argc == 1)
  {
    fprintf(logging, "用法：chs <文件名>\n");
    return 0;
  }
  // 确定是否需要创建（或重新创建trie文件），并载入trie。
  if (argc == 2 && !strcmp(argv[1], "-ctf"))
  {
    if (!load_user_dict())
      return 1;
    create_trie_from_dict();
    save_to_trie_file();
    fprintf(logging, "trie文件建立成功！\n");
    return 0;
  }
  // 确定输入文件名
  char* filename;
  if (argc == 3)
  {
    if (!strcmp(argv[1], "-h"))
      human_readable = true;
    filename = argv[2];
  }
  else if (argc == 2)
    filename = argv[1];
  else
  {
    fprintf(logging, "错误的参数个数！\n");
    return 1;
  }
  // 读入并分析输入文件。
  text = fopen(filename, "r");
  if (!text)
  {
    fprintf(logging, "无法打开文件%s！\n", filename);
    return 1;
  }
  if (!load_from_trie_file())
    return 1;
  if (!load_rule_file())
    return 1;
  do_lex();
  fclose(text);
  return 0;
}
