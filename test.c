#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <time.h>
#include <stdlib.h>

#define BRICK   0x2593
#define FRAME   0x2588

#define ALL_AREA_COLOUN 40
#define SIDE_AREA_COLOUN 16
#define GAME_AREA_COLOUN 23
#define GAME_AREA_ROW 21
#define UP_AREA_ROW 6
#define DOWN_AREA_ROW 14
#define GAME_AREA_CENTER_COLOUN 5
#define NEXT_AREA_ROW 11
#define NEXT_AREA_COLOUN 14


#define SITE_H 10
#define SITE_V 20

struct Block
{
	int space[4][4];
}block[7][4];

int game_area_sign[GAME_AREA_ROW][GAME_AREA_COLOUN] = {0};  //for further design

void move_cursor(int row, int col) 
{
    wprintf(L"\033[%d;%dH", row, col);
}

void DrawSpace(int shape, int form, int x, int y)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (block[shape][form].space[i][j] == 1) //if need to clean block of a position, move cursor to it.
			{
				move_cursor(x + i, 2 * (y + j));
				wprintf(L"  ");
			}
		}
	}
}

void DrawBlock(int shape, int form, int x, int y)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (block[shape][form].space[i][j] == 1) //if need to draw block of a position, move cursor to it.
			{
				move_cursor(x + i, 2 * (y + j));
				wprintf(L"%lc%lc", BRICK, BRICK);
			}
		}
	}
}

void Get_next_form(struct Block *src, struct Block *dest)
{
    int i = 0;
    int j = 0;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            dest->space[j][3 - i] = src->space[i][j];
        }
    }
}

void InitBlockInfo()
{
	//“T” type
	for (int i = 0; i <= 2; i++)
		block[0][0].space[1][i] = 1;
	block[0][0].space[2][1] = 1;

	//“L” type
	for (int i = 1; i <= 3; i++)
		block[1][0].space[i][1] = 1;
	block[1][0].space[3][2] = 1;

	//“J” type
	for (int i = 1; i <= 3; i++)
		block[2][0].space[i][2] = 1;
	block[2][0].space[3][1] = 1;

	for (int i = 0; i <= 1; i++)
	{
		//“Z” type
		block[3][0].space[1][i] = 1;
		block[3][0].space[2][i + 1] = 1;
		//“S” type
		block[4][0].space[1][i + 1] = 1;
		block[4][0].space[2][i] = 1;
		//“O” type
		block[5][0].space[1][i + 1] = 1;
		block[5][0].space[2][i + 1] = 1;
	}

	//“I” type
	for (int i = 0; i <= 3;i++)
		block[6][0].space[i][1] = 1;

	int temp[4][4];
	for (int shape = 0; shape < 7; shape++) //7 types
	{
		for (int form = 0; form < 3; form++) //4 shapes
		{
			Get_next_form(&block[shape][form], &block[shape][form+1]);
		}
	}
}



void clear_next_area(void)
{
    for (int i = UP_AREA_ROW + 3; i < GAME_AREA_ROW; i++)
    {
        move_cursor(i, GAME_AREA_COLOUN + 2);
        wprintf(L"\033[K");
        wprintf(L"              %lc%lc", FRAME, FRAME);
    }
}

int show_next_area(int shpae_index)
{
    DrawBlock(shpae_index, 0, NEXT_AREA_ROW, NEXT_AREA_COLOUN);
}

void print_site(void)  //print all areas
{
    int i = 0;
    for (i = 0; i < ALL_AREA_COLOUN; i++)
        wprintf(L"%lc", FRAME);
    wprintf(L"\n");
    for (i = 0; i < UP_AREA_ROW; i++)
        wprintf(L"%lc%lc                    %lc%lc              %lc%lc\n", FRAME, FRAME, FRAME, FRAME, FRAME, FRAME);
    wprintf(L"%lc%lc                    %lc%lc", FRAME, FRAME, FRAME, FRAME);
    for (i = 0; i < SIDE_AREA_COLOUN; i++)
        wprintf(L"%lc", FRAME);
    wprintf(L"\n");    
    for (i = 0; i < DOWN_AREA_ROW; i++)
        wprintf(L"%lc%lc                    %lc%lc              %lc%lc\n", FRAME, FRAME, FRAME, FRAME, FRAME, FRAME);        
    for (i = 0; i < ALL_AREA_COLOUN; i++)
        wprintf(L"%lc", FRAME);
    wprintf(L"\n");
}

void clear_line(int s, int e)
{
    int i;
    for (i = s; i <= e; i++)
        wprintf(L"\033[%d;%dH\033[K", i, 1);
    wprintf(L"\033[%d;%dH", s, 1);
}


int main() {
    int block_index = 0;
    int next_block_index = 0;
    int block_row_position = 1;

    int bottom_position = GAME_AREA_ROW - 1;  //temporary condition and parameter. waiting for further design.

    InitBlockInfo();
    setbuf(stdout,NULL);  //print in real time
    setlocale(LC_ALL, "");
    clear_line(1, 30);
    print_site();
    srand(time(NULL));
    block_index = rand() % 7;

    while(1)
    {
        DrawBlock(block_index, 0, block_row_position, GAME_AREA_CENTER_COLOUN);
        next_block_index = rand() % 7;
        show_next_area(next_block_index);
        while(1)
        {
            sleep(1);
            DrawSpace(block_index, 0, block_row_position, GAME_AREA_CENTER_COLOUN);
            block_row_position ++;
            DrawBlock(block_index, 0, block_row_position, GAME_AREA_CENTER_COLOUN);

            if (block_row_position >= bottom_position)  //temporary condition and parameter. waiting for further design.
            {
                bottom_position = bottom_position - 2;
                block_index = next_block_index;
                block_row_position = 1;
                break;
            }
        }
        
        clear_next_area();
    }

    return 0;
}

