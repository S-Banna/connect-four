#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "checkWin.h"

#define MAX_DEPTH 7   // increase for harder and slower bot, 6/7/8 is sweetspot for fun and difficulty
#define WIN_SCORE 1000000
#define LOSS_SCORE -1000000

// Transposition table entry
typedef struct {
    unsigned long long hash;
    int score;
    int depth;
    char flag; // 0=exact, 1=lower, 2=upper
} TTEntry;

#define TT_SIZE 8388593  // Prime number for hash table
static TTEntry* transTable = NULL;

// Zobrist hashing for transposition table
static unsigned long long zobristTable[6][7][3]; // [rows][cols][players]
static int zobristInitialized = 0;

static void initZobrist() {
    if (zobristInitialized) return;
    srand(12345); // Fixed seed for reproducibility
    for (int r = 0; r < 6; r++) {
        for (int c = 0; c < 7; c++) {
            for (int p = 0; p < 3; p++) {
                zobristTable[r][c][p] = ((unsigned long long)rand() << 32) | rand();
            }
        }
    }
    zobristInitialized = 1;
}

static unsigned long long computeHash(int** board, int rows, int cols) {
    unsigned long long hash = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (board[r][c] != 0) {
                hash ^= zobristTable[r][c][board[r][c]];
            }
        }
    }
    return hash;
}

static void initTransTable() {
    if (transTable == NULL) {
        transTable = (TTEntry*)calloc(TT_SIZE, sizeof(TTEntry));
    }
}

static int probeTT(unsigned long long hash, int depth, int alpha, int beta, int* score) {
    TTEntry* entry = &transTable[hash % TT_SIZE];
    
    if (entry->hash == hash && entry->depth >= depth) {
        if (entry->flag == 0) { // Exact score
            *score = entry->score;
            return 1;
        } else if (entry->flag == 1 && entry->score >= beta) { // Lower bound
            *score = entry->score;
            return 1;
        } else if (entry->flag == 2 && entry->score <= alpha) { // Upper bound
            *score = entry->score;
            return 1;
        }
    }
    return 0;
}

static void storeTT(unsigned long long hash, int depth, int score, char flag) {
    TTEntry* entry = &transTable[hash % TT_SIZE];
    if (entry->depth <= depth) {
        entry->hash = hash;
        entry->score = score;
        entry->depth = depth;
        entry->flag = flag;
    }
}

