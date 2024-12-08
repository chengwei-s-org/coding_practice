#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <time.h>
#include <stdlib.h>

#define BRICK   0x2593
#define FRAME   0x2588

#define SITE_H 10
#define SITE_V 20

void move_cursor(int row, int col) 
{
    wprintf(L"\033[%d;%dH", row, col);
}

void print_shape_I(int row, int col)
{
    move_cursor(row, col);
    wprintf(L"%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    move_cursor(row + 1, col);
    wprintf(L"%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    move_cursor(row + 2, col);
    wprintf(L"%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    move_cursor(row + 3, col);
    wprintf(L"%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
}

void print_shape_O(int row, int col)
{
    move_cursor(row, col);
    wprintf(L"%lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    move_cursor(row + 1, col);
    wprintf(L"%lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
}

void print_shape_T(int row, int col)
{
    move_cursor(row, col);
    wprintf(L"%lc%lc%lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK, BRICK, BRICK);
    move_cursor(row + 1, col);
    wprintf(L"  %lc%lc  \n", BRICK, BRICK);
}

void print_shape_L(int row, int col)
{
    move_cursor(row, col);
    wprintf(L"%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    move_cursor(row + 1, col);
    wprintf(L"%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    move_cursor(row + 2, col);
    wprintf(L"%lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
}

void print_shape_J(int row, int col)
{
    move_cursor(row, col);
    wprintf(L"  %lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    move_cursor(row + 1, col);
    wprintf(L"  %lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    move_cursor(row + 2, col);
    wprintf(L"%lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
}

void print_shape_Z(int row, int col)
{
    move_cursor(row, col);
    wprintf(L"  %lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    move_cursor(row + 1, col);
    wprintf(L"%lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
}

void print_shape_S(int row, int col)
{
    move_cursor(row, col);
    wprintf(L"%lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    move_cursor(row + 1, col);
    wprintf(L"  %lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
}



void clear_next_area(void)
{
    for (int i = 0; i < 14; i++)
    {
        move_cursor(i + 9, 25);
        wprintf(L"\033[K");
        wprintf(L"              %lc%lc", FRAME, FRAME);
    }
}

int show_next_area(void)
{
    int random_number = rand() % 7;
    switch (random_number) {
        case 0:
            print_shape_I(15, 30);
            break;
        case 1:
            print_shape_O(15, 30);
            break;
        case 2:
            print_shape_T(15, 30);
            break;
        case 3:
            print_shape_L(15, 30);
            break;
        case 4:
            print_shape_J(15, 30);
            break;
        case 5:
            print_shape_Z(15, 30);
            break;
        default:
            print_shape_S(15, 30);
            break;
    }

    return random_number;
}

void print_site(void)
{
    int i = 0;
    for (i = 0; i < 40; i++)
        wprintf(L"%lc", FRAME);
    wprintf(L"\n");
    for (i = 0; i < 6; i++)
        wprintf(L"%lc%lc                    %lc%lc              %lc%lc\n", FRAME, FRAME, FRAME, FRAME, FRAME, FRAME);
    wprintf(L"%lc%lc                    %lc%lc", FRAME, FRAME, FRAME, FRAME);
    for (i = 0; i < 16; i++)
        wprintf(L"%lc", FRAME);
    wprintf(L"\n");    
    for (i = 0; i < 14; i++)
        wprintf(L"%lc%lc                    %lc%lc              %lc%lc\n", FRAME, FRAME, FRAME, FRAME, FRAME, FRAME);        
    for (i = 0; i < 40; i++)
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
    int next_block_index = 0;

    setlocale(LC_ALL, "");
    clear_line(1, 30);
    print_site();
    srand(time(NULL));
    while(1)
    {
        next_block_index = show_next_area();
        sleep(2);
        clear_next_area();
    }

    return 0;
}

