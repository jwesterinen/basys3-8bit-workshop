#include <system16/system16.h>

int main()
{
    int *pLeds = LED_REG;
    int x;
    
    *pLeds = Add(-1, 2);
}

int Add(int a, int b)
{
    return a + b;
}

