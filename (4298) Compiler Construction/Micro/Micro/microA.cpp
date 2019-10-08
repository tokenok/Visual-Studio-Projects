/*	____________________________________________________________________________

                            M i c r o   C o m p i l e r

							          micro.cpp

                                    Version 2016
									
								  James L. Richards
						    Last Update: August 28,  2007
									
						  -- Generates SAM Assembly Code --
 

	Micro is a simple language defined by Charles N. Fischer and Richard J. 
	LeBlanc, Jr. in their textbook entitled "Crafting A Compiler," Chapter 2, 
	pp. 25-50 (1991).
	____________________________________________________________________________
 */

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
using namespace std;

                     // GLOBAL FILE STREAMS
ifstream sourceFile; // source program
ofstream outFile,    // object program
         listFile;   // compiler listing

#include "mascan.h"   // scanner component definition 
#include "maparse.h"	 // parser component definition
#include "macode.h"   // code generator component definition

                     // GLOBAL COMPONENT OBJECTS
Scanner scan;        // scanner
Parser parse;        // parser
CodeGen code;        // code generator

string Date();
// Returns the current date.

string Time();
// Returns the current time.

int main()
{
	string sourceName, outName, listName;

	cout 
		<< "\n" 
		<< " M I C R O   C O M P I L E R   2 0 0 7\n"
		<< " _____________________________________\n" 
		<< endl;

	cout << " Source file (.mca extension is assumed): ";
//	getline(cin, sourceName);
	sourceName = "test";

   // Add appropriate extensions to file names.
	outName = sourceName + ".asm";
	listName = sourceName + ".lst";
	sourceName += ".mca";

   // Open and initialize all files.
	sourceFile.open(sourceName.data());
	if (!sourceFile.is_open())
	{
		cout << "\n File not found. Compilation aborted!\n\n";
		cin.get();
		exit(1);
	}
	outFile.open(outName.data());
	listFile.open(listName.data());

	listFile
		<< "\n\n M I C R O   C O M P I L E R   L I S T I N G\n\n"
		<< "             James L. Richards\n"
		<< "               Version 2016\n\n"
		<< " Generated code is SAM assembly language for\n"
		<< " the MACC2 virtual computer.\n"
		<< " ___________________________________________\n\n";
	listFile << ' ' + Date() << " at " << Time() << endl;
	listFile << " Source file: " << sourceName << endl << endl;
	listFile << " LINE #" << endl;

	parse.SystemGoal();

	cout << endl
		<< "\n Successful Compilation\n"
		<< "\n Object code --> " << outName << endl
		<< "\n Listing file --> " << listName << endl << endl; 

//	cin.get();
	return 0;
}

string Date()
{
	const time_t current_time = time (NULL);
	char the_date[20];

	strftime (the_date, 20, "%B %d, %Y", localtime (&current_time));

	return the_date;
}

string Time()
{
	const time_t current_time = time (NULL);
	char the_time[10];


	strftime (the_time, 10, "%I:%M %p", localtime (&current_time));
	
	return the_time;
}

