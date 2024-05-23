/*
*   eval stack
*/

#define EVAL_STACK_SIZE 20

// eval stack and its index, i.e. eval stack pointer
int es[EVAL_STACK_SIZE], esp;

// init the stack
void InitEvalStack(void)
{
    esp = 0;
}

// push a onto stack
int Push(int a)
{
    es[esp++] = a;
    return a;
}

// pop the stack and return the value
int Pop(void)
{
    return es[--esp];
} 

// return TOS
int Top(void)
{
    return es[esp-1];
} 

// TOS = a
int Put(int a)
{
    es[esp-1] = a;
    return a;
}

void Add(void)
{
    // put the sum of the top 2 expr stack entries onto the top of the stack and set the new value
    Put(Pop() + Top());
}
                
void Multiply(void)
{
    // put the sum of the top 2 expr stack entries onto the top of the stack and set the new value
    Put(Pop() * Top());
}
                
void Subtract(void)
{
    // put the difference of the top 2 expr stack entries onto the top of the stack and set the new value
    int b = Pop();
    Put(Top() - b);
} 
               
void Divide(void)
{
    // put the difference of the top 2 expr stack entries onto the top of the stack and set the new value
    int b = Pop();
    Put(Top() / b);
}                

