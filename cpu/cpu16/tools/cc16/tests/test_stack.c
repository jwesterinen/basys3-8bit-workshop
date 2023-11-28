// test arrays

int stack[5];
int tos = 0;

void main()
{
    int i, n;
    
    i = 1;
    while (i <= 3)
    {
        // push
        stack[++tos] = i;
        ++i;
    }
    i = 0;
    while (i < 3)
    {
        // pop
        n = stack[tos];
        --tos;
        ++i;
    }
}
