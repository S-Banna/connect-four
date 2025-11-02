#include <stdlib.h>
#include "mediumBot.h"
#include "easyBot.h" // uses easyBot's implementation of random when no blocks/wins available
#include "checkWin.h"

int potentialWin(int** board, int rows, int cols, int player) {
    for (int col = 0; col < cols; col++) {
        int row = -1;
        for (int r = rows - 1; r >= 0; r--) {
            if (board[r][col] == 0) {
                row = r;
                break;
            }
        }
        if (row == -1) {
            continue;
        }
        board[row][col] = player;
        
        if (checkWin(board, row, col, player, rows, cols)) {
            board[row][col] = 0;
            return col + 1;
        }
        board[row][col] = 0;
    }
    
    return 0; 
}

int findWin(int** board, int rows, int cols) {
    return potentialWin(board, rows, cols, 2);
}

int blockWin(int** board, int rows, int cols) {
    return potentialWin(board, rows, cols, 1);
}

int mediumBot(int** board, int rows, int cols, int player, int* validColumns, int validCount) {
    static int seenFirstThreat = 0;

    int winCol = findWin(board, rows, cols); 
    if (winCol != 0) {
        if (isValidColumn(board, rows, cols, winCol - 1))
            return winCol;
    }

    int blockCol = blockWin(board, rows, cols); 
    if (blockCol != 0) {
        int chancePct;
        if (!seenFirstThreat) {
            chancePct = 90;      
            seenFirstThreat = 1; 
        } else {
            chancePct = 50;      
        }

        int roll = rand() % 100;
        if (roll < chancePct) {
            if (isValidColumn(board, rows, cols, blockCol - 1))
                return blockCol;
        }
    }

    if (validCount > 0 && validColumns != NULL) {
        return easyBot(validCount, 1, validColumns);
    }

    for (int c = 0; c < cols; ++c) {
        if (isValidColumn(board, rows, cols, c))
            return c + 1;
    }

    return 1;
}