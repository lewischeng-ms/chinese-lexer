// 文件: lex.c
// 作者: 成立
// 日期: 2010/4/3
// 描述: 词法分析器的实现

#include "global.h"
#include "lex.h"
#include "trie.h"
#include "tokens.h"
#include "util.h"
#include "rule.h"

// 当前记号。
static struct Token curr;

// 扫描英文单词
static void scan_eng_word
{
  assert(text);
  char ch;
  int index = 0;
  while (!feof(text) && isalpha(ch = fgetc(text)))
    curr.str[index++] = ch;
  backch(text);
  curr.str[index] = 0;
  curr.tid = TID_ENGLISH;
}

// 扫描数字
static void scan_num
{
  assert(text);
  char ch;
  curr.num = 0;
  while (!feof(text) &&isdigit(ch = fgetc(text)))
    curr.num = curr.num * 10 + (ch - '0');
  backch(text);
  curr.tid = TID_NUMBER;
}

// 根据trie扫描出所有可能的匹配。
static void scan_all_matches()
{
  struct Match* temp;
  // 若链表不空，则先释放其空间。
  struct Match* p = match_head.link;
  while (p)
  {
    temp = p->link;
    free(p);
    p = temp;
  }
  match_head.link = 0;
  // 记录下当前位置。
  unsigned start_pos = ftell(text);
  // 当前想要匹配的长度。
  int match_len;
  for (match_len = 1; match_len < MAX_TOKEN_WORD_LEN; match_len++)
  {
    // 读入当前欲测试匹配的字符串
    int i;
    for (i = 0; i < match_len; i++)
      curr.word[i] = nextch(text);
    curr.word[i] = 0;
    struct TrieNode* node = lookup_trie(curr.word);
    if (node)
    { 
      // 匹配成功，且是终端结点，添加链表节点
      if (node->terminable)
      {
        temp = (struct Match*)malloc(sizeof(struct Match));
        wstrcpy_(temp->word, curr.word);
        temp->tid = node->tid;
        temp->rule = node->rule;
        temp->next_pos = ftell(text);
        temp->link = match_head.link;
        match_head.link = temp;
      }
      fseek(text, start_pos, SEEK_SET);
    }
    else
    { // match_len长度匹配不成功。
      fseek(text, start_pos, SEEK_SET);
      break;
    }
  }
}

// 扫描扩展字符为首的记号，
// 可能是标点，可能是汉字，
// 也可能是其他文字（比如兽语？）。
static void scan_ech
{
  assert(text);
  wchar ch = nextch(text);
  if (is_punct(ch))
  { // 是标点
    curr.word[0] = ch;
    curr.word[1] = 0;
    curr.tid = TID_PUNCTUATION;
    return;
  }
  backch(text);
  // 扫描出所有匹配，并且已按从长到短排列。
  scan_all_matches();
  if (match_head.link)
  { // 找到了至少一个匹配。
    struct Match* match;
    // 在这里使用策略。
    match = select_best_match();
    // 使用该策略生成记号。
    wstrcpy_(curr.word, match->word);
    curr.tid = match->tid;
    fseek(text, match->next_pos, SEEK_SET);
  }
  else
  { // 字典中找不到匹配的词语。
    curr.word[0] = nextch(text);
    curr.word[1] = 0;
    curr.tid = TID_OTHER;
  }
}

// 取得下一个记号
static void next_token()
{
  assert(text);
  wchar ch = nextch(text);
  if (feof(text))
  {
    curr.tid = TID_INVALID;
    return;
  }
  while (ch == ' ' || ch == '\t' || ch == '\n')
  { // 空白字符，原样输出
    if (!human_readable)
      fprintf(logging, "%c", (char)ch);
    ch = nextch(text);
    continue;
  }
  if (isalpha(ch))
  { // 读入英语单词。
    backch(text);
    scan_eng_word;
  }
  else if (isdigit(ch))
  { // 读入数字。
    backch(text);
    scan_num;
  }
  else if (ch > 127)
  { // 读入扩展字符。
    backch(text);
    scan_ech;
  }
  else
  { // 其他
    curr.word[0] = ch;
    curr.word[1] = 0;
    curr.tid = TID_OTHER;
  }
}

// 输出当前记号。
static void print_token()
{
  assert(curr.tid != TID_INVALID);
  if (curr.tid == TID_NUMBER)
  {
    fprintf(logging, "%d", curr.num);
  }
  else if (curr.tid == TID_ENGLISH)
  {
    fprintf(logging, "%s", curr.str);
  }
  else
  {
    print_wstr(curr.word);
  }
  if (human_readable)
    fprintf(logging, "\t\t%s\n", tokens[curr.tid].desc);
  else
    fprintf(logging, "/%s  ", tokens[curr.tid].abbr);
}

// 词法分析主函数。
void do_lex()
{
  assert(text);
  while (true)
  {
    next_token();
    if (curr.tid == TID_INVALID)
      break;
    print_token();
  }
}
