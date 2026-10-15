#ifndef DEFINES_H
#define DEFINES_H

#define LEVEL_CNT_MAX 10

#define ROWS_MAP 24
#define COLS_MAP 10

#define SUCCESS 0
#define ERROR 1

#define MAX_SPEED 50
#define MIN_SPEED 1000
#define SPEED_STEP ((MIN_SPEED - MAX_SPEED) / 9)
#define SPEED(speed) (MIN_SPEED - (SPEED_STEP * (speed - 1)))

#define FIGURE_COUNT 7
#define FIGURES_LIST                                                \
  {                                                                 \
    {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}},       \
        {{0, 0, 0, 0}, {0, 6, 0, 0}, {0, 6, 6, 6}, {0, 0, 0, 0}},   \
        {{0, 0, 0, 0}, {0, 0, 0, 3}, {0, 3, 3, 3}, {0, 0, 0, 0}},   \
        {{0, 0, 0, 0}, {0, 7, 7, 0}, {0, 7, 7, 0}, {0, 0, 0, 0}},   \
        {{0, 0, 0, 0}, {0, 0, 2, 2}, {0, 2, 2, 0}, {0, 0, 0, 0}},   \
        {{0, 0, 0, 0}, {0, 5, 5, 0}, {0, 0, 5, 5}, {0, 0, 0, 0}}, { \
      {0, 0, 0, 0}, {0, 0, 4, 0}, {0, 4, 4, 4}, { 0, 0, 0, 0 }      \
    }                                                               \
  }

#endif