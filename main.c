#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define SITE_H 10
#define SITE_V 20

#define BRICK   0x2593
#define WALL    0x2588
#define BLANK   0X20  

#define LINE_SIZE   37

typedef enum shape_type {
    SHAPE_I,
    SHAPE_O,
    SHAPE_L,
    SHAPE_J,
    SHAPE_T,
    SHAPE_S,
    SHAPE_Z, 
    SHAPE_MAX
} shape_type;

typedef enum degree {
    DEGREE_0,
    DEGREE_90,
    DEGREE_180,
    DEGREE_270,
    DEGREE_MAX
} degree;

typedef struct point
{
    int line;
    int row;
} point;

typedef struct area
{
    point top_left;
    point bottom_right;
} area;

wchar_t base_graph[] = {
    WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, WALL, WALL, 0xa,
    WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, 0xa,
};

void clear_line(int s, int e)
{
    int i;
    for (i = s; i <= e; i++)
        wprintf(L"\033[%d;%dH\033[K", i, 1);
    wprintf(L"\033[%d;%dH", s, 1);
}

void reset_area(wchar_t *graph, area a)
{
    int i, j;
    int base = (a.top_left.line + 1) * LINE_SIZE + a.top_left.row;
    int width = a.bottom_right.row - a.top_left.row + 1;
    int height = a.bottom_right.line - a.top_left.line + 1;

    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++)
            graph[base + i * LINE_SIZE + j] = 0x20;
}

#define IO_ALL      L"\u2593\u2593\u2593\u2593\u2593\u2593\u2593\u2593"

#define J_0         L"\u2593\u2593\x20\x20\u2593\u2593\u2593\u2593\u2593\u2593"
#define J_90        L"\u2593\u2593\u2593\u2593\u2593\u2593\x20\x20\u2593\u2593\x20\x20"
#define J_180       L"\u2593\u2593\u2593\u2593\u2593\u2593\x20\x20\x20\x20\u2593\u2593"
#define J_270       L"\x20\x20\u2593\u2593\x20\x20\u2593\u2593\u2593\u2593\u2593\u2593"

#define L_0         L"\u2593\u2593\x20\x20\u2593\u2593\x20\x20\u2593\u2593\u2593\u2593"
#define L_90        L"\u2593\u2593\u2593\u2593\u2593\u2593\u2593\u2593\x20\x20\x20\x20"
#define L_180       L"\u2593\u2593\u2593\u2593\x20\x20\u2593\u2593\x20\x20\u2593\u2593"
#define L_270       L"\x20\x20\x20\x20\u2593\u2593\u2593\u2593\u2593\u2593\u2593\u2593"

#define T_0         L"\x20\x20\u2593\u2593\x20\x20\u2593\u2593\u2593\u2593\u2593\u2593"
#define T_90        L"\u2593\u2593\x20\x20\u2593\u2593\u2593\u2593\u2593\u2593\x20\x20"
#define T_180       L"\u2593\u2593\u2593\u2593\u2593\u2593\x20\x20\u2593\u2593\x20\x20"
#define T_270       L"\x20\x20\u2593\u2593\u2593\u2593\u2593\u2593\x20\x20\u2593\u2593"

#define S_0_180     L"\x20\x20\u2593\u2593\u2593\u2593\u2593\u2593\u2593\u2593\x20\x20"
#define S_90_270    L"\u2593\u2593\x20\x20\u2593\u2593\u2593\u2593\x20\x20\u2593\u2593"

#define Z_0_180     L"\u2593\u2593\u2593\u2593\x20\x20\x20\x20\u2593\u2593\u2593\u2593"
#define Z_90_270    L"\x20\x20\u2593\u2593\u2593\u2593\u2593\u2593\u2593\u2593\x20\x20"


typedef struct shape
{
    shape_type type;
    degree degree;
    int width;
    int height;
    wchar_t *arr;
} shape;

