// test for arithmetic with function calls

int foo(int a)
{
    return a;
}

/*
int bar(int a)
{
    return a;
}
*/

int main()
{
    int x;
    
    x = ~foo(5);
    //x = foo(4) + foo(5);
    //x = -3;
}

