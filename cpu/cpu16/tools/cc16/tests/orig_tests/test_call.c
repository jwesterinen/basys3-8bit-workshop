int g;

void foo(int *a)
{
    g = a;
}

int bar(int a)
{
    return a+1;
}

void baz()
{
    
}

void main()
{
    int x, y;

    baz();    
    x = 1;
    foo(x);
    x = x + 1;
    foo(x);
    y = bar(x);
}

