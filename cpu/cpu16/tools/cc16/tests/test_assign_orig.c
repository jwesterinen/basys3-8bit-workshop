int g;

void main(int a)
{
    int x,y;
    
    g = 1;      // store global
    a = 2;      // store param
    
    x = g;      // load global, store local
    x = 5;      // load const
    x = a;      // load param
    x = y;      // load local
}

