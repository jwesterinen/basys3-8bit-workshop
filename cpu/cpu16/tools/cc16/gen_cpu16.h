/*
 *  gen_hack.h - code generator definitions
 */

void GenBeginProg();
void GenEndProg();
void GenCall(const char *fctname);
void GenEntry(const char *fctname, const char *symbol);
void GenAlu(const char *mod, const char *comment);
void GenLoadImmed(const char *constant);
void GenDirect(const char *op, const char *vartype, int offset, const char *globalName);
void GenIndirect(const char *op, const char *vartype, int offset, const char *globalName, int isRhs);
void GenPointer(const char *op, const char *vartype, int offset, const char *globalName, int is_rhs);
void GenReference(const char *op, const char *vartype, int offset, const char *globalName);
void GenPop(const char *op, const char *comment);
void GenReturn(const char *op, const char *comment);
void GenJump(const char *op, const char *label, const char *comment);
void GenLabel(const char *label);
void GenEqu(const char *symbol, int value);

// end of gen_hack.h

