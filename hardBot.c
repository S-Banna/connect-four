#include <limits.h>
#include "checkWin.h"

#define MAX_DEPTH 6   // modify as needed, 6 is sweet spot for difficulty and time

static int evaluateBoard(int** board, int rows, int cols, int botPlayer);
static int minimax(int** board, int rows, int cols, int depth,
                   int maximizingPlayer, int botPlayer,
                   int alpha, int beta);

static int isValid(int** board, int rows, int col) {
    return board[0][col] == 0;
}

static int makeMove(int** board, int rows, int col, int player) {
    for (int r = rows - 1; r >= 0; r--) {
        if (board[r][col] == 0) {
            board[r][col] = player;
            return r;
        }
    }
    return -1;
}

static void undoMove(int** board, int row, int col) {
    board[row][col] = 0;
}

int hardBot(int** board, int rows, int cols, int botPlayer) {
    int bestCol = -1;
    int bestScore = INT_MIN;

    for (int col = 0; col < cols; col++) {
        if (!isValid(board, rows, col))
            continue;

        int r = makeMove(board, rows, col, botPlayer);

        if (checkWin(board, r, col, botPlayer, rows, cols)) {
            undoMove(board, r, col);
            return col + 1;
        }

        int score = minimax(board, rows, cols, MAX_DEPTH - 1,
                            0, botPlayer, INT_MIN, INT_MAX);

        undoMove(board, r, col);

        if (score > bestScore) {
            bestScore = score;
            bestCol = col;
        }
    }

    return bestCol + 1;
}

static int evaluateWindow(int* window, int botPlayer) {
    int opp = (botPlayer == 1) ? 2 : 1;

    int botCount = 0, oppCount = 0, empty = 0;

    for (int i = 0; i < 4; i++) {
        if (window[i] == botPlayer) botCount++;
        else if (window[i] == opp) oppCount++;
        else empty++;
    }

    if (botCount == 4) return 100000;
    if (botCount == 3 && empty == 1) return 100;
    if (botCount == 2 && empty == 2) return 20;

    if (oppCount == 3 && empty == 1) return -80;
    if (oppCount == 4) return -100000;

    return 0;
}


static int evaluateBoard(int** board, int rows, int cols, int botPlayer) {
    int score = 0;

    int center = cols / 2;
    for (int r = 0; r < rows; r++) {
        if (board[r][center] == botPlayer)
            score += 4;
    }

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols - 3; c++) {
            int window[4] = {
                board[r][c], board[r][c+1], board[r][c+2], board[r][c+3]
            };
            score += evaluateWindow(window, botPlayer);
        }
    }

    for (int c = 0; c < cols; c++) {
        for (int r = 0; r < rows - 3; r++) {
            int window[4] = {
                board[r][c], board[r+1][c], board[r+2][c], board[r+3][c]
            };
            score += evaluateWindow(window, botPlayer);
        }
    }

    for (int r = 0; r < rows - 3; r++) {
        for (int c = 0; c < cols - 3; c++) {
            int window[4] = {
                board[r][c], board[r+1][c+1], board[r+2][c+2], board[r+3][c+3]
            };
            score += evaluateWindow(window, botPlayer);
        }
    }

    for (int r = 3; r < rows; r++) {
        for (int c = 0; c < cols - 3; c++) {
            int window[4] = {
                board[r][c], board[r-1][c+1], board[r-2][c+2], board[r-3][c+3]
            };
            score += evaluateWindow(window, botPlayer);
        }
    }

    return score;
}

static int minimax(
    int** board, int rows, int cols, int depth,
    int maximizingPlayer, int botPlayer,
    int alpha, int beta) {
    int opp = (botPlayer == 1) ? 2 : 1;

    if (depth == 0)
        return evaluateBoard(board, rows, cols, botPlayer);

    if (maximizingPlayer) {
        int best = INT_MIN;

        for (int col = 0; col < cols; col++) {
            if (!isValid(board, rows, col))
                continue;

            int r = makeMove(board, rows, col, botPlayer);

            if (checkWin(board, r, col, botPlayer, rows, cols)) {
                undoMove(board, r, col);
                return 999999;  
            }

            int val = minimax(board, rows, cols,
                              depth - 1, 0, botPlayer, alpha, beta);

            undoMove(board, r, col);

            if (val > best) best = val;
            if (best > alpha) alpha = best;
            if (alpha >= beta) break;
        }
        return best;

    } else {  
        int best = INT_MAX;

        for (int col = 0; col < cols; col++) {
            if (!isValid(board, rows, col))
                continue;

            int r = makeMove(board, rows, col, opp);

            if (checkWin(board, r, col, opp, rows, cols)) {
                undoMove(board, r, col);
                return -999999;
            }

            int val = minimax(board, rows, cols,
                              depth - 1, 1, botPlayer, alpha, beta);

            undoMove(board, r, col);

            if (val < best) best = val;
            if (best < beta) beta = best;
            if (alpha >= beta) break;
        }
        return best;
    }
}