#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef enum gamestatus {
    ON,
    AT2048,
    AT4096,
    OVER
} GameStatus;

typedef struct board { 
    uint8_t dim; // I like it square
    uint16_t** tiles;
    uint16_t filled;
    GameStatus status;
} Board;

void place_rand_tiles(Board* board, uint16_t place_val) {
    //Random array no repeats operation
    //Array of full elements
    uint8_t x_arr[board->dim];
    for (int i = 0; i < board->dim; i++) {
        x_arr[i] = i;
    }
    uint8_t y_arr[board->dim];
    memcpy(y_arr, x_arr, sizeof(uint8_t)*board->dim);

    uint8_t roll, check;
    uint8_t* arr_holder; 

    //Shuffle arrays
    for (int turn = 0; turn < 2; turn++) {
        if (turn == 0) {
            arr_holder = x_arr;
        } else {
            arr_holder = y_arr;
        }

        for (int i = 0; i < board->dim; i++) {        
            
            check = arr_holder[i];
            roll = rand() % board->dim;

            arr_holder[i] = arr_holder[roll];
            arr_holder[roll] = check;
        }
    }
    uint16_t val, num_added;
    //Place in a random empty places
    for (int i = 0; i < board->dim; i++) {
        val = board->tiles[x_arr[i]][y_arr[i]];
        if (val == 0) {
            num_added++;
            board->tiles[x_arr[i]][y_arr[i]] = place_val;
            break;
        }
    }

    if (num_added == 0) {
        place_rand_tiles(board, place_val); //bad luck, try again
    }
    //count filled places
    board->filled = 0;
    for (int i = 0; i < board->dim; i++) {
        for (int j = 0; j < board->dim; j++) {
            if (board->tiles[i][j] != 0)
                board->filled++;
        }
    }
}

Board* init(uint8_t dim) {

    uint16_t** tiles = malloc(sizeof(uint16_t*) * dim);
    for (int i = 0; i < dim; i++) {
        tiles[i] = malloc(sizeof(uint16_t) * dim);
        memset(tiles[i], 0, sizeof(uint16_t) * dim);
    }

    Board* board = malloc(sizeof(Board));
    board->tiles = tiles;
    board->dim = dim;
    board->status = ON;
    board->filled = 0;

    return board;
}

void start_game(Board* board) {

    for (int i = 0; i < 3; i++)
        place_rand_tiles(board, 2);

    for (int i = 0; i < 2; i++) {
        uint8_t roll = rand() % 2;
        if (roll == 0) {
            place_rand_tiles(board, 4);
        } else {
            place_rand_tiles(board, 8);
        }
    }

}

void show_board(Board* board) {
    fflush(stdout);

    char board_pos[4];
    uint16_t val = 0;
    for (int i = 0; i < board->dim; i++) {
        for (int j = 0; j < board->dim; j++) {
            
            val = board->tiles[j][i];
            if (val == 0) {
                printf("____");
            } else {
                printf("%4d", val);
            }
            printf(" ");
        }
        printf("\n\r");
    }
    printf("\n\r");
}

/***
 * GAME RULES:
 * - A move stacks tiles with the same number in the direction
 * - Only 2 tiles are stacked in one move
 * 
 * 
 * */
