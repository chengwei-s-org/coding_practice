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
#define GAME_POS_ROW 23
#define GAME_POS_COLOUN 12
#define SCORE_POS_ROW 4
#define SCORE_POS_COLOUN 13


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
int space_index = 0;
int block_row_position = 1;
int block_col_position = GAME_AREA_CENTER_COLOUN;

int game_score = 0;

int game_area_sign[GAME_POS_ROW][GAME_POS_COLOUN] = {0};

/*basic functions*/
void move_cursor(int row, int col);
void DrawSpace(int shape, int form, int x, int y);
void DrawBlock(int shape, int form, int x, int y);

/*initial functions*/
void get_next_shape_of_current_block(struct Block *src, struct Block *dest);
void InitBlockInfo();
void clear_next_area();
int show_next_area(int shpae_index);
void print_site();
void put_game_area_side_into_sign();
void clear_line(int s, int e);
void set_stdin_nonblocking();
void reset_terminal_mode();

/*game operation functions*/
int check_keyboard_hit();
void command_process(char command_input, int *block_index, int *space_index, int *block_row_position, int *block_col_position);
void save_block_message(int block_index, int space_index, int block_row_position, int block_col_position);
void update_game_score(int score);
void gameover_judgement();
int score_and_gameover_judgement();
void game_timer_handler(int sig);
void command_input_and_process_in_time(int *block_index, int *space_index, int *block_row_position, int *block_col_position);
int legal_judgement(int block_index, int space_index, int block_row_position, int block_col_position);

void move_cursor(int row, int col) 
{
    wprintf(L"\033[%d;%dH", row, col);
}

void DrawSpace(int shape, int form, int x, int y)
{
    int i = 0;
    int j = 0;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (block_to_shape[shape][form].shape_to_space[i][j] == 1) //if need to clean block_to_shape of a position, move cursor to it.
			{
				move_cursor(x + i, 2 * (y + j) + 1);
				wprintf(L"  ");
			}
		}
	}
}

void DrawBlock(int shape, int form, int x, int y)
{
    int i = 0;
    int j = 0;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (block_to_shape[shape][form].shape_to_space[i][j] == 1) //if need to draw block_to_shape of a position, move cursor to it.
			{
				move_cursor(x + i, 2 * (y + j) + 1);
				wprintf(L"%lc%lc", BRICK, BRICK);
			}
		}
	}
}

void get_next_shape_of_current_block(struct Block *src, struct Block *dest)
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
			get_next_shape_of_current_block(&block_to_shape[shape][form], &block_to_shape[shape][form+1]);
		}
	}
}



void clear_next_area()
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

void print_site()  //print all areas
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
    
    put_game_area_side_into_sign();
}

void put_game_area_side_into_sign()
{
    int i = 0;
    for (i = 0; i < GAME_POS_COLOUN; i++)
    {
        game_area_sign[0][i] = 1;
        game_area_sign[GAME_POS_ROW][i] = 1;
    }
    for (i = 1; i < GAME_POS_ROW; i++)
    {
        game_area_sign[i][0] = 1;
        game_area_sign[i][GAME_POS_COLOUN - 1] = 1;
    }
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


void reset_terminal_mode() 
{  // recover terminal to std mode
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);  // open buffer and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}


int check_keyboard_hit() // get keyboard input（non-blocking）
{
    struct timeval keyboard_timeval = {0, 0};  // set as 0, non-blocking
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, &keyboard_timeval);
    return ret > 0;
}

int legal_judgement(int block_index, int space_index, int block_row_position, int block_col_position)
{
    int i = 0;
    int j = 0;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if ((block_to_shape[block_index][space_index].shape_to_space[i][j] == 1) && 
                (game_area_sign[block_row_position + i][block_col_position + j] == 1))
				return 0; //if next position already exist block, illegel.
		}
	}
	return 1;
}

void command_process(char command_input, int *block_index, int *space_index, int *block_row_position, int *block_col_position)  // process keyboard input command(direction part)
{
    int falling_down_index = 0;
    switch (command_input)
    {
        case LEFT:  // move block left 1 position
            if (legal_judgement(*block_index, *space_index, *block_row_position, (*block_col_position) - 1))
                *block_col_position = *block_col_position - 1;                
            break;
        case RIGHT:  // move block right 1 position
            if (legal_judgement(*block_index, *space_index, *block_row_position, (*block_col_position) + 1))
                *block_col_position = *block_col_position + 1;   
            break;
        case DOWN:  // move block to bottom
            while (legal_judgement(*block_index, *space_index, *block_row_position + falling_down_index, *block_col_position))
                falling_down_index ++;
            *block_row_position = *block_row_position + falling_down_index - 1;
            break;
        case UP:  // spin block to next shape
            if (legal_judgement(*block_index, (*space_index + 1) % 4, *block_row_position, *block_col_position + 1))
                *space_index = (*space_index + 1) % 4;
        default:
            break;
    }

}

void save_block_message(int block_index, int space_index, int block_row_position, int block_col_position)
{
    for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (block_to_shape[block_index][space_index].shape_to_space[i][j] == 1)
				game_area_sign[block_row_position + i][block_col_position + j] = 1; //sign this space as occupied
		}
	}
}

void update_game_score(int score)
{
    move_cursor(SCORE_POS_ROW,SCORE_POS_COLOUN * 2);
    wprintf(L"score: %d\n", score);
}

