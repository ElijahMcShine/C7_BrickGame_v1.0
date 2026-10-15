#ifndef DEFINES_H
#define DEFINES_H

#define WIN_INIT(time)    \
  {                       \
    initscr();            \
    noecho();             \
    curs_set(0);          \
    keypad(stdscr, TRUE); \
    timeout(time);        \
  }

#define GET_USER_INPUT getch()

#define GAME_FILES_DIR "gamefiles/"
#define HISCORE_FILE_NAME "hiscore"

#define LEVEL_CNT_MAX 10

#define ROWS_MAP 20
#define COLS_MAP 10

#define MIN_ROWS_COLS 3

#define GAME_GRID_ROWS (ROWS_MAP + 2)
#define GAME_GRID_COLS ((COLS_MAP * 2) + 2)

#define ADD_GRID_COLS (14)

#define SCORE_GRID_ROWS (4)
#define HISCORE_GRID_ROWS (4)
#define NEXT_GRID_ROWS (7)
#define LEVEL_GRID_ROWS (4)

#define CONTROL_GRID_ROWS (11)
#define CONTROL_GRID_COLS (MIN_ROWS_COLS + GAME_GRID_COLS + ADD_GRID_COLS)

#define GAME_GRID_START_ROW MIN_ROWS_COLS
#define GAME_GRID_START_COL (MIN_ROWS_COLS * 2)

#define SCORE_GRID_START_ROW MIN_ROWS_COLS
#define SCORE_GRID_START_COL ((MIN_ROWS_COLS * 3) + GAME_GRID_COLS)

#define HISCORE_GRID_START_ROW (MIN_ROWS_COLS + SCORE_GRID_ROWS + 1)
#define HISCORE_GRID_START_COL ((MIN_ROWS_COLS * 3) + GAME_GRID_COLS)

#define NEXT_GRID_START_ROW (HISCORE_GRID_START_ROW + HISCORE_GRID_ROWS + 1)
#define NEXT_GRID_START_COL ((MIN_ROWS_COLS * 3) + GAME_GRID_COLS)

#define LEVEL_GRID_START_ROW (NEXT_GRID_START_ROW + NEXT_GRID_ROWS + 1)
#define LEVEL_GRID_START_COL ((MIN_ROWS_COLS * 3) + GAME_GRID_COLS)

#define CONTROL_GRID_START_ROW (MIN_ROWS_COLS + GAME_GRID_ROWS + 1)
#define CONTROL_GRID_START_COL (MIN_ROWS_COLS * 2)

#define INFO_GRID_ROWS 6
#define INFO_GRID_COLS 25

#define INFO_GRID_START_ROW \
  (((CONTROL_GRID_START_ROW + CONTROL_GRID_ROWS) / 2) - (INFO_GRID_ROWS / 2))
#define INFO_GRID_START_COL \
  (((SCORE_GRID_START_COL + ADD_GRID_COLS) / 2) - (INFO_GRID_COLS / 2) + 3)

#define NO_INPUT -1

#define ESCAPE 27
#define ENTER_KEY 10

#define BUTTONS_COUNT 8

#define DRAW_ADD_BUTTON_COLOUR(box, colour, start_y, start_x, ch) \
  {                                                               \
    mvwhline(box, start_y, start_x,                               \
             ACS_ULCORNER | A_STANDOUT | COLOR_PAIR(colour), 1);  \
    mvwhline(box, start_y, start_x + 1,                           \
             ACS_HLINE | A_STANDOUT | COLOR_PAIR(colour), 1);     \
    mvwhline(box, start_y, start_x + 2,                           \
             ACS_HLINE | A_STANDOUT | COLOR_PAIR(colour), 1);     \
    mvwhline(box, start_y, start_x + 3,                           \
             ACS_URCORNER | A_STANDOUT | COLOR_PAIR(colour), 1);  \
    mvwhline(box, start_y + 1, start_x,                           \
             ACS_VLINE | A_STANDOUT | COLOR_PAIR(colour), 1);     \
    mvwhline(box, start_y + 1, start_x + 1,                       \
             ch | A_STANDOUT | COLOR_PAIR(colour), 1);            \
    mvwhline(box, start_y + 1, start_x + 2,                       \
             ' ' | A_STANDOUT | COLOR_PAIR(colour), 1);           \
    mvwhline(box, start_y + 1, start_x + 3,                       \
             ACS_VLINE | A_STANDOUT | COLOR_PAIR(colour), 1);     \
    mvwhline(box, start_y + 2, start_x,                           \
             ACS_LLCORNER | A_STANDOUT | COLOR_PAIR(colour), 1);  \
    mvwhline(box, start_y + 2, start_x + 1,                       \
             ACS_HLINE | A_STANDOUT | COLOR_PAIR(colour), 1);     \
    mvwhline(box, start_y + 2, start_x + 2,                       \
             ACS_HLINE | A_STANDOUT | COLOR_PAIR(colour), 1);     \
    mvwhline(box, start_y + 2, start_x + 3,                       \
             ACS_LRCORNER | A_STANDOUT | COLOR_PAIR(colour), 1);  \
  }

