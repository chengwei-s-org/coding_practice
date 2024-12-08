
#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <time.h>
#include <stdlib.h>

#define BRICK   0x2593
#define FRAME   0x2588

#define SITE_H 20
#define SITE_V 20
#define SHOW_H 18
#define FRAME_W 2
#define SHAPE_W 4

static int score = 0;

enum Shape
{
    I,
    O,
    T,
    L,
    J,
    Z,
    S,
    SHAPE_NUM
};

struct Blocks
{
    int shapes[SHAPE_W][SHAPE_W];
}blocks[SHAPE_NUM][4];

void init_blocks(void);
void shape_rotate(int ishape, int irotate);
void move_corsour(int row, int col);
void clear_line(int s, int e);
void clear_area(int sr, int er, int sc, int ec);
void clear_shape(int ishape, int irotate,int posx, int posy);
void print_site(void);
void print_score(int score);
void print_shape(int ishape, int irotate,int posx, int posy);
void print_next_shape(int ishape, int irotate);

int main() 
{
    setlocale(LC_ALL, "");
    clear_line(1, SITE_V+10);
    print_site();
    print_score(0);
    init_blocks();
    while(1)
    {
        int ishape = rand() % SHAPE_NUM;
        int irotate = rand() % 4;
        sleep(2);
        print_next_shape(ishape, 0);
    }
    return 0;
}

void init_blocks(void) 
{
    int shape0[][SHAPE_W][SHAPE_W] = 
    {
        {
            {1, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 0, 0, 0}
        },
        {
            {1, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {1, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {1, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        },
            {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
    };

    for (int s = 0; s < SHAPE_NUM; s++)
    {
        for (int i = 0; i < SHAPE_W; i++)
        {
            for (int j = 0; j < SHAPE_W; j++)
            {
                blocks[s][0].shapes[i][j] = shape0[s][i][j];
            }
        }
    }

    for (int ishape = 0; ishape < SHAPE_NUM; ishape++)
    {
        for (int irotate = 1; irotate < 4; irotate++)
            shape_rotate(ishape, irotate);
    }
}

void shape_rotate(int ishape, int irotate)
{
    for (int i = 0; i < SHAPE_W; i++)
    {
        for (int j = 0; j < SHAPE_W; j++)
        {
            blocks[ishape][irotate].shapes[i][j] = blocks[ishape][irotate-1].shapes[j][3-i];
        }
    }
}

void move_corsour(int row, int col)
{
    wprintf(L"\033[%d;%dH", row, col);
}

void clear_line(int s, int e)
{
    int i;
    for (i = s; i <= e; i++)
        wprintf(L"\033[%d;%dH\033[K", i, 1);
    wprintf(L"\033[%d;%dH", s, 1);
}

void clear_area(int sr, int er, int sc, int ec)
{
    int i, j;
    for (i = sr; i <= er; i++)
    {
        for (j = sc; j <= ec; j++)
        {
            wprintf(L"\033[%d;%dH ", i, j);
        }    
    }
    wprintf(L"\033[%d;%dH", sr, sc);
}

void clear_shape(int ishape, int irotate,int posx, int posy) 
{
    for (int i = 0; i < SHAPE_W; i++)
    {
        for (int j = 0; j < SHAPE_W; j++)
        {
            if (blocks[ishape][irotate].shapes[i][j] == 1)
            {
                move_corsour(posx+i,posy+2*j);
                wprintf(L"  ");
            }
        }
    }
}

void print_site(void)
{
    int i = 0;
    int j = 0;
    for (i = 0; i < FRAME_W+SITE_H+FRAME_W+SHOW_H+FRAME_W; i++)
        wprintf(L"%lc", FRAME);
    wprintf(L"\n");
    for (i = 0; i < SITE_V; i++) {
            for (j = 0; j < FRAME_W; j++)
                wprintf(L"%lc", FRAME);
            for (j = 0; j < SITE_H; j++)
                wprintf(L" ", FRAME);
            for (j = 0; j < FRAME_W; j++)
                wprintf(L"%lc", FRAME);
            for (j = 0; j < SHOW_H; j++)
                if (i == SITE_V/2-1) {
                    wprintf(L"%lc", FRAME);                
                }
                else{
                    wprintf(L" ", FRAME);
                }
            for (j = 0; j < FRAME_W; j++)
                wprintf(L"%lc", FRAME);
            wprintf(L"\n");
    }
    for (i = 0; i < FRAME_W+SITE_H+FRAME_W+SHOW_H+FRAME_W; i++)
        wprintf(L"%lc", FRAME);
    wprintf(L"\n");
    
    move_corsour(SITE_V/4,FRAME_W+SITE_H+FRAME_W+SHOW_H/2-2);
    wprintf(L"Score\n");
    wprintf(L"\033[%d;%dH\n", SITE_V+3, 1);
}

void print_score(int score)
{
    move_corsour(SITE_V/4+1,FRAME_W+SITE_H+FRAME_W+SHOW_H/2-2);
    wprintf(L"%d\n",score);
    wprintf(L"\033[%d;%dH\n", SITE_V+3, 1);
}

void print_shape(int ishape, int irotate,int posx, int posy)
{
    for (int i = 0; i < SHAPE_W; i++)
    {
        for (int j = 0; j < SHAPE_W; j++)
        {
            if (blocks[ishape][irotate].shapes[i][j] == 1)
            {
                move_corsour(posx+i,posy+2*j);
                wprintf(L"%lc%lc\n", BRICK, BRICK);
            }
        }
    }
    wprintf(L"\033[%d;%dH\n", SITE_V+3, 1);
}

void print_next_shape(int ishape, int irotate)
{
    int sr = SITE_V/2+SITE_V/4;
    int sc = FRAME_W+SITE_H+FRAME_W+SHOW_H/2-SHAPE_W+1;
    int er = sr + SHAPE_W-1;
    int ec = sc + 2*SHAPE_W-1;
    clear_area(sr, er, sc, ec);
    print_shape(ishape, irotate, sr, sc);
}