void make_move(Board* board, uint8_t dir) {

    uint16_t zcnt = 0, prev = 0;
    uint16_t* arr;
    uint16_t hor_arr[board->dim];
    uint16_t subarr[board->dim];
    //stack
    for (int rowi = 0; rowi < board->dim; rowi++) {
        //zeroed array to save stacked tiles
        memset(subarr, 0, sizeof(uint16_t) * board->dim);
        if (dir == 'w' || dir == 's') {

            arr = board->tiles[rowi]; //point to column
        } else if (dir == 'a' || dir == 'd') {
            memset(hor_arr, 0, sizeof(uint16_t) * board->dim);
            for (int i = 0; i < board->dim; i++) {
                hor_arr[i] = board->tiles[i][rowi];
            }

            arr = hor_arr;
        }

        zcnt = 0;
        for (int i = 0; i < board->dim; i++) {
            if (arr[i] != 0) {
                subarr[zcnt] = arr[i];
                zcnt++;
            }
        }
        //sum
        uint16_t sumarr[board->dim], sumcnt = 0;
        memset(sumarr, 0, sizeof(uint16_t) * board->dim);
        for (int i = 1; i < board->dim; i++) {
            if (subarr[i - 1] == 0)
                break;

            if (subarr[i] == subarr[i-1]) {
                sumarr[sumcnt] = subarr[i]*2;
                i++;
            } else {
                sumarr[sumcnt] = subarr[i - 1];
            }
            sumcnt++;
        }

        if (dir == 'w' ) {
            memcpy(arr, sumarr, sizeof(uint16_t) * board->dim);
        } else if (dir == 'a') {

            for (int i = 0; i < board->dim; i++) {
                board->tiles[i][rowi] = sumarr[i];
            }
        } else if (dir == 'd' || dir == 's') {
            zcnt = 0;
            //loop to count zeros
            // for (int i = board->dim - 1; i >= 0; i--) {
            for (int i = 0; i < board->dim; i++) {
                if (sumarr[i] == 0)
                    zcnt++;
            }
            //insert zeros first            

            if (dir == 'd') {
                for (int i = 0; i < zcnt; i++) {
                    board->tiles[i][rowi] = 0;
                }
                for (int i = 0; i < board->dim; i++) {
                    if (sumarr[i] != 0) {
                        board->tiles[zcnt][rowi] = sumarr[i];
                        zcnt++;
                    }
                }

            } else if (dir == 's') {
                for (int i = 0; i < zcnt; i++) {
                    board->tiles[rowi][i] = 0;
                }
                for (int i = 0; i < board->dim; i++) {
                    if (sumarr[i] != 0) {
                        board->tiles[rowi][zcnt] = sumarr[i];
                        zcnt++;
                    }
                }
            }
        }
    }
    //check for winning point
    int point = 0;
    for (int i = 0; i < board->dim; i++) {
        for (int j = 0; j < board->dim; j++) {
            point = board->tiles[i][j]; 
            if (point == 2048) {
                board->status = AT2048;
            } else if (point == 4096) {
                board->status = AT4096;
            }
        }
    }
}

void play_game(Board* board) {
    char move;
    uint8_t tile_val[3] = {2, 4, 8}, roll = 0;

    start_game(board);
    show_board(board);

    while(board->status != AT4096) {
        if (board->status == AT2048)
            printf("You hit 2048 boss, but don't hit me ;)\n\r");

        printf("Filled=%d\n\rMove: ", board->filled);
        move = getchar();
        fflush(stdin);
        printf("****************************\n\r");
        make_move(board, move);

        roll = rand() % 3;
        place_rand_tiles(board, tile_val[roll]);
        show_board(board);
    }
}

void self_play(Board* board) {
    int roll = 0;
    uint8_t tile_val[3] = {2, 4, 8};
    char moves[4] = {'a', 'd', 'w', 's'};
    char themove;
    for (int i = 0; i < 14; i++) {
        printf("****************************\n\r");
        if (i == 0) {
            start_game(board);
            show_board(board);
        } else {

            roll = rand() % 3;
            place_rand_tiles(board, tile_val[roll]);
            show_board(board);
        }
        
        roll = rand() % 4;
        themove = moves[roll];
        printf("\n\rmove-->%c\n\r", themove);
        make_move(board, themove);
        show_board(board);
    }    
}

int main(int argc, char** argv) {
    printf("4096 is starting ... ... ..\n\r");

    Board* board = init(5);
    printf("dim %d, sts %d\n\r", board->dim, board->status);
    // self_play(board);
    play_game(board);
    free(board->tiles);
    free(board);

    printf("4096 finished");
    return 0;
}