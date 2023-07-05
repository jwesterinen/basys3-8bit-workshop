/*
 * Parser class definition
 */

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

enum CmdType{A_COMMAND, C_COMMAND, L_COMMAND, E_COMMAND};

class Parser
{
public:
	//Parser(std::ifstream& _inputFile);
    Parser(std::stringstream& _srcBuffer);
	~Parser();
	bool HasMoreCommands();
	bool Advance();
	CmdType CommandType();
	const std::string& Symbol()
	{
		return symbol;
	}
	const std::string& Dest()
	{
		return dest;
	}
	const std::string& Comp()
	{
		return comp;
	}
	const std::string& Jump()
	{
		return jump;
	}
	int EquValue()
	{
		return equValue;
	}

private:
	//std::ifstream& inputFile;
	std::stringstream& srcBuffer;
	std::string curInstr;
	std::string symbol, dest, comp, jump;
	int equValue;
};


// end of parser.h

