#include "../../headers/cli/gui.h"

LibrarySet_t *lib_set(void) {
  static LibrarySet_t library_set;
  return &library_set;
}

char *proc_path(void) {
  static char process_path[PATH_MAX + FILENAME_MAX] = "";
  return process_path;
}

int get_procpath_size(void) { return PATH_MAX + FILENAME_MAX; }

GameStatus_t *gm_status(void) {
  static GameStatus_t game_status = Selecting;
  return &game_status;
}

void set_game_status(GameStatus_t status) {
  GameStatus_t *gm_st = gm_status();
  *gm_st = status;
}

GameStatus_t get_game_status(void) { return *(gm_status()); }

bool *is_colour(void) {
  static bool is_colour = false;
  return &is_colour;
}

void set_is_colour(bool state) {
  bool *is_cl = is_colour();
  *is_cl = state;
}

bool get_is_colour(void) { return *(is_colour()); }

int main(int argc, char **argv) {
  if ((argc > 0) && (get_gamefiles_path(argv[0]))) {
    GameFiles_t game_files_list;
    make_game_file_list(&game_files_list);
    if ((game_files_list.file_count != -1) &&
        (game_files_list.file_count != 0)) {
      WIN_INIT(50);
      init_colours();

      Button_t *buttons = (Button_t *)malloc(sizeof(Button_t) * BUTTONS_COUNT);
      define_buttons(buttons);

      select_and_game(&game_files_list, buttons);

      free(buttons);
      endwin();
    } else {
      if (game_files_list.file_count == -1) {
        printf("There is no directory %s, or can`t access.\n", GAME_FILES_DIR);
      } else {
        printf("There are no game libraries in directory %s.\n",
               GAME_FILES_DIR);
      }
    }
    free_game_file_list(game_files_list);
  } else {
    printf("Can`t find game file.\n");
  }
  return SUCCESS;
}

bool select_and_game(GameFiles_t *game_files_list, Button_t *buttons) {
  int selected_game = 0;
  HiScoreList_t hiscore_list;
  read_hiscore_list(&hiscore_list);
  LibrarySet_t *lib = lib_set();

  while (get_game_status() != Exit) {
    switch (GET_USER_INPUT) {
      // case 's':
      // case 'S':
      case ENTER_KEY:
        action_start_button(
            buttons, game_files_list->game_start_info[selected_game].file_name);
        break;
      case 'p':
      case 'P':
        action_pause_button(buttons, game_files_list, selected_game);
        break;
      case 'q':
      case 'Q':
        // case ESCAPE:
        action_quit_button(buttons, &hiscore_list, game_files_list,
                           selected_game);
        break;
      case KEY_LEFT:
        action_left_button(buttons, &selected_game);
        break;
      case KEY_RIGHT:
        action_right_button(buttons, &selected_game,
                            game_files_list->file_count);
        break;
      case KEY_UP:
        action_up_button(buttons);
        break;
      case KEY_DOWN:
        action_down_button(buttons);
        break;
      case ' ':
        action_action_button(buttons);
        break;
    }
    if ((get_game_status() != GameOver) && (get_game_status() != Paused) &&
        (get_game_status() != HiScoreWriteErr)) {
      if (get_game_status() == Started) {
        GameInfo_t curr_state = lib->updateCurrentState();
        if (curr_state.level == -1) {
          lib->userInput(Terminate, true);
          set_game_status(GameOver);
          draw_select_game_interface(
              game_files_list->game_start_info[selected_game], buttons);
        } else {
          update_current_hiscore(
              &hiscore_list,
              game_files_list->game_start_info[selected_game].game_name,
              curr_state.score);
          draw_curr_game_interface(
              curr_state, buttons,
              get_current_hiscore(
                  &hiscore_list,
                  game_files_list->game_start_info[selected_game].game_name));
        }
      }
      if (get_game_status() == Selecting)
        draw_select_game_interface(
            game_files_list->game_start_info[selected_game], buttons);
    }
  }
  free_hiscore_list(&hiscore_list);
  return true;
}

