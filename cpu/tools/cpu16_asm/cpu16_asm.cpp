/*
 * Main control module for the CPU16 assembler
 */

#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>
#include <list>
#include "parser.h"
#include "code.h"
#include "symbolTable.h"

#define DEFAULT_LIB_PATH "/usr/local/lib"
#define STDLIB_NAME "hack_stdlib.ho"

// global input file name and line number for error reporting
char *inputFileName;
unsigned line;

// command line options
std::string stdlibPath(DEFAULT_LIB_PATH);
std::list<std::string> inputFileNames;
std::string outputFileName;
bool nostdlib = false;
bool verbose = false;

static void ParseOptions(int argc, char* argv[])
{
	const char* optStr = "L:o:nvh";
	int opt;

	while ((opt = getopt(argc, argv, optStr)) != -1)
	{
		switch (opt)
		{
		    // cpp options, just pass thru
			case 'L':
				stdlibPath = optarg;
				break;
			case 'o':
				outputFileName = optarg;
				break;
			case 'n':
				nostdlib = true;
				break;
			case 'v':
				verbose = true;
				break;
			case 'h':
				printf("usage: hasm [-L <stdlib path>] [-o <filename>] [-n] [-v] [-h] <hasm files>\n");
				printf("\n");
				printf("     options:\n");
				printf("         -L <filename>: set the standard library path\n");
				printf("         -o <filename>: set the output file name\n");
				printf("         -n:            no standard library\n");
				printf("         -v:            set verbose mode\n");
				printf("         -h:            display this help\n");
				exit(0);
			default:
				printf("usage: hasm [-L <stdlib path>] [-o <filename>] [-n] [-v] [-h] <hasm files>\n");
				exit(-1);
		}
	}
	
	// input files follow the first non-option cmd line arg
	while (optind < argc)
	{
	    inputFileNames.push_back(argv[optind++]);
	}
}

// return true if a string represents a number, else return false
bool IsNumber(const std::string& s)
{
	// check for an actual zero numeral
	if (s == "0")
		return true;

	// if the string can be converted return true
	char* p;
	strtol(s.c_str(), &p, 10);
	return *p == 0;
}

int main(int argc, char** argv)
{
	if (argc < 2)
		return 1;

    ParseOptions(argc, argv);
    
    // open and concatenate the input files
    std::stringstream srcBuffer;
	for (std::list<std::string>::iterator it=inputFileNames.begin() ; it != inputFileNames.end(); ++it)
	{
	    std::ifstream inputFile(*it);
	    if (!inputFile.is_open())
	    {
		    std::cout << "hasm: cannot open file " << *it << std::endl;
		    exit(1);
	    }
        srcBuffer << inputFile.rdbuf();
    }
    
    // append the standard library file to the input files
    if (!nostdlib)
    {
        std::string stdlibName(STDLIB_NAME);
        stdlibName = stdlibPath + "/" + stdlibName;
        std::ifstream stdlibFile(stdlibName);
        if (!stdlibFile.is_open())
        {
	        std::cout << "hasm: cannot open file " << stdlibName << std::endl;
		    exit(1);
        }
        srcBuffer << stdlibFile.rdbuf();
    }
    
	// create the output file
	if (outputFileName.empty())
	{
        size_t pos = 0;
	    outputFileName = inputFileNames.front();
        if ((pos = outputFileName.find_last_of(".")) != std::string::npos)
        {
            outputFileName = outputFileName.substr(0, pos) + ".hack";
        }
        else
        {
            outputFileName += ".hack";
        }
	}
	std::ofstream outputFile(outputFileName);

	// init the parser and code generator
	Parser parser(srcBuffer);
	Code code;
	SymbolTable symbolTable;

	// first pass -- create the symbol table
	std::string symbol;
	int romAddr = 0;
	int ramAddr = 16;
	line = 0;
	while (parser.HasMoreCommands() && parser.Advance())
	{
		switch (parser.CommandType())
		{
			case A_COMMAND:
				romAddr++;
				break;

			case C_COMMAND:
				romAddr++;
				break;

			case L_COMMAND:
				symbol = parser.Symbol();
				if (!symbolTable.Contains(symbol))
				{
					symbolTable.AddEntry(symbol, romAddr);
				}
				break;

			case E_COMMAND:
				symbol = parser.Symbol();
				if (!symbolTable.Contains(symbol))
				{
					symbolTable.AddEntry(symbol, parser.EquValue());
				}
				else
				{
				    std::cout << "Duplicate equ" << symbol << std::endl;
				}
				break;

			default:
				std::cout << "Unimplemented instruction (pass 1)" << std::endl;
				break;
		}
	}

	// pass 2 -- convert assembly file to hack file line by line
	//inputFile.clear();
	//inputFile.seekg(0, inputFile.beg);
	srcBuffer.clear();
	srcBuffer.seekg(0, srcBuffer.beg);
	char symbolBuffer[80];
	line = 0;
	while (parser.HasMoreCommands() && parser.Advance())
	{
		switch (parser.CommandType())
		{
			case A_COMMAND:
				symbol = parser.Symbol();
				if (!IsNumber(symbol))
				{
					// determine whether the symbol has been defined yet
					int address;
					if (symbolTable.Contains(symbol))
					{
						// get the defined symbol's corresponding value
						address = symbolTable.GetAddress(symbol);
					}
					else
					{
						// this is an undefined variable so allocate the next RAM address and add to symbol table
						address = ramAddr++;
						symbolTable.AddEntry(symbol, address);
					}
					sprintf(symbolBuffer, "%d", address);
					symbol = symbolBuffer;
				}
				outputFile << "0" << code.Symbol(symbol) << std::endl;
				break;

			case C_COMMAND:
				outputFile << "111" << code.Comp(parser.Comp()) << code.Dest(parser.Dest()) << code.Jump(parser.Jump()) << std::endl;
				break;

            // labels and equates are only used in the first pass so ignore them in the second pass
			case L_COMMAND:
			case E_COMMAND:
				break;

			default:
				std::cout << "Unimplemented instruction (pass 2)" << std::endl;
				break;
		}
	}

    return 0;
}


// end of hasm.cpp

    
