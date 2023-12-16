int g;

int foo(a, b)
{
    b = a - g;
    if (b)
        return a;
    bar();
    return b;
}

bar()
{
    g = 7;
}

main()
{
    int x, y;
    x = foo(1, 2);    
    if (x <= 3)
        x = 1;
    else
        x = 0;
    while (x < 5)
    {
        x--;
        if (x > 5)
            continue;
        if (x == 0)
            break;
        x++;
    }    
}


