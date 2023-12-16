/*
 *  libasm.h
 *
 *  This file contains all of the system16 standard library assembly subroutine prototypes.
 *
 */

#ifndef LIBASM_H
#define LIBASM_H

int _AppendKeyValue(int value, int newEntry);
void _DelayMs(int ms);
void _Display(int value);
int _ReadButton();
int _ReadKeypad();
int _ReadSwitches();
void _ShowLeds(int value);

#endif // LIBASM_H
    
