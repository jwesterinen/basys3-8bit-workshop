/*  Second level assignment operator test
 *
 *  Test global/local/param variables with direct/indirect/pointer/reference operations.
 *
 *  Test passes if 
 *    - display = 0FFF
 */

#include <system16/libasm.h>

int results;
int g, ga[5], *pg;

void foo(int fa, int *pfb, int fc[])
{
    int fx, *pfx;

    // param direct
    fx = fa;
    fa = fa + 1;
    if (fa == fx + 1)
        results = results | 0x0100;

    // param reference        
    fx = *pfb;
    *pfb = *pfb + 1;
    if (*pfb == fx + 1)
        results = results | 0x0200; 
     
    // param pointer   
    pfx = pfb;
    pfb = pfb + 1;
    if (pfb == pfx + 1)
        results = results | 0x0400; 
        
    // param indirect        
    fx = fc[1];
    fc[1] = fc[1] + 1;
    if (fc[1] == fx + 1)
        results = results | 0x0800; 
}    

int main()
{
    int x, *px, *py;
    int a[5];

/*
*/
    // init test
    results = 0;
    ga[1] = 0x5b5b;
    a[1] = 0x5c5c;
    
    // local direct
	x = 0x1234;
	x = x + 1;
	if (x == 0x1235)
	    results = results | 0x0001;

    // local reference	    
	px = &x;
	py = px;
	px = px + 1;
	if (px == py + 1)
	    results = results | 0x0002;
	    
    // local pointer
    x = *px;
    *px = *px + 1;
    if (*px == x + 1)
        results = results | 0x0004;
        
    // local indirect        
    x = a[1];
    a[1] = a[1] + 1;
    if (a[1] == x + 1)
        results = results | 0x0008; 

    // global direct
	g = 0x2345;
	g = g + 1;
	if (g == 0x2346)
	    results = results | 0x0010;

    // global reference	    
	pg = &g;
	px = pg;
	pg = pg + 1;
	if (pg == px + 1)
	    results = results | 0x0020;

    // global pointer
    g = *pg;
    *pg = *pg + 1;
    if (*pg == g + 1)
        results = results | 0x0040;
        
    // global indirect        
    g = ga[1];
    ga[1] = ga[1] + 1;
    if (ga[1] == g + 1)
        results = results | 0x0080; 
        
    foo(0x5a5a, &x, a);

	_Display(results);
}