void action_start_button(Button_t *buttons, char *file_name) {
  if (get_game_status() == Selecting) {
    if (load_library(proc_path(), file_name, false, NULL) == 0) {
      LibrarySet_t *lib = lib_set();
      lib->userInput(Start, true);
      set_game_status(Started);
    }
  }
  if ((get_game_status() != GameOver) && (get_game_status() != Paused) &&
      (get_game_status() != HiScoreWriteErr))
    prepare_control_grid(buttons, Start);
}

void action_pause_button(Button_t *buttons, GameFiles_t *game_files_list,
                         int selected_game) {
  if (get_game_status() == Started) {
    LibrarySet_t *lib = lib_set();
    lib->userInput(Pause, true);
    set_game_status(Paused);
    draw_select_game_interface(game_files_list->game_start_info[selected_game],
                               buttons);
  }
  if ((get_game_status() != GameOver) && (get_game_status() != Paused) &&
      (get_game_status() != HiScoreWriteErr))
    prepare_control_grid(buttons, Pause);
}

void action_quit_button(Button_t *buttons, HiScoreList_t *hiscore_list,
                        GameFiles_t *game_files_list, int selected_game) {
  if (get_game_status() == Started) {
    LibrarySet_t *lib = lib_set();
    lib->userInput(Terminate, true);
    dlclose(lib->lib);
    set_game_status(Selecting);
  } else if (get_game_status() == Selecting) {
    if (!write_hiscore_list(hiscore_list)) {
      set_game_status(HiScoreWriteErr);
      draw_select_game_interface(
          game_files_list->game_start_info[selected_game], buttons);
    } else {
      set_game_status(Exit);
    }
  }
  if ((get_game_status() != GameOver) && (get_game_status() != Paused) &&
      (get_game_status() != HiScoreWriteErr))
    prepare_control_grid(buttons, Terminate);
}

void action_left_button(Button_t *buttons, int *selected_game) {
  if (get_game_status() == Selecting) {
    if (*selected_game > 0) {
      *selected_game = *selected_game - 1;
    }
  }
  if (get_game_status() == Started) {
    LibrarySet_t *lib = lib_set();
    lib->userInput(Left, true);
  }
  if ((get_game_status() != GameOver) && (get_game_status() != Paused) &&
      (get_game_status() != HiScoreWriteErr))
    prepare_control_grid(buttons, Left);
}

void action_right_button(Button_t *buttons, int *selected_game,
                         int file_count) {
  if (get_game_status() == Selecting) {
    if (*selected_game < file_count - 1) {
      *selected_game = *selected_game + 1;
    }
  }
  if (get_game_status() == Started) {
    LibrarySet_t *lib = lib_set();
    lib->userInput(Right, true);
  }
  if ((get_game_status() != GameOver) && (get_game_status() != Paused) &&
      (get_game_status() != HiScoreWriteErr))
    prepare_control_grid(buttons, Right);
}

void action_up_button(Button_t *buttons) {
  if (get_game_status() == Started) {
    LibrarySet_t *lib = lib_set();
    lib->userInput(Up, true);
  }
  if ((get_game_status() != GameOver) && (get_game_status() != Paused) &&
      (get_game_status() != HiScoreWriteErr))
    prepare_control_grid(buttons, Up);
}

void action_down_button(Button_t *buttons) {
  if (get_game_status() == Started) {
    LibrarySet_t *lib = lib_set();
    lib->userInput(Down, true);
  }
  if ((get_game_status() != GameOver) && (get_game_status() != Paused) &&
      (get_game_status() != HiScoreWriteErr))
    prepare_control_grid(buttons, Down);
}

void action_action_button(Button_t *buttons) {
  if (get_game_status() == Started) {
    LibrarySet_t *lib = lib_set();
    lib->userInput(Action, true);
  }
  if (get_game_status() == Paused) {
    set_game_status(Started);
    LibrarySet_t *lib = lib_set();
    lib->userInput(Pause, true);
    clear();
  }
  if (get_game_status() == HiScoreWriteErr) {
    set_game_status(Exit);
  }
  if (get_game_status() == GameOver) {
    set_game_status(Selecting);
    clear();
  }
  prepare_control_grid(buttons, Action);
}

