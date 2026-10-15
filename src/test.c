#include "./headers/tetris/tetris.h"
#include <check.h>
#include <unistd.h>

// Helpers to drive the game safely from tests
static void start_game(void) {
  userInput(Start, true);
}

static GameInfo_t snapshot(void) {
  return updateCurrentState();
}

static void pause_game(void) {
  userInput(Pause, true);
}

static void terminate_game(void) {
  userInput(Terminate, true);
}

static int field_nonzero(const GameInfo_t *st) {
  int nz = 0;
  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 10; x++) {
      if (st->field[y][x] != 0) nz++;
    }
  }
  return nz;
}

static int two_line_is_zero(const GameInfo_t *st) {
  int nz = 0;
    for (int x = 0; x < 10; x++) {
      if (st->field[0][x] != 0) nz++;
    }
  return nz;
}

static int wait_until_visible(int max_wait_ms) {
  int waited = 0;
  while (waited < max_wait_ms) {
    GameInfo_t st = snapshot();
    if ((field_nonzero(&st) > 0) && (two_line_is_zero(&st) == 0)) return 1;
    usleep(10000);
    waited += 10;
  }
  return 0;
}

static void copy_field(const GameInfo_t *st, int coords[20][10]) {
  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 10; x++) {
        coords[y][x] = st->field[y][x];
    }
  }
}



static int is_shifted_by_dx(int before[20][10], int after[20][10], int dx) {
  int res = 1;
  
  int dy1 = 0;
  int dy2 = 0;
  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 10; x++) {
      if (before[y][x] != 0) dy1 = y;
    }
  }
  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 10; x++) {
      if (after[y][x] != 0) dy2 = y;
    }
  }
  int dy = dy2 - dy1;

  for (int y = 0; y < 20 && res; y++) {
    for (int x = 0; x < 10 && res; x++) {
      if ((x + dx >= 0) && (x + dx < 10) && (y + dy >= 0) && (y + dy < 20)){
        if (before[y][x] != after[y + dy][x + dx]) res = 0;
      }
    }
  }
  return res;
}

static void copy_two_lines_figure(const GameInfo_t *st, int coords[2][10]){
  int flag = -1;
  for (int y = 0; y < 20 && flag == -1; y++) {
    for (int x = 0; x < 10; x++) {
      if (st->field[y][x] != 0) flag = y;
    }
  }
  for (int y = flag; y < flag + 2; y++){
    for (int x = 0; x < 10; x++) {
      coords[y - flag][x] = st->field[y][x];
    }
  }
}

static void copy_four_lines_figure(const GameInfo_t *st, int coords[4][10]){
  int flag = -1;
  for (int y = 0; y < 20 && flag == -1; y++) {
    for (int x = 0; x < 10; x++) {
      if (st->field[y][x] != 0) flag = y;
    }
  }
  for (int y = flag; y < (flag + 4); y++){
    for (int x = 0; x < 10; x++) {
      coords[y - flag][x] = st->field[y][x];
    }
  }
}


static void copy_two_bottom_lines_figure(const GameInfo_t *st, int coords[2][10]){
  for (int y = 18; y < 20; y++){
    for (int x = 0; x < 10; x++) {
      coords[y - 18][x] = st->field[y][x];
    }
  }
}

static void pre_check_rotate(int before_figure[4][4],int after_figure[4][4]){
  GameInfo_t before = snapshot();
  int b_figure[2][10];
  copy_two_lines_figure(&before, b_figure);
  userInput(Action, true);
  GameInfo_t after = snapshot();
  int a_figure[4][10];
  copy_four_lines_figure(&after, a_figure);
  int x1 = -1;
  for (int x = 0; x < 10 && x1 == -1; x++) {
    if ((b_figure[0][x] != 0) || (b_figure[1][x] != 0)) x1 = x;
  }
  for (int x = 0; x < 4; x++) {
    before_figure[0][x] = b_figure[0][x + x1];
    before_figure[1][x] = b_figure[1][x + x1];
  }
  x1 = -1;
  for (int x = 0; x < 10 && x1 == -1; x++) {
    for (int y = 0; y < 4; y++){
      if (a_figure[y][x] != 0) x1 = x;
    }
  }
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++){
      after_figure[y][x] = a_figure[y][x + x1];
    }
  }
}

START_TEST(test_getGameStartInfo_basic) {
  GameStartInfo_t info = getGameStartInfo();
  ck_assert_msg(info.game_name[0] != '\0', "game_name should be non-empty");
  int nonzero = 0;
  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 10; x++) {
      ck_assert_msg(info.start_screen[y][x] >= 0, "banner cell must be >= 0");
      if (info.start_screen[y][x] != 0) nonzero++;
    }
  }
  ck_assert_msg(nonzero >= 1, "banner should contain at least one non-zero cell");
}
END_TEST

