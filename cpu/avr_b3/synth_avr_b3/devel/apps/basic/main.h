/*
    main.h
*/

//#define VERBOSE
#ifdef VERBOSE
#define MESSAGE(s) Message(s)
#else
#define MESSAGE(s)
#endif

extern char message[80];
void Console(const char *string);
void Message(const char *message);
char *GetString(char *buffer);
void PutString(char *string);
void InitDisplay(void);


