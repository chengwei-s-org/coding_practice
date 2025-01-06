#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <sys/time.h>
#include <sys/select.h>
#include <signal.h>

// 定义按键的宏
#define KEY_ESC 27
#define KEY_CLOSE_BRACKET 91

// 方块相关的宏定义
#define BRICK   0x2593
#define FRAME   0x2588

#define SITE_W 20
#define SITE_H 20
#define SHOW_W 19
#define FRAME_W 2
#define SHAPE_W 4
#define SITE_BOUNDARY FRAME_W + SITE_W + FRAME_W
#define TETRISW SITE_BOUNDARY + SHOW_W + FRAME_W
#define TETRISH SITE_H + 2

#define NEXT_SHAPE_POSX SITE_BOUNDARY + SHOW_W / 2 - SHAPE_W + 1
#define NEXT_SHAPE_POSY SITE_H / 2 + SITE_H / 4 - 1

#define BOTTOM_X -4
#define TOP_X -2
#define SIDE_X -3
#define SUCCESS 1

// 方块形状的枚举
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

// 坐标点结构体
struct Point
{
    int posx;
    int posy;
};

struct Point point_tetris_tl = {0, 0};
struct Point point_tetris_br = {TETRISW, TETRISH};

// 用于表示俄罗斯方块的结构体
struct Blocks
{
    int shapes[SHAPE_W][SHAPE_W];
    int row;
    int col;
} blocks[SHAPE_NUM][4] =
    {
        {
            {
                // I
                {0, 0, 0, 0,
                1, 1, 1, 1,
                0, 0, 0, 0,
                0, 0, 0, 0}, 1, 4,
            },
            {
                {0, 1, 0, 0,
                0, 1, 0, 0,
                0, 1, 0, 0,
                0, 1, 0, 0},4,1,
            },
            {
                {0, 0, 0, 0,
                1, 1, 1, 1,
                0, 0, 0, 0,
                0, 0, 0, 0},1,4,
            },
            {
                {0, 1, 0, 0,
                0, 1, 0, 0,
                0, 1, 0, 0,
                0, 1, 0, 0},4,1,
            },
        },
        {
            {
                // O
                {0, 1, 1, 0,
                0, 1, 1, 0,
                0, 0, 0, 0,
                0, 0, 0, 0},2,2,
            },
            {
                {0, 1, 1, 0,
                0, 1, 1, 0,
                0, 0, 0, 0,
                0, 0, 0, 0},2,2,
            },
            {
                {0, 1, 1, 0,
                0, 1, 1, 0,
                0, 0, 0, 0,
                0, 0, 0, 0},2,2,
            },
            {
                {0, 1, 1, 0,
                0, 1, 1, 0,
                0, 0, 0, 0,
                0, 0, 0, 0},2,2,
            },
        },
        {
            {
                // T
                {0, 1, 0, 0,
                1, 1, 1, 0,
                0, 0, 0, 0,
                0, 0, 0, 0},2,3,
            },
            {
                {0, 1, 0, 0,
                1, 1, 0, 0,
                0, 1, 0, 0,
                0, 0, 0, 0},3,2,
            },
            {
                {0, 0, 0, 0,
                1, 1, 1, 0,
                0, 1, 0, 0,
                0, 0, 0, 0},2,3,
            },
            {
                {0, 1, 0, 0,
                0, 1, 1, 0,
                0, 1, 0, 0,
                0, 0, 0, 0},3,2,
            },
        },
        {
            {
                // L
                {0, 0, 1, 0,
                1, 1, 1, 0,
                0, 0, 0, 0,
                0, 0, 0, 0},2,3,
            },
            {
                {1, 1, 0, 0,
                0, 1, 0, 0,
                0, 1, 0, 0,
                0, 0, 0, 0},3,2,
            },
            {
                {1, 1, 1, 0,
                1, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0},2,3,
            },
            {
                {1, 0, 0, 0,
                1, 0, 0, 0,
                1, 1, 0, 0,
                0, 0, 0, 0},3,2,
            },
        },
        {
            {
                // J
                {1, 0, 0, 0,
                1, 1, 1, 0,
                0, 0, 0, 0,
                0, 0, 0, 0},2,3,
            },
            {
                {0, 1, 0, 0,
                0, 1, 0, 0,
                1, 1, 0, 0,
                0, 0, 0, 0},3,2,
            },
            {
                {1, 1, 1, 0,
                0, 0, 1, 0,
                0, 0, 0, 0,
                0, 0, 0, 0},2,3,
            },
            {
                {1, 1, 0, 0,
                1, 0, 0, 0,
                1, 0, 0, 0,
                0, 0, 0, 0},3,2,
            },
        },
        {
            {
                // Z
                {1, 1, 0, 0,
                0, 1, 1, 0,
                0, 0, 0, 0,
                0, 0, 0, 0},2,3,
            },
            {
                {0, 1, 0, 0,
                1, 1, 0, 0,
                1, 0, 0, 0,
                0, 0, 0, 0},3,2,
            },
            {
                {1, 1, 0, 0,
                0, 1, 1, 0,
                0, 0, 0, 0,
                0, 0, 0, 0},2,3,
            },
            {
                {0, 1, 0, 0,
                1, 1, 0, 0,
                1, 0, 0, 0,
                0, 0, 0, 0},3,2,
            },
        },
        {
            {
                // S
                {0, 1, 1, 0,
                1, 1, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0},2,3,
            },
            {
                {1, 0, 0, 0,
                1, 1, 0, 0,
                0, 1, 0, 0,
                0, 0, 0, 0},3,2,
            },
            {
                {0, 1, 1, 0,
                1, 1, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0},2,3,
            },
            {
                {1, 0, 0, 0,
                1, 1, 0, 0,
                0, 1, 0, 0,
                0, 0, 0, 0},3,2,
            }
        }
    };

