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

#define UP 65 //direction: up
#define DOWN 66 //direction: dowm
#define LEFT 68 //direction: left
#define RIGHT 67 //direction: right

#define SPACE 32 //sqace
#define ESC 27 //Esc(also incleded in direction input)

#define TIMER_INTERVAL 1  // set falling down time 1s. Need to find a way to modify.

#define SITE_H 10
#define SITE_V 20

struct Block
{
	int shape_to_space[4][4];
}block_to_shape[7][4];

int block_index = 0;
int next_block_index = 0;
int block_row_position = 1;
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
			if (block_to_shape[shape][form].shape_to_space[i][j] == 1) //if need to clean block_to_shape of a position, move cursor to it.
			{
				move_cursor(x + i, 2 * (y + j) - 1);
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
			if (block_to_shape[shape][form].shape_to_space[i][j] == 1) //if need to draw block_to_shape of a position, move cursor to it.
			{
				move_cursor(x + i, 2 * (y + j) - 1);
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
            dest->shape_to_space[j][3 - i] = src->shape_to_space[i][j];
        }
    }
}

void InitBlockInfo()
{
	//“T” type
	for (int i = 0; i <= 2; i++)
		block_to_shape[0][0].shape_to_space[1][i] = 1;
	block_to_shape[0][0].shape_to_space[2][1] = 1;

	//“L” type
	for (int i = 1; i <= 3; i++)
		block_to_shape[1][0].shape_to_space[i][1] = 1;
	block_to_shape[1][0].shape_to_space[3][2] = 1;

	//“J” type
	for (int i = 1; i <= 3; i++)
		block_to_shape[2][0].shape_to_space[i][2] = 1;
	block_to_shape[2][0].shape_to_space[3][1] = 1;

	for (int i = 0; i <= 1; i++)
	{
		//“Z” type
		block_to_shape[3][0].shape_to_space[1][i] = 1;
		block_to_shape[3][0].shape_to_space[2][i + 1] = 1;
		//“S” type
		block_to_shape[4][0].shape_to_space[1][i + 1] = 1;
		block_to_shape[4][0].shape_to_space[2][i] = 1;
		//“O” type
		block_to_shape[5][0].shape_to_space[1][i + 1] = 1;
		block_to_shape[5][0].shape_to_space[2][i + 1] = 1;
	}

	//“I” type
	for (int i = 0; i <= 3;i++)
		block_to_shape[6][0].shape_to_space[i][1] = 1;

	int temp[4][4];
	for (int shape = 0; shape < 7; shape++) //7 types
	{
		for (int form = 0; form < 3; form++) //4 shapes
		{
			Get_next_form(&block_to_shape[shape][form], &block_to_shape[shape][form+1]);
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


void set_stdin_nonblocking()  // set stdin as nonblocking to ensure input command not print on terminal
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);  // close buffer and echo
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);   // set to non-blocking mode
}


void reset_terminal_mode() {  // recover terminal to std mode
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);  // open buffer and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}


int check_keyboard_hit(void) // get keyboard input（non-blocking）
{
    struct timeval keyboard_timeval = {0, 0};  // set as 0, non-blocking
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, &keyboard_timeval);
    return ret > 0;
}

void command_process(char command_input)  // process keyboard input command(direction part)
{ 
    switch (command_input)
    {
        case LEFT:  // move block left 1 position
            if (block_col_position > 2)
                block_col_position --;
            break;
        case RIGHT:  // move block right 1 position
            if (block_col_position < SIDE_AREA_COLOUN - 7)
                block_col_position ++;   
            break;
        case DOWN:  // move block to bottom
            block_row_position = bottom_position;
            break;
        case UP:  // spin block to next shape
            space_index = (space_index + 1) % 4;
        default:
            break;
    }

}


void game_timer_handler(int sig)  //timer handler function, process when game timer trigger 
{
    int row_position_temp = 0;
    int col_position_temp = GAME_AREA_CENTER_COLOUN;
    int space_index_temp = 0;
    int t = 0;
    fd_set readfds;
    struct timeval game_timer_handler_timeval;
    char command_input;
    char arrow_input;
    char direction_input;

    // check stdin in every processing
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    game_timer_handler_timeval.tv_sec = 0;  // check stdin timer no waiting
    game_timer_handler_timeval.tv_usec = 0;



    if (block_row_position != 0)
        DrawSpace(block_index, space_index, block_row_position - 1, block_col_position);
    DrawBlock(block_index, space_index, block_row_position, block_col_position);



    while (check_keyboard_hit())     // if input, read all inputs
    {
        row_position_temp = block_row_position;
        col_position_temp = block_col_position;
        space_index_temp = space_index;
        command_input = getchar();
        if (command_input == 27) {  // get ESC
            arrow_input = getchar();  // get '['
            if (arrow_input == '[') {
                direction_input = getchar();  // get position value
                command_process(direction_input);  // process position value
                }
        }
        DrawSpace(block_index, space_index_temp, row_position_temp, col_position_temp);
        DrawBlock(block_index, space_index, block_row_position, block_col_position);
    }


    if (block_row_position >= bottom_position)  //temporary condition and parameter. waiting for further design.
    {
        bottom_position = bottom_position - 2;
        block_index = next_block_index;
        block_row_position = 1;
        block_col_position = GAME_AREA_CENTER_COLOUN;
        space_index = 0;
        next_block_index = rand() % 7;
        clear_next_area();
        show_next_area(next_block_index);
    }

    block_row_position ++;
}


int main() {

    struct itimerval game_timer;

    // define timer
    signal(SIGALRM, game_timer_handler);

    game_timer.it_value.tv_sec = TIMER_INTERVAL;  // set timer initial time 1 second
    game_timer.it_value.tv_usec = 0;
    game_timer.it_interval.tv_sec = TIMER_INTERVAL;  // set timer trigger each 1 second
    game_timer.it_interval.tv_usec = 0;
    // set timer as parameter above
    if (setitimer(ITIMER_REAL, &game_timer, NULL) == -1) {
        perror("setitimer");
        exit(EXIT_FAILURE);
    }

    // hide cursor, now still exist problem
    // printf("\033[?25l");
    // fflush(stdout);


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
    while (1) {
        pause();  // Puts the program into a pending state until a signal is received
    }

    reset_terminal_mode();

    return 0;
}

