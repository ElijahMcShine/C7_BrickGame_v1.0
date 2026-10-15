#include "../../headers/tetris/tetris.h"

static GameObject_t *game_obj(void) {
  static GameObject_t game_object;
  return &game_object;
}

static void stimer_sig_handler(int signo);
static void *execute_thread(void *arg);
static t_stimer *stimer_create(void);
static void stimer_destroy(t_stimer *timer);
static void stimer_set_interval(t_stimer *timer, int value);

static bool init_gameinfo(void);
static void fill_defaults(void);
static void free_gameobject(int status);
static void make_rand_next(void);
static void rotate_figure(void);
static void copy_next_to_current(void);
static void ontime_events(void);
static void prepare_next_iter(void);
static bool check_rotate(int test[4][4]);
static bool check_down(void);
static bool check_left(void);
static bool check_right(void);
static void action_pause(void);
static void action_left(void);
static void action_right(void);
static void action_down(void);
static void ramming_field(void);
static void ramming_field_0(void);
static void ramming_field_1(void);
static void ramming_field_10(void);
static void ramming_field_20(void);

GameStartInfo_t getGameStartInfo(void) {
  GameStartInfo_t res;
  strcpy(res.game_name, "Brick Game");
  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 10; x++) {
      res.start_screen[y][x] = 0;
    }
  }
  res.start_screen[11][4] = 3;
  res.start_screen[11][5] = 3;
  res.start_screen[12][5] = 3;
  res.start_screen[13][5] = 3;

  res.start_screen[17][0] = 7;
  res.start_screen[18][0] = 7;
  res.start_screen[17][1] = 7;
  res.start_screen[18][1] = 7;

  res.start_screen[19][0] = 1;
  res.start_screen[19][1] = 1;
  res.start_screen[19][2] = 1;
  res.start_screen[19][3] = 1;

  res.start_screen[18][2] = 6;
  res.start_screen[18][3] = 6;
  res.start_screen[18][4] = 6;
  res.start_screen[19][4] = 6;

  res.start_screen[18][6] = 5;
  res.start_screen[19][6] = 5;
  res.start_screen[17][7] = 5;
  res.start_screen[18][7] = 5;

  res.start_screen[18][8] = 4;
  res.start_screen[19][7] = 4;
  res.start_screen[19][8] = 4;
  res.start_screen[19][9] = 4;
  return res;
}

GameInfo_t updateCurrentState(void) {
  GameObject_t *game_object = game_obj();
  pthread_mutex_lock(&(game_object->state_lock));

  if (game_object->inited) {
    int temp[25][12];
    for (int i = 0; i < 25; i++) {
      for (int j = 0; j < 12; j++) {
        temp[i][j] = game_object->current_field[i][j];
      }
    }
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (game_object->current_figure[i][j])
          temp[i + game_object->current_figure_y]
              [j + game_object->current_figure_x] =
                  game_object->current_figure[i][j];
      }
    }
    for (int i = 0; i < 20; i++) {
      for (int j = 0; j < 10; j++) {
        game_object->GameInfo.field[i][j] = temp[i + 4][j + 1];
      }
    }
  }

  GameInfo_t result = game_object->GameInfo;
  pthread_mutex_unlock(&(game_object->state_lock));
  return result;
}

void userInput(UserAction_t action, bool hold) {
  GameObject_t *game_object = game_obj();
  (void)hold;  // unused parameter
  switch (action) {
    case Start:
      game_object->inited = init_gameinfo();
      break;
    case Pause:
      if (game_object->inited) {
        pthread_mutex_lock(&(game_object->state_lock));
        action_pause();
        pthread_mutex_unlock(&(game_object->state_lock));
      }
      break;
    case Terminate:
      if ((game_object->inited) && (!game_object->blocked)) free_gameobject(0);
      break;
    case Left:
      if ((game_object->inited) && (!game_object->blocked)) {
        pthread_mutex_lock(&(game_object->state_lock));
        action_left();
        pthread_mutex_unlock(&(game_object->state_lock));
      }
      break;
    case Right:
      if ((game_object->inited) && (!game_object->blocked)) {
        pthread_mutex_lock(&(game_object->state_lock));
        action_right();
        pthread_mutex_unlock(&(game_object->state_lock));
      }
      break;
    case Up:
      break;
    case Down:
      if ((game_object->inited) && (!game_object->blocked)) {
        pthread_mutex_lock(&(game_object->state_lock));
        action_down();
        pthread_mutex_unlock(&(game_object->state_lock));
      }
      break;
    case Action:
      if ((game_object->inited) && (!game_object->blocked)) {
        pthread_mutex_lock(&(game_object->state_lock));
        rotate_figure();
        pthread_mutex_unlock(&(game_object->state_lock));
      }
      break;
  }
}