// 将形状和旋转角度相关变量封装的结构体
struct Iblock
{
    int shape;
    int rotate;
};

struct termios original_tty; // 全局变量用于保存原始的termios设置
int tetris_area[TETRISH][TETRISW] = {0};
struct Update_block
{
    struct Iblock iblock;
    struct Point point;
    struct Iblock iblock_old;
    struct Point point_old;
    struct Iblock next_iblock;
} update_b;
int score = 0;
int collision = -1;
int refresh_flag = 0;
int end_flag = 0;
int flicker_ready = 0;

// 函数声明
void move_cursor(int row, int col);
void print_score(int score);
void print_tetris_area(struct Point *top_left, struct Point *bottom_right);
void init_site(void);
void clear_line(int s, int e);
void clear_area(int start_row, int end_row, int start_column, int end_column);
void fill_area(int start_row, int end_row, int start_column, int end_column);
void unfill_shape(struct Blocks *block, struct Point *point);
void fill_shape(struct Blocks *block, struct Point *point);
int check_collision(struct Blocks *block, struct Point *point);
void set_raw_mode(int fd);
void set_nonblocking(int fd);
void restore_original_mode(int fd);
void handle_keyboard_input(char *buffer, int len);
void setup_timer(timer_t timerid);
void check_and_clear_lines(int* res);
void relpace_block(int i);
void flicker_line(int* res);
int getscore();
void setscore(int n);
int getcollision();
void setcollision(int n);
int getrefresh_flag();
void setrefresh_flag(int n);
int getend_flag();
void setend_flag(int n);
int getflicker_ready();
void setflicker_ready(int n);

