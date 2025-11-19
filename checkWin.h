#include <pthread.h>

int checkWin(int**, int, int, int, int, int);
int checkDirection(int**, int, int, int, int, int, int, int);
void* checkDirectionThread(void* args);

typedef struct {
    int** board;
    int row;
    int column;
    int player;
    int ROWS;
    int COLS;
    int x;
    int y;
    int result;
} ThreadArgs;