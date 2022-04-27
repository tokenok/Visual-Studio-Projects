// stringtable.cpp
//
// Created by Pedro Faria (Jarulf).
//
// Many thanks to Peter Hatch (Ondo) for information
// about the structure and algorithms regarding
// the file "string.tbl".

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dll.h"
#include "common.h"

enum {
	// File info. Some are not used by program

	// Size info of various sections
	HeaderSize = 0x15,
	ElementSize = 0x02,
	NodeSize = 0x11,

	// Header info location
	CRCOffset = 0x00,      // word
	NumElementsOffset = 0x02,      // word
	HashTableSizeOffset = 0x04,      // dword
	VersionOffset = 0x08,      // byte (always 0)
	StringStartOffset = 0x09,      // dword
	NumLoopsOffset = 0x0D,      // dword
	FileSizeOffset = 0x11,      // dword

	// Element info location
	NodeNumOffset = 0x00,      // word

	// Node info location
	ActiveOffset = 0x00,      // byte
	IdxNbrOffset = 0x01,      // word
	HashValueOffset = 0x03,      // dword
	IdxStringOffset = 0x07,      // dword
	NameStringOffset = 0x0B,      // dword
	NameLenOffset = 0x0F,      // word

	// KeyNums
	StringKeyNum = 0,
	PatchStringKeyNum = 10000,
	ExpansionStringKeyNum = 20000
};

static bool      IsInit = false;
static char      *ptStringTable = NULL;
static char      *ptExpansionStringTable = NULL;
static char      *ptPatchStringTable = NULL;
static char      strStringFilename[] = "tbl\\string.tbl";
static char      strExpansionStringFilename[] = "tbl\\expansionstring.tbl";
static char      strPatchStringFilename[] = "tbl\\patchstring.tbl";
static char      strNameNotFound[] = "Missing string";
static char      strNull[] = "";

////////////////////
// Memory allocation
////////////////////

// just here to make compilation possible
// would be in other file normally

static void allocateMemory(void *ptMemx, int sizeMem) {
	void **ptMem = (void **)ptMemx;

	if ((*ptMem = malloc(sizeMem)) == NULL) {
		printf("Error: Can't allocate %d bytes of memory, program terminated.\n", sizeMem);
		exit(0);
	}
} // allocateMemory
void deallocateMemory(void  *ptMemx) {
	void **ptMem = (void **)ptMemx;

	if (*ptMem != NULL) {
		free(*ptMem);
		*ptMem = NULL;
	}
} // deallocateMemory

////////////////////
// Utility functions
////////////////////

static unsigned short getNumElements(char *ptTable) {
	return *(unsigned short *)(ptTable + NumElementsOffset);
} // getNumElements
static int getHashTableSize(char *ptTable) {
	return *(int *)(ptTable + HashTableSizeOffset);
} // getHashTableSize
static int getNumLoops(char *ptTable) {
	return *(int *)(ptTable + NumLoopsOffset);
} // getNumLoops
static char *getptStringStart(char *ptTable) {
	return ptTable + HeaderSize + ElementSize*getNumElements(ptTable) + NodeSize*getHashTableSize(ptTable);
} // getptStringStart
static char *getptStringEnd(char *ptTable) {
	return ptTable + (*(unsigned int *)(ptTable + FileSizeOffset));
} // getptStringEnd
static char *getptFirstNode(char *ptTable) {
	return ptTable + HeaderSize + ElementSize*getNumElements(ptTable);
} // getptFirstNode
static int getNodeNum(char *ptElement) {
	return *(unsigned short *)(ptElement + NodeNumOffset);
} // getNodeNum
static int getIdxNum(char *ptNode) {
	return (*(int *)(ptNode + IdxNbrOffset));
} // getIdxNum
static char *getptIdxString(char *ptTable, char *ptNode) {
	return ptTable + (*(int *)(ptNode + IdxStringOffset));
} // getptIdxString
static char *getptNameString(char *ptTable, char *ptNode) {
	return ptTable + (*(int *)(ptNode + NameStringOffset));
} // getptNameString
static unsigned int getFileSize(char *ptHeader) {
	return *(unsigned int *)(ptHeader + FileSizeOffset);
} // getFileSize

////////////////
// CRC functions
////////////////

