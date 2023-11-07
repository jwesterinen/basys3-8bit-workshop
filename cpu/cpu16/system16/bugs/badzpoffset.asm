// FIXME: there is a bug, likely in the sound generator, that corrupts zero page memory locations below 0x0010
#define FIRST_ZPRAM_ADDR    0x10    // Zero Page indeces are allocated from 0x10 to 0xff
#define ZPRAM_LENGTH        0x100


