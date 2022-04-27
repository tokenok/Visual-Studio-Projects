#ifndef STRING_TBL_H
#define STRING_TBL_H

#include <stdio.h>

extern int initStringTables();
extern bool closeStringTables(void);
extern int getNumStringByName(const char *ptKeyString, char *ptString);
extern int getHashGlobal(const char *ptString);
extern char *getStringByName(const char *ptKeyString);
extern char *getStringByNum(int KeyNum);
extern void teststringtable(void);

#endif
