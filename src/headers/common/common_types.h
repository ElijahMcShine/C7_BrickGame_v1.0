#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#ifndef bool
typedef unsigned char bool;
#endif

#define SUCCESS 0
#define ERROR 1

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef struct GameStartInfo_t {
  char game_name[256];
  char file_name[256];
  int start_screen[20][10];
} GameStartInfo_t;

#endif