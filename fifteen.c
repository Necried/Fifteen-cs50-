/**
 * fifteen.c
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */
 
#define _XOPEN_SOURCE 500

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

// board
int board[DIM_MAX][DIM_MAX];

// LUCAS ADD: unoccupied tile
int empty[2];

// dimensions
int d;

// prototypes
void clear(void);
void greet(void);
void init(void);
void draw(void);
bool move(int tile);
bool won(void);

int main(int argc, string argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // open log
    FILE *file = fopen("log.txt", "w");
    if (file == NULL)
    {
        return 3;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init();

    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                fprintf(file, "%i", board[i][j]);
                if (j < d - 1)
                {
                    fprintf(file, "|");
                }
            }
            fprintf(file, "\n");
        }
        fflush(file);

        // check for win
        if (won())
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = get_int();
        
        // quit if user inputs 0 (for testing)
        if (tile == 0)
        {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(500000);
        }

        // sleep thread for animation's sake
        usleep(500000);
    }
    
    // close log
    fclose(file);

    // success
    return 0;
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(2000000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).  
 */
void init(void)
{
    // Start from the highest number available for the board
    int tileNum = (d * d) - 1;
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            board[i][j] = tileNum;
            tileNum--;
        }
    }
    
    // Reverses the position of 1 and 2 if playing on an even-dimensioned board (to ensure solvability)
    if (d % 2 == 0)
    {
        board[d - 1][d - 3] = 1;
        board[d - 1][d - 2] = 2;
    }
    // Keep track of the empty tile on the board
    empty[0] = d - 1;
    empty[1] = d - 1;
    
    return;
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            // prints top horizontal blank line with segments '|'
            printf("    ");
            if (j != d - 1)
            {
                printf("|");
            }
            else
            {
                printf("\n");
            }
        }
        
        // prints mid horizontal line with number for each box
        for (int k = 0; k < d; k++)
        {
            if (board[i][k] >= 10)
            {
                printf(" %i ",board[i][k]);
            }
            else if (board[i][k] == 0)
            {
                printf(" __ ");
            }
            else
            {
                printf(" %i  ", board[i][k]);
            }
            
            
            if (k != d - 1)
            {
                printf("|");
            }
            else
            {
                printf("\n");
            }
        }
        
        //prints another horizontal blank line with "|" segment
        for (int j = 0; j < d; j++)
        {
            printf("    ");
            if (j != d - 1)
            {
                printf("|");
            }
            else
            {
                printf("\n");
            }
        }
        
        // Print final dotted line to segment rows. Does not print a line if it is the last column.
        if (i < d - 1)
        {
            for (int h = 0; h < d; h++)
            {
                printf("----");
                if (h != d - 1)
                {
                    printf("+");
                }
                else
                {
                    printf("\n");
                }
            }
        }
    }
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
bool move(int tile)
{
    // Ensures valid tile number
    if (tile > (d * d) - 1 || tile < 0)
    {
        return false;
    }
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            if (board[i][j] == tile)
            {
                // ensures adjacency of tiles
                if (abs(i - empty[0]) > 1 || abs(j - empty[1]) > 1)
                {
                    return false;
                }
                // ensures diagonal tiles cannot be moved
                else if (abs(i - empty[0]) != 0 && abs(j - empty[1]) != 0)
                {
                    return false;
                }
                else
                {
                    // assign current zero tile to future zero tile
                    board[i][j] = 0;
                    // then assign current occupied tile to future occupied tile
                    board[empty[0]][empty[1]] = tile;
                    // continue keeping track of unoccupied (zero) tile
                    empty[0] = i;
                    empty[1] = j;
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
bool won(void)
{
    int tileCheck = 1;
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            // Returns false immediately if a number is not in proper ascending order
            if (board[i][j] != tileCheck)
            {
                return false;
            }
            tileCheck = tileCheck + 1;
            // Passes check if all numbers are in ascending order.
            if (tileCheck == d * d)
            {
                break;
            }
        }
    }
    return true;
}

/* EOF
*/
