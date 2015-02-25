// 文件: trie.c
// 作者: 成立
// 日期: 2010/4/3
// 描述: trie的实现

#include "global.h"
#include "trie.h"
#include "tokens.h"
#include "util.h"

///////////////////////////////////////////////////////////////////////////////
//                                用户字典                               //
///////////////////////////////////////////////////////////////////////////////

// 字典结构链表头指针
static struct DictItem dict_head;

// 字典文件句柄
static FILE* dict;

// 行缓冲区
static wchar buf[MAX_DICT_BUF_SIZE];
static size_t buf_len = 0;

// 从字典中取出一行，返回fale表示文件结束
static bool getline()
{
  assert(dict);
  buf_len = 0;
  while (!feof(dict))
  {
    wchar ch = nextch(dict);
    if (ch == ' ' || ch == '\t' || ch == '\n')
      continue;
    if (ch == '#')
    { // 跳过注释行。
      while (!feof(dict) && (ch = nextch(dict)) !='\n');
      continue;
    }
    // 正常读取行
    while (!feof(dict) && ch !='\n')
    {
      buf[buf_len++] = ch;
      ch = nextch(dict);
    }
    buf[buf_len] = '\0';
    break;
  }
  if (buf_len == 0)
    return false;
  else
    return true;
}

// 从字典中一行分析出词条。
static struct DictItem* parse_dict_item()
{
  assert(buf_len > 0);
  size_t buf_index = 0;
  size_t index = 0;
  struct DictItem* itm = (struct DictItem*)malloc(sizeof(struct DictItem));
  // 扫描出词语
  while (buf[buf_index] != ':' && index < MAX_TOKEN_WORD_LEN)
  {
    itm->word[index++] = buf[buf_index++];
  }
  itm->word[index] = 0;
  buf_index++;
  // 扫描出类型
  char tid_str[MAX_TOKEN_ABBR_LEN];
  index = 0;
  while (buf[buf_index] != ':' && index < MAX_TOKEN_ABBR_LEN)
  {
    tid_str[index++] = buf[buf_index++];
  }
  tid_str[index] = 0;
  int i;
  for (i = 0; i < MAX_TOKEN_COUNT; ++i)
    if (!strcmp(tid_str, tokens[i].abbr))
      break;
  if (i == MAX_TOKEN_COUNT)
  {
    fprintf(logging, "未知类型：%s！\n", tid_str);
    return NULL;
  }
  else
  {
    itm->tid = tokens[i].tid;
  }
  buf_index++;
  // 扫描出策略id
  if (buf[buf_index] == 'x')
  {
    itm->rule = -1;
    buf_index++;
  }
  else
  {
    itm->rule = 0;
    while (buf_index < buf_len && isdigit(buf[buf_index]))
      itm->rule = itm->rule * 10 + (buf[buf_index++] - '0');
  }
  // 行缓冲区中的字符应当被耗尽！
  assert(buf_index == buf_len);
  return itm;
}

// 打印整本字典
static void print_user_dict()
{
  assert(dict_head.link);
  size_t count = 1;
  fprintf(logging, "\t\t+--------------+\n"
                   "\t\t|   用户字典   |\n"
                   "\t\t+--------------+\n"
                   " 序号\t\t词语\t类型\t缩写\t策略id\n"
         );
  struct DictItem* curr = dict_head.link;
  while (curr)
  {
    fprintf(logging, "[%05d]\t\t", count);
    print_wstr(curr->word);
    fprintf(logging, "\t%s\t%s\t%5d\n", tokens[curr->tid].desc,
            tokens[curr->tid].abbr, curr->rule);
    count++;
    curr = curr->link;
  }
}

// 载入用户字典。
bool load_user_dict()
{
  // 字典已经载入。
  if (dict_head.link) return true;

  dict = fopen("data/dict", "r");
  if (!dict)
  {
    fprintf(logging, "无法打开字典文件！\n");
    return false;
  }
  struct DictItem* last = &dict_head;
  while (getline(dict))
  {
    struct DictItem* temp = parse_dict_item();
    if (!temp) return false;
    temp->link = NULL;
    last->link = temp;
    last = temp;
  }
  fclose(dict);
  // 调试模式：打印读入的用户字典。
# ifndef NDEBUG
  print_user_dict();
# endif
  return true;
}

