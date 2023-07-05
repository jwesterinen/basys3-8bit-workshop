/*
 * SymbolTable class implementation
 */

#include "symbolTable.h"

// table of pre-defined symbols for the HACK CPU
struct predefinedSymbol
{
	const std::string symbol;
	int address;
} predefinedSymbolTable[] = {
	{ "SP",         0},
	{ "BP",         1},
	{ "EP",         2},
	{ "RETVAL",     3},
	{ "GLB",        4},
	{ "R0",         0},
	{ "R1",         1},
	{ "R2",         2},
	{ "R3",         3},
	{ "R4",         4},
	{ "R5",         5},
	{ "R6",         6},
	{ "R7",         7},
	{ "R8",         8},
	{ "R9",         9},
	{ "R10",       10},
	{ "R11",       11},
	{ "R12",       12},
	{ "R13",       13},
	{ "R14",       14},
	{ "R15",       15},
	{ "SCREEN", 16384},
	{ "KBD",    24576},
	{ "LED",    28672},
	{ "IODAT",  28673},
	{ "IODIR",  28674}
};
int predefinedSymbolTableSize = sizeof(predefinedSymbolTable) / sizeof(predefinedSymbol);

SymbolTable::SymbolTable()
{
	// load all of the predefined symbol to the table
	for (int i = 0; i < predefinedSymbolTableSize; i++)
	{
		AddEntry(predefinedSymbolTable[i].symbol, predefinedSymbolTable[i].address);
	}
}

SymbolTable::~SymbolTable()
{
}

// add a symbol to the table
void SymbolTable::AddEntry(const std::string& symbol, int address)
{
	symbolTable[symbol] = address;
}

// attempt to find a symbol and return true if found, else return false
bool SymbolTable::Contains(const std::string& symbol)
{
	return (symbolTable.find(symbol) != symbolTable.end());
}

// return the address associated with a defined symbol
int SymbolTable::GetAddress(const std::string& symbol)
{
	return symbolTable[symbol];
}


// end of symbolTable.cpp

