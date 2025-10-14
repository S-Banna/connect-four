#include "checkWin.h"

int checkWin(int** board, int row, int column, int player, int ROWS, int COLS) {
    if (checkDirection(board, row, column, player, 1, 0, ROWS, COLS)) return 1;
    if (checkDirection(board, row, column, player, 1, 1, ROWS, COLS)) return 1;
    if (checkDirection(board, row, column, player, 1, -1, ROWS, COLS)) return 1;
    if (checkDirection(board, row, column, player, 0, 1, ROWS, COLS)) return 1;

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