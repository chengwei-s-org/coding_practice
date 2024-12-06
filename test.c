#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>

#define SCREEN_HEIGHT 30

void print_shape_I(void)
{
    wprintf(L"%lc%lc\n%lc%lc\n", 10495, 10495, 10495, 10495);
    wprintf(L"%lc%lc\n%lc%lc\n", 10495, 10495, 10495, 10495);
}

void print_shape_O(void)
{
    wprintf(L"%lc%lc%lc%lc\n", 10495, 10495, 10495, 10495);
    wprintf(L"%lc%lc%lc%lc\n", 10495, 10495, 10495, 10495);
}

void print_shape_T(void)
{
    wprintf(L"%lc%lc%lc%lc%lc%lc\n", 10495, 10495, 10495, 10495, 10495, 10495);
    wprintf(L"  %lc%lc  \n", 10495, 10495);
}

void print_shape_L(void)
{
    wprintf(L"%lc%lc\n%lc%lc\n", 10495, 10495, 10495, 10495);
    wprintf(L"%lc%lc%lc%lc\n", 10495, 10495, 10495, 10495);
}

void print_shape_J(void)
{
    wprintf(L"  %lc%lc\n  %lc%lc\n", 10495, 10495, 10495, 10495);
    wprintf(L"%lc%lc%lc%lc\n", 10495, 10495, 10495, 10495);
}

void print_shape_Z(void)
{
    wprintf(L"  %lc%lc%lc%lc\n", 10495, 10495, 10495, 10495);
    wprintf(L"%lc%lc%lc%lc\n", 10495, 10495, 10495, 10495);
}

void print_shape_S(void)
{
    wprintf(L"%lc%lc%lc%lc\n", 10495, 10495, 10495, 10495);
    wprintf(L"  %lc%lc%lc%lc\n", 10495, 10495, 10495, 10495);
}

void clear_line(int line)
{
    int i;
    for (i = 1; i <= line; i++)
        wprintf(L"\033[%d;%dH\033[K", i, 1);
    wprintf(L"\033[H");
}

int main() {
    setlocale(LC_ALL, "");
    clear_line(SCREEN_HEIGHT);
    print_shape_I();
    sleep(1);
    clear_line(SCREEN_HEIGHT);
    print_shape_O();
    sleep(1);
    clear_line(SCREEN_HEIGHT);
    print_shape_L();
    sleep(1);
    clear_line(SCREEN_HEIGHT);
    print_shape_J();
    sleep(1);
    clear_line(SCREEN_HEIGHT);
    print_shape_T();
    sleep(1);
    clear_line(SCREEN_HEIGHT);
    print_shape_Z();
    sleep(1);
    clear_line(SCREEN_HEIGHT);
    print_shape_S();
    sleep(1);
    clear_line(SCREEN_HEIGHT);
    return 0;
}

