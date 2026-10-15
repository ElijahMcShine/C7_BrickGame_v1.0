#ifndef GUI_H
#define GUI_H

#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <limits.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include "../common/common_types.h"
#include "defines.h"

typedef enum {
  Selecting,
  Started,
  Exit,
  Paused,
  GameOver,
  HiScoreWriteErr
} GameStatus_t;

typedef struct {
  struct GameStartInfo_t *game_start_info;
  int file_count;
} GameFiles_t;

typedef struct HiScoreRecord_t {
  char game_name[256];
  int score;
} HiScoreRecord_t;

typedef struct {
  struct HiScoreRecord_t *records;
  int record_count;
} HiScoreList_t;

typedef struct Button_t {
  int b_type;
  int b_start_x;
  int b_start_y;
  int b_char;
  int b_count;
} Button_t;

typedef struct {
  void *lib;
  GameInfo_t (*updateCurrentState)(void);
  void (*userInput)(UserAction_t action, bool hold);
  GameStartInfo_t (*getGameStartInfo)(void);
} LibrarySet_t;

bool select_and_game(GameFiles_t *game_files_list, Button_t *buttons);
void action_start_button(Button_t *buttons, char *file_name);
void action_pause_button(Button_t *buttons, GameFiles_t *game_files_list,
                         int selected_game);
void action_quit_button(Button_t *buttons, HiScoreList_t *hiscore_list,
                        GameFiles_t *game_files_list, int selected_game);
void action_left_button(Button_t *buttons, int *selected_game);
void action_right_button(Button_t *buttons, int *selected_game, int file_count);
void action_up_button(Button_t *buttons);
void action_down_button(Button_t *buttons);
void action_action_button(Button_t *buttons);

void make_game_file_list(GameFiles_t *game_files_list);
int load_library(char *file_path, char *file_name, bool check,
                 GameStartInfo_t *file_info);
void free_game_file_list(GameFiles_t game_files_list);

bool read_hiscore_list(HiScoreList_t *hiscore_list);
void free_hiscore_list(HiScoreList_t *hiscore_list);
bool write_hiscore_list(HiScoreList_t *hiscore_list);
int get_current_hiscore(HiScoreList_t *hiscore_list, char *g_name);
bool update_current_hiscore(HiScoreList_t *hiscore_list, char *g_name,
                            int g_score);

void define_buttons(Button_t *buttons);
void init_colours(void);
void draw_curr_game_interface(GameInfo_t GameInfo, Button_t *buttons,
                              int hi_score);
void draw_select_game_interface(GameStartInfo_t game_start_info,
                                Button_t *buttons);

void prepare_game_grid(int game_field[ROWS_MAP][COLS_MAP]);
void prepare_score_grid(int score, char *gamename);
void prepare_hiscore_grid(int hiscore);
void prepare_next_grid(int next_field[4][4]);
void prepare_level_grid(int level);
void prepare_control_grid(Button_t *buttons, UserAction_t action);
void prepare_info_grid(void);

bool get_gamefiles_path(char *argv_0);

bool *is_colour(void);
void set_is_colour(bool state);
bool get_is_colour(void);
GameStatus_t *gm_status(void);
void set_game_status(GameStatus_t status);
GameStatus_t get_game_status(void);

LibrarySet_t *lib_set(void);

#endif