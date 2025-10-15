#include <stdlib.h>
#include <time.h>
#include "easyBot.h"

int easyBot(int maxcolumn, int mincolumn) {
    int rand_num = (rand() % (maxcolumn - mincolumn + 1)) + mincolumn;
    return rand_num;
}