int global;

main(a, b)
{
    int z;
    z = foo(a, b);
    global = z;
}

int foo(x, y)
{
    int c, d;    
    c = x;
    d = bar(c, y);
    return d;
}

int bar(x, y)
{
    int retval;    
    {
        int q;
        q = y;
        retval = x + q + 2;
    }
    return retval;
}
