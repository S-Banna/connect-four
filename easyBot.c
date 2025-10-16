#include <stdlib.h>
#include <time.h>
#include "easyBot.h"

int easyBot(int maxColumn, int minColumn) {
    srand(time(NULL));
    int rand_num = (rand() % (maxColumn - minColumn + 1)) + minColumn;
    return rand_num;
}