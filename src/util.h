// 文件: util.h
// 作者: 成立
// 日期: 2010/4/3
// 描述: 实用函数的接口

#ifndef CHS_UTIL_H_
#define CHS_UTIL_H_

// 将扩展字符转换为wchar字符。
// ech字符串应当长度为2.
wchar etow(const char* ech);

// 将wchar字符串转换为ANSI字符串。
char* wtos(const wchar* wstr);

// 将wchar字符串转换为ANSI字符串。
wchar* stow(const char* str);

// 返回文件流中下一个字符。
// 对于文件中连续两个字符表示的扩展字符（如汉字），
// 将一次性读取并返回。
wchar nextch(FILE* strm);

// 将当前字符放回文件流中。
// 实际操作是回退文件指针。
void backch(FILE* strm);

// 由于本程序中字符类型为wchar，
// 所以需要特别的打印函数。
void print_wch(wchar wch);

// 由于本程序中词语的字符类型为wchar，
// 所以需要特别的打印函数。
void print_wstr(const wchar* wstr);

// 比较两个wchar字符串是否相等
bool wstrcmp_(const wchar* wstr1, const wchar* wstr2);

// 将wstr2复制到wstr1中
void wstrcpy_(wchar* wstr1, const wchar* wstr2);

// 返回wchar字符串的长度（不含最后的0）
size_t wstrlen_(const wchar* wstr);

// 打印n个空格。
void space_indent(size_t n);

#endif // CHS_UTIL_H_