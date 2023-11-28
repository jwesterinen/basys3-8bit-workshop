/*
 *  Factorial
 */

#define KEYCODE_CLR         30  // CLR keycode
#define KEYCODE_MASK        15  // converts a digit keycode to an integer

int isNewEntry;
int keycode;

main()
{
    // init
    isNewEntry = 1;
    ClearDisplay();
    
    // perform factorial calculation on entry
    while (1)
    {
        // get the next key pressed
        keycode = ReadKey();
        
        // reset the system
        if (keycode == KEYCODE_CLR)
        {
            // reset the system
            ResetSys();
            continue;
        }
        
        // accept and process entries
        if (keycode < 26)
        {
            // rotate a new digit into the display
            if (isNewEntry)
                ClearDisplay();
            isNewEntry = 0;
            AppendNum3(keycode & KEYCODE_MASK);
        }
        else 
        {
            // print the factorial of the entry
            Printsd(Factorial(Getsd()));
            isNewEntry = 1;
        }
    }
}

int Factorial(n) 
{
    if (n >= 1)
        return n * Factorial(n-1);
    else
        return 1;
}