static int calcCRC(unsigned char *ptStart, unsigned char *ptEnd) {
	unsigned char   *ptCur;
	unsigned short   CRCValue;
	unsigned short   CRCTableEntry;

	static const unsigned short   CRCTable[256] = {
		0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
		0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6, 0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
		0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485, 0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
		0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4, 0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
		0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823, 0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
		0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, 0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
		0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, 0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
		0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70, 0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
		0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F, 0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
		0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E, 0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
		0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D, 0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
		0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C, 0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
		0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB, 0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
		0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A, 0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
		0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9, 0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
		0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, 0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0};

	ptCur = ptStart;
	CRCValue = 0xFFFF;
	while (ptCur < ptEnd) {
		CRCTableEntry = CRCValue / 0x0100;
		CRCTableEntry ^= (unsigned short)(*ptCur);
		CRCValue &= 0x000000FF;
		CRCValue *= 0x00000100;
		CRCValue ^= CRCTable[CRCTableEntry];
		ptCur++;
	}
	return CRCValue;
} // calcCRC
static int getCRC(char *ptTable) {
	char         *ptStart;
	char         *ptEnd;

	if (ptTable == NULL)
		return -1;
	ptStart = getptStringStart(ptTable);
	ptEnd = getptStringEnd(ptTable);

	return calcCRC((unsigned char *)ptStart, (unsigned char *)ptEnd);
} // getCRC
//static bool setCRC(char *ptTable)
//{
//   int   CRCValue;
//
//   if(ptTable==NULL)
//      return false;
//   if((CRCValue=getCRC(ptTable))!=-1)
//   {
//      (*(unsigned short *)(ptTable + CRCOffset)) = (unsigned short)CRCValue;
//      return true;
//   }
//   else
//      return false;
//} // setCRC

/////////////////
// Hash functions
/////////////////

static int getHash(const char *ptKeyString, int HashTableSize) {
	char         charValue;
	unsigned int   hashValue;
	const char         *ptKeyStringChar;

	hashValue = 0;
	ptKeyStringChar = ptKeyString;
	while ((charValue = *ptKeyStringChar++) != '\0') {
		hashValue *= 0x10;
		hashValue += charValue;
		if ((hashValue & 0xF0000000) != 0) {
			unsigned int tempValue = hashValue & 0xF0000000;
			tempValue /= 0x01000000;
			hashValue &= 0x0FFFFFFF;
			hashValue ^= tempValue;
		}
	}
	return hashValue % HashTableSize;
} // getHashz

///////////////////////////////////
// Internal string search functions
///////////////////////////////////

static int getString(char *ptTable, const char *ptKeyString, char **ptString) {
	int            HashTableSize;
	int            NumLoops;
	char         *ptFirstNode;
	char         *ptNode;
	int            HashValue;
	int            Loop;
	char         *ptIdxString;

	HashTableSize = getHashTableSize(ptTable);
	NumLoops = getNumLoops(ptTable);
	ptFirstNode = getptFirstNode(ptTable);
	HashValue = getHash(ptKeyString, HashTableSize);

	Loop = 0;
	while (Loop++ < NumLoops) {
		ptNode = ptFirstNode + NodeSize*HashValue;
		if (*ptNode + ActiveOffset == 1) {
			ptIdxString = getptIdxString(ptTable, ptNode);
			if (strcmp(ptIdxString, ptKeyString) == 0) {
				*ptString = getptNameString(ptTable, ptNode);
				return getIdxNum(ptNode);
			}
		}
		HashValue++;
		HashValue %= HashTableSize;
	}
	return -1;
} // getString
static char *getStringNum(char *ptTable, int KeyNum) {
	char         *ptFirstNode;
	char         *ptNode;
	char         *ptElement;
	int            NodeNum;

	ptFirstNode = getptFirstNode(ptTable);
	ptElement = ptTable + HeaderSize + ElementSize*KeyNum;
	NodeNum = getNodeNum(ptElement);
	ptNode = ptFirstNode + NodeSize*NodeNum;
	if (*ptNode + ActiveOffset == 1) {
		return getptNameString(ptTable, ptNode);
	}
	return NULL;
} // getStringNum

///////////////////////////////////
// Exported string search functions
///////////////////////////////////


int getNumStringByName(const char *ptKeyString, char **ptString) {
	int      IdxNbr;

	if ((!IsInit) || (ptKeyString == NULL)) {
		*ptString = NULL;
		return -1;
	}
	if (ptPatchStringTable != NULL) {
		if ((IdxNbr = getString(ptPatchStringTable, ptKeyString, ptString)) != -1)
			// KeyString found in patchstring.tbl
			return IdxNbr + PatchStringKeyNum;
	}
	if (ptExpansionStringTable != NULL) {
		if ((IdxNbr = getString(ptExpansionStringTable, ptKeyString, ptString)) != -1)
			// KeyString found in expansionstring.tbl
			return IdxNbr + ExpansionStringKeyNum;
	}
	if (ptStringTable != NULL) {
		if ((IdxNbr = getString(ptStringTable, ptKeyString, ptString)) != -1)
			// KeyString found in string.tbl
			return IdxNbr + StringKeyNum;
	}

	// KeyString was not found
	*ptString = strNameNotFound;
	return -1;
} // getNumStringByName
char *getStringByName(const char *ptKeyString) {
	char *ptString = NULL;

	getNumStringByName(ptKeyString, &ptString);
	return ptString;
} // getStringByName
char *getStringByNum(int KeyNum) {
	char   *ptString = NULL;

	if (!IsInit)
		return NULL;
	if (KeyNum >= ExpansionStringKeyNum) {
		if (ptExpansionStringTable != NULL) {
			if ((ptString = getStringNum(ptExpansionStringTable, KeyNum - ExpansionStringKeyNum)) != NULL)
				// KeyNum found in expansionstring.tbl
				return ptString;
		}
	}
	else if (KeyNum >= PatchStringKeyNum) {
		if (ptPatchStringTable != NULL) {
			if ((ptString = getStringNum(ptPatchStringTable, KeyNum - PatchStringKeyNum)) != NULL)
				// KeyNum found in patchstring.tbl
				return ptString;
		}
	}
	else {
		if (ptStringTable != NULL) {
			if ((ptString = getStringNum(ptStringTable, KeyNum - StringKeyNum)) != NULL)
				// KeyNum found in string.tbl
				return ptString;
		}
	}

	// KeyNum was not found
	return strNameNotFound;
} // getStringByNum

