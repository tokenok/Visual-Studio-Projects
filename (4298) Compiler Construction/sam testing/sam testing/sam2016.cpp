/*	____________________________________________________________________________

S A M 2 0 0 7


An Assembler for the MACC2 Virtual Computer

James L. Richards
Last Update: August 28, 2007
Last Update: January 2, 2016
by Marty J. Wolf
____________________________________________________________________________
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream>
using namespace std;

const int MAXINT = 32767;

typedef unsigned char Byte;
typedef unsigned short Word;

enum OpKind {
	OIN, IA, IS, IM, IDENT, FN, FA, FS, FM, FD,
	BI, BO, BA, IC, FC, JSR, BKT, LD, STO, LDA,
	FLT, FIX, J, SR, SL, SRD, SLD, RD, WR, TRNG,
	HALT, NOP, CLR, REALDIR, STRINGDIR, INTDIR,
	SKIPDIR, LABELDIR
};

enum ErrorKind {
	NO_ERROR, UNKNOWN_OP_NAME, BAD_REG_ADDR, BAD_GEN_ADDR,
	BAD_INTEGER, BAD_REAL, BAD_STR, BAD_NAME, ILL_REG_ADDR,
	ILL_MED_ADDR, BAD_SHFT_AMT, BAD_STRING, INVALID_NAME
};

enum WarnKind {
	LONG_NAME, MISSING_R, MISSING_NUM, MISSING_RP,
	MISSING_COMMA, NAME_DEFINED, BAD_SKIP, ESC_TOO_SHORT,
	STR_TOO_SHORT, TEXT_FOLLOWS
};

struct SymRec;
typedef SymRec* SymPtr;

struct SymRec {
	string  id;
	SymPtr  left, right;
	short   patch, loc;
};


// GLOBAL VARIABLES
// ----------------

Word
Inop, Iaop, Isop, Imop, Idop, Fnop, Faop, Fsop, Fmop, Fdop,
Biop, Boop, Baop, Icop, Fcop, Jsrop, Bktop, Ldop, Stoop, Ldaop,
Fltop, Fixop, Jop, Srop, Slop, Rdop, Wrop, Trngop, Haltop;

string       Source;
ifstream     InFile;
ofstream     ListFile;

bool         Saved;     // Flag for one character lookahead 
char         Ch;        // Current character from the input 

vector<Byte> Mem;       // Memory Image being created 
Word         Lc;        // Location Counter           
ofstream     MemFile;   // File for the memory image  

SymPtr       Symbols;
int          Line;      // Number of the current input line 

ErrorKind    Error;
bool         Errs;
bool         Warning;
bool         Morewarn;
WarnKind     Warns[11];
int          Windex;

string       Arg,
Memfname,
Srcfname;

const Word BYTE_MASK = 0x00FF;

void WordToBytes(Word w, Byte& hiByte, Byte& loByte)
//
//  Converts a word to two bytes.
//
{
	loByte = Byte(w & BYTE_MASK);
	hiByte = Byte((w >> 8) & BYTE_MASK);
}

void BytesToWord(Byte hiByte, Byte loByte, Word& w)
//
// Converts two bytes to a word.
{
	w = (Word(hiByte) << 8) | Word(loByte);
}

void InsertMem(Word w)
//
//  Puts one word into memory with the high byte first.
//
{
	Byte loByte, hiByte;

	WordToBytes(w, hiByte, loByte);
	Mem.push_back(hiByte);
	Mem.push_back(loByte);
	Lc += 2;
}

void CheckTab(SymPtr cur)
//
//  Checks for undefined symbols in the symbol table.
//
{
	if (cur != NULL) {
		CheckTab(cur->left);
		if (cur->loc < 0) {
			Warning = true;
			ListFile << "   WARNING -- " << cur->id << " Undefined"
				<< endl;
			cout << "   WARNING -- " << cur->id << " Undefined"
				<< endl;
		}
		CheckTab(cur->right);
	}
}

void Warn(WarnKind w)
//
// Adds a warning to the list of warnings.
//
{
	if (!Morewarn)
		Windex = 1;
	Morewarn = true;
	Warns[Windex] = w;
	Windex++;
}

void PrintWarn()
//
// Prints warning messages.
//
{
	ListFile << "\n   WARNING -- ";
	cout << "\n   WARNING -- ";
	switch (Warns[1]) {
		case TEXT_FOLLOWS:
			ListFile << "Text follows instruction";
			cout << "Text follows instruction";
			break;
		case ESC_TOO_SHORT:
			ListFile <<
				"Need 3 digits to specify an unprintable character";
			cout << "Need 3 digits to specify an unprintable character";
			break;
		case STR_TOO_SHORT:
			ListFile << "Missing \" in string";
			cout << "Missing \" in string";
			break;
		case BAD_SKIP:
			ListFile <<
				"Skip value must be positive, skip directive ignored";
			cout << "Skip value must be positive, skip directive ignored";
			break;
		case NAME_DEFINED:
			ListFile <<
				"Name already defined, earlier definition lost";
			cout << "Name already defined, earlier definition lost";
			break;
		case LONG_NAME:
			ListFile << "Name too long, only 7 characters used";
			cout << "Name too long, only 7 characters used";
			break;
		case MISSING_R:
			ListFile << "Missing R in Register Address";
			cout << "Missing R in Register Address";
			break;
		case MISSING_NUM:
			ListFile <<
				"Missing Number in Register Address (0 assumed)";
			cout << "Missing Number in Register Address (0 assumed)";
			break;
		case MISSING_RP:
			ListFile << "Missing "" in Indexed Address";
			cout << "Missing "" in Indexed Address";
			break;
		case MISSING_COMMA:
			ListFile << "Missing ", "";
			cout << "Missing ", "";
			break;
		default:;
	}
	ListFile << " on line " << Line << endl;
	cout << " on line " << Line << endl;
	for (int i = 2; i < Windex; i++)
		Warns[i - 1] = Warns[i];
	Windex--;
	if (Windex <= 1)
		Morewarn = false;
}

void InRegAddr(Word& w, int reg, int hbit)
//
// Insert a register address into a word
//
{
	Word mask1 = 0xFFFF,
		mask2 = 0xFFFF,
		wreg;

	wreg = Word(reg);
	wreg <<= hbit - 3;

	w &= ((mask1 << (hbit + 1)) | (mask2 >> (19 - hbit)));
	w |= wreg;
}

bool Eoln(istream& in)
//
// Returns true iff the next in stream character is a new line
// character.
//
{
	return (in.peek() == '\n');
}

void GetCh()
//
// Get a character from the input -- character may have been saved
//
{
	if (!Saved) {
		if (InFile.eof())
			Ch = '%';
		else if (!Eoln(InFile)) {
			do {
				InFile.get(Ch);
				ListFile << Ch;
			} while ((Ch == ' ' || Ch == '\t') && !Eoln(InFile));
			if (Ch == '%') // skip remainder of line
			{
				while (!Eoln(InFile)) {
					InFile.get(Ch);
					ListFile << Ch;
				}
				Ch = '%';
			}
		}
		else
			Ch = '%';
	}
	else
		Saved = false;
}

void ScanName(string& id)
//
// Builds a label.
//
{
	id = "";
	while (id.length() < 7 && ((Ch >= 'A' && Ch <= 'Z') || isdigit(Ch))) {
		id += Ch;
		GetCh();
	}
	if ((Ch >= 'A' && Ch <= 'Z') || isdigit(Ch)) {
		Warn(LONG_NAME);
		while ((Ch >= 'A' && Ch <= 'Z') || isdigit(Ch))
			GetCh();
	}
	Saved = true;
}

SymPtr FindName(const string& id)
//
//  Returns a pointer to the symbol table record containing id
//  or returns NULL pointer if id is not in the symbol table.
//
{
	SymPtr temp;
	bool found;

	temp = Symbols;
	found = false;
	while (!found && (temp != NULL))
		if (temp->id == id)
			found = true;
		else if (temp->id > id)
			temp = temp->left;
		else
			temp = temp->right;
		return temp;
}

SymPtr InName(const string& id)
//
//  Inserts id into the symbol table and returns a pointer
//  to its symbol table record.
//
{
	SymPtr cur, prev;

	cur = Symbols;
	prev = NULL;
	while (cur != NULL) {
		prev = cur;
		if (cur->id > id)
			cur = cur->left;
		else
			cur = cur->right;
	}
	cur = new SymRec;
	cur->left = NULL;
	cur->right = NULL;
	cur->id = id;
	if (prev == NULL)
		Symbols = cur;
	else if (prev->id > id)
		prev->left = cur;
	else
		prev->right = cur;
	return cur;
}

void ScanStr()
//
//  Gets a quoted string from the input stream.
//
{
	bool one;
	int  ival;
	Byte byte1, byte2;
	Word wf;

	one = true;
	GetCh();
	if (Ch == '"') {
		if (!Eoln(InFile)) {
			InFile.get(Ch);
			ListFile << Ch;
		}
		while (Ch != '"' && !Eoln(InFile)) {
			if (Ch == ':') {
				if (!Eoln(InFile)) {
					InFile.get(Ch);
					ListFile << Ch;
					if (isdigit(Ch)) {
						ival = int(Ch) - int('0');
						if (!Eoln(InFile)) {
							InFile.get(Ch);
							ListFile << Ch;
							if (isdigit(Ch)) {
								ival = ival * 10
									+ int(Ch) - int('0');
								if (!Eoln(InFile)) {
									InFile.get(Ch);
									ListFile << Ch;
									if (isdigit(Ch))
										ival = ival * 10
										+ int(Ch) - int('0');
									else
										Warn(ESC_TOO_SHORT);
								}
								else
									Warn(ESC_TOO_SHORT);
							}
							else
								Warn(ESC_TOO_SHORT);
						}
						else
							Warn(ESC_TOO_SHORT);
						Ch = char(ival);
					}
				}
				else
					Warn(ESC_TOO_SHORT);
			}

			if (one) {
				one = false;
				byte1 = Byte(Ch);
			}
			else {
				one = true;
				byte2 = Byte(Ch);
				BytesToWord(byte1, byte2, wf);
				InsertMem(wf);
			}
			if (!Eoln(InFile)) {
				InFile.get(Ch);
				ListFile << Ch;
			}
		}

		if (one)
			byte1 = Byte(0);
		else
			byte2 = Byte(0);
		BytesToWord(byte1, byte2, wf);
		InsertMem(wf);
		if (Ch != '"')
			Warn(STR_TOO_SHORT);
	}
	else
		Error = BAD_STR;
}

void ScanReal(Word& w1, Word& w2)
//
//  Gets a real number from the input stream.
//
{
	union FloatRec {
		Byte  b[4];
		float rf;
	};

	FloatRec real;
	float    dval = 10.0f, rf = 0.0f;
	bool     neg = false;

	real.rf = 0.0;
	GetCh();
	if (Ch == '-' || Ch == '+') {
		if (Ch == '-') neg = true;
		GetCh();
	}
	while (isdigit(Ch)) {
		real.rf = real.rf * 10 + int(Ch) - int('0');
		GetCh();
	}
	if (Ch == '.') {
		GetCh();
		while (isdigit(Ch)) {
			real.rf = real.rf + (int(Ch) - int('0')) / dval;
			dval = dval * 10.0f;
			GetCh();
		}
	}
	else
		Saved = true;
	if (neg) real.rf = -real.rf;
	BytesToWord(real.b[3], real.b[2], w1);
	BytesToWord(real.b[1], real.b[0], w2);
}

void ScanInt(Word& w)
//
//  Gets an integer from the input stream.
//
{
	int  temp;
	bool neg;

	neg = false;
	temp = 0;
	GetCh();
	if (Ch == '-' || Ch == '+') {
		if (Ch == '-')
			neg = true;
		GetCh();
	}
	while (isdigit(Ch)) {
		temp = temp * 10 + int(Ch) - int('0');
		GetCh();
	}
	Saved = true;    // Note the lookahead. 
	if (neg)
		temp = -temp;
	if (temp > MAXINT || temp < -MAXINT - 1)
		Error = BAD_INTEGER;
	else
		w = Word(temp);
}

int GetRegAddr()
//
//  Get a register address from the input stream.
//
{
	int temp;

	GetCh();
	if (Ch == 'R')
		GetCh();
	else
		Warn(MISSING_R);
	if (isdigit(Ch)) {
		temp = int(Ch) - int('0');
		GetCh();
		if (isdigit(Ch))  // check for two digits
			temp = temp * 10 + int(Ch) - int('0');
		else
			Saved = true;
		if (temp > 15)
			Error = BAD_REG_ADDR;
	}
	else
		Warn(MISSING_NUM);
	return temp;
}

void GetGenAddr(OpKind op, Word& w1, Word& w2, bool& flag)
//
//  Sets an addressing mode.
//
{
	int    reg;
	string id;
	SymPtr idrec;

	flag = false;
	GetCh();
	if (Ch == '*') {
		w1 = w1 | 0x0040; // [6]
		GetCh();
	}
	if (Ch >= 'A' && Ch <= 'Z' && Ch != 'R') {
		flag = true;
		ScanName(id);
		idrec = FindName(id);
		if (idrec == NULL) {
			idrec = InName(id);
			idrec->loc = -1;
			idrec->patch = Lc + 2;
			w2 = Word(-1);
		}
		else if (idrec->loc == -1) {
			w2 = Word(idrec->patch);
			idrec->patch = Lc + 2;
		}
		else
			w2 = Word(idrec->loc);
		GetCh();
		if (Ch == '(') {
			w1 = w1 | 0x0020; // [5]
			reg = GetRegAddr();
			if (Error == NO_ERROR) {
				InRegAddr(w1, reg, 3);
				GetCh();
				if (Ch != ')')
					Warn(MISSING_RP);
			}
		}
		else // Ch != ')'
			w1 = w1 | 0x0010; // [4]
	}

	else if (isdigit(Ch)) {
		Saved = true;
		w1 = w1 | 0x0010; // [4]
		flag = true;
		ScanInt(w2);
	}

	else
		switch (Ch) {
			case 'R': // direct register
				flag = false;
				Saved = true;
				reg = GetRegAddr();
				InRegAddr(w1, reg, 3);
				if ((op == JSR || op == BKT || op == LDA || op == J)
					&& !(w1 & 0x0040))
					Error = ILL_REG_ADDR;
				break;

			case '#': // immediate
				flag = true;
				if (w1 & 0x0040)
					Error = BAD_GEN_ADDR;
				else if (op == FN || op == FA || op == FS || op == FM
					|| op == FD || op == FC || op == FIX
					|| op == JSR || op == BKT || op == STO
					|| op == J || op == RD || op == TRNG)
					Error = ILL_MED_ADDR;
				else if (w1 == (Wrop | 0x0080))
					Error = ILL_MED_ADDR;
				else if (w1 == (Wrop | 0x0480))
					Error = ILL_MED_ADDR;
				else {
					w1 = w1 | 0x0030; // [4, 5]
					ScanInt(w2);
				}
				break;

			case '-': case '+': // indexed 
				w1 = w1 | 0x0020; // [5]
				flag = true;
				if (Ch == '-')
					Saved = true;
				ScanInt(w2);
				GetCh();
				if (Ch == '(') {
					reg = GetRegAddr();
					if (Error == NO_ERROR) {
						InRegAddr(w1, reg, 3);
						GetCh();
						if (Ch != ')')
							Warn(MISSING_RP);
					}
				}
				else // Ch != '('
					Error = BAD_GEN_ADDR;
				break;

			case '&':
				flag = true;
				if (w1 & 0x0040) // [6]
					Error = BAD_GEN_ADDR;
				else {
					w1 = w1 | 0x0070; // [4, 5, 6]
					ScanInt(w2);
				}
				break;

			default:
				Error = BAD_GEN_ADDR;
	}
}

void GetBop(OpKind& op, Word& wd)
//
//  Get an operator or directive name that begins with 'B'.
//
{
	GetCh();
	switch (Ch) {
		case 'A':
			op = BA;
			wd = Baop;
			break;

		case 'I':
			op = BI;
			wd = Biop;
			break;

		case 'K':
			GetCh();
			if (Ch == 'T') {
				op = BKT;
				wd = Bktop;
			}
			else
				Error = UNKNOWN_OP_NAME;
			break;

		case 'O':
			op = BO;
			wd = Boop;
			break;

		default:
			// character does not legally follow `B' 
			Error = UNKNOWN_OP_NAME;
	}
}

void GetFop(OpKind& op, Word& wd)
//
//  Get an operator or directive name that begins with 'F'.
//
{
	GetCh();
	switch (Ch) {
		case 'A':
			op = FA;
			wd = Faop;
			break;

		case 'C':
			op = FC;
			wd = Fcop;
			break;

		case 'D':
			op = FD;
			wd = Fdop;
			break;

		case 'I':
			GetCh();
			if (Ch == 'X') {
				op = FIX;
				wd = Fixop;
			}
			else
				Error = UNKNOWN_OP_NAME;
			break;

		case 'L':
			GetCh();
			if (Ch == 'T') {
				op = FLT;
				wd = Fltop;
			}
			else
				Error = UNKNOWN_OP_NAME;
			break;

		case 'M':
			op = FM;
			wd = Fmop;
			break;

		case 'N':
			op = FN;
			wd = Fnop;
			break;

		case 'S':
			op = FS;
			wd = Fsop;
			break;

		default: // character does not legally follow `F'
			Error = UNKNOWN_OP_NAME;
	}
}

void GetIop(OpKind& op, Word& wd)
//
//  Get an operator or directive name that begins with 'I'.
//
{
	GetCh();

	switch (Ch) {
		case 'A':
			op = IA;
			wd = Iaop;
			break;

		case 'C':
			op = IC;
			wd = Icop;
			break;

		case 'D':
			op = IDENT;
			wd = Idop;
			break;

		case 'M':
			op = IM;
			wd = Imop;
			break;

		case 'N':
			GetCh();
			if (Ch == 'T')
				op = INTDIR;
			else {
				op = OIN;
				wd = Inop;
				Saved = true;
			}
			break;

		case 'S':
			op = IS;
			wd = Isop;
			break;

		default:
			// character does not legally follow `I' 
			Error = UNKNOWN_OP_NAME;
	}
}

void GetJop(OpKind& op, Word& wd)
//
//  Get an operator or directive name that begins with 'J'.
//
{
	GetCh();
	op = J; // most are simple jumps--except JSR!! 
	switch (Ch) {
		case 'E':
			GetCh();
			if (Ch == 'Q')
				wd = Jop | 0x0180; // [7, 8]
			else
				Error = UNKNOWN_OP_NAME;
			break;

		case 'G':
			GetCh();
			if (Ch == 'E')
				wd = Jop | 0x0280; // [7, 9]
			else if (Ch == 'T')
				wd = Jop | 0x0300; // [8, 9]
			else
				Error = UNKNOWN_OP_NAME;
			break;

		case 'L':
			GetCh();
			if (Ch == 'E')
				wd = Jop | 0x0100; // [8]
			else if (Ch == 'T')
				wd = Jop | 0x0080; // [7]
			else
				Error = UNKNOWN_OP_NAME;
			break;

		case 'M':
			GetCh();
			if (Ch == 'P')
				wd = Jop;
			else
				Error = UNKNOWN_OP_NAME;
			break;

		case 'N':
			GetCh();
			if (Ch == 'E')
				wd = Jop | 0x0200; // [9]
			else
				Error = UNKNOWN_OP_NAME;
			break;

		case 'S':
			GetCh();
			if (Ch == 'R') {
				op = JSR;
				wd = Jsrop;
			}
			else
				Error = UNKNOWN_OP_NAME;
			break;

		default:
			//Ch not in ['E','G',...] }
			Error = UNKNOWN_OP_NAME;
	}
}

void GetLop(OpKind& op, Word& wd)
//
//  Get an operator or directive name that begins with 'L'.
//
{
	GetCh();
	switch (Ch) {
		case 'A':
			GetCh();
			if (Ch == 'B') {
				GetCh();
				if (Ch == 'E') {
					GetCh();
					if (Ch == 'L')
						op = LABELDIR;
					else
						Error = UNKNOWN_OP_NAME;
				}
				else
					Error = UNKNOWN_OP_NAME;
			}
			else
				Error = UNKNOWN_OP_NAME;
			break;

		case 'D':
			GetCh();
			if (Ch == 'A') {
				op = LDA;
				wd = Ldaop;
			}
			else {
				op = LD;
				wd = Ldop;
				Saved = true;
			}
			break;

		default:
			Error = UNKNOWN_OP_NAME;
	}
}

void GetRop(OpKind& op, Word& wd)
//
//  Get an operator or directive name that begins with 'R'.
//
{
	GetCh();
	if (Ch == 'D') {
		op = RD;
		GetCh();
		switch (Ch) {
			case 'B':
				GetCh();
				if (Ch == 'D')
					wd = Rdop | 0x0100; // [8]
				else if (Ch == 'W')
					wd = Rdop | 0x0180; // [7, 8]
				else
					Error = UNKNOWN_OP_NAME;
				break;

			case 'C':
				GetCh();
				if (Ch == 'H')
					wd = Rdop | 0x0400; // [10]
				else
					Error = UNKNOWN_OP_NAME;
				break;

			case 'F':
				wd = Rdop | 0x0080; // [7]
				break;

			case 'H':
				GetCh();
				if (Ch == 'D')
					wd = Rdop | 0x0300; // [8, 9]
				else if (Ch == 'W')
					wd = Rdop | 0x0380; // [7, 8, 9]
				else
					Error = UNKNOWN_OP_NAME;
				break;

			case 'I':
				wd = Rdop;
				break;

			case 'N':
				GetCh();
				if (Ch == 'L')
					wd = Rdop | 0x0580; // [7, 8, 10]
				else
					Error = UNKNOWN_OP_NAME;
				break;

			case 'O':
				GetCh();
				if (Ch == 'D')
					wd = Rdop | 0x0200; // [9]
				else if (Ch == 'W')
					wd = Rdop | 0x0280; // [7, 10]
				else
					Error = UNKNOWN_OP_NAME;
				break;

			case 'S':
				GetCh();
				if (Ch == 'T')
					wd = Rdop | 0x0480; // [7, 10]
				else
					Error = UNKNOWN_OP_NAME;
				break;

			default:
				// Ch not in ['B','C',...] }
				Error = UNKNOWN_OP_NAME;
		}
	}
	else if (Ch == 'E') {
		GetCh();
		if (Ch == 'A') {
			GetCh();
			if (Ch == 'L')
				op = REALDIR;
			else
				Error = UNKNOWN_OP_NAME;
		}
		else // Ch != 'A' 
			Error = UNKNOWN_OP_NAME;
	}
	else // Ch != 'E'
		Error = UNKNOWN_OP_NAME;
}

void GetSop(OpKind& op, Word& wd)
//
//  Get an operator or directive name that begins with 'S'.
//
{
	GetCh();
	switch (Ch) {
		case 'K':
			GetCh();
			if (Ch == 'I') {
				GetCh();
				if (Ch == 'P')
					op = SKIPDIR;
				else
					Error = UNKNOWN_OP_NAME;
			}
			else // Ch != 'I' 
				Error = UNKNOWN_OP_NAME;
			break;

		case 'L':
			GetCh();
			op = SL;
			switch (Ch) {
				case 'D':
					GetCh();
					op = SLD;
					switch (Ch) {
						case 'C':
							wd = Slop | 0x0070; // [4, 5, 6]
							break;
						case 'E':
							wd = Slop | 0x0060; // [5, 6]
							break;
						case 'O':
							wd = Slop | 0x0050; // [4, 6]
							break;
						case 'Z':
							wd = Slop | 0x0040; // [6]
							break;
						default:
							Error = UNKNOWN_OP_NAME;
					}
					break;

				case 'C':
					wd = Slop | 0x0030; // [4, 5]
					break;

				case 'E':
					wd = Slop | 0x0020; // [5]
					break;

				case 'O':
					wd = Slop | 0x0010; // [4]
					break;

				case 'Z':
					wd = Slop;
					break;

				default:
					Error = UNKNOWN_OP_NAME;
			}
			break;

		case 'R':
			GetCh();
			op = SR;
			switch (Ch) {
				case 'D':
					GetCh();
					op = SRD;
					switch (Ch) {
						case 'C':
							wd = Srop | 0x0070; // [4, 5, 6] 
							break;
						case 'E':
							wd = Srop | 0x0060; // [5, 6]
							break;
						case 'O':
							wd = Srop | 0x0050; // [4, 6]
							break;
						case 'Z':
							wd = Srop | 0x0040; // [6]
							break;
						default:
							Error = UNKNOWN_OP_NAME;
					}
					break;

				case 'C':
					wd = Srop | 0x0030; // [4, 5]
					break;

				case 'E':
					wd = Srop | 0x0020; // [5]
					break;

				case 'O':
					wd = Srop | 0x0010; // [4]
					break;

				case 'Z':
					wd = Srop;
					break;

				default:
					Error = UNKNOWN_OP_NAME;
			}
			break;

		case 'T':
			GetCh();
			if (Ch == 'O') {
				op = STO;
				wd = Stoop;
			}
			else if (Ch == 'R') {
				GetCh();
				if (Ch == 'I') {
					GetCh();
					if (Ch == 'N') {
						GetCh();
						if (Ch == 'G')
							op = STRINGDIR;
						else
							Error = UNKNOWN_OP_NAME;
					}
					else // Ch != 'N'
						Error = UNKNOWN_OP_NAME;
				}
				else // Ch != 'I'
					Error = UNKNOWN_OP_NAME;
			}
			else // Ch != 'R'
				Error = UNKNOWN_OP_NAME;
			break;

		default:
			Error = UNKNOWN_OP_NAME;
	}
}

void GetWop(OpKind& op, Word& wd)
//
//  Get an operator or directive name that begins with 'W'.
//
{
	GetCh();
	if (Ch == 'R') {
		op = WR;
		GetCh();
		switch (Ch) {
			case 'B':
				GetCh();
				if (Ch == 'D')
					wd = Wrop | 0x0100; // WRBD
				else if (Ch == 'W')
					wd = Wrop | 0x0180; // WRBW
				else
					Error = UNKNOWN_OP_NAME;
				break;

			case 'C':
				GetCh();
				if (Ch == 'H')
					wd = Wrop | 0x0400; // WRCH
				else
					Error = UNKNOWN_OP_NAME;
				break;;

			case 'F':
				wd = Wrop | 0x0080;     // WRF
				break;

			case 'H':
				GetCh();
				if (Ch == 'D')
					wd = Wrop | 0x0300; // WRHD
				else if (Ch == 'W')
					wd = Wrop | 0x0380; // WRHW
				else
					Error = UNKNOWN_OP_NAME;
				break;

			case 'I':                   // WRI
				wd = Wrop;
				break;

			case 'N':
				GetCh();
				if (Ch == 'L')
					wd = Wrop | 0x0580; // WRNL
				else
					Error = UNKNOWN_OP_NAME;
				break;

			case 'O':
				GetCh();
				if (Ch == 'D')
					wd = Wrop | 0x0200; // WROD
				else if (Ch == 'W')
					wd = Wrop | 0x0280; // WROW
				else
					Error = UNKNOWN_OP_NAME;
				break;

			case 'S':
				GetCh();
				if (Ch == 'T')
					wd = Wrop | 0x0480; // WRST
				else
					Error = UNKNOWN_OP_NAME;
				break;

			default:
				Error = UNKNOWN_OP_NAME;
		}
	}
	else // Ch != 'R'
		Error = UNKNOWN_OP_NAME;
}

void ProLine()
//
//  Process the current line of input.
//
{

	Word   wd = 0, wd2;
	Byte   b1, b2;
	OpKind op;
	int    reg;
	bool   twowds;
	short  i1, i2;
	string id;
	SymPtr idrec;

	twowds = false;
	Error = NO_ERROR;

	switch (Ch) {
		case 'B':
			GetBop(op, wd);
			break;

		case 'C':
			GetCh();
			if (Ch == 'L') {
				GetCh();
				if (Ch == 'R') {
					op = CLR;
					wd = Srop;
				}
				else // Ch != 'R'
					Error = UNKNOWN_OP_NAME;
			}
			else // Ch != 'L'
				Error = UNKNOWN_OP_NAME;
			break;

		case 'F':
			GetFop(op, wd);
			break;

		case 'H':
			GetCh();
			if (Ch == 'A') {
				GetCh();
				if (Ch == 'L') {
					GetCh();
					if (Ch == 'T') {
						op = HALT;
						wd = Haltop;
					}
					else // Ch != 'T'
						Error = UNKNOWN_OP_NAME;
				}
				else // Ch != 'L' 
					Error = UNKNOWN_OP_NAME;
			}
			else // Ch != 'A'
				Error = UNKNOWN_OP_NAME;
			break;

		case 'I':
			GetIop(op, wd);
			break;

		case 'J':
			GetJop(op, wd);
			break;

		case 'L':
			GetLop(op, wd);
			break;

		case 'N':
			GetCh();
			if (Ch == 'O') {
				GetCh();
				if (Ch == 'P') {
					op = NOP;
					wd = Jop | 0x0380; // [7, 8, 9]
				}
				else // Ch != 'P'
					Error = UNKNOWN_OP_NAME;
			}
			else // Ch != 'O'
				Error = UNKNOWN_OP_NAME;
			break;

		case 'R':
			GetRop(op, wd);
			break;

		case 'S':
			GetSop(op, wd);
			break;

		case 'T':
			GetCh();
			if (Ch == 'R') {
				GetCh();
				if (Ch == 'N') {
					GetCh();
					if (Ch == 'G') {
						op = TRNG;
						wd = Trngop;
					}
					else // Ch != 'G'
						Error = UNKNOWN_OP_NAME;
				}
				else // Ch != 'N'
					Error = UNKNOWN_OP_NAME;
			}
			else // Ch != 'R'
				Error = UNKNOWN_OP_NAME;
			break;

		case 'W':
			GetWop(op, wd);
			break;

		default:
			Error = UNKNOWN_OP_NAME;
	}

	if (Error == NO_ERROR) {
		switch (op) {
			case CLR:
				// need to find a reg address }
				reg = GetRegAddr();
				if (Error == NO_ERROR) {
					InRegAddr(wd, reg, 10);
					InsertMem(wd);
				}
				break;

			case SL:
			case SR:
			case SLD:
			case SRD:
				reg = GetRegAddr();
				if (Error == NO_ERROR)
					InRegAddr(wd, reg, 10);
				GetCh();
				if (Ch != ',') {
					Warn(MISSING_COMMA);
					Saved = true;
				}
				ScanInt(wd2);
				if (Error == NO_ERROR) {
					reg = short(wd2);
					if (reg == 16)
						reg = 0;
					if ((reg < 16) && (reg >= 0)) {
						InRegAddr(wd, reg, 3);
						InsertMem(wd);
					}
					else
						Error = BAD_SHFT_AMT;
				}
				break;

			case OIN:
			case IA:
			case IS:
			case IM:
			case IDENT:
			case FN:
			case FA:
			case FS:
			case FM:
			case FD:
			case BI:
			case BO:
			case BA:
			case IC:
			case FC:
			case JSR:
			case BKT:
			case LD:
			case STO:
			case LDA:
			case FLT:
			case FIX:
			case TRNG:
				reg = GetRegAddr();
				if (Error == NO_ERROR) {
					InRegAddr(wd, reg, 10);
					GetCh();
					if (Ch != ',') {
						Warn(MISSING_COMMA);
						Saved = true;
					}

					GetGenAddr(op, wd, wd2, twowds);

					if (Error == NO_ERROR)
						if (twowds) {
							InsertMem(wd);
							InsertMem(wd2);
						}
						else
							InsertMem(wd);
				}
				break;

			case J:
				GetGenAddr(op, wd, wd2, twowds);
				if (Error == NO_ERROR)
					if (twowds) {
						InsertMem(wd);
						InsertMem(wd2);
					}
					else
						InsertMem(wd);
				break;

			case RD:
			case WR:
				twowds = false;
				if (!((0x0400 & wd) && (0x0100 & wd)))
					GetGenAddr(op, wd, wd2, twowds);
				if (Error == NO_ERROR)
					if (twowds) {
						InsertMem(wd);
						InsertMem(wd2);
					}
					else
						InsertMem(wd);
				break;

			case NOP:
			case HALT:
				InsertMem(wd);
				break;

			case INTDIR:
				ScanInt(wd);
				InsertMem(wd);
				break;

			case REALDIR:
				ScanReal(wd, wd2);
				InsertMem(wd);
				InsertMem(wd2);
				break;

			case SKIPDIR:
				ScanInt(wd);
				i1 = int(wd);
				if (i1 < 0)
					Warn(BAD_SKIP);
				else {
					Lc = Lc + i1;
					for (int i = 1; i <= i1; i++)
						Mem.push_back(Byte(0));
				}
				break;

			case STRINGDIR:
				ScanStr();
				break;

			case LABELDIR:
				GetCh();
				if ((Ch >= 'A' && Ch <= 'Q') || (Ch >= 'S' && Ch <= 'Z')) {
					ScanName(id);
					idrec = FindName(id);
					if (idrec == NULL)
						idrec = InName(id);
					else {
						if (idrec->loc >= 0)
							Warn(NAME_DEFINED);
						i1 = idrec->patch;
						wd = Word(Lc);
						WordToBytes(wd, b1, b2);
						while (i1 >= 0) {
							BytesToWord(Mem[i1], Mem[i1 + 1], wd);
							i2 = int(wd);
							Mem[i1] = b1;
							Mem[i1 + 1] = b2;
							i1 = i2;
						}
					}
					idrec->patch = -1;
					idrec->loc = Lc;
				}
				else
					Error = INVALID_NAME;
		}
	} // if Error = NO_ERROR
}

void InitOpcodes()
//
//  Initalize all the opcodes.0
//
{
	Inop = 0x0000; // 00000
	Iaop = 0x0800; // 00001
	Isop = 0x1000; // 00010
	Imop = 0x1800; // 00011
	Idop = 0x2000; // 00100
	Fnop = 0x2800; // 00101
	Faop = 0x3000; // 00110
	Fsop = 0x3800; // 00111
	Fmop = 0x4000; // 01000
	Fdop = 0x4800; // 01001
	Biop = 0x5000; // 01010
	Boop = 0x5800; // 01011
	Baop = 0x6000; // 01100
	Icop = 0x6800; // 01101
	Fcop = 0x7000; // 01110
	Jsrop = 0x7800; // 01111
	Bktop = 0x8000; // 10000
	Ldop = 0x8800; // 10001
	Stoop = 0x9000; // 10010
	Ldaop = 0x9800; // 10011
	Fltop = 0xA000; // 10100
	Fixop = 0xA800; // 10101
	Jop = 0xB000; // 10110
	Srop = 0xB800; // 10111
	Slop = 0xC000; // 11000
	Rdop = 0xC800; // 11001
	Wrop = 0xD000; // 11010
	Trngop = 0xD800; // 11011
	Haltop = 0xF800; // 11111
}

//---------------------------------------------------------------------

string Date() {
	const string MONTH[] =
	{"January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November",
	"December"};
	/*
	char theDate[10];
	int moNumber;

	_strdate_s(theDate);
	string strDate(theDate, theDate+8);

	moNumber = 10 * (theDate[0] - '0') + theDate[1] - '0' - 1;
	return MONTH[moNumber] + ' '
	+ ((strDate[3] == '0') ? strDate.substr(4,1)
	: strDate.substr(3,2))
	+ ", 20" + strDate.substr(6,3);
	*/
	time_t now = time(0);

	tm *ltm = localtime(&now);

	return MONTH[ltm->tm_mon] + " " +
		static_cast<ostringstream*>(
		&(ostringstream() << ltm->tm_mday))->str()
		+ ", " +
		static_cast<ostringstream*>(
		&(ostringstream() << 1900 + ltm->tm_year))->str();


}

