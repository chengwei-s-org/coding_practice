
#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>

#define BRICK   0x2593
#define FRAME   0x2588

void print_shape_I(void)
{
    wprintf(L"%lc%lc\n%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    wprintf(L"%lc%lc\n%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
}

void print_shape_O(void)
{
    wprintf(L"%lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    wprintf(L"%lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
}

void print_shape_T(void)
{
    wprintf(L"%lc%lc%lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK, BRICK, BRICK);
    wprintf(L"  %lc%lc  \n", BRICK, BRICK);
}

void print_shape_L(void)
{
    wprintf(L"%lc%lc\n%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    wprintf(L"%lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
}

void print_shape_J(void)
{
    wprintf(L"  %lc%lc\n  %lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    wprintf(L"%lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
}

void print_shape_Z(void)
{
    wprintf(L"  %lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    wprintf(L"%lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
}

void print_shape_S(void)
{
    wprintf(L"%lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
    wprintf(L"  %lc%lc%lc%lc\n", BRICK, BRICK, BRICK, BRICK);
}

#define SITE_H 10
#define SITE_V 20
void print_site(void)
{
    int i = 0;
    for (i = 0; i < 24; i++)
        wprintf(L"%lc", FRAME);
    wprintf(L"\n");
    for (i = 0; i < 20; i++)
        wprintf(L"%lc%lc                    %lc%lc\n", FRAME, FRAME, FRAME, FRAME);
    for (i = 0; i < 24; i++)
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
    setlocale(LC_ALL, "");
    clear_line(1, 30);
    print_site();
    print_shape_T();
    sleep(1);
    clear_line(23, 28);
    print_shape_O();
    sleep(1);
    clear_line(23, 28);
    print_shape_L();
    sleep(1);
    clear_line(23, 28);
    print_shape_J();
    sleep(1);
    clear_line(23, 28);
    print_shape_T();
    sleep(1);
    clear_line(23, 28);
    print_shape_Z();
    sleep(1);
    clear_line(23, 28);
    print_shape_S();
    sleep(1);
    clear_line(23, 28);
    return 0;
}

