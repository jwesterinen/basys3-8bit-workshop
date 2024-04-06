/*
 * Conway's Game of Life
 * 
 * Rules
 *   For a cell that is populated:
 *    - Each cell with one or no neighbors dies, as if by solitude.
 *    - Each cell with four or more neighbors dies, as if by overpopulation.
 *    - Each cell with two or three neighbors survives.
 *   
 *   For a cell that is unpopulated
 *    - Each cell with three neighbors becomes populated.
 * 
 * This is the kernel code of the game of life algorithm.  It is meant to be included in
 * application code and depends greatly on definitions in the application in which it is 
 * included.  Among those things that must be defined:
 *  - the semantics of a PIXEL_BUFFER and 2 accessor functions
 *     - int GetPixel(); 
 *     - void SetPixel();
 *  - the following manifest constants:
 *     - XMAX and YMAX which is the resolution of the display HW
 *     - ON_COLOR and OFF_COLOR which describe what color to use for an "on" and "off" pixel
 *     - SEED_QTY which describes how many cells will be randomly made alive to begin
 *  - two HW specific functions
 *     - void ClearDisplay(); which clears the HW display
 *     - void Display(PIXEL_BUFFER buffer); which displays the contents of the pixel buffer on the HW display
 */


namespace Life
{
// ping pong buffers
PIXEL_BUFFER buf0, buf1;

// seed buffer
SEED_BUFFER *pSeedBuf = 0;

// game state
int tick = 0;
int state = 0;
bool clearFB = true;

// initialize the game
void InitGame() 
{
    int x = -1, y = -1;
    int xPrev = x, yPrev = y;
    
    // reset the generation
    tick = 0;
    state = 0;

    // clear the display
    HW::ClearDisplay(buf0, buf1, clearFB);

    // seed and display the ref buffer
    if (pSeedBuf)
    {
        // load the ref buffer with a fixed seed
        for (int y = 0; y < YMAX_SEED; y++)
        {
            for (int x = 0; x < XMAX_SEED; x++)
            {
                SetPixel(buf0, x, y, (*pSeedBuf)[y][x] ? ON_COLOR : OFF_COLOR);
            }
        }
    }
    else
    {
        // seed and display the ref buffer
        for (int i = 0; i < SEED_QTY; i++)
        {
            while (x == xPrev && y == yPrev)
            {
                x = random(XMAX);
                y = random(YMAX);
            }
            SetPixel(buf0, x, y, ON_COLOR);
            xPrev = x;
            yPrev = y;
        }
    }
    HW::Display(buf0);
}

// adjust x for the boarders to implement a toroidal array
int AdjX(int x)
{
    if (x == -1)
        x = XMAX-1;
    if (x == XMAX)
        x = 0;
    return x;
}

// adjust y for the boarders to implement a toroidal array
int AdjY(int y)
{
    if (y == -1)
        y = YMAX-1;
    if (y == YMAX)
        y = 0;
    return y;
}

// return the number of neighbors the cell has:
// Neighbor IDs:
//   1|2|3
//   4|-|5
//   6|7|8
int CountNeighbors(PIXEL_BUFFER buffer, int x, int y)
{
    int n = 0;

    if (GetPixel(buffer, AdjX(x-1), AdjY(y-1)))   // neighbor 1
        n++;
    if (GetPixel(buffer, x,         AdjY(y-1)))   // neighbor 2
        n++;
    if (GetPixel(buffer, AdjX(x+1), AdjY(y-1)))   // neighbor 3
        n++;
    if (GetPixel(buffer, AdjX(x-1), y        ))   // neighbor 4
        n++;
    if (GetPixel(buffer, AdjX(x+1), y        ))   // neighbor 5
        n++;
    if (GetPixel(buffer, AdjX(x-1), AdjY(y+1)))   // neighbor 6
        n++;
    if (GetPixel(buffer, x,         AdjY(y+1)))   // neighbor 7
        n++;
    if (GetPixel(buffer, AdjX(x+1), AdjY(y+1)))   // neighbor 8
        n++;

    return n;
}

/*
 *  This is the heart of the game.  It implements the basic rules of the game.
 *  The rules are performed against the reference buffer and the results go into
 *  the frame buffer.  It is the frame buffer that is displayed.
 */
bool CreateNextFrame(PIXEL_BUFFER refBuf, PIXEL_BUFFER frameBuf)
{
    bool changed = false;
    for (int y = 0; y < YMAX; y++)
    {
        for (int x = 0; x < XMAX; x++)
        {
            // count the qty of neighbors
            int n = CountNeighbors(refBuf, x, y);
            bool alive;

            // For a cell that is populated:
            // - Each cell with one or no neighbors dies, as if by solitude.
            // - Each cell with four or more neighbors dies, as if by overpopulation.
            if ((alive = (GetPixel(refBuf, x, y) == ON_COLOR)))
            {
                if (n <= 1 || n >= 4)
                {
                    alive = false;
                    changed |= true;
                }
            }

            // For a cell that is unpopulated:
            // - Each cell with three neighbors becomes populated.
            else
            {
                if (n == 3)
                {
                    alive = true;
                    changed |= true;
                }
            }

            // modify the cell in the frame buffer: if alive is true the cell is populated
            SetPixel(frameBuf, x, y, (alive) ? ON_COLOR : OFF_COLOR);
        }
    }

    // display the contents of the current frame buffer
    HW::Display(frameBuf);

    // return true if any changes were made to the frame buffer
    return changed;
}

/*
 * This is the main function of the algorighm that the app calls to show the next frame of the game.
 */
void ShowNextFrame()
{
    bool changed;

    // create the next frame using the ping pong buffers
    if (state == 0)
    {
        // ref buf is buf0, frame buf if buf1
        changed = CreateNextFrame(buf0, buf1);
        state = 1;
    }
    else
    {
        // ref buf is buf1, frame buf if buf0
        changed = CreateNextFrame(buf1, buf0);
        state = 0;
    }

    // control frame rate    
    delay(TICK_DURATION);

    // restart if there are no changes from the last frame
    if (!changed || tick++ > GENERATION_DURATION)
    {
        InitGame();
    }
}
}

