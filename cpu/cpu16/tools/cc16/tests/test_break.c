main()
{
    int x;
    x = 0;
    while (1)
    {
        ++x;
        if (x == 2)
            continue;
        else if (x > 5)
            break;
        Printsd(x);
        DelayMs(500);
    }
}