START_TEST(test_start_initial_state) {
  start_game();
  GameInfo_t st = snapshot();
  ck_assert_int_eq(st.level, 1);
  ck_assert_int_eq(st.speed, 1);
  ck_assert_int_eq(st.pause, 0);
  ck_assert_int_eq(st.score, 0);
  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 10; x++) {
      ck_assert_msg(st.field[y][x] >= 0, "field cell must be >= 0");
    }
  }
  int next_nonzero = 0;
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (st.next[y][x] != 0) next_nonzero++;
    }
  }
  ck_assert_msg(next_nonzero > 0, "next figure should contain non-zero cells");
  terminate_game();
}
END_TEST

START_TEST(test_pause_toggle) {
  start_game();
  GameInfo_t st1 = snapshot();
  ck_assert_int_eq(st1.pause, 0);

  pause_game();
  GameInfo_t st2 = snapshot();
  ck_assert_int_eq(st2.pause, 1);

  pause_game();
  GameInfo_t st3 = snapshot();
  ck_assert_int_eq(st3.pause, 0);

  terminate_game();
}
END_TEST

START_TEST(test_move_left_right) {
  start_game();
  ck_assert_msg(wait_until_visible(5000) == 1, "piece did not become visible in time");
  GameInfo_t before_left = snapshot();
  int b_table[20][10];
  int a_table[20][10];
  copy_field(&before_left, b_table);

  userInput(Left, true);
  GameInfo_t after_left = snapshot();
  copy_field(&after_left, a_table);
  ck_assert_msg(is_shifted_by_dx(b_table, a_table, -1) == 1, "left move should shift all occupied cells by -1");

  GameInfo_t before_ritht = snapshot();
  copy_field(&before_ritht, b_table);
  userInput(Right, true);
  GameInfo_t after_right = snapshot();
  copy_field(&after_right, a_table);
  ck_assert_msg(is_shifted_by_dx(b_table, a_table, 1) == 1, "right move should shift all occupied cells by +1");
  terminate_game();
}
END_TEST

START_TEST(test_down_accelerates_drop) {
  start_game();
  ck_assert_msg(wait_until_visible(5000) == 1, "piece did not become visible in time");
  GameInfo_t st0 = snapshot();
  int b_figure[2][10];
  copy_two_lines_figure(&st0, b_figure);

  userInput(Down, true);
  usleep(500000);

  GameInfo_t st1 = snapshot();
  int a_figure[2][10];
  copy_two_bottom_lines_figure(&st1, a_figure);

  int res = 1;
  for (int y = 0; y < 2; y++){
    for (int x = 0; x < 10; x++) {
      if (b_figure[y][x] != a_figure[y][x]) res = 0;
    }
  }

  ck_assert_msg(res == 1, "after Down, the piece should be lower on the field");
  terminate_game();
}
END_TEST



START_TEST(test_action_rotate) {
  start_game();
  ck_assert_msg(wait_until_visible(5000) == 1, "piece did not become visible in time");
  int before_figure[4][4] = {0};
  int after_figure[4][4] = {0};
  pre_check_rotate(before_figure, after_figure);
  int check = 1;
  if (before_figure[0][0] == 7){
    for (int y = 0; y < 4; y++){
      for (int x = 0; x < 4; x++) {
        if (before_figure[y][x] != after_figure[y][x]) check = 0;
      }
    }
  } else {
    for (int i = 0; i < 3; i++) {
      for (int j1 = 0, j2 = 2; j1 < 3; j1++, j2--) {
        if (before_figure[i][j1] != after_figure[j2][i]) check = 0;
      }
    }
  }
  ck_assert_msg(check == 1, "rotated field must be within bounds and non-negative");
  terminate_game();
}
END_TEST

Suite* tetris_suite(void) {
  Suite *s = suite_create("tetris");

  TCase *tc_api = tcase_create("api");
  tcase_add_test(tc_api, test_getGameStartInfo_basic);
  suite_add_tcase(s, tc_api);

  TCase *tc_game = tcase_create("game");
  tcase_add_test(tc_game, test_start_initial_state);
  tcase_add_test(tc_game, test_pause_toggle);
  tcase_add_test(tc_game, test_move_left_right);
  tcase_add_test(tc_game, test_down_accelerates_drop);
  tcase_add_test(tc_game, test_action_rotate);
  suite_add_tcase(s, tc_game);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s = tetris_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_ENV);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? 0 : 1;
}
