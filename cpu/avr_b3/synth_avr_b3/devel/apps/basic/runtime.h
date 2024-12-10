/*
*   runtime.h
*
*/

#define MAX_PROGRAM_LEN 100
#define MAX_CMDLINE_LEN 80

extern char resultStr[];
extern bool ready;
extern bool textMode;

bool ProcessCommand(char *cmdStr);
bool InstallBuiltinFcts(void);


