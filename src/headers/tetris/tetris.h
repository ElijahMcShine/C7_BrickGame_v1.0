#ifndef TETRIS_H
#define TETRIS_H

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../common/common_types.h"
#include "defines.h"

typedef struct stimer t_stimer;

typedef struct stimer {
  struct timespec sleep_period;
  int s_p;

  pthread_t tid;
  bool terminate;

} t_stimer;

typedef struct GameObject_t {
  GameInfo_t GameInfo;
  bool inited;
  bool blocked;
  int current_field[25][12];
  int current_figure[4][4];
  int current_figure_x;
  int current_figure_y;
  int ramming_stage;
  t_stimer* timer;
  pthread_mutex_t state_lock;
} GameObject_t;

GameStartInfo_t getGameStartInfo(void);

void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();

#endif