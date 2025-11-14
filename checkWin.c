#include "checkWin.h"

void* checkDirectionThread(void* args) {
    ThreadArgs* t = (ThreadArgs*) args;
    t->result = checkDirection(t->board, t->row, t->column, t->player, t->x, t->y, t->ROWS, t->COLS);
    return NULL;
}

int checkWin(int** board, int row, int column, int player, int ROWS, int COLS) {
    pthread_t threads[4];
    ThreadArgs args[4];
    int directions[4][2] = {{1, 0}, {1, 1}, {1, -1}, {0, 1}};

    for (int i = 0; i < 4; i++) {
        args[i].board = board;
        args[i].row = row;
        args[i].column = column;
        args[i].player = player;
        args[i].ROWS = ROWS;
        args[i].COLS = COLS;
        args[i].x = directions[i][0];
        args[i].y = directions[i][1];
        args[i].result = 0;
        
        pthread_create(&threads[i], NULL, checkDirectionThread, &args[i]);
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < 4; i++) {
        if (args[i].result) {
            return 1;
        }
    }
    
    return 0;
}

int checkDirection(int** board, int row, int column, int player, int x, int y, int ROWS, int COLS) {
    int count = 0;
    int rowC = row, columnC = column;
    
    rowC -= 3 * y;
    columnC -= 3 * x;
    int cY, cX;

    for (int i = 0; i < COLS; i++) {
        cY = rowC + i * y;
        cX = columnC + i * x;
        if (cY < 0 || cX < 0 || cY >= ROWS || cX >= COLS) {
            count = 0;
            continue;
        }

        if (board[cY][cX] == player) count++;
        else count = 0;
        
        if (count == 4) return 1;
    }

    return 0;
}