#include <stdlib.h>
#include <time.h>
#include "easyBot.h"

int easyBot(int maxColumn, int minColumn, int* validColumns) {
    srand(time(NULL));
    return validColumns[rand() % maxColumn + minColumn - 1];
}