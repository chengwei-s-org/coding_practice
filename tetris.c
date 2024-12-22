
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

#define BOTTOM_X -1
#define TOP_X -2
#define SIDE_X -3
#define SUCCESS 1

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

struct Point
{
    int posx;
    int posy;
} ;
struct Point point_tetris_tl = {0, 0};
struct Point point_tetris_br = {TETRISW, TETRISH};

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

struct termios original_tty; //全局变量用于保存原始的termios设置
int tetris_area[TETRISH][TETRISW] = {0};
int timer_flag = 0;

void move_cursor(int row, int col);
void print_score(int score);
void print_tetris_area(struct Point *top_left, struct Point *bottom_right);
void init_site(void);
void clear_line(int s, int e);
void clear_area(int startrow, int endrow, int startcolumn, int endcolumn);
void unfill_shape(struct Blocks *block, struct Point *point);
void fill_shape(struct Blocks *block, struct Point *point);
int check_collision(struct Blocks *block, struct Point *point);
void set_raw_mode(int fd);
void set_nonblocking(int fd);
void restore_original_mode(int fd);
void timer_handler(int signum, siginfo_t *si, void *uc);
void handle_key(char *buffer, int len);


int main() 
{

    int ishape = rand() % SHAPE_NUM;
    int irotate = rand() % 4;
    int ishape_next = 0;
    int irotate_next = 0;
    int irotate_old = 0;
    int score = 0;
    int collision = 0;
    int end = 0;
    int refresh = 0;
    struct Point point = {0, 0};
    struct Point point_old = {0, 0};
    struct Point point_next = {NEXT_SHAPE_POSX, NEXT_SHAPE_POSY};

    char ch;
    struct sigaction sa;
    struct sigevent sev;
    struct itimerspec its;
    struct timeval timeout;
    timer_t timerid;
    fd_set readfds;
    char buffer[10]; // 缓冲区大小需要足够大以容纳方向键的序列（通常是3个字节）
    int len;
    int stdin_fd = fileno(stdin);

 
    // 设置信号处理程序
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
 
    // 创建定时器
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }
 
    struct termios tty;
    if (tcgetattr(stdin_fd, &tty) != 0) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    original_tty = tty; // 保存原始的termios设置
 
    // 设置定时器为每秒触发一次
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 1;
    its.it_interval.tv_nsec = 0;
    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&readfds);
    FD_SET(stdin_fd, &readfds);
    timeout.tv_sec = 1; // 设置select的超时时间为1秒，与定时器间隔相匹配
    timeout.tv_usec = 0;

    setlocale(LC_ALL, "");
    clear_line(1, SITE_H+2);
    init_site();
    print_tetris_area(&point_tetris_tl, &point_tetris_br);
    print_score(score);


    while (1)
    {
        point_old = point;
        irotate_old = irotate;
        // 设置终端为非阻塞和原始模式
        // set_nonblocking(stdin_fd);
        set_raw_mode(stdin_fd);
        if ((len = read(stdin_fd, buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[len] = '\0';
            // 处理输入
            if (len == 3 && buffer[0] == 27 && buffer[1] == 91) //Esc,[
            {
                switch (buffer[2])
                {
                case 65: // Arrow Up:A
                    irotate = (irotate+1)%4;
                    break;
                case 66: // Arrow Down:B
                    point.posy++;
                    break;
                case 68: // Arrow Left:D
                    point.posx = point.posx-2;
                    break;
                case 67: // Arrow Right:C
                    point.posx = point.posx+2;
                    break;
                default:
                    break;
                }
                unfill_shape(&blocks[ishape][irotate_old], &point_old);
                collision = check_collision(&blocks[ishape][irotate], &point);
                if (collision == SUCCESS)
                {
                    fill_shape(&blocks[ishape][irotate], &point);
                    point_old = point;
                    irotate_old = irotate;
                    refresh = 1;
                }
                else
                {
                    fill_shape(&blocks[ishape][irotate_old], &point_old);
                }
            }
            else if (len == 1 && buffer[0] == 27) //Esc
            {
                restore_original_mode(stdin_fd);
                break;
            }
        }
        restore_original_mode(stdin_fd);

        if (timer_flag == 1)
        {
            if (collision == BOTTOM_X || collision == 0)
            {
                // Print next shape
                unfill_shape(&blocks[ishape_next][irotate_next], &point_next);
                ishape_next = rand() % SHAPE_NUM;
                irotate_next = rand() % 4;
                fill_shape(&blocks[ishape_next][irotate_next], &point_next);
                ishape = ishape_next;
                irotate = irotate_next;

                point.posy = 2-blocks[ishape][irotate].row;
                point.posx = SITE_W / 2 - SHAPE_W / 2;
                collision = check_collision(&blocks[ishape][irotate], &point);
                if (collision == SUCCESS)
                {
                    fill_shape(&blocks[ishape][irotate], &point);
                    refresh = 1;
                    point_old = point;
                    irotate_old = irotate;
                }
                else if (collision == BOTTOM_X)
                {
                    break;
                }
            }
            else
            {
                unfill_shape(&blocks[ishape][irotate_old], &point_old);
                point.posy++;
                collision = check_collision(&blocks[ishape][irotate], &point);
                if (collision == SUCCESS)
                {
                    unfill_shape(&blocks[ishape][irotate], &point_old);
                    fill_shape(&blocks[ishape][irotate], &point);
                    point_old = point;
                    irotate_old = irotate;
                    refresh = 1;
                }
                else
                {
                    fill_shape(&blocks[ishape][irotate_old], &point_old);
                }
                // int retval = select(stdin_fd, &readfds, NULL, NULL, &timeout);
                // if (retval == -1)
                // {
                //     perror("select");
                //     break;
                // }
                // else if (retval == 0)
                // {
                //     // 超时，没有数据可读，可能是定时器触发了
                //     // 这里不需要做特别处理，因为定时器信号处理程序会处理定时器事件
                // }
                // else
                // {
                //     // 有数据可读，检查键盘输入
                //     char c;
                //     if ((len = read(stdin_fd, buffer, sizeof(buffer) - 1)) > 0)
                //     {
                //         // 确保缓冲区以null字符结尾，以便安全地打印字符串
                //         buffer[len] = '\0';
                //         // 处理输入
                //         handle_key(buffer, len);
                //     }
                //     if (len == -1)
                //     {
                //         perror("read");
                //     }
                // }
            }
            timer_flag = 0;
        }
        if (refresh == 1)
        {
            print_tetris_area(&point_tetris_tl, &point_tetris_br);
            refresh = 0;
        }
    }
    print_tetris_area(&point_tetris_tl, &point_tetris_br);
    getchar(); //End pause
    // 删除定时器（实际上在程序退出前不会执行到这一步）
    timer_delete(timerid);
    // 恢复终端设置
    // atexit(restore_original_mode);
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
        tetris_area[TETRISH-1][i] = 1;
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

void load_site(int load[TETRISH][TETRISW] , int temp[TETRISH][TETRISW]){
    int i = 0;
    int j = 0;
    for (i = 0; i < TETRISH; i++)
    {
        for (j = 0; i < TETRISW; j++)
        {
            load[i][j] = temp[i][j];
        }
    }
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
    move_cursor(top_left->posx+1, top_left->posy+1);
    for (i = top_left->posy; i < bottom_right->posy; i++) //y
    {
        for (j = top_left->posx; j < bottom_right->posx; j++) //x
        {
            if (i > 0 && i < SITE_H / 2 - 1 && j >= SITE_BOUNDARY && j < TETRISW - 2)
            {
                if (j = TETRISW - 3)
                {
                    move_cursor(i+1, TETRISW - 1);
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

void unfill_shape(struct Blocks *block, struct Point *point) 
{
    int i = 0;
    int j = 0;
    for (i = 0; i < SHAPE_W; i++)
    {
        for (j = 0; j < SHAPE_W; j++)
        {
            if (block->shapes[i][j] == 1 && point->posy+i != 0 && point->posy+i != TETRISH-1)
            {
                tetris_area[point->posy+i][point->posx+2*j] = 0;
                tetris_area[point->posy+i][point->posx+2*j+1] = 0;
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
                //val = block->shapes[i][j] || tetris_area[point->posy + i][point->posx + 2 * j];
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

    for (i = SHAPE_W-1; i >= 0; i--)
    {
        for (j = 0; j < SHAPE_W; j++)
        {
            if (block->shapes[i][j] && tetris_area[point->posy + i][point->posx + 2 * j] == 1)
            {
                if (point->posy + i != 0)
                {
                    return BOTTOM_X;
                }
                if (point->posx + 2 * j <= 1 || point->posx + 2 * j >= SITE_BOUNDARY - 2)
                {
                    return SIDE_X;
                }
            }
        }
    }
    return SUCCESS;
}

// 定时器信号处理程序
void timer_handler(int signum, siginfo_t *si, void *uc)
{
    timer_flag = 1;
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
    if (tcgetattr(fd, &tty) != 0)
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
    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

// 恢复原始终端设置的回调函数
void restore_original_mode(int fd)
{
    int stdin_fd = fileno(stdin);
    if (tcsetattr(stdin_fd, TCSANOW, &original_tty) != 0)
    {
        perror("tcsetattr (restore)");
    }
}

// 处理方向键输入的函数
void handle_key(char *buffer, int len)
{
    if (len == 3 && buffer[0] == 0x00 && buffer[1] == 0x00)
    {
        switch (buffer[2])
        {
        case 0x48: // Arrow Up
            
            break;
        case 0x50: // Arrow Down
            
            break;
        case 0x4B: // Arrow Left
            
            break;
        case 0x4D: // Arrow Right
            
            break;
        default:
            
            break;
        }
    }
    else
    {
        // 对于非方向键的输入，这里只是简单地打印出来
        // 注意：在原始模式下，输入会立即发送到程序，包括特殊字符和控制字符
        for (int i = 0; i < len; i++)
        {
            // printf("Key pressed: %c (ASCII: %d)\n", buffer[i] >= 32 && buffer[i] <= 126 ? buffer[i] : '.', buffer[i]);
        }
    }
}