void make_game_file_list(GameFiles_t *game_files_list) {
  game_files_list->game_start_info =
      (struct GameStartInfo_t *)malloc(sizeof(struct GameStartInfo_t) * 0);
  int res = 0;
  DIR *dr = opendir(proc_path());
  if (dr != NULL) {  // opendir returns NULL if couldn't open directory
    struct dirent *find_files;
    while ((find_files = readdir(dr)) != NULL) {
      if ((find_files->d_type == DT_REG) &&
          (strcmp(find_files->d_name, HISCORE_FILE_NAME) != 0)) {
        GameStartInfo_t cur_file;
        if (load_library(proc_path(), find_files->d_name, true, &cur_file) ==
            0) {
          res++;
          game_files_list->game_start_info = (struct GameStartInfo_t *)realloc(
              game_files_list->game_start_info,
              sizeof(struct GameStartInfo_t) * (res));
          game_files_list->game_start_info[res - 1] = cur_file;
        }
      }
    }
    closedir(dr);
  } else {
    res = -1;
  }
  game_files_list->file_count = res;
}

int load_library(char *file_path, char *file_name, bool check,
                 GameStartInfo_t *file_info) {
  int res = 0;
  char filename[PATH_MAX + FILENAME_MAX];
  snprintf(filename, sizeof(filename), "%s%s", file_path, file_name);
  LibrarySet_t *lib = lib_set();
  lib->lib = dlopen(filename, RTLD_LAZY);

  if (!lib->lib) {
    res = 1;
  } else {
    *(void **)(&(lib->updateCurrentState)) =
        dlsym(lib->lib, "updateCurrentState");
    if (dlerror() != NULL) {
      dlclose(lib->lib);
      res = 2;
    }
    if (res == 0) {
      *(void **)(&(lib->userInput)) = dlsym(lib->lib, "userInput");
      if (dlerror() != NULL) {
        dlclose(lib->lib);
        res = 2;
      }
    }
    if ((res == 0) && (check)) {
      *(void **)(&(lib->getGameStartInfo)) =
          dlsym(lib->lib, "getGameStartInfo");
      if (dlerror() != NULL) {
        strcpy(file_info->file_name, file_name);
        strcpy(file_info->game_name, file_name);
        for (int i = 0; i < 20; i++) {
          for (int j = 0; j < 10; j++) {
            file_info->start_screen[i][j] = 0;
          }
        }
      } else {
        *file_info = lib->getGameStartInfo();
        strcpy(file_info->file_name, file_name);
      }
      dlclose(lib->lib);
    }
  }

  return res;
}

void free_game_file_list(GameFiles_t game_files_list) {
  free(game_files_list.game_start_info);
}

bool read_hiscore_list(HiScoreList_t *hiscore_list) {
  bool res = false;
  hiscore_list->record_count = 0;
  hiscore_list->records =
      (struct HiScoreRecord_t *)malloc(sizeof(struct HiScoreRecord_t) * 0);

  char filename[PATH_MAX + (FILENAME_MAX * 2)];
  snprintf(filename, sizeof(filename), "%s%s", proc_path(), HISCORE_FILE_NAME);
  FILE *ptrFile = fopen(filename, "rb");

  if (ptrFile != NULL) {
    if (fread(&(hiscore_list->record_count), sizeof(int), 1, ptrFile) == 1) {
      res = true;
      hiscore_list->records = (struct HiScoreRecord_t *)realloc(
          hiscore_list->records,
          sizeof(struct HiScoreRecord_t) * hiscore_list->record_count);
      size_t status = 1;
      for (int i = 0; i < hiscore_list->record_count && status == 1; i++) {
        status = fread(&(hiscore_list->records[i]),
                       sizeof(struct HiScoreRecord_t), 1, ptrFile);
      }
      if (status != 1) {
        free(hiscore_list->records);
        hiscore_list->records = (struct HiScoreRecord_t *)malloc(
            sizeof(struct HiScoreRecord_t) * 0);
        hiscore_list->record_count = 0;
        res = false;
      }
    }
    fclose(ptrFile);
  }
  return res;
}