void encodeBoard(int** board, int rows, int cols, char* out, int botPlayer) {
    int idx = 0;
    for (int c = 0; c < cols; c++) {
        for (int r = 0; r < rows; r++) {
            int cell = board[rows - 1 - r][c];
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
        if (strlen(line) < 42) continue;
        if (strncmp(line, key, 42) == 0) {
            char* result = line + 42;
            if (strstr(result, "win"))  { fclose(f); return +1; }
            if (strstr(result, "loss")) { fclose(f); return -1; }
            fclose(f);
            return 0;
        }
    }
    fclose(f);
    return 9999;
}

static int evaluateBoard(int** board, int rows, int cols, int botPlayer);
static int minimax(int** board, int rows, int cols, int depth,
                   int maximizingPlayer, int botPlayer,
                   int alpha, int beta, unsigned long long hash);

static int isValid(int** board, int rows, int col) {
    return col >= 0 && col < 7 && board[0][col] == 0;
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

// Check if board is full
static int isBoardFull(int** board, int rows, int cols) {
    for (int c = 0; c < cols; c++) {
        if (board[0][c] == 0) return 0;
    }
    return 1;
}

// Move ordering: prioritize center columns and immediate threats
static void orderMoves(int** board, int rows, int cols, int botPlayer, int* moveOrder) {
    int scores[7];
    int center = cols / 2;
    
    for (int col = 0; col < cols; col++) {
        moveOrder[col] = col;
        scores[col] = -abs(col - center); // Prefer center
        
        if (isValid(board, rows, col)) {
            int r = makeMove(board, rows, col, botPlayer);
            
            // Strongly prefer winning moves
            if (checkWin(board, r, col, botPlayer, rows, cols)) {
                scores[col] += 10000;
            }
            
            undoMove(board, r, col);
            
            // Check for blocking opponent wins
            int opp = (botPlayer == 1) ? 2 : 1;
            r = makeMove(board, rows, col, opp);
            if (checkWin(board, r, col, opp, rows, cols)) {
                scores[col] += 5000;
            }
            undoMove(board, r, col);
        } else {
            scores[col] = -99999; // Invalid moves last
        }
    }
    
    // Simple bubble sort for move ordering
    for (int i = 0; i < cols - 1; i++) {
        for (int j = 0; j < cols - i - 1; j++) {
            if (scores[moveOrder[j]] < scores[moveOrder[j+1]]) {
                int temp = moveOrder[j];
                moveOrder[j] = moveOrder[j+1];
                moveOrder[j+1] = temp;
                
                int tempScore = scores[moveOrder[j]];
                scores[moveOrder[j]] = scores[moveOrder[j+1]];
                scores[moveOrder[j+1]] = tempScore;
            }
        }
    }
}

int hardBot(int** board, int rows, int cols, int botPlayer) {
    initZobrist();
    initTransTable();
    
    int bestCol = -1;
    int bestScore = INT_MIN;
    unsigned long long hash = computeHash(board, rows, cols);
    
    // Check for immediate win
    for (int col = 0; col < cols; col++) {
        if (!isValid(board, rows, col)) continue;
        
        int r = makeMove(board, rows, col, botPlayer);
        if (checkWin(board, r, col, botPlayer, rows, cols)) {
            undoMove(board, r, col);
            return col + 1;
        }
        undoMove(board, r, col);
    }
    
    // Check for必block opponent win
    int opp = (botPlayer == 1) ? 2 : 1;
    for (int col = 0; col < cols; col++) {
        if (!isValid(board, rows, col)) continue;
        
        int r = makeMove(board, rows, col, opp);
        if (checkWin(board, r, col, opp, rows, cols)) {
            undoMove(board, r, col);
            return col + 1;
        }
        undoMove(board, r, col);
    }
    
    // Use iterative deepening for better move ordering
    int moveOrder[7];
    orderMoves(board, rows, cols, botPlayer, moveOrder);
    
    for (int i = 0; i < cols; i++) {
        int col = moveOrder[i];
        
        if (!isValid(board, rows, col)) continue;
        
        int r = makeMove(board, rows, col, botPlayer);
        unsigned long long newHash = hash ^ zobristTable[r][col][botPlayer];
        
        int score = minimax(board, rows, cols, MAX_DEPTH - 1,
                          0, botPlayer, INT_MIN, INT_MAX, newHash);
        
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

    // Winning positions
    if (botCount == 4) return 100000;
    if (botCount == 3 && empty == 1) return 500;
    if (botCount == 2 && empty == 2) return 50;
    
    // Blocking opponent
    if (oppCount == 3 && empty == 1) return -400;
    if (oppCount == 2 && empty == 2) return -30;
    if (oppCount == 4) return -100000;

    return 0;
}

static int evaluateBoard(int** board, int rows, int cols, int botPlayer) {
    int score = 0;

    // Favor center column more strongly
    int center = cols / 2;
    for (int r = 0; r < rows; r++) {
        if (board[r][center] == botPlayer)
            score += 6;
    }
    
    // Near-center columns
    if (center > 0) {
        for (int r = 0; r < rows; r++) {
            if (board[r][center-1] == botPlayer) score += 3;
            if (center + 1 < cols && board[r][center+1] == botPlayer) score += 3;
        }
    }

    // Horizontal
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols - 3; c++) {
            int window[4] = {
                board[r][c], board[r][c+1], board[r][c+2], board[r][c+3]
            };
            score += evaluateWindow(window, botPlayer);
        }
    }

    // Vertical
    for (int c = 0; c < cols; c++) {
        for (int r = 0; r < rows - 3; r++) {
            int window[4] = {
                board[r][c], board[r+1][c], board[r+2][c], board[r+3][c]
            };
            score += evaluateWindow(window, botPlayer);
        }
    }

    // Diagonal (positive slope)
    for (int r = 0; r < rows - 3; r++) {
        for (int c = 0; c < cols - 3; c++) {
            int window[4] = {
                board[r][c], board[r+1][c+1], board[r+2][c+2], board[r+3][c+3]
            };
            score += evaluateWindow(window, botPlayer);
        }
    }

    // Diagonal (negative slope)
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

static int minimax(int** board, int rows, int cols, int depth,
                   int maximizingPlayer, int botPlayer,
                   int alpha, int beta, unsigned long long hash) {
    
    // Check transposition table
    int ttScore;
    if (probeTT(hash, depth, alpha, beta, &ttScore)) {
        return ttScore;
    }
    
    // CSV lookup at shallow depths
    if (depth >= MAX_DEPTH - 2) {
        char key[64];
        encodeBoard(board, rows, cols, key, botPlayer);
        int csvResult = checkCSVDirect("connect-4.csv", key);
        
        if (csvResult != 9999) {
            if (csvResult == +1)  return WIN_SCORE;
            if (csvResult == -1)  return LOSS_SCORE;
        }
    }

    int opp = (botPlayer == 1) ? 2 : 1;

    // Terminal conditions
    if (depth == 0 || isBoardFull(board, rows, cols)) {
        return evaluateBoard(board, rows, cols, botPlayer);
    }

    if (maximizingPlayer) {
        int best = INT_MIN;
        int moveOrder[7];
        orderMoves(board, rows, cols, botPlayer, moveOrder);

        for (int i = 0; i < cols; i++) {
            int col = moveOrder[i];
            if (!isValid(board, rows, col)) continue;

            int r = makeMove(board, rows, col, botPlayer);

            if (checkWin(board, r, col, botPlayer, rows, cols)) {
                undoMove(board, r, col);
                storeTT(hash, depth, WIN_SCORE, 0);
                return WIN_SCORE;
            }

            unsigned long long newHash = hash ^ zobristTable[r][col][botPlayer];
            int val = minimax(board, rows, cols, depth - 1, 0, botPlayer, alpha, beta, newHash);

            undoMove(board, r, col);

            if (val > best) best = val;
            if (best > alpha) alpha = best;
            if (alpha >= beta) break; // Beta cutoff
        }
        
        char flag = (best <= alpha) ? 2 : (best >= beta) ? 1 : 0;
        storeTT(hash, depth, best, flag);
        return best;

    } else {
        int best = INT_MAX;
        int moveOrder[7];
        orderMoves(board, rows, cols, opp, moveOrder);

        for (int i = 0; i < cols; i++) {
            int col = moveOrder[i];
            if (!isValid(board, rows, col)) continue;

            int r = makeMove(board, rows, col, opp);

            if (checkWin(board, r, col, opp, rows, cols)) {
                undoMove(board, r, col);
                storeTT(hash, depth, LOSS_SCORE, 0);
                return LOSS_SCORE;
            }

            unsigned long long newHash = hash ^ zobristTable[r][col][opp];
            int val = minimax(board, rows, cols, depth - 1, 1, botPlayer, alpha, beta, newHash);

            undoMove(board, r, col);

            if (val < best) best = val;
            if (best < beta) beta = best;
            if (alpha >= beta) break; // Alpha cutoff
        }
        
        char flag = (best <= alpha) ? 2 : (best >= beta) ? 1 : 0;
        storeTT(hash, depth, best, flag);
        return best;
    }
}