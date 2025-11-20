#include <stdlib.h>
#include <pthread.h>
#include "mediumBot.h"
#include "easyBot.h" // uses easyBot's implementation of random when no blocks/wins available
#include "checkWin.h"

typedef struct {
    int** board;
    int rows;
    int cols;
    int result;
} WBArg;

void* runFind(void* arg) {
    WBArg* a = arg;
    a->result = potentialWin(a->board, a->rows, a->cols, 2);
    return NULL;
}

void* runBlock(void* arg) {
    WBArg* a = arg;
    a->result = potentialWin(a->board, a->rows, a->cols, 1);
    return NULL;
}

int mediumBot(int** board, int rows, int cols, int player, int* validColumns, int validCount) {
    static int seenFirstThreat = 0;

    pthread_t t1, t2;
    WBArg a1 = { board, rows, cols, 0 };
    WBArg a2 = { board, rows, cols, 0 };

    pthread_create(&t1, NULL, runFind, &a1);
    pthread_create(&t2, NULL, runBlock, &a2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

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