bool write_hiscore_list(HiScoreList_t *hiscore_list) {
  if (hiscore_list->record_count < 1) {
    return false;
  }

  bool res = false;
  char filename[PATH_MAX + (FILENAME_MAX * 2)];
  snprintf(filename, sizeof(filename), "%s%s", proc_path(), HISCORE_FILE_NAME);
  FILE *ptrFile = fopen(filename, "wb");

  if (ptrFile != NULL) {
    if (fwrite(&(hiscore_list->record_count), sizeof(int), 1, ptrFile) == 1) {
      res = true;
      for (int i = 0; i < hiscore_list->record_count && res; i++) {
        res = (fwrite(&(hiscore_list->records[i]),
                      sizeof(struct HiScoreRecord_t), 1, ptrFile) == 1);
      }
    }
    fclose(ptrFile);
    if (res == false) {
      FILE *ptrFile = fopen(filename, "wb");
      fclose(ptrFile);
    }
  }
  return res;
}

void free_hiscore_list(HiScoreList_t *hiscore_list) {
  free(hiscore_list->records);
  hiscore_list->record_count = 0;
}

int get_current_hiscore(HiScoreList_t *hiscore_list, char *g_name) {
  int res = 0;
  for (int i = 0, status = 1; i < hiscore_list->record_count && status; i++) {
    if (strcmp(hiscore_list->records[i].game_name, g_name) == 0) {
      res = hiscore_list->records[i].score;
      status = 0;
    }
  }
  return res;
}

bool update_current_hiscore(HiScoreList_t *hiscore_list, char *g_name,
                            int g_score) {
  bool res = false;
  for (int i = 0, status = 1; i < hiscore_list->record_count && status; i++) {
    if (strcmp(hiscore_list->records[i].game_name, g_name) == 0) {
      if (hiscore_list->records[i].score <= g_score)
        hiscore_list->records[i].score = g_score;
      status = 0;
      res = true;
    }
  }
  if (!res) {
    hiscore_list->record_count = hiscore_list->record_count + 1;
    hiscore_list->records = (struct HiScoreRecord_t *)realloc(
        hiscore_list->records,
        sizeof(struct HiScoreRecord_t) * hiscore_list->record_count);
    snprintf(
        hiscore_list->records[hiscore_list->record_count - 1].game_name,
        sizeof(hiscore_list->records[hiscore_list->record_count - 1].game_name),
        "%s", g_name);
    hiscore_list->records[hiscore_list->record_count - 1].score = g_score;
    res = true;
  }
  return res;
}

void define_buttons(Button_t *buttons) {
  buttons[0].b_char = 'S';
  buttons[0].b_type = 0;
  buttons[0].b_start_y = 2;
  buttons[0].b_start_x = 14;
  buttons[0].b_count = 0;

  buttons[1].b_char = 'P';
  buttons[1].b_type = 0;
  buttons[1].b_start_y = 2;
  buttons[1].b_start_x = 8;
  buttons[1].b_count = 0;

  buttons[2].b_char = 'Q';
  buttons[2].b_type = 0;
  buttons[2].b_start_y = 2;
  buttons[2].b_start_x = 2;
  buttons[2].b_count = 0;

  buttons[3].b_char = ACS_LARROW;
  buttons[3].b_type = 1;
  buttons[3].b_start_y = 4;
  buttons[3].b_start_x = 26;
  buttons[3].b_count = 0;

  buttons[4].b_char = ACS_RARROW;
  buttons[4].b_type = 1;
  buttons[4].b_start_y = 4;
  buttons[4].b_start_x = 34;
  buttons[4].b_count = 0;

  buttons[5].b_char = ACS_UARROW;
  buttons[5].b_type = 1;
  buttons[5].b_start_y = 2;
  buttons[5].b_start_x = 30;
  buttons[5].b_count = 0;

  buttons[6].b_char = ACS_DARROW;
  buttons[6].b_type = 1;
  buttons[6].b_start_y = 6;
  buttons[6].b_start_x = 30;
  buttons[6].b_count = 0;

  buttons[7].b_char = ' ';
  buttons[7].b_type = 2;
  buttons[7].b_start_y = 4;
  buttons[7].b_start_x = 20;
  buttons[7].b_count = 0;
}

