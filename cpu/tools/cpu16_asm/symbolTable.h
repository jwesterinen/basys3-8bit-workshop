/*
 * SymbolTable class definition
 */

#pragma once

#include <string>
#include "map"

class SymbolTable
{
public:
	SymbolTable();
	~SymbolTable();
	void AddEntry(const std::string& symbol, int address);
	bool Contains(const std::string& symbol);
	int GetAddress(const std::string& symbol);
	void AddEqu(const std::string& symbol, unsigned value);
	unsigned GetEqu(const std::string& symbol);

private:
	std::map<const std::string, int> symbolTable;
};


// end of symbolTable.h

