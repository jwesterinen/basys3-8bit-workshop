/*
 * code.cpp
 */

#include <iostream>
#include "code.h"

extern char *inputFileName;
extern unsigned line;

struct compConversion
{
	const std::string hasm;
	const std::string hack;
} compTable[] = {
	{ "0",   "0101010" },
	{ "1",   "0111111" },
	{ "-1",  "0111010" },
	{ "D",   "0001100" },
	{ "A",   "0110000" },
	{ "M",   "1110000" },
	{ "!D",  "0001101" },
	{ "!A",  "0110001" },
	{ "!M",  "1110001" },
	{ "-D",  "0001111" },
	{ "-A",  "0110011" },
	{ "-M",  "1110011" },
	{ "D+1", "0011111" },
	{ "A+1", "0110111" },
	{ "M+1", "1110111" },
	{ "D-1", "0001110" },
	{ "A-1", "0110010" },
	{ "M-1", "1110010" },
	{ "D+A", "0000010" },
	{ "A+D", "0000010" },
	{ "D+M", "1000010" },
	{ "M+D", "1000010" },
	{ "D-A", "0010011" },
	{ "D-M", "1010011" },
	{ "A-D", "0000111" },
	{ "M-D", "1000111" },
	{ "D&A", "0000000" },
	{ "A&D", "0000000" },
	{ "D&M", "1000000" },
	{ "M&D", "1000000" },
	{ "D|A", "0010101" },
	{ "A|D", "0010101" },
	{ "D|M", "1010101" },
	{ "M|D", "1010101" }
};
int compTableSize = sizeof(compTable) / sizeof(compConversion);

struct destConversion
{
	const std::string hasm;
	const std::string hack;
} destTable[] = {
	{ "",    "000" },
	{ "M",   "001" },
	{ "D",   "010" },
	{ "MD",  "011" },
	{ "DM",  "011" },
	{ "A",   "100" },
	{ "AM",  "101" },
	{ "MA",  "101" },
	{ "AD",  "110" },
	{ "DA",  "110" },
	{ "AMD", "111" },
	{ "ADM", "111" },
	{ "MAD", "111" },
	{ "MDA", "111" },
	{ "DAM", "111" },
	{ "DMA", "111" }
};
int destTableSize = sizeof(destTable) / sizeof(destConversion);

struct jumpConversion
{
	const std::string hasm;
	const std::string hack;
} jumpTable[] = {
	{ "",    "000" },
	{ "JGT", "001" },
	{ "JEQ", "010" },
	{ "JGE", "011" },
	{ "JLT", "100" },
	{ "JNE", "101" },
	{ "JLE", "110" },
	{ "JMP", "111" }
};
int jumpTableSize = sizeof(jumpTable) / sizeof(jumpConversion);

Code::Code() : errMsg("")
{
}

Code::~Code()
{
}

// return a string depicting the binary format of a symbol
const std::string& Code::Symbol(const std::string& symbol)
{
	unsigned short value = atoi(symbol.c_str());
	symbolBin.clear();

	symbolBin += (value & 0x4000) ? "1" : "0";
	symbolBin += (value & 0x2000) ? "1" : "0";
	symbolBin += (value & 0x1000) ? "1" : "0";
	symbolBin += (value & 0x0800) ? "1" : "0";
	symbolBin += (value & 0x0400) ? "1" : "0";
	symbolBin += (value & 0x0200) ? "1" : "0";
	symbolBin += (value & 0x0100) ? "1" : "0";
	symbolBin += (value & 0x0080) ? "1" : "0";
	symbolBin += (value & 0x0040) ? "1" : "0";
	symbolBin += (value & 0x0020) ? "1" : "0";
	symbolBin += (value & 0x0010) ? "1" : "0";
	symbolBin += (value & 0x0008) ? "1" : "0";
	symbolBin += (value & 0x0004) ? "1" : "0";
	symbolBin += (value & 0x0002) ? "1" : "0";
	symbolBin += (value & 0x0001) ? "1" : "0";

	return symbolBin;
}

// return the binary field that corresponds to the symbolic comp portion of a D-instruction
const std::string& Code::Comp(const std::string& comp)
{
	for (int i = 0; i < compTableSize; i++)
	{
		if (comp == compTable[i].hasm)
			return compTable[i].hack;
	}

    std::cout << inputFileName << ": unknown comp in line " << line << ": " << comp << std::endl;
    errMsg = "*******";
	return errMsg;
}

// return the binary field that corresponds to the symbolic dest portion of a D-instruction
const std::string& Code::Dest(const std::string& dest)
{
	for (int i = 0; i < destTableSize; i++)
	{
		if (dest == destTable[i].hasm)
			return destTable[i].hack;
	}

    std::cout << inputFileName << ": unknown dest in line " << line << ": " << dest << std::endl;
	errMsg = "***";
	return errMsg;
}

// return the binary field that corresponds to the symbolic jump portion of a D-instruction
const std::string& Code::Jump(const std::string& jump)
{
	for (int i = 0; i < compTableSize; i++)
	{
		if (jump == jumpTable[i].hasm)
			return jumpTable[i].hack;
	}

    std::cout << inputFileName << ": unknown jump in line " << line << ": " << jump << std::endl;
	errMsg = "***";
	return errMsg;
}


// end of code.cpp

