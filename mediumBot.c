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

typedef struct {
    int** boardCopy;
    int rows;
    int cols;
    int result;
} WBArg;

int** copyBoard(int** board, int rows, int cols) {
    int** b = malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++) {
        b[i] = malloc(cols * sizeof(int));
        for (int j = 0; j < cols; j++)
            b[i][j] = board[i][j];
    }
    return b;
}

void* runFind(void* arg) {
    WBArg* a = arg;
    a->result = potentialWin(a->boardCopy, a->rows, a->cols, 2);
    return NULL;
}

void* runBlock(void* arg) {
    WBArg* a = arg;
    a->result = potentialWin(a->boardCopy, a->rows, a->cols, 1);
    return NULL;
}

int mediumBot(int** board, int rows, int cols, int player, int* validColumns, int validCount) {
    static int seenFirstThreat = 0;

    pthread_t t1, t2;
    WBArg a1 = { copyBoard(board, rows, cols), rows, cols, 0 };
    WBArg a2 = { copyBoard(board, rows, cols), rows, cols, 0 };

    pthread_create(&t1, NULL, runFind, &a1);
    pthread_create(&t2, NULL, runBlock, &a2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    for (int i = 0; i < rows; i++) free(a1.boardCopy[i]);
    free(a1.boardCopy);

    for (int i = 0; i < rows; i++) free(a2.boardCopy[i]);
    free(a2.boardCopy);

    int winCol = a1.result;
    if (winCol != 0) {
        if (isValidColumn(board, rows, cols, winCol - 1))
            return winCol;
    }

    int blockCol = a2.result;
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