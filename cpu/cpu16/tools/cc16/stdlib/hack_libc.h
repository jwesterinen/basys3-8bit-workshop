/*
    This is the equivalent of the standard C library for the Hack computer.
 */

// memory map 
#define RAM             0       // 0x0000 - 0x3fff
#define SCREEN          16384   // 0x4000 - 0x5fff
#define KEYBOARD        24576   // 0x6000
#define LEDS            28672   // 0x7000
#define GPIO_DATA       28673   // 0X7001
#define GPIO_DIR        28674   // 0X7002
#define SWITCHES        28675   // 0x7003
#define BUTTONS         28676   // 0x7004

// screen constants
#define BITS_PER_WORD   16      // pixels/word
#define WORDS_PER_ROW   32      // words/row
#define LINE_SIZE       512     // 0x200 pixels
#define ROW_QTY         256     // 0X100 rows
#define FB_SIZE         8192    // 0x2000 words
#define OFF             0       // draw pixel off
#define ON              1       // draw pixel on

// << operator
int ShiftLeft(int a, int b)
{
    int i = 0;    
    while (i < b)
    {
        a *= 2;
        ++i;
    }    
    return a;
}

// mod operator
#define mod(a,b) (a - (b * (a / b)))

// set a pixel at (col,row) to OFF or ON
void DrawPixel(int col, int row, int set)
{
    int* dest = SCREEN;
    int normWord;

    // calc the word in screen that contains the pixel
    dest += row * WORDS_PER_ROW + col / BITS_PER_WORD;
    
    // set the bit in the screen word's normalized column
    //   1 << normalized_col
    //   normalized_col = col % BITS_PER_WORD    
    //   normalized word = 1 << (col % BITS_PER_WORD)
    normWord = ShiftLeft(1, mod(col, BITS_PER_WORD));
    
    // for setting pixel to 1 => OR current word value with normalized word
    // for setting pixel to 0 => AND current word value with bitwise inverse of normalized word
    if (set)
        *dest = *dest | normWord;
    else
        *dest = *dest & ~normWord;
}

void DrawLine(int x1, int y1, int x2, int y2)
{
    int col, row;
    
    if (x1 == x2)
    {
        // draw vertical line
        while (y1 <= y2)
        {
            DrawPixel(x1, y1, ON);
            ++y1; 
        }
    }    
    else if (y1 == y2)
    {
        // draw horizontal line
        while (x1 <= x2)
        {
            DrawPixel(x1, y1, ON);
            ++x1;
        }
    }
}

void DrawRect(int x1, int y1, int x2, int y2)
{
    // draw top, left, right, bottom lines
    DrawLine(x1, y1, x2, y1);    
    DrawLine(x1, y1, x1, y2);    
    DrawLine(x2, y1, x2, y2);    
    DrawLine(x1, y2, x2, y2);    
}