#define DRAW_ARROW_BUTTON_COLOUR(box, colour, start_y, start_x, ch) \
  {                                                                 \
    mvwhline(box, start_y, start_x,                                 \
             ACS_ULCORNER | A_STANDOUT | COLOR_PAIR(colour), 1);    \
    mvwhline(box, start_y, start_x + 1,                             \
             ACS_HLINE | A_STANDOUT | COLOR_PAIR(colour), 1);       \
    mvwhline(box, start_y, start_x + 2,                             \
             ACS_URCORNER | A_STANDOUT | COLOR_PAIR(colour), 1);    \
    mvwhline(box, start_y + 1, start_x,                             \
             ACS_VLINE | A_STANDOUT | COLOR_PAIR(colour), 1);       \
    mvwhline(box, start_y + 1, start_x + 1,                         \
             ch | A_STANDOUT | COLOR_PAIR(colour), 1);              \
    mvwhline(box, start_y + 1, start_x + 2,                         \
             ACS_VLINE | A_STANDOUT | COLOR_PAIR(colour), 1);       \
    mvwhline(box, start_y + 2, start_x,                             \
             ACS_LLCORNER | A_STANDOUT | COLOR_PAIR(colour), 1);    \
    mvwhline(box, start_y + 2, start_x + 1,                         \
             ACS_HLINE | A_STANDOUT | COLOR_PAIR(colour), 1);       \
    mvwhline(box, start_y + 2, start_x + 2,                         \
             ACS_LRCORNER | A_STANDOUT | COLOR_PAIR(colour), 1);    \
  }

#define DRAW_SPACE_BUTTON_COLOUR(box, colour, start_y, start_x)  \
  {                                                              \
    mvwhline(box, start_y, start_x,                              \
             ACS_ULCORNER | A_STANDOUT | COLOR_PAIR(colour), 1); \
    mvwhline(box, start_y, start_x + 1,                          \
             ACS_HLINE | A_STANDOUT | COLOR_PAIR(colour), 1);    \
    mvwhline(box, start_y, start_x + 2,                          \
             ACS_HLINE | A_STANDOUT | COLOR_PAIR(colour), 1);    \
    mvwhline(box, start_y, start_x + 3,                          \
             ACS_HLINE | A_STANDOUT | COLOR_PAIR(colour), 1);    \
    mvwhline(box, start_y, start_x + 4,                          \
             ACS_URCORNER | A_STANDOUT | COLOR_PAIR(colour), 1); \
    mvwhline(box, start_y + 1, start_x,                          \
             ACS_VLINE | A_STANDOUT | COLOR_PAIR(colour), 1);    \
    mvwhline(box, start_y + 1, start_x + 1,                      \
             ' ' | A_STANDOUT | COLOR_PAIR(colour), 1);          \
    mvwhline(box, start_y + 1, start_x + 2,                      \
             ' ' | A_STANDOUT | COLOR_PAIR(colour), 1);          \
    mvwhline(box, start_y + 1, start_x + 3,                      \
             ' ' | A_STANDOUT | COLOR_PAIR(colour), 1);          \
    mvwhline(box, start_y + 1, start_x + 4,                      \
             ACS_VLINE | A_STANDOUT | COLOR_PAIR(colour), 1);    \
    mvwhline(box, start_y + 2, start_x,                          \
             ACS_LLCORNER | A_STANDOUT | COLOR_PAIR(colour), 1); \
    mvwhline(box, start_y + 2, start_x + 1,                      \
             ACS_HLINE | A_STANDOUT | COLOR_PAIR(colour), 1);    \
    mvwhline(box, start_y + 2, start_x + 2,                      \
             ACS_HLINE | A_STANDOUT | COLOR_PAIR(colour), 1);    \
    mvwhline(box, start_y + 2, start_x + 3,                      \
             ACS_HLINE | A_STANDOUT | COLOR_PAIR(colour), 1);    \
    mvwhline(box, start_y + 2, start_x + 4,                      \
             ACS_LRCORNER | A_STANDOUT | COLOR_PAIR(colour), 1); \
  }

