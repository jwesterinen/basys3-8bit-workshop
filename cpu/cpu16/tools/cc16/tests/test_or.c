#define RAM     0  
#define SCREEN  16384            // 0x4000
#define LEDS    28672

int main()
{
    int* dest = SCREEN;
    //int* dest = LEDS;
    //int* dest = RAM;
    int* leds = LEDS;
    //int value;
    
    dest += 10;
    *dest = 0;

#if 0
    value = *dest;
    *dest = value | 1;    
    value = *dest;
    *dest = value | 2;    
    value = *dest;
    *dest = value | 4;    
    value = *dest;
    *dest = value | 8;    
    value = *dest;
    *dest = value | 16;    
    value = *dest;
    *dest = value | 32;    
    value = *dest;
    *dest = value | 64;    
    value = *dest;
    *dest = value | 128;  
    value = *dest;
    *dest = value | 256;    
    value = *dest;
    *dest = value | 512;    
    value = *dest;
    *dest = value | 1024;    
    value = *dest;
    *dest = value | 2048;    
    value = *dest;
    *dest = value | 4096;    
    value = *dest;
    *dest = value | 8192;    
    value = *dest;
    *dest = value | 16384;    
    value = *dest;
    *dest = value | 32767+1;
    //*dest = value | 32768;
#else
    *dest = *dest | 1;    
    *dest = *dest | 2;    
    *dest = *dest | 4;    
    *dest = *dest | 8;    
    *dest = *dest | 16;    
    *dest = *dest | 32;    
    *dest = *dest | 64;    
    *dest = *dest | 128;  
    *dest = *dest | 256;    
    *dest = *dest | 512;    
    *dest = *dest | 1024;    
    *dest = *dest | 2048;    
    *dest = *dest | 4096;    
    *dest = *dest | 8192;    
    *dest = *dest | 16384;    
    *dest = *dest | 32767+1;
    //*dest = value | 32768;
#endif    
    
    //value = *dest;
    //*leds = value;
    *leds = *dest;
}
