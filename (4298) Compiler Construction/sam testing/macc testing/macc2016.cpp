/*	____________________________________________________________________________

	C++ Version of the MACC1 Interpreter for the MACC2 Byte-Addressable Machine

	                            Macc2007.cpp

	    Author: C.T. Wilkes (from the original MACC1 source by A.B. Maccabe)

					C++ Translation by James L. Richards
	                   Last Modified: August 28, 2007
			   Last Modified: Jan 3, 2016 by Marty J. Wolf

   _____________________________________________________________________________
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>
using namespace std;

const short
	MEM_SIZE = 32767,  // 15-bit addresses
	REG_SIZE = 16,     // 16-bit registers
	MAX_INT  = 32767,

				  //  Addressing Modes 
				  //-------------------
	DREG   = 0,   // direct register   
	DMEM   = 1,   // direct memory     
	INDXD  = 2,   // indexed           
	IMMED  = 3,   // immediate         
	IREG   = 4,   // indirect register 
	IMEM   = 5,   // indirect memory   
	IINDXD = 6,   // indirect indexed  
	PCREL  = 7,   // Pc relative       

	// Opcodes     |      Machine Operations       
	//-------------|-------------------------------
	IN     = 0,   // Integer Negation              
	IA     = 1,   // Integer Addition              
	IS     = 2,   // Integer Subtraction           
	IM     = 3,   // Integer Multiplication        
	ID     = 4,   // Integer Division              
	FN     = 5,   // Floating point Negation      
	FA     = 6,   // Floating point Addition       
	FS     = 7,   // Floating point Subtraction    
	FM     = 8,   // Floating point Multiplication 
	FD     = 9,   // Floating point Division       
	BI     = 10,  // Bitwise Inversion             
	BO     = 11,  // Bitwise or                    
	BA     = 12,  // Bitwise and                   
	IC     = 13,  // Integer Comparison            
	FC     = 14,  // Floating point Comparison     
	JSR    = 15,  // Jump to SubRoutine            
	BKT    = 16,  // BlocK Transfer                
	LD     = 17,  // Load                          
	STO    = 18,  // STOre                         
	LDA    = 19,  // LoaD Address                  
	FLT    = 20,  // Integer to FLoat              
	FIX    = 21,  // Float to Integer              
	J      = 22,  // Jump                          
	SR     = 23,  // Shift Right                   
	SL     = 24,  // Shift Left                    
	RD     = 25,  // ReaD                          
	WR     = 26,  // WRite                         
	TRNG   = 27,  // Test RaNGe                    
	HALT   = 31;  // HALT execution                

	typedef short RegAddr;           //  = 0..15

	typedef unsigned short MemAddr;  //  = 0..MEM_SIZE

	typedef unsigned char Byte;      //  = bits 0..7

	typedef unsigned short Word;     //  = bits 0..15

	typedef unsigned int DoubleWord; //  = bits 0..31

	typedef short WordRange;         //  = -MAX_INT..MAX_INT

	typedef char ByteRange;          //  = -128..127
 
	union ICrec
	{
		Word      wf;
		WordRange sintf;
	};

	union BCrec
	{
        Byte      bytef;
		ByteRange hintf;
	};

	union RCrec
	{
		float rf;
		Byte  b[4];
	};

// global variables

	Byte     Mem[MEM_SIZE];   // Memory
	Word     Regs[REG_SIZE];  // Registers
	MemAddr  Pc;              // Program Counter
	Word     Ir;              // Instruction Register 

	bool     StopProgram;     // Machine halt flag
	bool     Lt, Eq, Gt;      // CPU status bits
	MemAddr  oldPc;

	ICrec    Icr;             // Integer conversion record   
	RCrec    Rcr;             // Real conversion record      
	BCrec    Bcr;             // Boolean conversion record  

	bool     MemImage;        // true iff nonempty memory image exists 
	bool     Trace;           // true iff a trace file is desired

	string   Source;          // name of object code file

	ofstream TraceFile;       // file stream for tracing


void WordToBytes(Word w, Byte& hiByte, Byte& loByte)
{
	loByte = Byte(w & 0x00FF);
	hiByte = Byte(w >> 8);
}

void BytesToWord(Byte hiByte, Byte loByte, Word& w)
{
	w = (Word(hiByte) << 8) | Word(loByte);
}

void DoublewordToWords(DoubleWord dw, Word& loWord, Word& hiWord)
{
	loWord = Word(dw & 0x0000FFFF);
	hiWord = Word(dw >> 16);
}

void WordsToDoubleword(Word loWord, Word hiWord, DoubleWord& dw)
{
	dw = (DoubleWord(hiWord) << 16) | DoubleWord(loWord);
}

//[][][][][][][][][][][][][ Tracing Routines  ][][][][][][][][][][][][][]

void ShowReg (short reg)
//
//  Writes a register symbol to the listing.
//
{
	TraceFile << 'r' << reg;
}

void ShowIndxd(short r, short a)
//
//  Writes a symbolic indexed address to the trace file.
//
{
	if (a >= 0) TraceFile << '+';
    TraceFile << a << '(';
	ShowReg (r);
	TraceFile << ')';
}

void ShowAddr(short location, short mode, short r2, short a2)
//
//  Writes a symbolic general address to the trace file.
//
{
    switch (mode)
	{
	case DREG:
		ShowReg(r2);
		break;
	case DMEM:
		TraceFile << hex << a2 << dec;
		break;
	case INDXD:
		ShowIndxd (r2, a2);
		break;
	case IMMED:
		TraceFile << '#' << a2;
		break;
	case IREG:
		TraceFile << '*';
		ShowReg (r2);
		break;
	case IMEM:
		TraceFile << '*' << hex << a2 << dec;
		break;
	case IINDXD:
		TraceFile << '*';
		ShowIndxd (r2, a2);
		break;
	case PCREL:
		TraceFile << '&' << hex << a2 << hex;
		break;
	default:
		TraceFile << "  invalid addressing mode = " << mode;
	}
}

void WriteInst (short loc, short opcode, short field1, short field2, 
				short field3, short addr2)
//
//  Writes out an instruction to the trace file.
//
{
	TraceFile << setw(5) << ' ' << setw(5) << loc << setw(5) << ' ';
	switch (opcode)
	{
	case IN:  TraceFile << "IN    "; break;

	case IA:  TraceFile << "IA    "; break;
	case IS:  TraceFile << "IS    "; break;
	case IM:  TraceFile << "IM    "; break;
	case ID:  TraceFile << "ID    "; break;

	case FN:  TraceFile << "FN    "; break;
	case FA:  TraceFile << "FA    "; break;
	case FS:  TraceFile << "FS    "; break;
	case FM:  TraceFile << "FM    "; break;
	case FD:  TraceFile << "FD    "; break;

	case BI:  TraceFile << "BI    "; break;
	case BO:  TraceFile << "BO    "; break;
	case BA:  TraceFile << "BA    "; break;

	case IC:  TraceFile << "IC    "; break;
	case FC:  TraceFile << "FC    "; break;

	case JSR: TraceFile << "JSR   "; break;

	case BKT: TraceFile << "BKT   "; break;
	case LD:  TraceFile << "LD    "; break;
	case STO: TraceFile << "STO   "; break;
	case LDA: TraceFile << "LDA   "; break;

	case FLT: TraceFile << "FLT   "; break;
	case FIX: TraceFile << "FIX   "; break;

    case J:
		switch (field1)
		{
		case 0: TraceFile << "JMP   "; break;
		case 1: TraceFile << "JLT   "; break;
		case 2: TraceFile << "JLE   "; break;
		case 3: TraceFile << "JEQ   "; break;
		case 4: TraceFile << "JNE   "; break;
		case 5: TraceFile << "JGE   "; break;
		case 6: TraceFile << "JGT   "; break;
		case 7: TraceFile << "NOP   "; break;
		default:
			TraceFile << "  Invalid J instruction, type = " << field1 << ": ";
		}
		break;

    case SR:
		switch (field2)
		{
		case 0: TraceFile << "SRZ   "; break;
		case 1: TraceFile << "SRO   "; break;
		case 2: TraceFile << "SRE   "; break;
		case 3: TraceFile << "SRC   "; break;
		case 4: TraceFile << "SRCZ  "; break;
		case 5: TraceFile << "SRCO  "; break;
		case 6: TraceFile << "SRCE  "; break;
		case 7: TraceFile << "SRCC  "; break;
		default:
			TraceFile << "  Invalid SR instruction, mode = " << field2 << ": ";
		}
		break;

    case SL:
		switch (field2)
		{
		case 0: TraceFile << "SLZ   "; break;
		case 1: TraceFile << "SLP   "; break;
		case 2: TraceFile << "SLE   "; break;
		case 3: TraceFile << "SLC   "; break;
		case 4: TraceFile << "SLCZ  "; break;
		case 5: TraceFile << "SLCO  "; break;
		case 6: TraceFile << "SLCE  "; break;
		case 7: TraceFile << "SLCC  "; break;
		default:
			TraceFile << "  Invalid SL instruction, mode = " << field2 << ": ";
		}
		break;

    case RD:
		switch (field1)
		{
		case  0: TraceFile << "RDI   "; break;
		case  1: TraceFile << "RDF   "; break;
		case  2: TraceFile << "RDBD  "; break;
		case  3: TraceFile << "RDBW  "; break;
		case  4: TraceFile << "RDOD  "; break;
		case  5: TraceFile << "RDOW  "; break;
		case  6: TraceFile << "RDHD  "; break;
		case  7: TraceFile << "RDHW  "; break;
		case  8: TraceFile << "RDCH  "; break;
		case  9: TraceFile << "RDST  "; break;
		case 10: TraceFile << "RDIN  "; break;
		case 11: TraceFile << "RDLN  "; break;
		default:
			TraceFile << "  Invalid RD instruction, fmt = " << field1 << ": ";
		}
		break;

    case WR:
		switch (field1)
		{
		case  0: TraceFile << "WRI   "; break;
		case  1: TraceFile << "WRF   "; break;
		case  2: TraceFile << "WRBD  "; break;
		case  3: TraceFile << "WRBW  "; break;
		case  4: TraceFile << "WROD  "; break;
		case  5: TraceFile << "WROW  "; break;
		case  6: TraceFile << "WRHD  "; break;
		case  7: TraceFile << "WRHW  "; break;
		case  8: TraceFile << "WRCH  "; break;
		case  9: TraceFile << "WRST  "; break;
		case 10: TraceFile << "WRIN  "; break;
		case 11: TraceFile << "WRLN  "; break;
		default:
			TraceFile << "  Invalid WR instruction, fmt = " 
						<< field1 << ": ";
		}
		break;

	case TRNG: TraceFile << "TRNG  "; break;

    case HALT: TraceFile << "HALT  "; break;

    default:
		TraceFile << "  Invalid instruction, opcode = " 
					<< opcode << ": '";
	}

	if ((opcode >= IN && opcode <= FLT) || opcode == TRNG) 
	{ // 2 address
		ShowReg(field1);
		TraceFile << ", ";
		ShowAddr (loc, field2, field3, addr2);
	}
	else if ((opcode == J && field1 != 7)      // jump but not nop   
			|| (opcode == WR && field1 != 11)  // write but not wrnl 
            || (opcode == RD))
		ShowAddr (loc, field2, field3, addr2);
	else if (opcode == SR || opcode == SL)     // 1 register address 
    {
      ShowReg(field1);
      TraceFile << ", ";
      TraceFile << field3;  // amnt 
    }
	TraceFile << endl;
}

void WriteBoolean (bool b)
{
	TraceFile << boolalpha << b;
}

void DisplayRegs()
{
	TraceFile << "  Pc = " << Pc << "  Lt = ";
	WriteBoolean (Lt);
	TraceFile << "  Eq = ";
	WriteBoolean (Eq);
	TraceFile << "  Gt = ";
	WriteBoolean (Gt);
	TraceFile << endl << endl;
	for (short i = 0; i < REG_SIZE; i++)
	{
		Icr.wf = Regs[i];
		TraceFile << "  Reg[" << setw(2) << dec << i << "] = " 
			<< setw(5) << hex << Icr.sintf;
		if ((i + 1) % 4 == 0)
			TraceFile << endl;
	}
	TraceFile << dec << endl;
}

//[][][][][][][][][][][][ End of Tracing Routines ][][][][][][][][][][][]

void FetchWord (Word& w)
//
// Retrieves the word at the memory address Pc and increments Pc by 2.
{
  BytesToWord(Mem[Pc], Mem[Pc+1], w);
  Pc = (Pc + 2) % MEM_SIZE;
}

short Eac(short mode, RegAddr reg, short& w2)
//
//  Perform effective address calculation -- update Pc as needed for 2
//  Word instructions.
{
	short taddr; // temporary address

	w2 = 0;
	switch (mode)
	{
	case DREG:
		taddr = -1;		// indicates direct reg
		break;

	case DMEM:
		FetchWord (Icr.wf);
		taddr = Icr.sintf ;
		w2 = taddr;
		break;

	case INDXD:
		FetchWord (Icr.wf);
		w2 = Icr.sintf ;
		Icr.wf = Regs[reg];
		taddr = (w2 + Icr.sintf) % MEM_SIZE;
		break;

    case IMMED:
		taddr = Pc;
		BytesToWord (Mem[taddr], Mem[taddr+1], Icr.wf);
		w2 = Icr.sintf ;
		Pc = (Pc + 2) % MEM_SIZE;
		break;

    case IREG:
        Icr.wf = Regs[reg];
        taddr = Icr.sintf;
        break;

    case IMEM:
		FetchWord (Icr.wf);
		w2 = Icr.sintf ;
		BytesToWord(Mem[w2], Mem[w2+1], Icr.wf);
		taddr = Icr.sintf;
		break;

    case IINDXD:
		FetchWord (Icr.wf);
		w2 = Icr.sintf ;
		Icr.wf = Regs[reg];
		taddr = (w2 + Icr.sintf ) % MEM_SIZE;
		BytesToWord(Mem[taddr], Mem[taddr+1], Icr.wf);
		// Icr.wf := Icr.wf - [15]; 
		taddr = Icr.sintf;
		break;

    case PCREL:
        FetchWord(Icr.wf);
        w2 = Icr.sintf;
        taddr = (w2 + Pc) % MEM_SIZE;
	}
	return taddr;
}

void TwoAddr (short opcode, short r1, short amode, short r2, short& w2)
//
// Perform two address operation.
//
{
	Word  wd;
	short addr, iop1, iop2, ans;
	float rop1, rop2;

	addr = Eac(amode, r2, w2);
	if (opcode == IN || opcode == IA || opcode == IS 
			|| opcode == IM || opcode == ID || opcode == IC)
	{
		if (addr >= 0)
			BytesToWord (Mem[addr], Mem[addr+1], Icr.wf);
		else
			Icr.wf = Regs[r2];
		iop2 = Icr.sintf ;
		Icr.wf = Regs[r1];
		iop1 = Icr.sintf ;
		switch (opcode)
		{
        case IN:
			ans = -iop2;
			break;
        case IA:
			ans = iop1 + iop2;
			break;
        case IS:
			ans = iop1 - iop2;
			break;
        case IM:
			ans = iop1 * iop2;
			break;
        case ID:
			ans = iop1 / iop2;
			break;
        case IC:
            ans = iop1;    // to restore state of Regs[r1] 
            Lt = false;
            Eq = false;
            Gt = false;
            if (iop1 < iop2)
              Lt = true;
            else if (iop1 == iop2)
              Eq = true;
            else
              Gt = true;
            break;
		}
		Icr.sintf = ans;
		Regs[r1] = Icr.wf;
	}

	else if (opcode == FN || opcode == FA || opcode == FS 
				|| opcode == FM || opcode == FD || opcode == FC)
	{
		WordToBytes(Regs[r1], Rcr.b[3], Rcr.b[2]) ;
		WordToBytes(Regs[(r1 + 1) % REG_SIZE], Rcr.b[1], Rcr.b[0]);
		rop1 = Rcr.rf;
		if (addr >= 0)
        {
			Rcr.b[3] = Mem [addr];
			Rcr.b[2] = Mem [addr + 1];
			Rcr.b[1] = Mem [addr + 2];
			Rcr.b[0] = Mem [addr + 3];
        }
		else
        {
			WordToBytes(Regs[r2], Rcr.b[3], Rcr.b[2]);
			WordToBytes(Regs[(r2 + 1) % REG_SIZE], Rcr.b[1], Rcr.b[0]);
        }
		rop2 = Rcr.rf;
		switch (opcode)
		{
        case FN:
			rop1 = -rop2;
			break;
        case FA:
			rop1 = rop1 + rop2;
			break;
        case FS:
			rop1 = rop1 - rop2;
			break;
        case FM:
			rop1 = rop1 * rop2;
			break;
        case FD:
			rop1 = rop1 / rop2;
			break;
        case FC:
			Lt = false;
			Eq = false;
			Gt = false;
			if (rop1 < rop2)
				Lt = true;
			else if (rop1 == rop2)
				Eq = true;
			else
				Gt = true;
		}
		Rcr.rf = rop1;
		BytesToWord(Rcr.b[3], Rcr.b[2], Regs[r1]) ;
		BytesToWord(Rcr.b[1], Rcr.b[0], Regs[(r1 + 1) % REG_SIZE]);
	}
	else if (opcode == BI || opcode == BO || opcode == BA)
	{
		if (addr >= 0)
			BytesToWord(Mem[addr], Mem[addr+1], wd);
		else
			wd = Regs[r2];
		Word mask = 0x0001;
		switch (opcode)
		{
		case BI:
			Regs[r1] = 0x0000;
			for (short i = 0; i <= 15; i++)
			{
				if (!(mask & wd))
					Regs[r1] = Regs[r1] | mask;
				mask = mask << 1;
			}
			break;
		case BO:
			Regs[r1] = Regs[r1] | wd;
			break;
		case BA:
			Regs[r1] = Regs[r1] & wd;
		}
	}
	else if (opcode == JSR)
    {
		Icr.sintf = Pc;
		Regs[r1] = Icr.wf;
		if (addr >= 0)
			Pc = addr;
		else
		{
			cout << "JSR to a Register " << (Pc - 2) << endl;
			if (Trace)
				TraceFile << "JSR to a Register " << (Pc - 2) << endl;
			StopProgram = true;
		}
	}
	else if (opcode == BKT)
		if (addr < 0)
		{
			cout << "Address of BKT is a Register " << (Pc - 2) << endl;
			if (Trace)
				TraceFile << "Address of BKT is a Register " 
					<< (Pc - 2) << endl;
			StopProgram = true;
		}
		else
		{
			Icr.wf = Regs[r1];
			iop2 = Icr.sintf;
			Icr.wf = Regs[(r1 + 1) % REG_SIZE];
			for (short i = 0; i < Icr.sintf; i++)
				Mem[(addr + i) % MEM_SIZE] = Mem[(iop2 + i) % MEM_SIZE];
		}
    else if (opcode == LD)
		if (addr >= 0)
			BytesToWord(Mem[addr], Mem[addr+1], Regs[r1]);
		else
			Regs[r1] = Regs[r2];

	else if (opcode == STO)
		if (addr >= 0)
		{
			WordToBytes(Regs[r1], Mem[addr], Mem[addr+1]);
			if (Trace)
            {
				BytesToWord(Mem[addr], Mem[addr+1], Icr.wf);
				TraceFile << "  ***** Value " << Icr.sintf
						<< " stored at address " << hex << addr 
						<< dec << endl;
            }
		}
		else
			Regs[r2] = Regs[r1];

	else if (opcode == LDA)
		if (addr >= 0)
		{
			Icr.sintf = addr;
			Regs[r1] = Icr.wf;
		}
		else
		{
			cout << "Address of LDA is a Register " << (Pc - 2) << endl;
			if (Trace)
				TraceFile << "Address of LDA is a Register " 
					<< (Pc - 2) << endl;
			StopProgram = true;
		}

	else if (opcode == FLT)
	{
        if (addr >= 0)
			BytesToWord(Mem[addr], Mem[addr+1], Icr.wf);
        else
			Icr.wf = Regs[r2];
        Rcr.rf = float(Icr.sintf);
        BytesToWord(Rcr.b[3], Rcr.b[2], Regs[r1]) ;
        BytesToWord(Rcr.b[1], Rcr.b[0], Regs[(r1 + 1) % REG_SIZE]);
	}

	else if (opcode == FIX)
	{
		if (addr >= 0)
		{
			Rcr.b[3] = Mem [ addr     ];
			Rcr.b[2] = Mem [ addr + 1 ];
			Rcr.b[1] = Mem [ addr + 2 ];
			Rcr.b[0] = Mem [ addr + 3 ];
		}
		else
		{
			WordToBytes(Regs[r2], Rcr.b[3], Rcr.b[2]);
			WordToBytes(Regs[(r2 + 1) % REG_SIZE], Rcr.b[1], Rcr.b[0]);
		}
		Icr.sintf = short(Rcr.rf + 0.5f);
		Regs[r1] = Icr.wf;
	}
	else if (opcode == TRNG)
		if (addr >= 0)
		{
			BytesToWord(Mem[addr], Mem[addr+1], Icr.wf);
			iop1 = Icr.sintf;		// lower bound 
			BytesToWord(Mem[addr+2], Mem[addr+3], Icr.wf);
			iop2 = Icr.sintf;		// upper bound 
			Icr.wf = Regs[r1];
			ans = Icr.sintf ;		// test expression 
			if (ans < iop1 || ans > iop2)
			{
				cout << "Index expression out of bounds at " << (Pc-2)
					<< " (expr = " << ans << " lower = " << iop1
					<< " upper = " << iop2 << ")" << endl;
				if (Trace)
					TraceFile << "Index expression out of bounds at " 
						<< (Pc-2)
						<< " (expr = " << ans << " lower = " << iop1
						<< " upper = " << iop2 << ")" << endl;
				StopProgram = true;
			}
		}
		else
		{
			cout << "Address of TRNG is a register at " 
				<< (Pc-2) << endl;
			if (Trace)
				cout << "Address of TRNG is a register at " 
					<< (Pc-2) << endl;
			StopProgram = true;
		}
}


void Jop(int jmode, short amode, short reg, short& w2)
//
//  Perform jump operation.
//
{
	short addr;

	addr = Eac(amode, reg, w2);
	if (addr < 0)
	{
		cout << "Jump to a Register " << (Pc - 2) << endl;
		if (Trace)
			TraceFile << "Jump to a Register " << (Pc - 2) << endl;
		StopProgram = true;
	}
	else if (jmode >= 0 && jmode <= 7)
		switch (jmode)
		{
		case 0: // JMP
			Pc = addr;
			break;
		case 1: // JLT
			if (Lt) Pc = addr;
			break;
		case 2:	// JLE
			if (Lt || Eq) Pc = addr;
			break;
		case 3: // JEQ
			if (Eq) Pc = addr;
			break;
		case 4: // JNE
			if (!Eq) Pc = addr;
			break;
		case 5: // JGE
			if (Gt || Eq) Pc = addr;
			break;
		case 6: // JGT
			if (Gt) Pc = addr;
			break;
		case 7:; // no jump 
		}
	else
	{
		cout << "Invalid Jump Mode " << (Pc - 2) << endl;
		if (Trace)
			TraceFile << "Invalid Jump Mode " << (Pc - 2) << endl;
		StopProgram = true;
	}
}

void SRop (short reg, short smode, short amnt)
//
//  Perform right shift.
//
{
	Word w1, w2, mask1, mask2;

	switch (smode)
	{
    case 0:                       // SRZ -- fill with 0
    case 1:                       // SRO -- fill with 1
    case 2:                       // SRE -- bit extend
    case 3:                       // SRC -- circular
		w1 = Regs[reg];
		if ((smode == 1) || ((smode == 2) && (0x8000 & w1)))
			mask1 = 0xFFFF << (16 - amnt);
		else if (smode == 3)
			mask1 = w1 << (16 - amnt);
		else
			mask1 = 0x0000;
		if (amnt == 0)
			w1 = 0x0000;
		else
			w1 = (w1 >> amnt) | mask1;
		Regs[reg] = w1;
		break;

    case 4:                       // SRDZ--DoubleWord fill with 0 
	case 5:                       // SRDO--DoubleWord fill with 1 
    case 6:                       // SRDE--DoubleWord bit extend
    case 7:                       // SRDC--DoubleWord circular
		w1 = Regs[reg];
		w2 = Regs[(reg + 1) % REG_SIZE]; 
		if (amnt == 0)
			amnt = 16;
		if ((smode == 5) || ((smode == 6) && (0x8000 & w1)))
		{
			mask1 = w1 << (16 - amnt);
			mask2 = 0xFFFF << (16 - amnt);
		}
		else if (smode == 7)
		{
			mask1 = w1 << (16 - amnt);
			mask2 = w2 << (16 - amnt);
		}
		else
		{
			mask1 = w1 << (16 - amnt);
			mask2 = 0x0000;	
		}
		w1 = (w1 >> amnt) | mask2;
		w2 = (w2 >> amnt) | mask1;
        Regs[reg] = w1;
        Regs[(reg + 1) % REG_SIZE] = w2;
	}
}

void SLop (short reg, short smode, short amnt)
//
// Perform left shift.
//
{
	Word w1, w2, mask1, mask2;

	switch (smode)
	{
    case 0:                       // SLZ -- fill with 0
    case 1:                       // SLO -- fill with 1
    case 2:                       // SLE -- bit extend
    case 3:                       // SLC -- circular
		w1 = Regs[reg];
		if ((smode == 1) || ((smode == 2) && (0x8000 & w1)))
			mask1 = 0xFFFF >> (16 - amnt);
		else if (smode == 3)
			mask1 = w1 >> (16 - amnt);
		else
			mask1 = 0x0000;
		if (amnt == 0)
			w1 = 0x0000;
		else
			w1 = (w1 << amnt) | mask1;
		Regs[reg] = w1;
		break;

    case 4:                       // SLDZ--DoubleWord fill with 0 
	case 5:                       // SLDO--DoubleWord fill with 1 
    case 6:                       // SLDE--DoubleWord bit extend
    case 7:                       // SLDC--DoubleWord circular
		w1 = Regs[reg];
		w2 = Regs[(reg + 1) % REG_SIZE]; 
		if (amnt == 0)
			amnt = 16;
		if ((smode == 5) || ((smode == 6) && (0x8000 & w1)))
		{
			mask1 = w2 >> (16 - amnt);
			mask2 = 0xFFFF >> (16 - amnt);
		}
		else if (smode == 7)
		{
			mask1 = w2 >> (16 - amnt);
			mask2 = w1 >> (16 - amnt);
		}
		else
		{
			mask1 = w2 >> (16 - amnt);
			mask2 = 0x0000;	
		}
		w1 = (w1 << amnt) | mask1;
		w2 = (w2 << amnt) | mask2;
        Regs[reg] = w1;
        Regs[(reg + 1) % REG_SIZE] = w2;
	}
}

void RDop (short rmode, short amode, short reg, short& w2)
//
//  Perform a read operation.
//
{
	Word wd = 0;
	bool flag;
	int  addr, i, ival;
	char ch;

	if (rmode >= 0 && rmode <= 11)
	{
		addr = Eac(amode, reg, w2);
		switch (rmode)
		{
		case 0:                          // RDI--read integer 
			cin >> i;
			if ((i > MAX_INT) || (i < -MAX_INT))
			{
				cout << "Invalid Input: RDI " << (Pc - 2) << endl;
				if (Trace)
					TraceFile << "Invalid Input: RDI " << (Pc - 2) << endl;
				StopProgram = true;
			}
			else
			{
				Icr.sintf = i;
				if (addr >= 0)
					WordToBytes (Icr.wf, Mem[addr], Mem[addr+1]);
				else
					Regs[reg] = Icr.wf;
			}
			break;

		case 1:                          // RDF--read floating point 
			cin >> Rcr.rf;
			if (addr >= 0)
			{
				Mem[addr] = Rcr.b[3];
				Mem[addr + 1] = Rcr.b[2];
				Mem[addr + 2] = Rcr.b[1];
				Mem[addr + 3] = Rcr.b[0];
			}
			else
			{
				BytesToWord(Rcr.b[3], Rcr.b[2], Regs[reg]);
				BytesToWord(Rcr.b[1], Rcr.b[0], Regs[(reg + 1) % REG_SIZE]);
			}
			break;

		case 2:                          // RDBD--read binary digit 
			cin >> ch;
			if (ch == '0')
				if (addr >= 0)
					Mem[addr] = Mem[addr] ^ 0x80;
				else
					Regs[addr] = Regs[addr] ^ 0x8000;
			else if (ch == '1')
				if (addr >= 0)
					Mem[addr] = Mem[addr] | 0x80;
				else
					Regs[addr] = Regs[addr] | 0x8000;
			else
			{
				cout << "Invalid Input: RDBD " << (Pc - 2) << endl;
				if (Trace)
					TraceFile << "Invalid Input: RDBD " << (Pc - 2) 
						<< endl;
				StopProgram = true;
			}
			break;

		case 3:                           // RDBW--read binary Word
			i = 15;
			flag = true;
			while ((i >= 0) && flag)
			{
				cin >> ch;
				if (ch == '1')
					wd = wd | (0x0001 << i);
				else if (ch == '0')
					; // do nothing
				else
				{
					cout << "Invalid Input: RDBW " << ( Pc - 2) 
						<< endl;
					if (Trace)
						TraceFile << "Invalid Input: RDBW " 
							<< ( Pc - 2) << endl;
					flag = false;
					StopProgram = true;
				}
				i--;
			}
			if (flag)
				if (addr >= 0)
					WordToBytes(wd, Mem[addr], Mem[addr+1]);
				else
					Regs[reg] = wd;
			break;

		case 4:                           // RDOD--read octal digit 
		case 5:                           // RDOW--read octal Word          
			cout << "Operations RDOD and RDOW not presently"
				<< " implemented, at " << (Pc-2) << endl;
			if (Trace)
				TraceFile << "Operations RDOD and RDOW not presently"
					<< " implemented, at " << (Pc-2) << endl;
			StopProgram = true;
			break;

		case 6:                           // RDHD--read hex digit 
            cin >> ch;
			ch = toupper(ch);
			if ((ch >= 'A' && ch <= 'F') || isdigit(ch))
			{
                switch (ch)
				{
				case 'A': wd = 0xA000; break;
				case 'B': wd = 0xB000; break;
				case 'C': wd = 0xC000; break;
				case 'D': wd = 0xD000; break;
				case 'E': wd = 0xE000; break;
				case 'F': wd = 0xF000; break;
				case '0': wd = 0x0000; break;
				case '1': wd = 0x1000; break;
				case '2': wd = 0x2000; break;
				case '3': wd = 0x3000; break;
				case '4': wd = 0x4000; break;
				case '5': wd = 0x5000; break;
				case '6': wd = 0x6000; break;
				case '7': wd = 0x7000; break;
				case '8': wd = 0x8000; break;
				case '9': wd = 0x9000;
				}

                if (addr >= 0)
					Mem[addr] = (Mem[addr] & 0x0FFF) | wd;
                else
                    Regs[reg] = (Regs[reg] & 0x0FFF) | wd;
			}
			else
			{
				cout << "Invalid Input: RDHD " << (Pc - 2) << endl;
				if (Trace)
					TraceFile << "Invalid Input: RDHD " << (Pc - 2) 
						<< endl;
				StopProgram = true;
			}
			break;

		case 7:                          // RDHW--read hex Word 
			i = 15;
			flag = true;
			wd = 0x0000;
            while ((i >= 1) && flag)
			{
				cin >> ch;
				ch = toupper(ch);
				if (ch >= 'A' && ch <= 'F')
					ival = short(ch) - short('A') + 10;
				else if (isdigit(ch))
					ival = short(ch) - short('0');
				else
				{
					cout << "Invalid Input: RDHW " << (Pc - 2) << endl;
					if (Trace)
						TraceFile << "Invalid Input: RDHW " << (Pc - 2) 
							<< endl;
					flag = false;
					StopProgram = true;
				}
				if (flag)
				{
					if (ival > 7)
					{
						wd = wd | (0x0001 << i);
						ival = ival - 8;
					}
					i--;
					if (ival > 3)
					{
						wd = wd | (0x0001 << i);
						ival = ival - 4;
					}
					i--;
					if (ival > 1)
					{
						wd = wd | (0x0001 << i);
						ival = ival - 2;
					}
					i--;
					if (ival > 0)
						wd = wd | (0x0001 << i);
					i--;
				}
			}
			if (addr >= 0)
				WordToBytes(wd, Mem[addr], Mem[addr+1]);
			else
				Regs[reg] = wd;
			break;

		case 8:                           // RDCH--read ASCII character 
			cin.get(ch);
			if (addr >= 0)
				Mem[addr] = Byte(ch);
			else
				Regs[reg] = (Regs[reg] & 0xFF00) | (Word(ch) & 0x00FF);
			break;

		case 9:                           // RDST--read ASCII string 
			if (addr < 0)
			{
				cout << "Invalid Address: RDST " << (Pc - 2) << endl;
				if (Trace)
					TraceFile << "Invalid Address: RDST " << (Pc - 2) 
						<< endl;
				StopProgram = true;
			}
			else
			{
				flag = true;
				while (cin.peek() != '\n')
					if (flag)
					{
						flag = false;
						cin.get(ch);
						Mem[addr] = Byte(ch);
						addr = (addr + 1) % MEM_SIZE;
					}
					else
					{
						flag = true;
						cin.get(ch);
						Mem[addr] = Byte(ch);
						addr = (addr + 1) % MEM_SIZE;
					}
				Mem[addr] = 0;
				if (flag)
				    addr = (addr + 2) % MEM_SIZE;
				else
				    addr = (addr + 1) % MEM_SIZE;
			}
			cin.ignore(256,'\n');
			break;

		case 10:                          // RDIN--read instruction 
			cout << "Operation RDIN not presently implemented"
				<< " at " << (Pc-2) << endl;
			if (Trace)
				TraceFile << "Operation RDIN not presently implemented"
					<< " at " << (Pc-2) << endl;
			StopProgram = true;
			break;

		case 11:                          // RDNL--read newline 
			cin.ignore(256, '\n');
		}
	}
	else
    {
		cout << "Invalid Read Mode Detected at " << (Pc - 2) << endl;
		if (Trace)
			TraceFile << "Invalid Read Mode Detected at " << (Pc - 2) 
				<< endl;
		StopProgram = true;
    }
}

void WRop (short wmode, short amode, short reg, short& w2)
//
//  Perform a write operation.
//
{
	short  addr;
	Word wd;
	int bval, ival;
	char ch;

	if (wmode >= 0 && wmode <= 11)
	{
		addr = Eac(amode, reg, w2);
		switch (wmode)
		{
		case 0:                           // WRI--write integer 
            if (addr >= 0)
				BytesToWord(Mem[addr], Mem[addr+1], Icr.wf);
            else
				  Icr.wf = Regs[reg];
            cout << ' ' << Icr.sintf;
            break;

		case 1:                           // WRF--write float 
			if (addr >= 0)
			{
				Rcr.b[3] = Mem[addr     ];
				Rcr.b[2] = Mem[addr + 1 ];
				Rcr.b[1] = Mem[addr + 2 ];
				Rcr.b[0] = Mem[addr + 3 ];
			}
			else
			{
				WordToBytes(Regs[reg], Rcr.b[3], Rcr.b[2]);
				WordToBytes(Regs[(reg + 1) % REG_SIZE], Rcr.b[1],
								Rcr.b[0]);
			}
			cout << Rcr.rf;
			break;

		case 2:                           // WRBD--write binary digit 
			if (addr >= 0)
				BytesToWord(Mem[addr], Mem[addr+1], wd);
			else
				wd = Regs[reg];
			if (0x8000 & wd)
				cout << '1';
			else
				cout << '0';
			break;

		case 3:                           // WRBW--write binary Word 
			if (addr >= 0)
				BytesToWord(Mem[addr], Mem[addr+1], wd);
			else
				wd = Regs[reg];
			for (int i = 15; i >= 0; i--)
				if ((0x0001 << i) & wd)
					cout << '1';
				else
					cout << '0';
			break;

		case 4:                           // WROD--write octal digit 
		case 5:                           // WROW--write octal Word  
			cout << "Operations WROD and WROW not presently"
				<< " implemented, at " << (Pc-2) << endl;
			if (Trace)
				TraceFile << "Operations WROD and WROW not presently"
					<< " implemented, at " << (Pc-2) << endl;
			StopProgram = true;
			break;

		case 6:                           // WRHD--write hex digit 
			if (addr >= 0)
				BytesToWord(Mem[addr], Mem[addr+1], wd);
			else
				wd = Regs[reg];
			bval = 8;
			ival = 0;
			for (short i = 15;  i >= 12; i--)
			{
				if ((0x0001 << i) & wd)
					ival = ival + bval;
				bval = bval / 2;
			}
			cout << uppercase << hex << ival;
			break;

		case 7:                           // WRHW--write hex Word 
			if (addr >= 0)
				BytesToWord(Mem[addr], Mem[addr+1], wd);
			else
				wd = Regs[reg];
			bval = MAX_INT + 1;
			ival = 0;
			for (short i = 15; i >= 0; i--)
			{
				if ((0x0001 << i) & wd)
					ival = ival + bval;
				bval = bval / 2;
			}
			if (ival < 4096)
				cout << '0';
			if (ival < 256)
				cout << '0';
			if (ival < 16)
				cout << '0';
			cout << uppercase << hex << ival;
			break;

		case 8:                           // WRCH--write character
			if (amode == 3) // immediate mode
				cout << char(w2 & 0x00FF);
			else if (addr >= 0)
				cout << char(Mem[addr]);
			else // register direct mode
				cout << char(Regs[reg] & 0x00FF);
			break;

		case 9:                           // WRST--write string 
			ch = ' ';
			do
			{
				if (Mem[addr] != 0)
				{
					cout << char(Mem[addr]);
					if (Mem[addr+1] != 0)
						cout << char(Mem[addr+1]);
					else
						ch = '\0';
				}
				else
					ch = '\0';
				addr = (addr + 2) % MEM_SIZE;
			} while (ch != '\0');
			break;

		case 11:                          // WRNL--write newline
			cout << endl;
		}
	}
	else
    {
		cout << "Invalid Write Mode Dectected at " << (Pc - 1) << endl;
		if (Trace)
			TraceFile << "Invalid Write Mode Dectected at " << (Pc - 1) 
				<< endl;
		StopProgram = true;
    }
}

void Execute()
{
    short opcode, f1, f2, f3, w2;

	// opcode is the leftmost 5 bits of Ir (bits 11 through 15)
	opcode = (Ir & 0xF800) >> 11;

	// Ir bits 7 through 10 contain a register number.
	f1 = (Ir & 0x0780) >> 7;

	// Ir bits 4 through 6 contain an addressing mode.
	f2 = (Ir & 0x0070) >> 4;

	// Ir bits 0 through 3 contain a register number.
	f3 = (Ir & 0x000F);

	w2 = 0;

	switch (opcode)
	{
    case IN:   case IA:   case IS:   case IM:   case ID:
	case FN:   case FA:   case FS:   case FM:   case FD:
	case BI:   case BO:   case BA:   case IC:   case FC:
	case JSR:  case BKT:  case LD:   case STO:  case LDA:
	case FIX:  case FLT:  case TRNG:
		TwoAddr(opcode, f1, f2, f3, w2);
		break;
    case J:
		Jop(f1, f2, f3, w2);
		break;
    case SR:
		SRop(f1, f2, f3);
		break;
    case SL:
		SLop(f1, f2, f3);
		break;
    case RD:
		RDop(f1, f2, f3, w2);
		break;
    case WR:
		WRop(f1, f2, f3, w2);
		break;
    case HALT:
		StopProgram = true;
		break;
    case 28:
	case 29:
	case 30:
        cout << "Bad opcode" << (Pc - 2) << endl;
		if (Trace)
			TraceFile << "Bad opcode" << (Pc - 2) << endl;
        StopProgram = true;
	}
	if (Trace)
	{
		TraceFile << "\n  ";
		for (int i = 1; i <= 68; i++) TraceFile << '-';
		TraceFile << endl;
		WriteInst (oldPc, opcode, f1, f2, f3, w2);
		TraceFile << "  ";
		for (int i = 1; i <= 68; i++) TraceFile << '-';
		TraceFile << endl;
		DisplayRegs();
	}
}

void Rdmem(char* s)
{
	ifstream Memf;
	short i;

	cout << "MACC 2016 Virtual Machine Emulator\n" << endl; 
	cout << "Object file name (.obj extension assumed): ";
//	getline(cin, Source);
	Source = s;
	cout << Source << '\n';
	cout << endl;
	Memf.open((Source+".obj").data(), ios_base::binary);
	if (!Memf.is_open())
	{
		cout << "\nFile \"" << Source + ".obj" << "\" not found!" 
			 << "\nExecution aborted.\n" << endl;
		cin.get();
		exit(1);
	}
	Memf.peek();
	if (Memf.eof())
		MemImage = false;
	else
	{
		i = 0;
		if (Trace)
		{
			TraceFile << "MACC 2016 Virtual Machine Trace\n" << endl;
			TraceFile << "Program File: " << Source + ".obj\n" << endl;
			TraceFile << endl << hex << uppercase << setw(6) << "MEM |";
			for (int k = 0; k < 16; k++) TraceFile << setw(4) << k;
			TraceFile << endl;
			TraceFile << "-----+";
			for (int k = 0; k < 16; k++) TraceFile << "----";
		}
		while (!Memf.eof())
		{
			Memf.get(Bcr.hintf);
			Mem[i] = Bcr.bytef;
			if (Trace)
			{
				if (i % 16 == 0)
					TraceFile << endl << setw(4) << i << " |"; 
				if (Mem[i] >= 16)
					TraceFile << setw(4) << short(Mem[i]);
				else
					TraceFile << setw(3) << "0" << short(Mem[i]);
			}
			i++;
		}
		if (Trace) TraceFile << dec << endl;
	}
	Memf.close();
}

int main(int argc, char* argv[]) // MACC2 Machine
{
	MemImage = true;
	Trace = true;
	Lt = false;
	Eq = false;
	Gt = false;
	Pc = 0;
	StopProgram = false;

	if (Trace)
		TraceFile.open("trace.txt");

	if (argc > 1) {
		Rdmem(argv[1]);
	}

	if (MemImage)
	{
		// clear all registers
		for (short RegIndex = 0; RegIndex <= 15; RegIndex++)
			Regs[RegIndex] = 0x0000;
		do
		{
			oldPc = Pc;
			FetchWord(Ir);         // fetch the next instruction      
			Execute();             // execute the current instruction 
		} while (!StopProgram);
	}

	if (Trace)
		TraceFile.close();

	cout << endl;
	cout << "*** End of MACC run. ***" << endl;
//	cin.ignore(256, '\n');
	return 0;
}
