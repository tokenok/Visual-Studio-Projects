/*	____________________________________________________________________________

	                Parser Component for the Micro Compiler

	                                mparse.h

	                              Version 2007
 
	                           James L. Richards
	                     Last Update: August 28, 2007

	The routines in this unit are based on those provided in the book 
	"Crafting A Compiler" by Charles N. Fischer and Richard J. LeBlanc, Jr., 
	Benjamin Cummings Publishing Co. (1991).

	See Section 2.3-2.4, pp. 31-40.
	____________________________________________________________________________

						 M I C R O A   G R A M M A R

	<program>        -> #Start begin <statement list> end

	<statement list> -> <statement> { <statement> }

	<statement>      -> <ident> := <expression> #Assign ;
					 | read ( <id list> ) ;
					 | write ( <expr list> ) #NewLine ;

	<id list>        -> <ident> #ReadId { , <ident> #ReadId }

	<expr list>      -> <expression> #WriteExpr
						 { , <expression> #WriteExpr }

	<expression>     -> <primary> { <add op> <primary> #GenInfix }

	<primary>        -> ( <expression> )
					 | <ident>
					 | INT_LITERAL #ProcessLiteral
					 | STR_LITERAL #WriteString

	<add op>         -> PLUS_OP #ProcessOp
					 | MINUS_OP #ProcessOp

	<ident>          -> ID #ProcessId

	<system goal>    -> <program> EOF_SYM #Finish
	____________________________________________________________________________
*/

#ifndef PARSER
#define PARSER

#include "macode.h"

class Parser
{
public:

	Parser();
	// Initializes the parser object.

	void SystemGoal();
	// Checks <system goal> syntax and semantics.

private:

	Token savedToken;
	bool tokenAvailable;

/* _____________________________________________________________________________
*/
	void SyntaxError(Token t);
	// Processes a syntax error and aborts compilation.

	Token NextToken();
	// Gets the next token and saves it.

	void Match(Token t);
	// Returns successfully if the next token matches t; otherwise, a syntax 
	// error occurs and compilation is aborted.

/* _____________________________________________________________________________
*/
	void ProgramProc();
	// Checks <program> syntax and semantics.

	void StatementList();
	// Checks <statement list> syntax and semantics.

	void Statement();
	// Checks <statement> syntax and semantics.

	void IdList();
	// Checks <id list> syntax and semantics.

	void ExprList();
	// Checks <expr list> syntax and semantics.

	void Expression(ExprRec& result);
	// Checks <expression> syntax and semantics.

	void Primary(ExprRec& result);
	// Checks <primary> syntax and semantics.

	void AddOp(OpRec& op);
	// Attempts to match an operator and process it.

	void Ident(ExprRec& result);
	// Attempts to match an identifier and process it.
};

#endif