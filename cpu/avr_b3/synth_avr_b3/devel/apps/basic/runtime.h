/*
*   runtime.h
*
*/

extern char errorStr[];
extern char resultStr[];
extern bool ready;
extern bool textMode;
    
bool ProcessCommand(char *cmdStr);
bool InstallBuiltinFcts(void);


