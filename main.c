#include "checkWin.h"
#include <stdio.h>
#include <stdlib.h>
#include "easyBot.h"
#define ROWS 6
#define COLS 7

int** allocBoard(int rows, int cols);
void freeBoard(int** board, int rows);
void resetBoard(int** board, int rows, int cols);
void printBoard(int** board, int rows, int cols);
int isValidColumn(int** board, int rows, int cols, int col);
int dropChecker(int** board, int rows, int col, int player, int* outRow);

int main(void) {
    int** board = allocBoard(ROWS, COLS);
    if (!board)
        return 1;

    int currentPlayer;
    int moves;
    int won;
    char again;

    do {
        resetBoard(board, ROWS, COLS);
        currentPlayer = 1;
        moves = 0;
        won = 0;

        while (moves < ROWS * COLS && !won) {
            printBoard(board, ROWS, COLS);

            char playerChar;
            if (currentPlayer == 1) {
                playerChar = 'A';
            } else if (currentPlayer == 2) {
                playerChar = 'B';
            }

            int colInput;
            printf("Player %c, choose column (1-%d): ", playerChar, COLS);
            if (scanf("%d", &colInput) != 1) {
                freeBoard(board, ROWS);
                return 1;
            }

            int col = colInput - 1;
            if (!isValidColumn(board, ROWS, COLS, col)) {
                printf("Invalid column. Try again.\n");
                continue;
            }

            int placedRow;
            dropChecker(board, ROWS, col, currentPlayer, &placedRow);

            if (checkWin(board, placedRow, col, currentPlayer)) {
                printBoard(board, ROWS, COLS);
                printf("Player %c WINS!\n", playerChar);
                won = 1;
                break;
            }

            if (currentPlayer == 1) {
                currentPlayer = 2;
            } else {
                currentPlayer = 1;
            }

            moves++;
        }

        if (!won) {
            printBoard(board, ROWS, COLS);
            printf("It's a DRAW!\n");
        }

        printf("Play again? (y/n): ");
        scanf(" %c", &again);

    } while (again == 'y' || again == 'Y');

    freeBoard(board, ROWS);
    return 0;
}

int** allocBoard(int rows, int cols) {
    int** board = (int**)calloc(rows, sizeof(int*));
    if (!board)
        return NULL;
    for (int r = 0; r < rows; r++) {
        board[r] = (int*)calloc(cols, sizeof(int));
        if (!board[r]) {
            for (int i = 0; i < r; i++)
                free(board[i]);
            free(board);
            return NULL;
        }
    }
    return board;
}

void freeBoard(int** board, int rows) {
    if (!board)
        return;
    for (int r = 0; r < rows; r++)
        free(board[r]);
    free(board);
}

void resetBoard(int** board, int rows, int cols) {
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            board[r][c] = 0;
}

void printBoard(int** board, int rows, int cols) {
    printf("\n");
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            char symbol;
            if (board[r][c] == 0)
                symbol = '.';
            else if (board[r][c] == 1)
                symbol = 'A';
            else
                symbol = 'B';
            printf("%c ", symbol);
        }
        printf("\n");
    }
    for (int c = 1; c <= cols; c++)
        printf("%d ", c);
    printf("\n\n");
}

int isValidColumn(int** board, int rows, int cols, int col) {
    if (col < 0 || col >= cols)
        return 0;
    return (board[0][col] == 0);
}

int dropChecker(int** board, int rows, int col, int player, int* outRow) {
    for (int r = rows - 1; r >= 0; r--) {
        if (board[r][col] == 0) {
            board[r][col] = player;
            if (outRow)
                *outRow = r;
            return 1;
        }
    }
    return 0;
}
