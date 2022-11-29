#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_X_DIM   256
#define MAX_Y_DIM   256
#define WIN_POINTS  3
typedef enum {
    E = 0,
    O,
    X,
} TickType;

typedef enum gamestatus{
    ON = 0,
    XWINS,
    OWINS,
} GameStatus;

typedef struct board {
    uint8_t dim_x;
    uint8_t dim_y;
    TickType** b; 
    GameStatus status;
} Board;

Board* init(uint8_t dim_x, uint8_t dim_y) {

    Board* board = malloc(sizeof(Board*));
    board->dim_x = dim_x;
    board->dim_y = dim_y;
    if (dim_x > MAX_X_DIM) {
        board->dim_x = MAX_X_DIM - 1;
    }

    if (dim_y > MAX_Y_DIM) {
        board->dim_y = MAX_Y_DIM - 1; 
    }

    board->b = malloc(sizeof(TickType*) * dim_x);

    for (int i = 0; i < dim_x; i++) {

        board->b[i] = malloc(sizeof(TickType) * dim_y);
    }

    for (int i = 0; i < dim_x; i++) {
        for (int j = 0; j < dim_y; j++) {
            board->b[i][j] = E;
        }
    }
    board->status = ON;
    printf("board->status %d\n\r", board->status);
    return board;
}

void show_board(Board* board) {
    char board_pos;
    for (int i = 0; i < board->dim_y; i++) {
        for (int j = 0; j < board->dim_x; j++) {
            // printf("%d", board->b[i][j]);
            switch (board->b[j][i]) {
                case E:
                    board_pos = '_';
                    break;
                case O:
                    board_pos = 'O';
                    break;
                case X:
                    board_pos = 'X';
                    break;
                default:
                    printf("bum ");
            }
            printf("%c ", board_pos);
        }
        printf("\n\r");
    }
    printf("\n\r");
}

void get_neighbours(uint8_t* neighbours, Board* board, uint8_t x, uint8_t y) {
    //Start from top left, left to right then top to bottom
    if (x == 0) {

        neighbours[0] = 0;
        neighbours[3] = 0;
        neighbours[5] = 0;
    } else if (x == board->dim_x - 1) {

        neighbours[2] = 0;
        neighbours[4] = 0;
        neighbours[7] = 0;
    } 
    
    if (y == 0) {

        neighbours[0] = 0;
        neighbours[1] = 0;
        neighbours[2] = 0;
    } else if (y == board->dim_y - 1) {

        neighbours[5] = 0;
        neighbours[6] = 0;
        neighbours[7] = 0;
    }
}

uint8_t check_game(Board* board) {
    //check rows
    TickType* arrR;
    for (int i = 0; i < board->dim_x; i++) {
        arrR = board->b[i];

        for (int j = 0; j < board->dim_y - WIN_POINTS; j++) {
            if ((arrR[j] != E) && (arrR[j] == arrR[j + 1] && arrR[j + 1] == arrR[j + 2])) {
                
                return 1;
            }
        }
    }
    //check columns
    TickType arr[board->dim_y];
    for (int i = 0; i < board->dim_y; i++) {
        for (int j = 0; j < board->dim_x; j++) {
            arr[j] = board->b[j][i];
        }

        for (int j = 0; j < board->dim_x - WIN_POINTS; j++) {
            if ((arr[j] != E) && (arr[j] == arr[j + 1] && arr[j + 1] == arr[j + 2])) {
                return 1;
            }
        }
    }

    //check forward diagonal
    for (int i = 0; i < board->dim_x - WIN_POINTS; i++) {
        for (int j = 0; j < board->dim_y - WIN_POINTS; j++) {

            if ((board->b[i][j] != E) && (board->b[i][j] == board->b[i+1][j+1] && board->b[i+1][j+1] == board->b[i+2][j+2])) {
                return 1;
            }
        }
    }


    //check backward diagonal
    for (int i = 0; i < board->dim_x - WIN_POINTS; i++) {
        for (int j = WIN_POINTS - 1; j < board->dim_y; j++) {

            if ((board->b[i][j] != E) && (board->b[i][j] == board->b[i+1][j-1] && board->b[i+1][j-1] == board->b[i+2][j-2])) {
                return 1;
            }
        }
    }

    return 0;
}

uint8_t make_move(Board* board, uint8_t pos_x, uint8_t pos_y, TickType move) {
    if (board->b[pos_x][pos_y] == E) {
        
        board->b[pos_x][pos_y] = move;
        
        show_board(board);
        uint8_t res = check_game(board);

        if (res == 1) {
            switch (move) {
                case O:
                    board->status = OWINS;
                    break;
                case X:
                    board->status = XWINS;
                    break;
                }
        }
    }    
}

int main(int argcs, char **argv) {

    uint8_t stop = 0;
    printf("TIC TAC TOE HO HO HO\n\r");

    Board* board = init(15, 15);
    
    int x_pos, y_pos;
    TickType tick = X;
    char tickC = 'X';
    while(board->status == ON) {

        if (tick == X) {
            tick = O;
            tickC = 'O';
        } else {
            tick = X;
            tickC = 'X';
        }
        printf("%c turn | x position | y position: ", tickC);
        scanf("%d%d", &x_pos, &y_pos);
        make_move(board, x_pos, y_pos, tick);
    }
    printf("%c WINS\n\r", tickC);
    for (int i = 0; i < board->dim_x; i++) {
        free(board->b[i]);
    }
    free(board->b);
    free(board);   
    return 0;
}