void gameover_judgement()
{
    int j = 0;
	for (j = 1; j < GAME_POS_COLOUN - 1; j++)
	{
		if (game_area_sign[1][j] == 1) //the first row contains blocks
		{
			move_cursor(DOWN_AREA_ROW, GAME_AREA_CENTER_COLOUN * 2);
			wprintf(L"GAME OVER");
            pause();
		}
	}
}

int move_blocks_down(int row_space_sum, int row_move_index, int space_move_index, int row_check_index)
{
	for (int j = 1; j < GAME_POS_COLOUN - 1; j++)
	{
		game_area_sign[row_check_index][j] = 0;
		move_cursor(2 * j, row_check_index);
		wprintf(L"  ");
	}
	//move all blocks and spaces above down falling 1 row
	for (int row_move_index = row_check_index; row_move_index > 1; row_move_index --)
	{
		row_space_sum = 0;
		for (int space_move_index = 1; space_move_index < GAME_POS_COLOUN - 1; space_move_index ++)
		{
			row_space_sum += game_area_sign[row_move_index - 1][space_move_index];
			game_area_sign[row_move_index][space_move_index] = game_area_sign[row_move_index - 1][space_move_index];
			if (game_area_sign[row_move_index][space_move_index] == 1)
			{
				move_cursor(row_move_index, space_move_index * 2 + 1);
				wprintf(L"%lc%lc", BRICK, BRICK);
			}
			else
			{
				move_cursor(row_move_index, space_move_index * 2 + 1);
				wprintf(L"  ");
			}
		}
		if (row_space_sum == 0) //row move over
			return 1;
	}
    return 0;
}

void row_block_shine(int row_check_index)
{
    int shine_loop = 0;
    for (shine_loop = 0; shine_loop < 2; shine_loop ++)
    {
        move_cursor(row_check_index, 3);
        wprintf(L"                    ");
        usleep(200000);
        for (int j = 1; j < GAME_POS_COLOUN - 1; j ++)  //recore block num of this row
        {
            move_cursor(row_check_index, 2 * j + 1);
            wprintf(L"%lc%lc");
        }
        usleep(200000);
    }
}

int score_and_gameover_judgement()
{
    int row_check_index = 0;
    int j = 0;
    int row_space_sum = 0;  
    int row_move_index = 0;
    int space_move_index = 0;

	//judge get score or not
	for (row_check_index = GAME_POS_ROW - 1; row_check_index > 0; row_check_index--)
	{
		row_space_sum = 0;
		for (int j = 1; j < GAME_POS_COLOUN - 1; j ++)  //recore block num of this row
		{
			row_space_sum += game_area_sign[row_check_index][j];
		}
		if (row_space_sum == 0)
			break;
		if (row_space_sum == GAME_POS_COLOUN - 2) //this row contains enough blocks
		{
            row_block_shine(row_check_index);
			if(move_blocks_down(row_space_sum, row_move_index, space_move_index, row_check_index))
                return 1;
		}
	}
    gameover_judgement();

	return 0;
}

void game_timer_handler(int sig)  //timer handler function, process when game timer trigger 
{
    int score_rows = 0;

    if (block_row_position != 0)
        DrawSpace(block_index, space_index, block_row_position - 1, block_col_position);
    DrawBlock(block_index, space_index, block_row_position, block_col_position);

    if (legal_judgement(block_index, space_index, block_row_position + 1, block_col_position) == 0) 
    {
        game_score += 10;
        update_game_score(game_score);
        save_block_message(block_index, space_index, block_row_position, block_col_position);
        block_index = next_block_index;
        block_row_position = 1;
        block_col_position = GAME_AREA_CENTER_COLOUN;
        space_index = 0;
        next_block_index = rand() % 7;
        clear_next_area();
        show_next_area(next_block_index);
    }
    while(score_and_gameover_judgement())
    {
        score_rows ++;
    }
    game_score += ((score_rows + 1) * score_rows / 2) * 100;
    update_game_score(game_score);

    block_row_position ++;
}

void command_input_and_process_in_time(int *block_index, int *space_index, int *block_row_position, int *block_col_position)
{
    // define command process
    int row_position_temp = 0;
    int col_position_temp = GAME_AREA_CENTER_COLOUN;
    int space_index_temp = 0;
    char command_input;
    char arrow_input;
    char direction_input;

    while (check_keyboard_hit())     // if input, read all inputs
    {
        row_position_temp = *block_row_position;
        col_position_temp = *block_col_position;
        space_index_temp = *space_index;
        command_input = getchar();
        if (command_input == 27) {  // get ESC
            arrow_input = getchar();  // get '['
            if (arrow_input == '[') {
                direction_input = getchar();  // get position value
                command_process(direction_input, block_index, space_index, block_row_position, block_col_position);  // process position value
                }
        }
        DrawSpace(*block_index, space_index_temp, row_position_temp, col_position_temp);
        DrawBlock(*block_index, *space_index, *block_row_position, *block_col_position);
    }
}

int main() {

    // define game timer
    struct itimerval game_timer;

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
    update_game_score(game_score);
    while (1) {
        command_input_and_process_in_time(&block_index, &space_index, &block_row_position, &block_col_position);
    }

    reset_terminal_mode();

    return 0;
}

