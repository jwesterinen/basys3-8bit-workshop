/*
 * Parser class implementation
 */

#include <cctype> 
#include "parser.h"

extern unsigned line;

//Parser::Parser(std::ifstream& _srcBuffer) : srcBuffer(_srcBuffer)
Parser::Parser(std::stringstream& _srcBuffer) : srcBuffer(_srcBuffer)
{
}

Parser::~Parser()
{
}

// returns whether there are more instructions in the input
bool Parser::HasMoreCommands()
{
	std::getline(srcBuffer, curInstr);
	line++;
	return !srcBuffer.eof();
}

bool Parser::Advance()
{
	while (curInstr == "" || curInstr[0] == '/' || curInstr.find_first_not_of(" \t") == std::string::npos)
	{
		std::getline(srcBuffer, curInstr);
		line++;
		if (srcBuffer.eof())
		    return false;
	}
	return true;
}

CmdType Parser::CommandType()
{
	size_t pos;

	// parse each instruction:
	// <hasm program>   ::= <instruction>+
	// <instruction>    ::= <A-instruction> | <D-instruction> | <L-instruction> | <comment> | <equate>
	// <A-instruction>  ::= @<symbol>
	// <D-instruction>  ::= [<dest>=]<comp>[;<jump>]
	// <L-instruction>  ::= (<symbol>)
	// <comment>        ::= //<any text>
	// <equate>         ::= <symbol> equ <value>

    // check for equ
	if ((pos = curInstr.find("equ")) != std::string::npos)
	{
        char symbuf[80];
        
	    // equate so parse into symbol and value
        sscanf(curInstr.c_str(), "%s\tequ\t%d", symbuf, &equValue);
        symbol = symbuf;
        
		curInstr.clear();
		return E_COMMAND;
	}
	
	// strip off whitespace and in-line comments
	if ((pos = curInstr.find_first_not_of(" \t")) != std::string::npos)
	{
		curInstr = curInstr.substr(pos);
	}
	if ((pos = curInstr.find_first_of(" \t")) != std::string::npos)
	{
		curInstr = curInstr.substr(0, pos);
	}

    // return the command type
	symbol.clear();
	if ((pos = curInstr.find("@")) != std::string::npos)
	{
		// A-instruction so parse the symbol
		symbol = curInstr.substr(pos+1);

		curInstr.clear();
		return A_COMMAND;
	}
	else if ((pos = curInstr.find("(")) != std::string::npos)
	{
		// L-instruction so parse the symbol
		curInstr.erase(0, pos + 1);
		if ((pos = curInstr.find(')')) != std::string::npos)
		{
			symbol = curInstr.substr(0, pos);
		}

		curInstr.clear();
		return L_COMMAND;
	}
	else
	{
		// D-instruction so parse as: [<dest>=]<comp>[;<jump>]

		// parse the optional dest
		dest = "";
		if ((pos = curInstr.find('=')) != std::string::npos)
		{
			dest = curInstr.substr(0, pos);
			curInstr.erase(0, pos+1);
		}

		// parse the comp
		jump = "";
		if ((pos = curInstr.find(';')) != std::string::npos)
		{
			comp = curInstr.substr(0, pos);

			// parse the optional jump
			jump = curInstr.substr(pos+1);
		}
		else
		{
			comp = curInstr;
		}

		curInstr.clear();
		return C_COMMAND;
	}
}


// end of parser.cpp

