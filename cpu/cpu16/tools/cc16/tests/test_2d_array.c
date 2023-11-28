// test arrays

int b[5][4];
int n;

void main()
{
    int a[3][2];
    int x;

    a[2][1] = 7;   // local array assignment
    x = a[2][1];   // local var assignment from localarray
    b[3][2] = 8;   // global array assignment    
    n = b[3][2];   // global var assignment from global array
}
