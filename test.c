#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>

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

#define UP 65 //方向键：上
#define DOWN 66 //方向键：下
#define LEFT 68 //方向键：左
#define RIGHT 67 //方向键：右

#define SPACE 32 //空格键
#define ESC 27 //Esc键

#define TIMER_INTERVAL 1  // 定时器间隔为 1 秒

#define SITE_H 10
#define SITE_V 20

struct Block
{
	int space[4][4];
}block[7][4];

int block_index = 0;
int next_block_index = 0;
int block_row_position = 0;
int block_col_position = GAME_AREA_CENTER_COLOUN;
int space_index = 0;
int bottom_position = GAME_AREA_ROW - 1;  //temporary condition and parameter. waiting for further design.


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

// 设置标准输入为非阻塞模式
void set_stdin_nonblocking() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);  // 禁用缓冲和回显
    term.c_cc[VMIN] = 1;  // 最小字符数
    term.c_cc[VTIME] = 0; // 设置超时为 0
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    // 设置为非阻塞模式
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
}

// 恢复终端为标准模式
void reset_terminal_mode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);  // 获取当前终端设置
    term.c_lflag |= (ICANON | ECHO);  // 打开规范模式和回显
    tcsetattr(STDIN_FILENO, TCSANOW, &term);  // 恢复设置
}

// 获取键盘输入（非阻塞）
int kbhit(void) {
    struct timeval tv = {0, 0};  // 设置为 0，不阻塞
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return ret > 0;
}

void command_process(char command_input) {
    switch (command_input)
    {
        case LEFT:
            if (block_col_position > 0)
                block_col_position --;
            break;
        case RIGHT:
            if (block_col_position < SIDE_AREA_COLOUN - 8)
                block_col_position ++;   
            break;
        case DOWN:
            block_row_position = bottom_position;
            break;
        case UP:
            space_index = (space_index + 1) % 4;
        default:
            break;
    }

}

// 信号处理函数：定时器触发时调用
void game_timer_handler(int sig) {

    int row_position_temp = 0;
    int col_position_temp = GAME_AREA_CENTER_COLOUN;
    int space_index_temp = 0;
    int t = 0;
    fd_set readfds;
    struct timeval tv;
    char command_input;
    char arrow_input;
    char direction_input;

    // 每次定时器触发时检查标准输入
    FD_ZERO(&readfds);  // 清空文件描述符集
    FD_SET(STDIN_FILENO, &readfds);  // 将标准输入添加到文件描述符集

    tv.tv_sec = 0;  // 不做等待
    tv.tv_usec = 0;



    if (block_row_position != 0)
        DrawSpace(block_index, space_index, block_row_position - 1, block_col_position);
    DrawBlock(block_index, space_index, block_row_position, block_col_position);


    // 如果有输入，循环读取所有键盘输入
    while (kbhit()) {
        row_position_temp = block_row_position;
        col_position_temp = block_col_position;
        space_index_temp = space_index;
        command_input = getchar();
        if (command_input == 27) {  // ESC
            arrow_input = getchar();  // 获取 '['
            if (arrow_input == '[') {
                direction_input = getchar();  // 获取方向键的值
                command_process(direction_input);  // 处理方向键
                }
        }
        DrawSpace(block_index, space_index_temp, row_position_temp, col_position_temp);
        DrawBlock(block_index, space_index, block_row_position, block_col_position);
    }


    if (block_row_position >= bottom_position)  //temporary condition and parameter. waiting for further design.
    {
        bottom_position = bottom_position - 2;
        block_index = next_block_index;
        block_row_position = 0;
        block_col_position = GAME_AREA_CENTER_COLOUN;
        space_index = 0;
        next_block_index = rand() % 7;
        clear_next_area();
        show_next_area(next_block_index);
    }

    block_row_position ++;

    // 定时器会继续触发，所以不需要显式的循环
}


int main() {

    struct itimerval game_timer;

    // 配置定时器，每秒触发一次
    signal(SIGALRM, game_timer_handler);  // 捕获 SIGALRM 信号并调用 timer_handler

    game_timer.it_value.tv_sec = TIMER_INTERVAL;  // 定时器触发的初始时间为 1 秒
    game_timer.it_value.tv_usec = 0;
    game_timer.it_interval.tv_sec = TIMER_INTERVAL;  // 设置定时器每秒触发一次
    game_timer.it_interval.tv_usec = 0;
    // 设置定时器
    if (setitimer(ITIMER_REAL, &game_timer, NULL) == -1) {
        perror("setitimer");
        exit(EXIT_FAILURE);
    }

   // 隐藏光标
    // printf("\033[?25l");
    // fflush(stdout);  // 刷新输出，确保转义序列及时发送到终端


    InitBlockInfo();
    setbuf(stdout,NULL);  //print in real time
    setlocale(LC_ALL, "");
    clear_line(1, 30);
    print_site();
    srand(time(NULL));

    set_stdin_nonblocking();

    block_index = rand() % 7;
    next_block_index = rand() % 7;
    show_next_area(next_block_index);
    // 程序没有使用 while(1) 循环，直接返回并等待信号触发
    while (1) {
        // 这里只是为了保证程序持续运行，等待信号
        pause();  // 使程序进入挂起状态，直到接收到信号（如 SIGALRM）
    }

    reset_terminal_mode();

    return 0;
}

