// 文件: trie.h
// 作者: 成立
// 日期: 2010/4/3
// 描述: trie的接口

// 字典词条结构
struct DictItem
{
  wchar word[MAX_TOKEN_WORD_LEN]; // 词语
  int tid; // 类别
  int rule; // 策略id（-1表示没有）
  struct DictItem* link;
};

// trie结点结构
struct TrieNode
{
  wchar ch;                     // 该结点存储的汉字
  bool terminable;              // 该结点是否是可终止结点，即是否有从根到该结点的词语。
  int rule;                     // 若可终结，指向对应策略id，-1表示没有。
  int tid;                      // 若可终结，表示该词语的类别。
  union {
    struct TrieNode* mem_ptr;   // 内存中的指针
    unsigned raw_ptr;           // 文件中的偏移
  } first_child;                // 指向第一个孩子结点的指针
  union {
    struct TrieNode* mem_ptr;
    unsigned raw_ptr;
  } sibling;                    // 指向下一个兄弟结点的指针
};

// 载入用户字典。
bool load_user_dict();

// 使用给定wchar字符串查字典，返回对应链表节点（未找到则返回NULL）。
struct DictItem* lookup_user_dict(wchar* wstr);

// 载入已存在的trie文件。
bool load_from_trie_file();

// 根据用户字典，建立trie。
void create_trie_from_dict();

// 将整个trie树持久化到trie文件，
// 以加快下次访问速度，
// 在逻辑上，它与用户字典是等价的，
// 所以若用户字典发生了变化，则应
// 当重新建立trie文件。
// 算法基本思想：使用前序递归遍历trie树。
// 处理指针的方法：内存中的指针与文件中的偏移共用一个空间。
void save_to_trie_file();

// 判断是否是标点。
bool is_punct(wchar ch);

// 使用给定wchar字符串查找trie树，返回对应的节点（未找到则返回NULL）。
struct TrieNode* lookup_trie(wchar* wstr);