#include <system16/system16.h>

int main()
{
    int *pLeds = LED_REG;
    *pLeds = Five();
}

int Five()
{
    return 5;
}