shape Is[SHAPE_MAX][DEGREE_MAX] = {
    [SHAPE_I][DEGREE_0] = {
        .type = SHAPE_I,
        .degree = DEGREE_0,
        .width = 2,
        .height = 4,
        .arr = IO_ALL,
    },
    [SHAPE_I][DEGREE_90] = {
        .type = SHAPE_I,
        .degree = DEGREE_90,
        .width = 8,
        .height = 1,
        .arr = IO_ALL,
    },
    [SHAPE_I][DEGREE_180] = {
        .type = SHAPE_I,
        .degree = DEGREE_180,
        .width = 2,
        .height = 4,
        .arr = IO_ALL,
    },
    [SHAPE_I][DEGREE_270] = {
        .type = SHAPE_I,
        .degree = DEGREE_270,
        .width = 8,
        .height = 1,
        .arr = IO_ALL,
    },
    [SHAPE_O][DEGREE_0] = {
        .type = SHAPE_O,
        .degree = SHAPE_O,
        .width = 4,
        .height = 2,
        .arr = IO_ALL,
    },
    [SHAPE_O][DEGREE_90] = {
        .type = SHAPE_O,
        .degree = DEGREE_90,
        .width = 4,
        .height = 2,
        .arr = IO_ALL,
    },
    [SHAPE_O][DEGREE_180] = {
        .type = SHAPE_O,
        .degree = DEGREE_180,
        .width = 4,
        .height = 2,
        .arr = IO_ALL,
    },
    [SHAPE_O][DEGREE_270] = {
        .type = SHAPE_O,
        .degree = DEGREE_270,
        .width = 4,
        .height = 2,
        .arr = IO_ALL,
    },
    [SHAPE_L][DEGREE_0] = {
        .type = SHAPE_L,
        .degree = DEGREE_0,
        .width = 4,
        .height = 3,
        .arr = L_0,
    },
    [SHAPE_L][DEGREE_90] = {
        .type = SHAPE_L,
        .degree = DEGREE_90,
        .width = 6,
        .height = 2,
        .arr = L_90,
    },
    [SHAPE_L][DEGREE_180] = {
        .type = SHAPE_L,
        .degree = DEGREE_180,
        .width = 4,
        .height = 3,
        .arr = L_180,
    },
    [SHAPE_L][DEGREE_270] = {
        .type = SHAPE_L,
        .degree = DEGREE_270,
        .width = 6,
        .height = 2,
        .arr = L_270,
    },
    [SHAPE_J][DEGREE_0] = {
        .type = SHAPE_J,
        .degree = DEGREE_0,
        .width = 6,
        .height = 2,
        .arr = J_0,
    },
    [SHAPE_J][DEGREE_90] = {
        .type = SHAPE_J,
        .degree = DEGREE_90,
        .width = 4,
        .height = 3,
        .arr = J_90,
    },
    [SHAPE_J][DEGREE_180] = {
        .type = SHAPE_J,
        .degree = DEGREE_180,
        .width = 6,
        .height = 2,
        .arr = J_180,
    },
    [SHAPE_J][DEGREE_270] = {
        .type = SHAPE_J,
        .degree = DEGREE_270,
        .width = 4,
        .height = 3,
        .arr = J_270,
    },
    [SHAPE_T][DEGREE_0] = {
        .type = SHAPE_T,
        .degree = DEGREE_0,
        .width = 6,
        .height = 2,
        .arr = T_0,
    },
    [SHAPE_T][DEGREE_90] = {
        .type = SHAPE_T,
        .degree = DEGREE_90,
        .width = 4,
        .height = 3,
        .arr = T_90,
    },
    [SHAPE_T][DEGREE_180] = {
        .type = SHAPE_T,
        .degree = DEGREE_180,
        .width = 6,
        .height = 2,
        .arr = T_180,
    },
    [SHAPE_T][DEGREE_270] = {
        .type = SHAPE_T,
        .degree = DEGREE_270,
        .width = 4,
        .height = 3,
        .arr = T_270,
    },
    [SHAPE_S][DEGREE_0] = {
        .type = SHAPE_T,
        .degree = DEGREE_0,
        .width = 6,
        .height = 2,
        .arr = S_0_180,
    },
    [SHAPE_S][DEGREE_90] = {
        .type = SHAPE_T,
        .degree = DEGREE_90,
        .width = 4,
        .height = 3,
        .arr = S_90_270,
    },
    [SHAPE_S][DEGREE_180] = {
        .type = SHAPE_T,
        .degree = DEGREE_180,
        .width = 6,
        .height = 2,
        .arr = S_0_180,
    },
    [SHAPE_S][DEGREE_270] = {
        .type = SHAPE_T,
        .degree = DEGREE_270,
        .width = 4,
        .height = 3,
        .arr = S_90_270,
    },
    [SHAPE_Z][DEGREE_0] = {
        .type = SHAPE_T,
        .degree = DEGREE_0,
        .width = 6,
        .height = 2,
        .arr = Z_0_180,
    },
    [SHAPE_Z][DEGREE_90] = {
        .type = SHAPE_T,
        .degree = DEGREE_90,
        .width = 4,
        .height = 3,
        .arr = Z_90_270,
    },
    [SHAPE_Z][DEGREE_180] = {
        .type = SHAPE_T,
        .degree = DEGREE_180,
        .width = 6,
        .height = 2,
        .arr = Z_0_180,
    },
    [SHAPE_Z][DEGREE_270] = {
        .type = SHAPE_T,
        .degree = DEGREE_270,
        .width = 4,
        .height = 3,
        .arr = Z_90_270,
    },
};

