
#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <time.h>
// #include <ncurses.h>
#include <stdlib.h>

#define BRICK   0x2593
#define FRAME   0x2588

#define SITE_W 20
#define SITE_H 20
#define SHOW_W 18
#define FRAME_W 2
#define SHAPE_W 4
#define SITE_BOUNDARY FRAME_W + SITE_W + FRAME_W
#define TETRISW SITE_BOUNDARY + SHOW_W + FRAME_W
#define TETRISH SITE_H + 2

#define NEXT_SHAPE_POSX SITE_BOUNDARY + SHOW_W / 2 - SHAPE_W + 1
#define NEXT_SHAPE_POSY SITE_H / 2 + SITE_H / 4 - 1

enum Shape
{
    I_shape,
    O_shape,
    T_shape,
    L_shape,
    J_shape,
    Z_shape,
    S_shape,
    SHAPE_NUM
};

struct Blocks
{
    int shapes[SHAPE_W][SHAPE_W];
}blocks[SHAPE_NUM][4] = 
    {
        { //I
            {0, 0, 0, 0,
             1, 1, 1, 1,
             0, 0, 0, 0,
             0, 0, 0, 0},
            {0, 1, 0, 0,
             0, 1, 0, 0,
             0, 1, 0, 0,
             0, 1, 0, 0},
            {0, 0, 0, 0,
             1, 1, 1, 1,
             0, 0, 0, 0,
             0, 0, 0, 0},
            {0, 1, 0, 0,
             0, 1, 0, 0,
             0, 1, 0, 0,
             0, 1, 0, 0},
        },
        { //O
            {0, 1, 1, 0,
             0, 1, 1, 0,
             0, 0, 0, 0,
             0, 0, 0, 0},
            {0, 1, 1, 0,
             0, 1, 1, 0,
             0, 0, 0, 0,
             0, 0, 0, 0},
            {0, 1, 1, 0,
             0, 1, 1, 0,
             0, 0, 0, 0,
             0, 0, 0, 0},
            {0, 1, 1, 0,
             0, 1, 1, 0,
             0, 0, 0, 0,
             0, 0, 0, 0},
        },
        { //T
            {0, 1, 0, 0,
             1, 1, 1, 0,
             0, 0, 0, 0,
             0, 0, 0, 0},
            {0, 1, 0, 0,
             1, 1, 0, 0,
             0, 1, 0, 0,
             0, 0, 0, 0},
            {0, 0, 0, 0,
             1, 1, 1, 0,
             0, 1, 0, 0,
             0, 0, 0, 0},
            {0, 1, 0, 0,
             0, 1, 1, 0,
             0, 1, 0, 0,
             0, 0, 0, 0},
        },
        { //L
            {0, 0, 1, 0,
             1, 1, 1, 0,
             0, 0, 0, 0,
             0, 0, 0, 0},
            {1, 1, 0, 0,
             0, 1, 0, 0,
             0, 1, 0, 0,
             0, 0, 0, 0},
            {1, 1, 1, 0,
             1, 0, 0, 0,
             0, 0, 0, 0,
             0, 0, 0, 0},
            {1, 0, 0, 0,
             1, 0, 0, 0,
             1, 1, 0, 0,
             0, 0, 0, 0},
        },
        { //J
            {1, 0, 0, 0,
             1, 1, 1, 0,
             0, 0, 0, 0,
             0, 0, 0, 0},
            {0, 1, 0, 0,
             0, 1, 0, 0,
             1, 1, 0, 0,
             0, 0, 0, 0},
            {1, 1, 1, 0,
             0, 0, 1, 0,
             0, 0, 0, 0,
             0, 0, 0, 0},
            {1, 1, 0, 0,
             1, 0, 0, 0,
             1, 0, 0, 0,
             0, 0, 0, 0},
        },
        { //Z
            {1, 1, 0, 0,
             0, 1, 1, 0,
             0, 0, 0, 0,
             0, 0, 0, 0},
            {0, 1, 0, 0,
             1, 1, 0, 0,
             1, 0, 0, 0,
             0, 0, 0, 0},
            {1, 1, 0, 0,
             0, 1, 1, 0,
             0, 0, 0, 0,
             0, 0, 0, 0},
            {0, 1, 0, 0,
             1, 1, 0, 0,
             1, 0, 0, 0,
             0, 0, 0, 0},
        },
        { //S
            {0, 1, 1, 0,
             1, 1, 0, 0,
             0, 0, 0, 0,
             0, 0, 0, 0},
            {1, 0, 0, 0,
             1, 1, 0, 0,
             0, 1, 0, 0,
             0, 0, 0, 0},
            {0, 1, 1, 0,
             1, 1, 0, 0,
             0, 0, 0, 0,
             0, 0, 0, 0},
            {1, 0, 0, 0,
             1, 1, 0, 0,
             0, 1, 0, 0,
             0, 0, 0, 0},
        }
    };