static void fill_defaults(void) {
  GameObject_t *game_object = game_obj();
  game_object->GameInfo.score = 0;
  game_object->GameInfo.high_score = 0;
  game_object->GameInfo.level = 1;
  game_object->GameInfo.speed = 1;
  game_object->GameInfo.pause = 0;

  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 10; x++) {
      game_object->GameInfo.field[y][x] = 0;
    }
  }
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      game_object->GameInfo.next[y][x] = 0;
    }
  }

  srand(time(NULL));
  make_rand_next();
  copy_next_to_current();

  int check = 0;
  for (int i = 0; i < 4; i++) {
    if (game_object->current_figure[3][i] != 0) check = 1;
  }
  game_object->current_figure_x = 4;
  if (check) {
    game_object->current_figure_y = 0;
  } else {
    game_object->current_figure_y = 1;
  }

  stimer_set_interval(game_object->timer, SPEED(game_object->GameInfo.speed));

  for (int y = 0; y < 24; y++) {
    game_object->current_field[y][0] = 1;
    game_object->current_field[y][11] = 1;
  }
  for (int x = 0; x < 12; x++) {
    game_object->current_field[24][x] = 1;
  }

  game_object->blocked = false;
  game_object->ramming_stage = 0;
  game_object->inited = true;
}

static bool init_gameinfo(void) {
  GameObject_t *game_object = game_obj();
  int status = 0;
  game_object->GameInfo.field = (int **)malloc(20 * sizeof(int *));
  if (!game_object->GameInfo.field) status = 1;

  if (status == 0) {
    for (int y = 0; y < 20; y++) game_object->GameInfo.field[y] = NULL;
    game_object->GameInfo.next = (int **)malloc(4 * sizeof(int *));
    if (!game_object->GameInfo.next) status = 2;
  }

  if (status == 0) {
    for (int y = 0; y < 4; y++) game_object->GameInfo.next[y] = NULL;
    game_object->timer = stimer_create();
    if (game_object->timer == NULL) status = 3;
  }

  if (status == 0) {
    if (pthread_mutex_init(&(game_object->state_lock), NULL) != 0) status = 4;
  }

  if (status == 0) {
    for (int y = 0; y < 20 && (status == 0); y++) {
      game_object->GameInfo.field[y] = (int *)malloc(10 * sizeof(int));
      if (!game_object->GameInfo.field[y]) {
        for (int y1 = 0; y1 < y; y1++) {
          free(game_object->GameInfo.field[y1]);
        }
        status = 5;
      }
    }
    for (int y = 0; y < 4 && (status == 0); y++) {
      game_object->GameInfo.next[y] = (int *)malloc(4 * sizeof(int));
      if (!game_object->GameInfo.next[y]) {
        for (int y1 = 0; y1 < y; y1++) {
          free(game_object->GameInfo.next[y1]);
        }
        status = 6;
      }
    }
  }
  if (status == 0) {
    fill_defaults();
  }
  if (status != 0) free_gameobject(status);
  return status == 0 ? true : false;
}

static void free_gameobject(int status) {
  GameObject_t *game_object = game_obj();
  if (status == 0) {
    for (int y = 0; y < 4; y++) {
      if (game_object->GameInfo.next[y]) free(game_object->GameInfo.next[y]);
    }
  }
  if ((status > 5) || (status == 0)) {
    for (int y = 0; y < 20; y++) {
      if (game_object->GameInfo.field[y]) free(game_object->GameInfo.field[y]);
    }
  }
  if ((status > 4) || (status == 0))
    pthread_mutex_destroy(&(game_object->state_lock));
  if ((status > 3) || (status == 0)) stimer_destroy(game_object->timer);
  if ((status > 2) || (status == 0)) free(game_object->GameInfo.next);
  if ((status > 1) || (status == 0)) free(game_object->GameInfo.field);
  for (int y = 0; y < 25; y++) {
    for (int x = 0; x < 12; x++) {
      game_object->current_field[y][x] = 0;
    }
  }
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      game_object->current_figure[y][x] = 0;
    }
  }
  game_object->inited = false;
}

static void ontime_events(void) {
  GameObject_t *game_object = game_obj();
  pthread_mutex_lock(&(game_object->state_lock));

  if (game_object->GameInfo.pause == 0) {
    if (game_object->ramming_stage != 0) {
      ramming_field();
    } else if (check_down()) {
      if (game_object->current_figure_y < 2) {
        game_object->GameInfo.level = -1;
      } else {
        if (game_object->blocked) {
          stimer_set_interval(game_object->timer,
                              SPEED(game_object->GameInfo.speed));
          game_object->blocked = false;
        }
        prepare_next_iter();
        ramming_field();
      }
    } else {
      game_object->current_figure_y = game_object->current_figure_y + 1;
    }
  }

  pthread_mutex_unlock(&(game_object->state_lock));
}

