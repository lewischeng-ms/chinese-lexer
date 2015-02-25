// 文件: util.c
// 作者: 成立
// 日期: 2010/4/3
// 描述: 实用函数的实现

#include "global.h"
#include "util.h"

// 将扩展字符转换为wchar字符。
// ech字符串应当长度为2.
wchar etow(const char* ech)
{
  return ((wchar)((unsigned char)*ech) << 8) | ((unsigned char)*(ech + 1));
}

// 将wchar字符串转换为ANSI字符串。
char* wtos(const wchar* wstr)
{
  assert(false);
  return NULL;
}

// 将wchar字符串转换为ANSI字符串。
wchar* stow(const char* str)
{
  assert(str);
  size_t len = strlen(str) + 1;
  wchar* wstr = (wchar*)malloc(len * sizeof(wchar));
  size_t index = 0;
  while (*str)
  {
    wchar ch = (wchar)((unsigned char)(*str));
    if (ch > 127)
    {
      wchar ch1 = (wchar)((unsigned char)(*(str + 1)));
      wstr[index++] = (ch << 8) | ch1;
      str += 2;
    }
    else
      wstr[index++] = *str++;
  }
  wstr[index] = 0;
  return wstr;
}

// 记录当前字符占用的字节数。
static int step = 0;

// 返回文件流中下一个字符。
// 对于文件中连续两个字符表示的扩展字符（如汉字），
// 将一次性读取并返回。
wchar nextch(FILE* strm)
{
  assert(strm);
  wchar ch = fgetc(strm);
  if (ch > 127)
  { // 接着连续的两个字符表示扩展字符。
    ch = (ch << 8) | (fgetc(strm));
    step = 2;
  }
  else
    step = 1;
  return ch;
}

// 将当前字符放回文件流中。
// 实际操作是回退文件指针。
void backch(FILE* strm)
{
  assert(strm);
  fseek(strm, -step, SEEK_CUR);
}

// 由于本程序中字符类型为wchar，
// 所以需要特别的打印函数。
void print_wch(wchar wch)
{
  char s[3];
  s[2] = 0;
  if (wch > 127)
  { // 扩展字符
    s[0] = wch >> 8;
    s[1] = wch;
    fprintf(logging, "%s", s);
  }
  else
  {
    fprintf(logging, "%c", (char)wch);
  }
}

// 由于本程序中词语的字符类型为wchar，
// 所以需要特别的打印函数。
void print_wstr(const wchar* wstr)
{
  assert(wstr);
  while (*wstr)
    print_wch(*wstr++);
}

// 比较两个wchar字符串是否相等
bool wstrcmp_(const wchar* wstr1, const wchar* wstr2)
{
  assert(wstr1);
  assert(wstr2);
  while (*wstr1 && *wstr2 && *wstr1 == *wstr2)
  {
    wstr1++;
    wstr2++;
  }
  if (*wstr1 || *wstr2)
    return false;
  else
    return true;
}

// 将wstr2复制到wstr1中
void wstrcpy_(wchar* wstr1, const wchar* wstr2)
{
  assert(wstr1);
  assert(wstr2);
  while (*wstr1++ = *wstr2++);
}

// 返回wchar字符串的长度（不含最后的0）
size_t wstrlen_(const wchar* wstr)
{
  assert(wstr);
  size_t len = 0;
  while (*wstr++)
    len++;
  return len;
}

// 打印n个空格。
void space_indent(size_t n)
{
  while (n-- > 0)
    fprintf(logging, " ");
}