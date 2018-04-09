/*	____________________________________________________________________________

	    Semantics and Coding Component Implementation for the Micro Compiler

	                               mcode.cpp

	                              Version 2007
 
	                           James L. Richards
	                     Last Update: August 28, 2007
	                     Update by M. J. Wolf: January 21,2016

	The routines in this unit are based on those provided in the book 
	"Crafting A Compiler" by Charles N. Fischer and Richard J. LeBlanc, Jr., 
	Benjamin Cummings Publishing Co. (1991).

	See Section 2.3-2.4, pp. 31-40.
	____________________________________________________________________________
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

extern ifstream sourceFile;
extern ofstream outFile, listFile;

#include "mascan.h"   // Scanner class definition
#include "macode.h"   // CodeGen class definition

extern Scanner scan; // global Scanner object declared in micro.cpp

// *******************
// **  Constructor  **
// *******************

CodeGen::CodeGen()
{
	maxTemp = 0;
}

// *******************************
// ** Private Member Functions  **
// *******************************

void CodeGen::CheckId(const string & s)
{
	if (!LookUp(s))  // variable not declared yet
		Enter(s);
}

void CodeGen::Enter(const string & s)
{
	symbolTable.push_back(s);
}

void CodeGen::CheckStr(const string & s) {
	if (!LookUpStr(s))
		EnterStr(s);
}

void CodeGen::EnterStr(const string & s) {
	stringTable.push_back(s + "\0");
}

void CodeGen::ExtractExpr(const ExprRec & e, string& s)
{
	string t;
	int k, n;

	switch (e.kind)
	{
	case ID_EXPR:
	case TEMP_EXPR:  // operand form: +k(R15)
		s = e.name;
		n = 0;
		while (symbolTable[n] != s) n++;
		k = 2 * n;  // offset: 2 bytes per variable
		IntToAlpha(k, t);
		s = "+" + t + "(R15)";
		break;
	case LITERAL_EXPR:
		if (e.name == "&str" && e.val == STR_LITERAL) {
			n = 0;
			k = 0;
			while (stringTable[k] != e.str) {
				n += stringTable[k].size() + stringTable[k].size() % 2;
				k++;
			}
			IntToAlpha(n, t);
			s = "+" + t + "(R14)";
		}
		else {
			IntToAlpha(e.val, t);
			s = "#" + t;
		}
	}
}

string CodeGen::ExtractOp(const OpRec & o)
{
	if (o.op == PLUS)
		return "IA        ";
	else
		return "IS        ";
}

void CodeGen::Generate(const string & s1, const string & s2, const string & s3)
{
	listFile.width(20);
	listFile << ' ' << s1;
	outFile << s1;
	if (s2.length() > 0)
	{
		listFile << s2;
		outFile << s2;
		if (s3.length() > 0)
		{
			listFile << ',' << s3;
			outFile << ',' << s3;
		}
	}
	listFile << endl;
	outFile << endl;
}

string CodeGen::GetTemp()
{
	string s;
	static string t;

	t = "Temp&";
	IntToAlpha(++maxTemp, s);
	t += s;
	CheckId(t);
	return t;
}

void CodeGen::IntToAlpha(int val, string& str)
{
	int k;
	char temp;

	str = "";
	if (val == 0) str = "0";
	while (val > 0)
	{
		str.append(1, (char)(val % 10 + (int)'0'));
		val /= 10;
	}
	k = int(str.length());
	for (int i = 0; i < k/2; i++)
	{
		temp = str[i];
		str[i] = str[k-i-1];
		str[k-i-1] = temp;
	}
}

bool CodeGen::LookUp(const string & s)
{
	for (unsigned i = 0; i < symbolTable.size(); i++)
	if (symbolTable[i] == s)
		return true;

	return false;
}

bool CodeGen::LookUpStr(const string & s) {
	for (unsigned i = 0; i < stringTable.size(); i++)
		if (stringTable[i] == s)
			return true;

	return false;
}

// ******************************
// ** Public Member Functions  **
// ******************************

void CodeGen::Assign(const ExprRec & target, const ExprRec & source)
{
	string s;

	ExtractExpr(source, s);
	Generate("LD        ", "R0", s);
	ExtractExpr(target, s);
	Generate("STO       ", "R0", s);
}

void CodeGen::Finish()
{
	string s;

	listFile.width(6);
	listFile << ++scan.lineNumber << "  " << scan.lineBuffer << endl;
	Generate("HALT      ", "", "");
	Generate("LABEL     ", "VARS", "");
	IntToAlpha(int(2*(symbolTable.size()+1)), s);
	Generate("SKIP      ", s, "");

	Generate("LABEL     ", "STRS", "");
	int ss = 0;
	for (int i = 0; i < stringTable.size(); i++) {
		ss += stringTable[i].size();
		Generate("STRING    ", "\"" + stringTable[i] + "\"", "");
	}
	IntToAlpha(ss, s);
	Generate("SKIP      ", s, "");

	outFile.close();
	listFile << endl << endl;
	listFile << " _____________________________________________\n";
	listFile << " <><><><>   S Y M B O L   T A B L E   <><><><>\n"
		<< endl;
	listFile << " Relative" << endl;
	listFile << " Address      Identifier" << endl;
	listFile << " --------     --------------------------------" 
		<< endl;
	for (unsigned i = 0; i < symbolTable.size(); i++)
	{
		listFile.width(7);
		listFile << 2*i << "       " << symbolTable[i] << endl;
	}
	listFile << " _____________________________________________" 
		<< endl;
	listFile << endl;


	




	listFile << " Normal successful compilation." << endl;
	listFile.close();
}

void CodeGen::GenInfix(const ExprRec & e1, const OpRec & op, 
                       const ExprRec & e2, ExprRec& e)
{
	string opnd;

	if (e1.kind == LITERAL_EXPR && e2.kind == LITERAL_EXPR)
	{
		e.kind = LITERAL_EXPR;
		switch (op.op)
		{
		case PLUS:
			e.val = e1.val + e2.val;
			break;
		case MINUS:
			e.val = e1.val - e2.val;
		}
	}
	else
	{
		e.kind = TEMP_EXPR;
		e.name = GetTemp();
		ExtractExpr(e1, opnd);
		Generate("LD        ", "R0", opnd);
		ExtractExpr(e2, opnd);
		Generate(ExtractOp(op), "R0", opnd);
		ExtractExpr(e, opnd);
		Generate("STO       ", "R0", opnd);
	}
}

void CodeGen::NewLine()
{
	Generate("WRNL      ", "", "");
}

void CodeGen::ProcessId(ExprRec& e)
{
	CheckId(scan.tokenBuffer);
	e.kind = ID_EXPR;
	e.name = scan.tokenBuffer;
}

void CodeGen::ProcessLiteral(ExprRec& e)
{
	e.kind = LITERAL_EXPR;
	e.val = atoi(scan.tokenBuffer.data());
}

void CodeGen::ProcessStrLiteral(ExprRec& e) {
	CheckStr(scan.tokenBuffer.data());
	e.kind = LITERAL_EXPR;
	e.name = "&str";
	e.val = STR_LITERAL;
	e.str = scan.tokenBuffer.data();
}

void CodeGen::ProcessOp(OpRec& o)
{
	if (scan.tokenBuffer == "+")
		o.op = PLUS;
	else
		o.op = MINUS;
}

void CodeGen::ReadId(const ExprRec & inVar)
{
	string s;

	ExtractExpr(inVar, s);
	Generate("RDI       ", s, "");
}

void CodeGen::Start()
{
	Generate("LDA       ", "R15", "VARS");
	Generate("LDA       ", "R14", "STRS");
}

void CodeGen::WriteExpr(const ExprRec & outExpr)
{
	string s;

	ExtractExpr(outExpr, s);

	Generate("WRI       ", s, "");
}

void CodeGen::write_string(const ExprRec & outExpr) {
	string s;

	ExtractExpr(outExpr, s);

	Generate("WRST      ", s, "");
}