static void make_rand_next(void) {
  GameObject_t *game_object = game_obj();
  int standard_figures[FIGURE_COUNT][4][4] = FIGURES_LIST;
  int r = rand() % FIGURE_COUNT;
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      game_object->GameInfo.next[y][x] = standard_figures[r][y][x];
    }
  }
}

static void copy_next_to_current(void) {
  GameObject_t *game_object = game_obj();
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      game_object->current_figure[y][x] = game_object->GameInfo.next[y][x];
    }
  }
  make_rand_next();
}

static void rotate_figure(void) {
  GameObject_t *game_object = game_obj();
  int temp[4][4];
  for (int i = 0; i < 4; i++) {
    for (int j1 = 0, j2 = 3; j1 < 4; j1++, j2--) {
      temp[j2][i] = game_object->current_figure[i][j1];
    }
  }
  if (check_rotate(temp)) {
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        game_object->current_figure[y][x] = temp[y][x];
      }
    }
  }
}

static void prepare_next_iter(void) {
  GameObject_t *game_object = game_obj();
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (game_object->current_figure[y][x])
        game_object->current_field[y + game_object->current_figure_y]
                                  [x + game_object->current_figure_x] =
            game_object->current_figure[y][x];
    }
  }
  copy_next_to_current();
  bool check = false;
  for (int x = 0; x < 4; x++) {
    if (game_object->current_figure[3][x] != 0) check = true;
  }
  game_object->current_figure_x = 4;
  if (check) {
    game_object->current_figure_y = 0;
  } else {
    game_object->current_figure_y = 1;
  }
}

static bool check_rotate(int test[4][4]) {
  GameObject_t *game_object = game_obj();
  bool res = true;
  for (int y = 0; y < 4 && res; y++) {
    for (int x = 0; x < 4 && res; x++) {
      if ((test[y][x] != 0) &&
          (game_object->current_field[game_object->current_figure_y + y]
                                     [game_object->current_figure_x + x] !=
           0)) {
        res = false;
      }
    }
  }
  return res;
}

static bool check_down(void) {
  GameObject_t *game_object = game_obj();
  bool res = false;
  for (int i = 0; i < 4 && !res; i++) {
    for (int j = 0; j < 4 && !res; j++) {
      if ((game_object->current_figure[i][j] != 0) &&
          (game_object->current_field[game_object->current_figure_y + i + 1]
                                     [game_object->current_figure_x + j] !=
           0)) {
        res = true;
      }
    }
  }
  return res;
}

static bool check_left(void) {
  GameObject_t *game_object = game_obj();
  bool res = false;
  for (int x = 0; x < 4 && !res; x++) {
    for (int y = 0; y < 4 && !res; y++) {
      if ((game_object->current_figure[y][x] != 0) &&
          (game_object->current_field[game_object->current_figure_y + y]
                                     [game_object->current_figure_x + x - 1] !=
           0)) {
        res = true;
      }
    }
  }
  return res;
}

static bool check_right(void) {
  GameObject_t *game_object = game_obj();
  bool res = false;
  for (int x = 3; x >= 0 && !res; x--) {
    for (int y = 0; y < 4 && !res; y++) {
      if ((game_object->current_figure[y][x] != 0) &&
          (game_object->current_field[game_object->current_figure_y + y]
                                     [game_object->current_figure_x + x + 1] !=
           0)) {
        res = true;
      }
    }
  }
  return res;
}

static void action_pause(void) {
  GameObject_t *game_object = game_obj();
  if (game_object->GameInfo.pause == 1) {
    game_object->GameInfo.pause = 0;
    game_object->blocked = false;
  } else {
    if (!game_object->blocked) {
      game_object->GameInfo.pause = 1;
      game_object->blocked = true;
    }
  }
}

static void action_left(void) {
  GameObject_t *game_object = game_obj();
  if (!check_left())
    game_object->current_figure_x = game_object->current_figure_x - 1;
}

static void action_right(void) {
  GameObject_t *game_object = game_obj();
  if (!check_right())
    game_object->current_figure_x = game_object->current_figure_x + 1;
}

static void action_down(void) {
  GameObject_t *game_object = game_obj();
  game_object->blocked = true;
  pthread_kill(game_object->timer->tid, SIGUSR1);
  stimer_set_interval(game_object->timer, 10);
}

static void ramming_field(void) {
  GameObject_t *game_object = game_obj();
  if (game_object->ramming_stage == 0) {
    ramming_field_0();
  } else if (game_object->ramming_stage > 20) {
    ramming_field_20();
  } else if (game_object->ramming_stage > 10) {
    ramming_field_10();
  } else if ((game_object->ramming_stage < 10) &&
             (game_object->ramming_stage > 0)) {
    ramming_field_1();
  }
}

