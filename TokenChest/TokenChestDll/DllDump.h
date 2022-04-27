#ifndef DUMP_HEADER_GUARD
#define DUMP_HEADER_GUARD

#include <vector>

#include "Dllstringtbl.h"
#include "Dllremote.h"
#include "DllD2Struct.h"

extern std::vector<int> StatsOrder;

int AdjustPrivilege();

bool initBins();

UINT initStatOrder();

bool DumpGame(HANDLE hProcess, DWORD D2Client_base);

DWORD getDumps();

int desc_priority_cmp(const void *aa, const void *bb);

#endif