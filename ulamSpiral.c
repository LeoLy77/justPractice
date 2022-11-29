#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define PRINT_COLOR     printf("\033[0;33m")
#define PRINT_REG       printf("\033[0;37m")

#define ANIMATION       0
#define SIZE            31
#define REFRESH_RATE    80

typedef struct Grid {
    uint16_t** board;
    uint8_t x;
    uint8_t y;
    uint8_t size;
    uint16_t step;
    uint8_t state;
    uint16_t num_steps;
    uint16_t turn_cnt;
} Grid;

uint8_t memo[SIZE*SIZE + 1];
uint8_t isPrime(int num, int counter) {
    if (memo[num] != 2) {
        return memo[num];
    }

    if (num == 0 || num == 1 || (num % counter == 0 && num != counter)) {
        memo[num] = 0;
        return 0;
    } else if (num % counter == 0 && num == counter) {
        memo[num] = 1;
        return 1;
    }
    
    counter++;
    return isPrime(num, counter);
}

int makeMove(Grid* grid) {
    switch (grid->state) {
        case 0:
            grid->x += 1;
            break;
        case 1:
            grid->y -= 1;
            break;
        case 2:
            grid->x -= 1;
            break;
        case 3:
            grid->y += 1;
            break;
        default:
            break;
    }

    if (grid->step % grid->num_steps == 0) {
        grid->state = (grid->state + 1) % 4;
        grid->turn_cnt++;
        if (grid->turn_cnt % 2 == 0) {
            grid->num_steps++;
        }
    }
    
    grid->board[grid->x][grid->y] = grid->step;
    grid->step++;
    return 0;
}

void draw(Grid* grid) {
    char snum[3];
    uint8_t centre = grid->size/2;
    system("cls");
    Sleep(5);
    for (int i = 0; i < grid->size; i++) {
        for (int j = 0; j < grid->size; j++) {

            uint16_t val = grid->board[i][j];
            if (i == centre && j == centre) {

                printf("  0 ");
            } else if (val == 0) {

                printf("___ ");
            } else {

                if (isPrime(val, 2) == 1) {
                    PRINT_COLOR;
                } else {
                    PRINT_REG;
                }

                sprintf(snum, "%3d", val);
                printf("%s ", snum);
            }
        }
        printf("\n");
    }
}

int main(char* argc, char** argv) {
    Grid grid;
    memset(&grid, 0, sizeof(Grid));    
    grid.size = SIZE;

    grid.num_steps = 1;
    grid.x = grid.size/2;
    grid.y = grid.size/2;
    grid.step = 1;

    grid.board = malloc(grid.size * sizeof(uint16_t*));
    for (uint8_t i = 0; i < grid.size; i++) {
        grid.board[i] = malloc(grid.size * sizeof(uint16_t));
        memset(grid.board[i], 0, sizeof(uint16_t)*grid.size);
    }
    memset(memo, 2, sizeof(uint8_t)*(SIZE*SIZE+1));

    while (grid.step != grid.size*grid.size) {
        makeMove(&grid);

        if (ANIMATION) {
            draw(&grid);
            Sleep(REFRESH_RATE);
        }
    }

    if (!ANIMATION) {
        draw(&grid);
    }
    free(grid.board);
    return 0;
}