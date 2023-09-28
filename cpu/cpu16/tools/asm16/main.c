/*
 * main function for parser
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <getopt.h>
#include "y.tab.h"

extern FILE* yyout;
extern FILE *yyerfp;

// options
static char *outfileName = 0;
static char *infileName = 0;
int objOnly = 0;
int emitVmCode = 0;
int verbose = 0;

// assembler pass
int asm_pass;
// TODO make this a command line option
unsigned short cur_addr;

static void ParseOptions(int argc, char* argv[])
{
	const char* optStr = "CD:I:PU:So:ivh";
	int opt;

	while ((opt = getopt(argc, argv, optStr)) != -1)
	{
		switch (opt)
		{
		    // cpp options, just pass thru
		    case 'C':
		    case 'D':
		    case 'I':
		    case 'P':
		    case 'U':
		        break;
		    
			case 'o':
				outfileName = optarg;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
				printf("usage: asm16 [-o <filename>] [-v] [-h]\n");
				printf("\n");
				printf("     options:\n");
				printf("         -S:            compile to hasm object\n");
				printf("         -o <filename>: set the output file name\n");
				printf("         -v:            set verbose mode\n");
				printf("         -h:            display this help\n");
				exit(0);
			default:
				printf("usage: asm16 [-o <filename>] [-v] [-h]\n\n");
				exit(-1);
		}
	}
	
	// the input file name is the first non-option cmd line arg
	if (optind < argc)
	{
	    infileName = argv[optind];
	}
}

/*
 * cpp() -- preprocess lex input() through C preprocessor
 */

#ifndef CPP
#   define CPP "/usr/bin/cpp"
#endif

int cpp(int argc, char** argv)
{
    char **argp, *cmd;
    extern FILE* yyin;      // for lex input
    int i;
    
    for (i = 0, argp = argv; *++argp; )
    {
        if (**argp == '-' && strchr("CDEIUP", (*argp)[i]))
        {
            i += strlen(*argp) + 1;
        }
    }
    
    if (!(cmd = (char*)calloc(i + sizeof CPP, sizeof(char))))
    {
        return -1;
    }
    
    strcpy(cmd, CPP);
    for (argp = argv; *++argp; )
    {
        if (**argp == '-' && strchr("CDEIUP", (*argp)[i]))
        {
            strcat(cmd, " "), strcat(cmd, *argp);
        }
    }
    
    if ((yyin = popen(cmd, "r")))
        i = 0;      // all's well
    else
        i = -1;     // no preprocessor
    free(cmd);
    
    return i;
}

/*
 *  main() -- run C preprocessor then yyparse()
 */
int main(int argc, char** argv)
{
    char name[80];
    
    // default output file to stdout
    yyout = stdout;
    
    // init the error stream
    //yyerfp = stdout;
    yyerfp = stderr;

#undef YYDEBUG
#ifdef YYDEBUG
    extern int yydebug;
    yydebug = 1;
#endif

    // for now cpp will always be called
    int cppflag = 1;
    
	// parse the command line options
	ParseOptions(argc, argv);

    // default to the base name of the input file name with the extension changed to .bin
    if (!outfileName)
    {
        char *nextc;
        
        strcpy(name, basename(infileName));
        for (nextc = name; *nextc != '.'; nextc++)
            ;
        *nextc = '\0';
        strcat(name, ".bin");
        outfileName = name;    
    }
    
    // open the output file
    if ((yyout = fopen(outfileName, "w")) == 0)
    {
        fprintf(stderr, "hcc: cannot open file %s\n", outfileName);
        exit(EXIT_FAILURE);
    }
    
    // run the first pass
    asm_pass = 1;
    
    // redirect infileName as stdin      
    if (infileName && !freopen(infileName, "r", stdin))
    {
        perror(infileName);
        exit(EXIT_FAILURE);
    }
    
    // run the C preprocessor
    if (cppflag && cpp(argc, argv))
    {
        perror("C preprocessor");
        exit(EXIT_FAILURE);
    }  
    cur_addr = 0xf000;      
    yyparse();
    
    // run the second pass
    asm_pass = 2;
    
    // redirect infileName as stdin      
    if (infileName && !freopen(infileName, "r", stdin))
    {
        perror(infileName);
        exit(EXIT_FAILURE);
    }
    
    // run the C preprocessor
    if (cppflag && cpp(argc, argv))
    {
        perror("C preprocessor");
        exit(EXIT_FAILURE);
    }        
    cur_addr = 0xf000;      
    yyparse();
}

// end of main.c