static int score = 0;
static int collision = 0;
int tetrisArea[TETRISH][TETRISW] = {0};

void init_blocks(void);
void shape_rotate(int ishape, int irotate);
void move_cursor(int row, int col);
void init_site(void);
void print_score();
void print_tetris_area();
void clear_line(int s, int e);
void clear_area(int startrow, int endrow, int startcolumn, int endcolumn);
void unfill_shape(struct Blocks *block, int posx, int posy);
void fill_shape(struct Blocks *block, int posx, int posy);

int main() 
{
    int ishape = rand() % SHAPE_NUM;
    int irotate = rand() % 4;
    int ishape_next = 0;
    int irotate_next = 0;
    int posx = 0;
    int posy = 0;
    int end = 0;
    setlocale(LC_ALL, "");
    clear_line(1, SITE_H+10);
    init_site();
    print_tetris_area();
    print_score();
    init_blocks();
    while(1)
    {
        posy = 0;
        posx = SITE_W/2-SHAPE_W/2;
        fill_shape(&blocks[ishape][irotate], posx, posy);
        if (collision == -2)
        {
            break;
        }
        // Print next shape
        unfill_shape(&blocks[ishape_next][irotate_next], NEXT_SHAPE_POSX, NEXT_SHAPE_POSY);
        ishape_next = rand() % SHAPE_NUM;
        irotate_next = rand() % 4;
        fill_shape(&blocks[ishape_next][irotate_next], NEXT_SHAPE_POSX, NEXT_SHAPE_POSY);

        while (1)
        {
            print_tetris_area();
            sleep(1);
            unfill_shape(&blocks[ishape][irotate], posx, posy);
            posy++;
            fill_shape(&blocks[ishape][irotate], posx, posy);
            if (collision == -1)
            {
                ishape = ishape_next;
                irotate = irotate_next;
                break;
            }
            if (collision == -2)
            {
                break;
            }
        }
        if (collision == -2)
        {
            break;
        }
    }
    print_tetris_area();
    getchar(); //End pause
    // pause();
    return 0;
}

void init_blocks(void) 
{
    // int ishape = 0;
    // int irotate = 1;
    // for (ishape = 0; ishape < SHAPE_NUM; ishape++)
    // {
    //     for (irotate = 1; irotate < 4; irotate++)
    //         shape_rotate(ishape, irotate);
    // }
}

void shape_rotate(int ishape, int irotate)
{
    int i = 0;
    int j = 0;
    for (i = 0; i < SHAPE_W; i++)
    {
        for (j = 0; j < SHAPE_W; j++)
        {
            blocks[ishape][irotate].shapes[i][j] = blocks[ishape][irotate-1].shapes[j][3-i];
        }
    }
}

void move_cursor(int row, int col)
{
    wprintf(L"\033[%d;%dH", row, col);
}

