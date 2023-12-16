main()
{
    int x, y;
    x = 5;
    y = x + 2;
    y += 3;
    x++;
    x = 4, y = 5;
    x = 1 + 2 * 3;
    
    if (x < y)
        x = 7;
    else
        y = 2;
        
    while (x < y)
    {
        x++;
        if (x == 1)
            break;
            
        if (y == 2)
            continue;
        y++;
    }
}