static void ramming_field_0(void) {
  GameObject_t *game_object = game_obj();
  int full_rows = 0;
  for (int y = 4; y < 24; y++) {
    bool check = true;
    for (int x = 1; x < 12 && check; x++) {
      if (game_object->current_field[y][x] == 0) check = false;
    }
    if (check) full_rows++;
  }
  if (full_rows > 0) {
    switch (full_rows) {
      case 1:
        game_object->GameInfo.score += 100;
        break;
      case 2:
        game_object->GameInfo.score += 300;
        break;
      case 3:
        game_object->GameInfo.score += 700;
        break;
      case 4:
        game_object->GameInfo.score += 1500;
        break;
    }
    game_object->ramming_stage = 20 + full_rows;
    game_object->GameInfo.level =
        ((game_object->GameInfo.score / 600) % 10) + 1;
    game_object->GameInfo.speed =
        ((game_object->GameInfo.score / 600) % 10) + 1;
    if (game_object->GameInfo.high_score < game_object->GameInfo.score)
      game_object->GameInfo.high_score = game_object->GameInfo.score;
    game_object->blocked = true;
    stimer_set_interval(game_object->timer, 1000);
  }
}

static void ramming_field_20(void) {
  GameObject_t *game_object = game_obj();
  for (int y = 4; y < 24; y++) {
    bool check = true;
    for (int x = 1; x < 12 && check; x++) {
      if (game_object->current_field[y][x] == 0) check = false;
    }
    if (check) {
      for (int x = 1; x < 12 && check; x++) {
        game_object->current_field[y][x] = 7;
      }
    }
  }
  game_object->ramming_stage = game_object->ramming_stage - 10;
}

static void ramming_field_10(void) {
  GameObject_t *game_object = game_obj();
  for (int y = 4; y < 24; y++) {
    bool check = true;
    for (int x = 1; x < 12 && check; x++) {
      if (game_object->current_field[y][x] == 0) check = false;
    }
    if (check) {
      for (int x = 1; x < 12; x++) {
        game_object->current_field[y][x] = 0;
      }
    }
  }
  game_object->ramming_stage = game_object->ramming_stage - 10;
}

static void ramming_field_1(void) {
  GameObject_t *game_object = game_obj();
  bool iter = true;
  for (int y = 23; y > 3 && iter; y--) {
    bool check = true;
    for (int x = 1; x < 12 && check; x++) {
      if (game_object->current_field[y][x] != 0) check = false;
    }
    if (check) {
      for (int y1 = y; y1 > 4; y1--) {
        for (int x = 1; x < 12; x++) {
          game_object->current_field[y1][x] =
              game_object->current_field[y1 - 1][x];
        }
      }
      for (int x = 1; x < 12; x++) {
        game_object->current_field[4][x] = 0;
      }
      iter = false;
    }
  }
  game_object->ramming_stage = game_object->ramming_stage - 1;
  if (game_object->ramming_stage == 0) {
    stimer_set_interval(game_object->timer, SPEED(game_object->GameInfo.speed));
    game_object->blocked = false;
  }
}

static void stimer_sig_handler(int signo) { (void)signo; }

static void stimer_setup_signal_handler(void) {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = stimer_sig_handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGUSR1, &sa, NULL);
}

static void *execute_thread(void *arg) {
  int status;

  t_stimer *timer = (t_stimer *)arg;

  while (timer->terminate == false) {
    status = nanosleep(&timer->sleep_period, NULL);

    if (status == 0) {
      ontime_events();
    }
  }
  return NULL;
}

static t_stimer *stimer_create(void) {
  int status = 0;

  t_stimer *new_stimer = malloc(sizeof(t_stimer));
  if (new_stimer == NULL) {
    return NULL;
  }

  new_stimer->terminate = false;
  new_stimer->sleep_period.tv_sec = 1;
  new_stimer->sleep_period.tv_nsec = 0L;

  status = pthread_create(&new_stimer->tid, 0, execute_thread, new_stimer);

  if (status == 0) {
    stimer_setup_signal_handler();
    return new_stimer;
  } else {
    free(new_stimer);
    return NULL;
  }
}

static void stimer_destroy(t_stimer *timer) {
  timer->terminate = true;
  pthread_kill(timer->tid, SIGUSR1);
  pthread_join(timer->tid, NULL);

  free(timer);
  timer = NULL;
}

static void stimer_set_interval(t_stimer *timer, int value) {
  ldiv_t t = ldiv(value, 1000);
  timer->sleep_period.tv_sec = t.quot;
  timer->sleep_period.tv_nsec = t.rem * 1000000;
  timer->s_p = value;
}