void init_colours(void) {
  if (has_colors()) {
    set_is_colour(true);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    init_pair(8, COLOR_BLACK, COLOR_WHITE);
  } else {
    set_is_colour(false);
  }
}

void draw_curr_game_interface(GameInfo_t GameInfo, Button_t *buttons,
                              int hi_score) {
  int game_field[ROWS_MAP][COLS_MAP];
  for (int x = 0; x < ROWS_MAP; x++) {
    for (int y = 0; y < COLS_MAP; y++) {
      game_field[x][y] = GameInfo.field[x][y];
    }
  }
  prepare_game_grid(game_field);
  prepare_score_grid(GameInfo.score, "");
  prepare_hiscore_grid(hi_score);
  int next_field[4][4];
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      next_field[x][y] = GameInfo.next[x][y];
    }
  }
  prepare_next_grid(next_field);
  prepare_level_grid(GameInfo.level);
  prepare_control_grid(buttons, -1);
  if ((int)get_game_status() >= 3) {
    prepare_info_grid();
  }
}

void draw_select_game_interface(GameStartInfo_t game_start_info,
                                Button_t *buttons) {
  prepare_game_grid(game_start_info.start_screen);
  prepare_score_grid(0, game_start_info.game_name);
  prepare_hiscore_grid(0);
  int next_field[4][4] = {0};
  prepare_next_grid(next_field);
  prepare_level_grid(0);
  prepare_control_grid(buttons, -1);
  if ((int)get_game_status() >= 3) {
    prepare_info_grid();
  }
}

void prepare_game_grid(int game_field[ROWS_MAP][COLS_MAP]) {
  WINDOW *game_grid =
      newwin(GAME_GRID_ROWS, GAME_GRID_COLS, MIN_ROWS_COLS, MIN_ROWS_COLS * 2);
  box(game_grid, 0, 0);
  for (int i = 0; i < ROWS_MAP; i++) {
    for (int j = 0; j < COLS_MAP; j++) {
      if ((game_field[i][j] != 0) && (get_is_colour())) {
        int colour = game_field[i][j];
        if (colour < 0) {
          colour = 7;
        }
        if (colour > 7) {
          colour = 7;
        }
        mvwhline(game_grid, i + 1, (j * 2) + 1,
                 '[' | A_STANDOUT | COLOR_PAIR(colour), 1);
        mvwhline(game_grid, i + 1, (j * 2) + 2,
                 ']' | A_STANDOUT | COLOR_PAIR(colour), 1);
      }
      if ((game_field[i][j] != 0) && (!get_is_colour())) {
        mvwhline(game_grid, i + 1, (j * 2) + 1, '[' | A_STANDOUT, 1);
        mvwhline(game_grid, i + 1, (j * 2) + 2, ']' | A_STANDOUT, 1);
      }
    }
  }
  wrefresh(game_grid);
  delwin(game_grid);
}

void prepare_score_grid(int score, char *gamename) {
  WINDOW *score_grid = newwin(SCORE_GRID_ROWS, ADD_GRID_COLS,
                              SCORE_GRID_START_ROW, SCORE_GRID_START_COL);
  box(score_grid, 0, 0);
  if (get_game_status() == Selecting) {
    mvwprintw(score_grid, 1, 1, "%s", gamename);
  } else {
    mvwprintw(score_grid, 1, 7, "SCORE");
    if (score > 1999999999) {
      score = 1999999999;
    }
    mvwprintw(score_grid, 2, 2, "%10d", score);
  }
  wrefresh(score_grid);
  delwin(score_grid);
}