// 使用给定wchar字符串查字典，返回对应链表节点。
struct DictItem* lookup_user_dict(wchar* wstr)
{
  assert(wstr);
  struct DictItem* curr = dict_head.link;
  while (curr)
  {
    if (wstrcmp_(wstr, curr->word))
      return curr;
    curr = curr->link;
  }
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//                                    trie                                   //
///////////////////////////////////////////////////////////////////////////////

// trie树根节点
static struct TrieNode trie_root;

// trie文件句柄
static FILE* trie;

#ifndef NDEBUG
static int counter;
#endif

// 从指定根节点的所有孩子中查找
static struct TrieNode* lookup_childs(struct TrieNode* root, wchar ch)
{
  assert(root);
  struct TrieNode* child = root->first_child.mem_ptr;
  while (child)
  {
    if (child->ch == ch)
      return child;
    child = child->sibling.mem_ptr;
  }
  return NULL;
}

// 前序遍历将trie树按缩进格式打印出来。
static void print_trie(struct TrieNode* root, size_t indent)
{
  if (!root)
    return;
  // 打印根结点
  space_indent(indent);
  fprintf(logging, "(");
  wchar s[2];
  s[0] = root->ch;
  s[1] = 0;
  print_wstr(s);
  fprintf(logging, ", ");
  if (root->terminable)
    fprintf(logging, "T, %d, %s)\n", root->rule, tokens[root->tid].abbr);
  else
    fprintf(logging, "N)\n");
  struct TrieNode* child = root->first_child.mem_ptr;
  while (child)
  {
    print_trie(child, indent + TRIE_INDENT_WIDTH);
    child = child->sibling.mem_ptr;
  }
}

// 将trie序列化至文件，过程中将使用两个指针的裸形式。
static struct TrieNode* save_trie_rec(struct TrieNode* root)
{
  assert(root);
  assert(trie);
  unsigned my_pos = ftell(trie);
# ifndef NDEBUG
  printf("跟踪写入 - ");
  print_wch(root->ch);
  printf(", 计数 = %d, 文件偏移 = %d\n", ++counter, my_pos);
# endif
  // 为本结点预留位置。
  fseek(trie, sizeof(struct TrieNode), SEEK_CUR);
  struct TrieNode* child = root->first_child.mem_ptr;
  // 设置第一个孩子的裸指针。
  if (root->first_child.raw_ptr > 0)
    root->first_child.raw_ptr = ftell(trie);
  while (child)
  {
    struct TrieNode* child_sibling = save_trie_rec(child);
    child = child_sibling;
  }
  struct TrieNode* my_sibling = root->sibling.mem_ptr;
  // 设置下一个兄弟的裸指针。
  if (root->sibling.raw_ptr > 0)
    root->sibling.raw_ptr = ftell(trie);
  // 写入本结点。
  unsigned sibling_pos = ftell(trie);
  fseek(trie, my_pos, SEEK_SET);
  fwrite(root, sizeof(struct TrieNode), 1, trie);
  fseek(trie, sibling_pos, SEEK_SET);
  return my_sibling;
}

// 载入已存在的trie文件。
bool load_from_trie_file()
{
  // trie文件已经载入
  if (trie_root.first_child.mem_ptr)
    return true;
  trie = fopen("data/trie", "rb");
  if (!trie)
  {
    fprintf(logging, "无法打开trie文件，请检查或者重建！\n");
    return false;
  }
  // 这里文件大小即等于实际消耗的内存大小。
  fseek(trie, 0, SEEK_END);
  unsigned file_size = ftell(trie);
  fseek(trie, 0, SEEK_SET);
  // 采用一次分配，可以极大地增加效率。
  struct TrieNode* base = (struct TrieNode*)malloc(file_size);
  fread(base, file_size, 1, trie);
  unsigned item_count = file_size / sizeof(struct TrieNode);
  // 逐个处理结构中的指针。
  int i;
  for (i = 0; i < item_count; ++i)
  {
    if (base[i].first_child.raw_ptr > 0)
      base[i].first_child.raw_ptr += (unsigned)base;
    if (base[i].sibling.raw_ptr > 0)
      base[i].sibling.raw_ptr += (unsigned)base;
  }
  fclose(trie);
  // 设置根节点数据。
  memcpy(&trie_root, base, sizeof(struct TrieNode));
  // 调试模式：打印载入的trie。
#ifndef NDEBUG
  fprintf(logging, "\t\t+--------------+\n"
                   "\t\t|  载入的trie |\n"
                   "\t\t+--------------+\n");
  print_trie(&trie_root, 0);
#endif
  return true;
}

// 根据用户字典，建立trie。
void create_trie_from_dict()
{
  assert(dict_head.link);
  trie_root.ch = etow("根");
  struct DictItem* curr = dict_head.link; // 指向字典链表中当前词语。
  struct TrieNode* root;                  // 指向当前搜索的子树根节点。
  struct TrieNode* child;                 // 指向找到的孩子结点。
  while (curr)
  {
    root = &trie_root; 
    wchar* p = curr->word; // 指向当前词语中下一个字符位置。
    while (*p)
    {
      // Step 1. 在root的所有孩子结点中查找字符*p。
      child = lookup_childs(root, *p);
      if (!child)
      { // 没有找到，则新建一个孩子结点，并更新当前根节点。
        child = (struct TrieNode*)malloc(sizeof(struct TrieNode));
        child->ch = *p;
        child->terminable = false;
        child->rule = -1;
        child->tid = TID_INVALID;
        child->first_child.mem_ptr = NULL;
        child->sibling.mem_ptr = root->first_child.mem_ptr;
        root->first_child.mem_ptr = child;
      }
      root = child;
      p++;
    }
    // 当前词语搜索完毕，root指向含有该词语最后一个字符的结点。
    // 设置root结点相关信息。
    root->terminable = true;
    root->rule = curr->rule;
    root->tid = curr->tid;
    // 递进到下一个词条。
    curr = curr->link;
  }
  // 调试模式：打印创建的trie。
#ifndef NDEBUG
  fprintf(logging, "\t\t+--------------+\n"
                   "\t\t|   创建的trie |\n"
                   "\t\t+--------------+\n");
  print_trie(&trie_root, 0);
#endif
}

// 将整个trie树持久化到trie文件，
// 以加快下次访问速度，
// 在逻辑上，它与用户字典是等价的，
// 所以若用户字典发生了变化，则应
// 当重新建立trie文件。
// 算法基本思想：使用前序递归遍历trie树。
// 处理指针的方法：内存中的指针与文件中的偏移共用一个空间。
void save_to_trie_file()
{
  assert(trie_root.first_child.mem_ptr);
  trie = fopen("data/trie", "wb");
  if (!trie)
  {
    fprintf(logging, "无法打开trie文件！\n");
    return;
  }
# ifndef NDEBUG
  fprintf(logging, "\t\t+--------------+\n"
                   "\t\t| 写入trie文件 |\n"
                   "\t\t+--------------+\n");
  counter = 0;
# endif
  save_trie_rec(&trie_root);
# ifndef NDEBUG
  printf("总结点计数: %d, trie文件大小为 %d 字节.\n", counter, counter * sizeof(struct TrieNode));
# endif
  fclose(trie);
}

// 判断是否是标点。
bool is_punct(wchar ch)
{
  assert(trie_root.first_child.mem_ptr);
  struct TrieNode* node = lookup_childs(&trie_root, ch);
  if (node && node->tid == TID_PUNCTUATION)
    return true;
  else
    return false;
}

// 使用给定wchar字符串查找trie树，返回对应的节点（未找到则返回NULL）。
struct TrieNode* lookup_trie(wchar* wstr)
{
  assert(wstr);
  assert(trie_root.first_child.mem_ptr);
  struct TrieNode* root = &trie_root;
  while (*wstr && root)
  {
    struct TrieNode* child = lookup_childs(root, *wstr);
    // 不存在该字符
    if (!child)
      return NULL;
    root = child;
    wstr++;
  }
  // 超出trie范围
  if (*wstr)
    return NULL;
  return root;
}