/////////////////
// initialization
/////////////////

static bool initTable(char **ptTable, std::string ptFileName) {
	FILE         *Source;
	char         Header[HeaderSize];
	unsigned int   FileSize;
	
	if ((fopen_s(&Source, ptFileName.c_str(), "rb")) != 0)
		return false;
	bool IsOK = false;
	if (fread(Header, sizeof(char), sizeof(Header), Source) == sizeof(Header)) {
		FileSize = getFileSize(Header);
		allocateMemory(ptTable, FileSize);
		rewind(Source);
		if (fread(*ptTable, sizeof(char), FileSize, Source) == FileSize)
			IsOK = true;
		else {
			free(*ptTable);
			*ptTable = NULL;
		}
	}
	fclose(Source);
	//   fileclose(Source);
	return IsOK;
} // initTable
int initStringTables() {
	int ret = 0;
	if (!IsInit) {
		if (!initTable(&ptStringTable, g_TokenChestPath + "\\" + strStringFilename))
			ret += 1;
		if (!initTable(&ptExpansionStringTable, g_TokenChestPath + "\\" + strExpansionStringFilename))
			ret += 2;
		if (!initTable(&ptPatchStringTable, g_TokenChestPath + "\\" + strPatchStringFilename))
			ret += 4;
			
		IsInit = true;
	}
	return ret;
} // initStringTables
bool closeTable(char *ptTable) {
	if (ptTable != NULL)
		deallocateMemory(&ptTable);
	return true;
} // closeTable
bool closeStringTables(void) {
	if (IsInit) {
		closeTable(ptStringTable);
		closeTable(ptExpansionStringTable);
		closeTable(ptPatchStringTable);
		IsInit = false;
	}
	return true;
} // closeStringTables

////////////////
// testing stuff
////////////////

void writefile(char filename[], char *ptTable) {
	FILE   *target;

	if ((fopen_s(&target, filename, "w")) != 0)
		return;

	unsigned short		NumElements;
	int					HashTableSize;
	int					NumLoops;
	char				*ptFirstNode;
	char				*ptNode;
	char				*ptElement;
	int					NodeNum;
	int					idx;

	NumElements = getNumElements(ptTable);
	HashTableSize = getHashTableSize(ptTable);
	NumLoops = getNumLoops(ptTable);
	ptFirstNode = getptFirstNode(ptTable);

	fprintf(target, "%s\n", filename);
	fprintf(target, "Elements: %d, Hashs: %d, Loops: %d\n", NumElements, HashTableSize, NumLoops);
	fprintf(target, " Num    EIdx Act HEIdx   Hash    Len   String\n");
	for (idx = 0; idx<HashTableSize; idx++) {
		ptElement = ptTable + HeaderSize + ElementSize*idx;
		NodeNum = getNodeNum(ptElement);
		ptNode = ptFirstNode + NodeSize*idx;

		fprintf(target, "%5d", idx);
		if (idx<NumElements)
			fprintf(target, "  %5d", NodeNum);
		else
			fprintf(target, "       ");
		fprintf(target, "  %1d  %5d  %5d  %5d", *ptNode + ActiveOffset, (*(unsigned short *)(ptNode + IdxNbrOffset)), (*(int *)(ptNode + HashValueOffset)), (*(unsigned short *)(ptNode + NameLenOffset)));
		fprintf(target, "   %-25s", getptIdxString(ptTable, ptNode));
		fprintf(target, "   %-80s", getptNameString(ptTable, ptNode));
		fprintf(target, "\n");
	}
	fclose(target);
} // writefile
void teststringtable(void) {
	if (initStringTables(/*stdout*/)) {
		getCRC(ptStringTable);
		getCRC(ptPatchStringTable);
		getCRC(ptExpansionStringTable);
		writefile("infostring.txt", ptStringTable);
		writefile("infopstring.txt", ptPatchStringTable);
		writefile("infoestring.txt", ptExpansionStringTable);
	}
} // teststringtable
