/*
 *  euclid.c - Euclid's algorithm
 */

main()
{
    int a, b;
    
    a = 36;
    b = 54;
    
    while (a != b)
    {
        if (a > b)
            a -= b;
        else
            b -= a;
    }
}