void prepare_hiscore_grid(int hiscore) {
  WINDOW *hiscore_grid = newwin(HISCORE_GRID_ROWS, ADD_GRID_COLS,
                                HISCORE_GRID_START_ROW, HISCORE_GRID_START_COL);
  box(hiscore_grid, 0, 0);
  if (get_game_status() == Selecting) {
    wattron(hiscore_grid, A_BLINK);
    // mvwprintw(hiscore_grid, 1, 2, "Press 'S'");
    mvwprintw(hiscore_grid, 1, 2, "Press ENTER");
    mvwprintw(hiscore_grid, 2, 3, "to start.");
    wattroff(hiscore_grid, A_BLINK);
  } else {
    mvwprintw(hiscore_grid, 1, 4, "HI-SCORE");
    if (hiscore > 1999999999) {
      hiscore = 1999999999;
    }
    mvwprintw(hiscore_grid, 2, 2, "%10d", hiscore);
  }
  wrefresh(hiscore_grid);
  delwin(hiscore_grid);
}

void prepare_next_grid(int next_field[4][4]) {
  WINDOW *next_grid = newwin(NEXT_GRID_ROWS, ADD_GRID_COLS, NEXT_GRID_START_ROW,
                             NEXT_GRID_START_COL);
  box(next_grid, 0, 0);
  mvwprintw(next_grid, 1, 8, "NEXT");
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if ((next_field[i][j] != 0) && (get_is_colour())) {
        int colour = next_field[i][j];
        if (colour < 0) {
          colour = 7;
        }
        if (colour > 7) {
          colour = 7;
        }
        mvwhline(next_grid, i + 2, (j * 2) + 3,
                 '[' | A_STANDOUT | COLOR_PAIR(colour), 1);
        mvwhline(next_grid, i + 2, (j * 2) + 4,
                 ']' | A_STANDOUT | COLOR_PAIR(colour), 1);
      }
      if ((next_field[i][j] != 0) && (!get_is_colour())) {
        mvwhline(next_grid, i + 2, (j * 2) + 3, '[' | A_STANDOUT, 1);
        mvwhline(next_grid, i + 2, (j * 2) + 4, ']' | A_STANDOUT, 1);
      }
    }
  }
  wrefresh(next_grid);
  delwin(next_grid);
}

void prepare_level_grid(int level) {
  WINDOW *level_grid = newwin(LEVEL_GRID_ROWS, ADD_GRID_COLS,
                              LEVEL_GRID_START_ROW, LEVEL_GRID_START_COL);
  box(level_grid, 0, 0);
  mvwprintw(level_grid, 1, 7, "LEVEL");
  if (get_game_status() == Started) {
    mvwprintw(level_grid, 2, 2, "%10d", level);
  }
  wrefresh(level_grid);
  delwin(level_grid);
}