#define DRAW_ADD_BUTTON(box, state, start_y, start_x, ch)             \
  {                                                                   \
    mvwhline(box, start_y, start_x, ACS_ULCORNER | state, 1);         \
    mvwhline(box, start_y, start_x + 1, ACS_HLINE | state, 1);        \
    mvwhline(box, start_y, start_x + 2, ACS_HLINE | state, 1);        \
    mvwhline(box, start_y, start_x + 3, ACS_URCORNER | state, 1);     \
    mvwhline(box, start_y + 1, start_x, ACS_VLINE | state, 1);        \
    mvwhline(box, start_y + 1, start_x + 1, ch | state, 1);           \
    mvwhline(box, start_y + 1, start_x + 2, ' ' | state, 1);          \
    mvwhline(box, start_y + 1, start_x + 3, ACS_VLINE | state, 1);    \
    mvwhline(box, start_y + 2, start_x, ACS_LLCORNER | state, 1);     \
    mvwhline(box, start_y + 2, start_x + 1, ACS_HLINE | state, 1);    \
    mvwhline(box, start_y + 2, start_x + 2, ACS_HLINE | state, 1);    \
    mvwhline(box, start_y + 2, start_x + 3, ACS_LRCORNER | state, 1); \
  }

#define DRAW_ARROW_BUTTON(box, state, start_y, start_x, ch)           \
  {                                                                   \
    mvwhline(box, start_y, start_x, ACS_ULCORNER | state, 1);         \
    mvwhline(box, start_y, start_x + 1, ACS_HLINE | state, 1);        \
    mvwhline(box, start_y, start_x + 2, ACS_URCORNER | state, 1);     \
    mvwhline(box, start_y + 1, start_x, ACS_VLINE | state, 1);        \
    mvwhline(box, start_y + 1, start_x + 1, ch | state, 1);           \
    mvwhline(box, start_y + 1, start_x + 2, ACS_VLINE | state, 1);    \
    mvwhline(box, start_y + 2, start_x, ACS_LLCORNER | state, 1);     \
    mvwhline(box, start_y + 2, start_x + 1, ACS_HLINE | state, 1);    \
    mvwhline(box, start_y + 2, start_x + 2, ACS_LRCORNER | state, 1); \
  }

#define DRAW_SPACE_BUTTON(box, state, start_y, start_x)               \
  {                                                                   \
    mvwhline(box, start_y, start_x, ACS_ULCORNER | state, 1);         \
    mvwhline(box, start_y, start_x + 1, ACS_HLINE | state, 1);        \
    mvwhline(box, start_y, start_x + 2, ACS_HLINE | state, 1);        \
    mvwhline(box, start_y, start_x + 3, ACS_HLINE | state, 1);        \
    mvwhline(box, start_y, start_x + 4, ACS_URCORNER | state, 1);     \
    mvwhline(box, start_y + 1, start_x, ACS_VLINE | state, 1);        \
    mvwhline(box, start_y + 1, start_x + 1, ' ' | state, 1);          \
    mvwhline(box, start_y + 1, start_x + 2, ' ' | state, 1);          \
    mvwhline(box, start_y + 1, start_x + 3, ' ' | state, 1);          \
    mvwhline(box, start_y + 1, start_x + 4, ACS_VLINE | state, 1);    \
    mvwhline(box, start_y + 2, start_x, ACS_LLCORNER | state, 1);     \
    mvwhline(box, start_y + 2, start_x + 1, ACS_HLINE | state, 1);    \
    mvwhline(box, start_y + 2, start_x + 2, ACS_HLINE | state, 1);    \
    mvwhline(box, start_y + 2, start_x + 3, ACS_HLINE | state, 1);    \
    mvwhline(box, start_y + 2, start_x + 4, ACS_LRCORNER | state, 1); \
  }

#endif