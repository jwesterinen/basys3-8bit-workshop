int foo = 3;

main(a, b)
{
    a=b;
    {a;b;}
    if (a==b){a;b;}
    if (a==b+1) a; else b;
    while (a==b) {a; break;}
    return;
}

int f() {int x; int y; return x+y;}

f(a, b) 
{
    int b, c; 
    int c, d; 
    e=b; 
    f=c+d; 
    a(b); 
    g();
}

h(a, a)
{}

int foo()
{
    return 4;
}