void prepare_control_grid(Button_t *buttons, UserAction_t action) {
  WINDOW *control_grid = newwin(CONTROL_GRID_ROWS, CONTROL_GRID_COLS,
                                CONTROL_GRID_START_ROW, CONTROL_GRID_START_COL);
  box(control_grid, 0, 0);

  if ((int)action >= 0) {
    buttons[(int)action].b_count = 5;
  }
  for (int i = 0; i < BUTTONS_COUNT; i++) {
    if (buttons[i].b_type == 0) {
      if (buttons[i].b_count > 0) {
        if (get_is_colour()) {
          DRAW_ADD_BUTTON_COLOUR(control_grid, 8, buttons[i].b_start_y,
                                 buttons[i].b_start_x, buttons[i].b_char);
        } else {
          DRAW_ADD_BUTTON(control_grid, A_DIM, buttons[i].b_start_y,
                          buttons[i].b_start_x, buttons[i].b_char);
        }
        buttons[i].b_count = buttons[i].b_count - 1;
      } else {
        if (get_is_colour()) {
          DRAW_ADD_BUTTON_COLOUR(control_grid, 7, buttons[i].b_start_y,
                                 buttons[i].b_start_x, buttons[i].b_char);
        } else {
          DRAW_ADD_BUTTON(control_grid, A_BOLD, buttons[i].b_start_y,
                          buttons[i].b_start_x, buttons[i].b_char);
        }
      }
    }
    if (buttons[i].b_type == 1) {
      if (buttons[i].b_count > 0) {
        if (get_is_colour()) {
          DRAW_ARROW_BUTTON_COLOUR(control_grid, 8, buttons[i].b_start_y,
                                   buttons[i].b_start_x, buttons[i].b_char);
        } else {
          DRAW_ARROW_BUTTON(control_grid, A_DIM, buttons[i].b_start_y,
                            buttons[i].b_start_x, buttons[i].b_char);
        }
        buttons[i].b_count = buttons[i].b_count - 1;
      } else {
        if (get_is_colour()) {
          DRAW_ARROW_BUTTON_COLOUR(control_grid, 7, buttons[i].b_start_y,
                                   buttons[i].b_start_x, buttons[i].b_char);
        } else {
          DRAW_ARROW_BUTTON(control_grid, A_BOLD, buttons[i].b_start_y,
                            buttons[i].b_start_x, buttons[i].b_char);
        }
      }
    }
    if (buttons[i].b_type == 2) {
      if (buttons[i].b_count > 0) {
        if (get_is_colour()) {
          DRAW_SPACE_BUTTON_COLOUR(control_grid, 8, buttons[i].b_start_y,
                                   buttons[i].b_start_x);
        } else {
          DRAW_SPACE_BUTTON(control_grid, A_DIM, buttons[i].b_start_y,
                            buttons[i].b_start_x);
        }
        buttons[i].b_count = buttons[i].b_count - 1;
      } else {
        if (get_is_colour()) {
          DRAW_SPACE_BUTTON_COLOUR(control_grid, 7, buttons[i].b_start_y,
                                   buttons[i].b_start_x);
        } else {
          DRAW_SPACE_BUTTON(control_grid, A_BOLD, buttons[i].b_start_y,
                            buttons[i].b_start_x);
        }
      }
    }
  }

  mvwprintw(control_grid, 5, 2, "Quit");
  // mvwprintw(control_grid, 6, 2, "(ESC)");
  mvwprintw(control_grid, 5, 7, "Pause");
  mvwprintw(control_grid, 5, 13, "Start");
  mvwprintw(control_grid, 6, 12, "(ENTER)");
  mvwprintw(control_grid, 7, 19, "Action");
  mvwprintw(control_grid, 8, 19, "(SPACE)");
  wrefresh(control_grid);
  delwin(control_grid);
}

void prepare_info_grid(void) {
  WINDOW *info_grid = newwin(INFO_GRID_ROWS, INFO_GRID_COLS,
                             INFO_GRID_START_ROW, INFO_GRID_START_COL);
  box(info_grid, 0, 0);
  if (get_game_status() == Paused) {
    mvwprintw(info_grid, 2, (INFO_GRID_COLS / 2) - 2, "PAUSE");
  }
  if (get_game_status() == GameOver) {
    mvwprintw(info_grid, 2, (INFO_GRID_COLS / 2) - 5, "GAME OVER");
  }
  if (get_game_status() == HiScoreWriteErr) {
    mvwprintw(info_grid, 2, (INFO_GRID_COLS / 2) - 10, "CAN`T WRITE HI-SCORE");
  }
  wattron(info_grid, A_BLINK);
  mvwprintw(info_grid, 3, (INFO_GRID_COLS / 2) - 11, "Press SPACE to continue");
  wattroff(info_grid, A_BLINK);

  wrefresh(info_grid);
  delwin(info_grid);
}

bool get_gamefiles_path(char *argv_0) {
  bool res = false;
  if (argv_0[0] != '/') {
    if (getcwd(proc_path(), get_procpath_size()) != NULL) {
      strcat(proc_path(), "/");
      strcat(proc_path(), argv_0);
      res = true;
    }
  } else {
    strcpy(proc_path(), argv_0);
    res = true;
  }
  if (res) {
    int len = strlen(proc_path());
    for (int i = len - 1, check = 1; (i >= 0) && (check); i--) {
      if (proc_path()[i] == '/') {
        proc_path()[i + 1] = '\0';
        check = 0;
      }
    }
    strcat(proc_path(), GAME_FILES_DIR);
  }
  return res;
}