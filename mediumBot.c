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
    return 0; // placeholder
}