void init_site(void)
{
    int i = 0;
    for (i = 0; i < TETRISW; i++)
    {
        tetrisArea[0][i] = 1;
        tetrisArea[TETRISH-1][i] = 1;
    }
    for (i = 0; i < TETRISH; i++)
    {
        tetrisArea[i][0] = 1;
        tetrisArea[i][1] = 1;
        tetrisArea[i][SITE_BOUNDARY - 2] = 1;
        tetrisArea[i][SITE_BOUNDARY - 1] = 1;
        tetrisArea[i][TETRISW - 2] = 1;
        tetrisArea[i][TETRISW - 1] = 1;
    }

    for (i = 0; i < SHOW_W; i++)
        tetrisArea[SITE_H / 2 - 1][SITE_BOUNDARY + i] = 1;
}

void print_score()
{
    move_cursor(SITE_H / 4, SITE_BOUNDARY + SHOW_W / 2 - 2);
    wprintf(L"score\n");

    move_cursor(SITE_H / 4 + 1, SITE_BOUNDARY + SHOW_W / 2 - 2);
    wprintf(L"%d\n", score);
    wprintf(L"\033[%d;%dH\n", SITE_H + 3, 1);
}

void print_tetris_area()
{
    int i = 0;
    int j = 0;
    move_cursor(1, 1);
    for (i = 0; i < TETRISH; i++)
    {
        for (j = 0; j < TETRISW; j++)
        {
            if (i > 0 && i < SITE_H / 2 - 1 && j >= SITE_BOUNDARY && j < TETRISW - 2)
            {
                // if (j = TETRISW - 3)
                // {
                    move_cursor(i+1, TETRISW - 1);
                // }
            }
            else
            {
                if (tetrisArea[i][j] == 1)
                {
                    wprintf(L"%lc", BRICK);
                }
                else
                {
                    wprintf(L" ", BRICK);
                }
            }
        }
        wprintf(L"\n", BRICK);
    }
    wprintf(L"\033[%d;%dH\n", SITE_H + 3, 1);
}

void clear_line(int s, int e)
{
    int i;
    for (i = s; i <= e; i++)
        wprintf(L"\033[%d;%dH\033[K", i, 1);
    wprintf(L"\033[%d;%dH", s, 1);
}

void clear_area(int startrow, int endrow, int startcolumn, int endcolumn)
{
    int i, j;
    for (i = startrow; i <= endrow; i++)
    {
        for (j = startcolumn; j <= endcolumn; j++)
        {
            wprintf(L"\033[%d;%dH ", i, j);
        }    
    }
    wprintf(L"\033[%d;%dH", startrow, startcolumn);
}

void unfill_shape(struct Blocks *block,int posx, int posy) 
{
    int i = 0;
    int j = 0;
    for (i = 0; i < SHAPE_W; i++)
    {
        for (j = 0; j < SHAPE_W; j++)
        {
            if (block->shapes[i][j] == 1 && posy+i != 0 && posy+i != TETRISH-1)
            {
                tetrisArea[posy+i][posx+2*j] = 0;
                tetrisArea[posy+i][posx+2*j+1] = 0;
            }
        }
    }
}

void fill_shape(struct Blocks *block, int posx, int posy)
{
    int i = 0;
    int j = 0;
    for (i = SHAPE_W-1; i >= 0; i--)
    {
        for (j = 0; j < SHAPE_W; j++)
        {
            if (block->shapes[i][j] == 1)
            {
                if (tetrisArea[posy + i][posx + 2 * j] == 1) // Check collision
                {
                    // if (posy + i == 0)
                    // {
                    //     collision = -2; // Top collision
                    //     return;
                    // }else{
                        collision = -1; // Bottom collision
                        return;
                    // }
                }
                else
                {
                    tetrisArea[posy + i][posx + 2 * j] = 1;
                    tetrisArea[posy + i][posx + 2 * j + 1] = 1;
                }
            }
        }
    }
    collision = 0;
    return;
}