string Time() {
	const  string HOUR[]
		= {"12", "1", "2", "3", "4", "5", "6",
		"7", "8", "9", "10", "11"};
	int    hrNumber;
	string suffix;
	string pad = "";
	time_t now = time(0);

	tm *ltm = localtime(&now);

	hrNumber = ltm->tm_hour;
	if (ltm->tm_hour < 12)
		suffix = " A.M.";
	else {
		suffix = " P.M.";
		hrNumber -= 12;
	}

	if (ltm->tm_min < 10)
		pad = "0";
	return HOUR[hrNumber] + ':' + pad +
		static_cast<ostringstream*>(
		&(ostringstream() << ltm->tm_min))->str()
		+ suffix;
}

int main(int argc, char* argv[]) {

	cout << "SAM 2016 ASSEMBLER\n" << endl;
	cout << "SAM source file name: ";
	//	getline(cin, Source);
	if (argc > 1) {
		Source = argv[1];
		cout << Source << '\n';
	}
	else
		Source = "C:\\Users\\Josh_2.Josh-PC\\Documents\\Visual Studio 2013\\Projects\\macc testing\\Release\\a";

	InFile.open((Source + ".asm").data());
	if (!InFile.is_open()) {
		cout << "\nFile \"" << Source + ".asm" << "\" not found!"
			<< "\nAssembly aborted.\n" << endl;
		cin.get();
		exit(1);
	}

	ListFile.open((Source + ".lis").data());
	ListFile << endl;
	ListFile << "   SAM 2016 Assembler Listing\n" << endl;
	ListFile << "   " + Date() << " at " << Time() << endl;
	ListFile << "   SOURCE FILE: " + Source + ".asm" << endl;

	Symbols = NULL;
	InitOpcodes();
	Line = 0;
	Lc = 0;
	Errs = false;
	Warning = false;
	Saved = false;

	InFile.peek();
	if (!InFile.eof()) {
		ListFile << endl;
		ListFile << setw(10) << "LN" << setw(6) << "LC" << endl;
		ListFile << setw(10) << Line
			<< setw(6) << hex << uppercase << Lc
			<< dec << ": ";
	}
	else {
		InFile.close();
		ListFile.close();
		cout << "\nFile is empty.\n" << endl;
		exit(1);
	}

	InFile.peek();
	while (!InFile.eof()) {
		GetCh();
		InFile.peek();
		while (!InFile.eof() && (Ch == '%')) {
			InFile.ignore(256, '\n');
			InFile.peek();
			if (!InFile.eof()) {
				ListFile << endl << flush;
				ListFile << setw(10) << ++Line
					<< setw(6) << hex << uppercase << Lc
					<< dec << ": ";
				GetCh();
			}
		}
		if (Eoln(InFile)) {//skip over blank lines in input file
			GetCh();
			continue;
		}
		if (!InFile.eof()) { // instruction processing
			Error = NO_ERROR;
			Morewarn = false;
			ProLine();
			GetCh();
			if (Ch != '%' && !isspace(Ch)) { // skip text after instruction
				Warn(TEXT_FOLLOWS);
				do {
					InFile.get(Ch);
					ListFile << Ch;
				} while (Ch != '\n');
			}

			if (Error != NO_ERROR) {
				ListFile << endl;
				Errs = true;
				ListFile << "   ERROR -- ";
				switch (Error) {
					case INVALID_NAME:
						ListFile <<
							"Invalid Name in Label Directive";
						break;
					case UNKNOWN_OP_NAME:
						ListFile <<
							"Unknown Operation or Directive Name";
						break;
					case BAD_STR:
						ListFile <<
							"Improper String Directive -- Missing \"";
						break;
					case BAD_GEN_ADDR:
						ListFile <<
							"Improperly Formed General Address";
						break;
					case BAD_REG_ADDR:
						ListFile <<
							"Register Address Out of Range";
						break;
					case BAD_INTEGER:
						ListFile <<
							"Improperly Formed Integer Constant";
						break;
					case BAD_REAL:
						ListFile <<
							"Improperly Formed Real Constant";
						break;
					case BAD_STRING:
						ListFile <<
							"Improperly Formed String Constant";
						break;
					case BAD_NAME:
						ListFile <<
							"Improperly Formed Name";
						break;
					case ILL_REG_ADDR:
						ListFile <<
							"Direct Register Address not Permitted";
						break;
					case ILL_MED_ADDR:
						ListFile <<
							"Immediate Address not Permitted";
						break;
					case BAD_SHFT_AMT:
						ListFile <<
							"Shift Amount not in Range";
						break;
					default:;
				}
				ListFile << " detected on line " << Line << endl;
			}
			while (Morewarn)
				PrintWarn();
		}
	}

	ListFile << endl << endl;
	CheckTab(Symbols);

	if (!Errs && !Warning) {
		MemFile.open((Source + ".obj").data(), ios_base::binary);
		ListFile << "\nMACC Memory Hexadecimal Dump\n" << endl;
		ListFile << " ADDR |";
		for (int i = 0; i < 16; i++) ListFile << setw(4) << hex << i;
		ListFile << endl;
		ListFile << "------+";
		for (int i = 0; i < 16; i++) ListFile << "----";
		for (int i = 0; i < Lc; i++) {
			MemFile.put(Mem[i]);
			if (i % 16 == 0) ListFile << endl << setw(5) << i << " |";
			ListFile << "  ";
			if (Mem[i] < 16) ListFile << "0";
			ListFile << hex << short(Mem[i]);
		}
		ListFile << endl << endl;
		MemFile.close();

		cout << "   SUCCESSFUL ASSEMBLY." << endl;
		cout << "   OBJECT CODE FILE: " + Source + ".obj" << endl;
	}
	else {
		cout << "   ERRORS/WARNINGS IN ASSEMBLY CODE." << endl;
		cout << "   NO OBJECT CODE FILE WAS GENERATED." << endl;
	}

	ListFile.close();
	InFile.close();

	//cin.ignore(256, '\n');
	//cin.get(); // wait for Enter 

	string cmd = "macc \"" + Source + "\"";
	system(cmd.c_str());

	cin.get();

	return 0;
}