int main()
{
    update_b.next_iblock.shape = rand() % SHAPE_NUM;
    update_b.next_iblock.rotate = rand() % 4;

    char ch;
    char buffer[10]; // 缓冲区大小需要足够大以容纳方向键的序列（通常是3个字节）
    int len;

    timer_t timerid;

    struct termios tty;

    fd_set readfds;
    int stdin_fd = fileno(stdin);

    setup_timer(timerid);

    if (tcgetattr(stdin_fd, &tty) != 0)
    {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    original_tty = tty; // 保存原始的termios设置

    FD_ZERO(&readfds);
    FD_SET(stdin_fd, &readfds);
    struct timeval timeout;
    timeout.tv_sec = 1; // 设置select的超时时间为1秒，与定时器间隔相匹配
    timeout.tv_usec = 0;

    setlocale(LC_ALL, "");
    clear_line(1, SITE_H + 2);
    init_site();
    print_tetris_area(&point_tetris_tl, &point_tetris_br);
    print_score(getscore());

    while (1)
    {
        if (getend_flag()==1)
        {
            break;
        }
        // 设置终端为非阻塞和原始模式
        set_raw_mode(stdin_fd);
        if ((len = read(stdin_fd, buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[len] = '\0';
            handle_keyboard_input(buffer, len);
        }
        restore_original_mode(stdin_fd);

        if (getrefresh_flag() == 1)
        {
            print_tetris_area(&point_tetris_tl, &point_tetris_br);
            setrefresh_flag(0);
        }
    }
    print_tetris_area(&point_tetris_tl, &point_tetris_br);
    // 删除定时器
    timer_delete(timerid);
    return 0;
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
        tetris_area[0][i] = 1;
        tetris_area[TETRISH - 1][i] = 1;
    }
    for (i = 0; i < TETRISH; i++)
    {
        tetris_area[i][0] = 1;
        tetris_area[i][1] = 1;
        tetris_area[i][SITE_BOUNDARY - 2] = 1;
        tetris_area[i][SITE_BOUNDARY - 1] = 1;
        tetris_area[i][TETRISW - 2] = 1;
        tetris_area[i][TETRISW - 1] = 1;
    }

    for (i = 0; i < SHOW_W; i++)
        tetris_area[SITE_H / 2 - 1][SITE_BOUNDARY + i] = 1;
}

void print_score(int score)
{
    move_cursor(SITE_H / 4, SITE_BOUNDARY + SHOW_W / 2 - 1);
    wprintf(L"score\n");

    move_cursor(SITE_H / 4 + 1, SITE_BOUNDARY + SHOW_W / 2 - 1);
    wprintf(L"%d\n", score);
    wprintf(L"\033[%d;%dH\n", SITE_H + 3, 1);
}

void print_tetris_area(struct Point *top_left, struct Point *bottom_right)
{
    int i = 0;
    int j = 0;
    move_cursor(top_left->posx + 1, top_left->posy + 1);
    for (i = top_left->posy; i < bottom_right->posy; i++) // y
    {
        for (j = top_left->posx; j < bottom_right->posx; j++) // x
        {
            if (i > 0 && i < SITE_H / 2 - 1 && j >= SITE_BOUNDARY && j < TETRISW - 2)
            {
                if (j == TETRISW - 3)
                {
                    move_cursor(i + 1, TETRISW - 1);
                }
            }
            else
            {
                if (tetris_area[i][j] == 1)
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

void clear_area(int start_row, int end_row, int start_column, int end_column)
{
    int i, j;
    for (i = start_row; i <= end_row; i++)
    {
        for (j = start_column; j <= end_column; j++)
        {
            wprintf(L"\033[%d;%dH ", i, j);
        }
    }
    wprintf(L"\033[%d;%dH\n", SITE_H + 3, 1);
}

void fill_area(int start_row, int end_row, int start_column, int end_column)
{
    int i, j;
    for (i = start_row; i <= end_row; i++)
    {
        for (j = start_column; j <= end_column; j++)
        {
            wprintf(L"\033[31m\033[%d;%dH%lc", i, j,BRICK);
        }
    }
    wprintf(L"\033[30m\033[%d;%dH\n", SITE_H + 3, 1);
}

void unfill_shape(struct Blocks *block, struct Point *point)
{
    int i = 0;
    int j = 0;
    for (i = 0; i < SHAPE_W; i++)
    {
        for (j = 0; j < SHAPE_W; j++)
        {
            if (block->shapes[i][j] == 1 && point->posy + i!= 0 && point->posy + i!= TETRISH - 1)
            {
                tetris_area[point->posy + i][point->posx + 2 * j] = 0;
                tetris_area[point->posy + i][point->posx + 2 * j + 1] = 0;
            }
        }
    }
}

void fill_shape(struct Blocks *block, struct Point *point)
{
    int i = 0;
    int j = 0;
    int mark[SHAPE_W][SHAPE_W] = {0};
    for (i = SHAPE_W - 1; i >= 0; i--)
    {
        for (j = 0; j < SHAPE_W; j++)
        {
            if (block->shapes[i][j] == 1)
            {
                tetris_area[point->posy + i][point->posx + 2 * j] = 1;
                tetris_area[point->posy + i][point->posx + 2 * j + 1] = 1;
            }
        }
    }
}

int check_collision(struct Blocks *block, struct Point *point)
{
    int i = 0;
    int j = 0;

    for (i = SHAPE_W - 1; i >= 0; i--)
    {
        for (j = 0; j < SHAPE_W; j++)
        {
            if (block->shapes[i][j] && tetris_area[point->posy + i][point->posx + 2 * j] == 1)
            {
                if (point->posy + i!= 0)
                {
                     return TRUE;
                }
                if (point->posx + 2 * j <= 1 || point->posx + 2 * j >= SITE_BOUNDARY - 2)
                {
                    return SIDE_X;
                }
            }
        }
    }
     return FALSE;
}

// 设置终端为非阻塞模式
void set_nonblocking(int fd)
{
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1 ||
        fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
}

// 设置终端为原始模式（无回显、无行缓冲）
void set_raw_mode(int fd)
{
    struct termios tty;
    if (tcgetattr(fd, &tty)!= 0)
    {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    original_tty = tty; // 保存原始的termios设置
    cfmakeraw(&tty);
    tty.c_lflag &= ~(ICANON | ECHO); // 禁用规范模式和回显
    tty.c_iflag |= ICRNL;
    tty.c_cc[VMIN] = 1;  // 设置最小读取字符数
    tty.c_cc[VTIME] = 0; // 设置读取超时时间为0
    // 设置后重新设置终端属性
    if (tcsetattr(fd, TCSANOW, &tty)!= 0)
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

// 恢复原始终端设置的回调函数
void restore_original_mode(int fd)
{
    int stdin_fd = fileno(stdin);
    if (tcsetattr(stdin_fd, TCSANOW, &original_tty)!= 0)
    {
        perror("tcsetattr (restore)");
    }
}

// 定时器处理函数
void timer_handler(int signum, siginfo_t *si, void *uc)
{
    int collision;
    int res[2] = {0};
    struct Point next_point = {NEXT_SHAPE_POSX, NEXT_SHAPE_POSY};
    if (getflicker_ready() == 1 || getflicker_ready() == 0)
    {
        if (getcollision() == TRUE || getcollision() == -1)
        {
            update_b.iblock.shape = update_b.next_iblock.shape;
            update_b.iblock.rotate = update_b.next_iblock.rotate;
            update_b.point.posy = 2 - blocks[update_b.iblock.shape][update_b.iblock.rotate].row;
            update_b.point.posx = SITE_W / 2 - SHAPE_W / 2;

            // 绘制下一方块
            unfill_shape(&blocks[update_b.next_iblock.shape][update_b.next_iblock.rotate], &next_point);
            update_b.next_iblock.shape = rand() % SHAPE_NUM;
            update_b.next_iblock.rotate = rand() % 4;
            fill_shape(&blocks[update_b.next_iblock.shape][update_b.next_iblock.rotate], &next_point);
            collision = check_collision(&blocks[update_b.iblock.shape][update_b.iblock.rotate], &update_b.point);
            setcollision(collision);
            if (getcollision() == TRUE)
            {
                setend_flag(1); //Game over
            }
        }
        else
        {
            unfill_shape(&blocks[update_b.iblock_old.shape][update_b.iblock_old.rotate], &update_b.point_old);
            update_b.point.posy++;
            collision = check_collision(&blocks[update_b.iblock.shape][update_b.iblock.rotate], &update_b.point);
            setcollision(collision);
            if (getcollision() != FALSE)
            {
                fill_shape(&blocks[update_b.iblock_old.shape][update_b.iblock_old.rotate], &update_b.point_old);
                check_and_clear_lines(res);
            }
        }
        if (getcollision() == FALSE)
        {
            fill_shape(&blocks[update_b.iblock.shape][update_b.iblock.rotate], &update_b.point);
            update_b.point_old.posx = update_b.point.posx;
            update_b.point_old.posy = update_b.point.posy;
            update_b.iblock_old.shape = update_b.iblock.shape;
            update_b.iblock_old.rotate = update_b.iblock.rotate;
        }
    }
    if (res[1] != 0)
    {
        setflicker_ready(-1);
        flicker_line(res);
    }
    setrefresh_flag(1);
}

// 键盘输入处理函数
void handle_keyboard_input(char *buffer, int len)
{
    int collision;
    if (len == 3 && buffer[0] == KEY_ESC && buffer[1] == KEY_CLOSE_BRACKET)
    {
        switch (buffer[2])
        {
        case 65: // Arrow Up:A
            update_b.iblock.rotate = (update_b.iblock_old.rotate + 1) % 4;
            break;
        case 66: // Arrow Down:B
            update_b.point.posy++;
            break;
        case 68: // Arrow Left:D
            update_b.point.posx = update_b.point.posx - 2;
            break;
        case 67: // Arrow Right:C
            update_b.point.posx = update_b.point.posx + 2;
            break;
        default:
            break;
        }
        unfill_shape(&blocks[update_b.iblock_old.shape][update_b.iblock_old.rotate], &update_b.point_old);
        collision = check_collision(&blocks[update_b.iblock.shape][update_b.iblock.rotate], &update_b.point);
        setcollision(collision);
        if (getcollision() == FALSE)
        {
            fill_shape(&blocks[update_b.iblock.shape][update_b.iblock.rotate], &update_b.point);
            update_b.point_old.posx = update_b.point.posx;
            update_b.point_old.posy = update_b.point.posy;
            update_b.iblock_old.shape = update_b.iblock.shape;
            update_b.iblock_old.rotate = update_b.iblock.rotate;
            setrefresh_flag(1);
        }
        else
        {
            fill_shape(&blocks[update_b.iblock_old.shape][update_b.iblock_old.rotate], &update_b.point_old);
        }
    }
    else if (len == 1 && buffer[0] == KEY_ESC)
    {
        restore_original_mode(fileno(stdin));
        setend_flag(1);
    }
}

// 创建定时器、设置定时器、绑定定时器处理函数
void setup_timer(timer_t timerid)
{
    struct sigaction sa;
    struct sigevent sev;
    struct itimerspec its;

    // 设置信号处理程序
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGRTMIN, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // 创建定时器
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
    {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    // 设置定时器为每秒触发一次
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 1;
    its.it_interval.tv_nsec = 0;
    if (timer_settime(timerid, 0, &its, NULL) == -1)
    {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }
}

// 检查并消除满行
void check_and_clear_lines(int* res)
{
    int i, j;
    int score;
    int n = 0;
    int start = 0;
    for (i = 1; i <= SITE_H; i++)
    {
        int is_full = 1;
        for (j = FRAME_W; j < SITE_BOUNDARY - 2; j++)
        {
            if (tetris_area[i][j] == 0)
            {
                is_full = 0;
                break;
            }
        }
        if (is_full)
        {
            if (n==0)
            {
                start = i;
            }
            n++;
            relpace_block(i);
            // 分数增加，消除1行数增加1分
            i--;
        }
    }
    score = getscore() + (n + 1)*n/2;
    setscore(score);
    print_score(score);
    res[0] = start;
    res[1] = n;
}

void relpace_block(int i)
{
    int k, j;
    for (k = i; k > 1; k--)
    {
        for (j = FRAME_W; j < SITE_BOUNDARY - 2; j++)
        {
            tetris_area[k][j] = tetris_area[k - 1][j];
        }
    }
    for (j = FRAME_W; j < SITE_BOUNDARY - 2; j++)
    {
        tetris_area[1][j] = 0;
    }
}

void flicker_line(int* res)
{
    int i;
    int nflicker = 3;
    int start_row, end_row, start_column, end_column;
    start_row = res[0]+1;
    end_row = res[0]+res[1];
    start_column = 3;
    end_column = FRAME_W + SITE_W;
    for (i = nflicker; i > 0; i--)
    {
        clear_area(start_row, end_row, start_column, end_column);
        usleep(200000);
        fill_area(start_row, end_row, start_column, end_column);
        usleep(200000);
    }
    flicker_ready = 1;
}

int getscore()
{
    return score;
}

void setscore(int n)
{
    score = n;
}

int getcollision()
{
    return collision;
}

void setcollision(int n)
{
    collision = n;
}

int getrefresh_flag()
{
    return refresh_flag;
}

void setrefresh_flag(int n)
{
    refresh_flag = n;
}

int getend_flag()
{
    return end_flag;
}

void setend_flag(int n)
{
    end_flag = n;
}

int getflicker_ready()
{
    return flicker_ready;
}

void setflicker_ready(int n)
{
    flicker_ready = n;
}