shape *get_shape(shape_type type, degree degree)
{
    return &Is[type][degree];
}

void paint_shape(wchar_t *graph, point p, shape *s)
{
    int i, j;
    int base = (p.line + 1) * LINE_SIZE + p.row;

    for (i = 0; i < s->height; i++)
        for(j = 0; j < s->width; j++)
            if (s->arr[i * s->width + j] == BRICK)
                graph[base + i * LINE_SIZE + j] = BRICK;
}

void paint_text(wchar_t *graph, wchar_t *text, point p)
{
    int base;
    int i = 0;

    base = (p.line + 1) * LINE_SIZE + p.row;
    for (i = 0; i < wcslen(text); i++)
        graph[base+i] = text[i];
}

void refresh(wchar_t *graph)
{
    clear_line(1, 30);
    wprintf(L"%ls", graph);
}

point get_next_center(void)
{
    return (point){.line = 11, .row = 29};
}

void update_next(void)
{
    shape *s;
    point p_next;
    static area a;

    p_next = get_next_center();
    reset_area(base_graph, a);

    s = get_shape(rand()&(SHAPE_MAX- 1), rand()&(DEGREE_MAX - 1));
    //s = get_shape(SHAPE_I, rand()&(DEGREE_MAX - 1));
    p_next.line -= (s->height>>1);
    p_next.row -= (s->width>>1);

    a.top_left = p_next;
    a.bottom_right.line = p_next.line + s->height;
    a.bottom_right.row = p_next.row + s->width;
    paint_shape(base_graph, p_next, s);
}

void alarm_handler(int signum)
{
    update_next();
    refresh(base_graph);
    alarm(1);
}

int main(int argc, char *argv[])
{
    area a;
    point p_txt_score = {1, 26};
    point p_txt_next = {6,27};

    setlocale(LC_ALL, "");
    clear_line(1, 30);
    paint_text(base_graph, L"SCORE:", p_txt_score);
    paint_text(base_graph, L"NEXT", p_txt_next);

    signal(SIGALRM, alarm_handler);
    alarm(1);

    while (1) {
        pause();  // 等待信号
    }
 
    return 0;
}