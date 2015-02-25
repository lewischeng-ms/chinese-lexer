# chs - 汉字词法分析器makefile
# by 成立@2010/4/3

CC := gcc
BIN := bin
SRC := src
OBJ := obj
EXE := $(BIN)/chs.exe

GLOBAL_H := $(SRC)/global.h
LEX_H := $(SRC)/lex.h
TRIE_H := $(SRC)/trie.h
UTIL_H := $(SRC)/util.h
TOKENS_H := $(SRC)/tokens.h
RULE_H := $(SRC)/rule.h

CHS_C := $(SRC)/chs.c
LEX_C := $(SRC)/lex.c
TRIE_C := $(SRC)/trie.c
UTIL_C := $(SRC)/util.c
TOKENS_C := $(SRC)/tokens.c
RULE_C := $(SRC)/rule.c

OBJS := chs.o lex.o rule.o tokens.o trie.o util.o

all: $(OBJS)
	$(CC) -O2 -o $(EXE) $(OBJ)/chs.o $(OBJ)/lex.o $(OBJ)/rule.o $(OBJ)/tokens.o $(OBJ)/trie.o $(OBJ)/util.o
chs.o: $(GLOBAL_H) $(LEX_H) $(TRIE_H) $(CHS_C)
	$(CC) -o $(OBJ)/chs.o -c $(CHS_C)
lex.o: $(GLOBAL_H) $(LEX_H) $(LEX_C)
	$(CC) -o $(OBJ)/lex.o -c $(LEX_C)
trie.o: $(GLOBAL_H) $(TRIE_H) $(TRIE_C)
	$(CC) -o $(OBJ)/trie.o -c $(TRIE_C)
util.o: $(GLOBAL_H) $(UTIL_H) $(UTIL_C)
	$(CC) -o $(OBJ)/util.o -c $(UTIL_C)
tokens.o: $(GLOBAL_H) $(TOKENS_H) $(TOKENS_C)
	$(CC) -o $(OBJ)/tokens.o -c $(TOKENS_C)
rule.o: $(GLOBAL_H) $(RULE_H) $(RULE_C)
	$(CC) -o $(OBJ)/rule.o -c $(RULE_C)
clean:
	del $(OBJ)\*.o
	del $(BIN)\*.exe
