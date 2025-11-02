#include <stdlib.h>
#include "mediumBot.h"
#include "easyBot.h" // uses easyBot's implementation of random when no blocks/wins available
#include "checkWin.h"

// potentialWin checks if placing a checker in a column would result in a win for the given player
// Returns the column number (1-based) if a win is found, 0 otherwise
int potentialWin(int** board, int rows, int cols, int player) {
    // Check each column
    for (int col = 0; col < cols; col++) {
        // Find the bottom available row in this column (checkers fall to the bottom)
        int row = -1;
        for (int r = rows - 1; r >= 0; r--) {
            if (board[r][col] == 0) {
                row = r;
                break;
            }
        }
        
        // Skip if column is full
        if (row == -1) {
            continue;
        }
        
        // Simulate placing the checker
        board[row][col] = player;
        
        // Check if this move results in a win
        if (checkWin(board, row, col, player, rows, cols)) {
            // Undo the move before returning
            board[row][col] = 0;
            // Return 1-based column number
            return col + 1;
        }
        
        // Undo the move
        board[row][col] = 0;
    }
    
    return 0; // No winning move found
}

// findWin finds a winning move for player 2 (the bot)
int findWin(int** board, int rows, int cols) {
    return potentialWin(board, rows, cols, 2);
}

// blockWin finds a move that would block player 1 from winning
int blockWin(int** board, int rows, int cols) {
    return potentialWin(board, rows, cols, 1);
}

// mediumBot placeholder - to be implemented by friend
int mediumBot(int** board, int rows, int cols, int player, int* validColumns, int validCount) {
     /* Keeps track whether we've seen the first blocking opportunity already.
       This persists between calls during program execution. */
    static int seenFirstThreat = 0;

    /* 1) If the bot can win now, take that move immediately. */
    int winCol = findWin(board, rows, cols); /* 1-based column or 0 */
    if (winCol != 0) {
        /* double-check it's still valid (convert to 0-based for isValidColumn) */
        if (isValidColumn(board, rows, cols, winCol - 1))
            return winCol;
    }

    /* 2) See if the opponent has a direct win; decide whether to block. */
    int blockCol = blockWin(board, rows, cols); /* 1-based column or 0 */
    if (blockCol != 0) {
        int chancePct;
        if (!seenFirstThreat) {
            chancePct = 90;      /* first time -> 90% chance to block */
            seenFirstThreat = 1; /* mark that we've encountered the first threat */
        } else {
            chancePct = 50;      /* subsequent times -> 50% chance to block */
        }

        /* roll 0..99 */
        int roll = rand() % 100;
        if (roll < chancePct) {
            if (isValidColumn(board, rows, cols, blockCol - 1))
                return blockCol;
            /* if blockCol somehow no longer valid, fall through to random pick */
        }
        /* if roll >= chancePct, we intentionally do not block this time */
    }

    /* 3) No win or decided not to block -> pick a random valid column using easyBot.
          easyBot expects (maxColumn, minColumn, validColumns) where minColumn is 1
          according to the version you gave. */
    if (validCount > 0 && validColumns != NULL) {
        return easyBot(validCount, 1, validColumns);
    }

    /* 4) Fallback: scan for any valid column and return the first (1-based) */
    for (int c = 0; c < cols; ++c) {
        if (isValidColumn(board, rows, cols, c))
            return c + 1;
    }

    /* If board is full or something unexpected happened, return 1 as a last resort. */
    return 1;
}