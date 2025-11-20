#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "checkWin.h"

#define MAX_DEPTH 7   // modify as needed, 6 or 7 is sweet spot for difficulty and time

void encodeBoard(int** board, int rows, int cols, char* out, int botPlayer) {
    int idx = 0;

    for (int c = 0; c < cols; c++) {            // left -> right
        for (int r = 0; r < rows; r++) {        // bottom -> top
            int cell = board[rows - 1 - r][c];  // flip row

            if (cell == 0) out[idx++] = 'b';
            else if (cell == botPlayer) out[idx++] = 'x';
            else out[idx++] = 'o';
        }
    }

    out[idx] = '\0';
}

int checkCSVDirect(const char* csvFile, const char* key) {
    FILE* f = fopen(csvFile, "r");
    if (!f) return 9999; 

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        // must be 42 chars for board + label
        if (strlen(line) < 42) continue;

        // compare first 42 chars
        if (strncmp(line, key, 42) == 0) {
            // find last token (win/loss/draw)
            char* result = line + 42;

            if (strstr(result, "win"))  { fclose(f); return +1; }
            if (strstr(result, "loss")) { fclose(f); return -1; }
            // draw -> return 0
            fclose(f);
            return 0;
        }
    }

    fclose(f);
    return 9999;   // code for "not found"
}

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

static int minimax(int** board, int rows, int cols, int depth, int maximizingPlayer, int botPlayer, int alpha, int beta) {
    
    if (depth >= MAX_DEPTH - 1) { // only cross reference csv on first two depths, otherwise
        char key[64];             // too far from current game state and takes too long
        encodeBoard(board, rows, cols, key, botPlayer);

        int csvResult = checkCSVDirect("connect-4.csv", key);

        if (csvResult != 9999) {
            printf("slam (%d)\n", csvResult);

            if (csvResult == +1)  return  999999;
            if (csvResult == -1)  return -999999;
            // draw -> ignore (continue heuristic)
        }
    }

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