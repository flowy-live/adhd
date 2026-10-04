#ifndef STATE_H
#define STATE_H

#include "common.h"
#include <stddef.h>
#include <time.h>

enum mode_t {
  WINDOW = 0,
  OVERLAY
};
typedef enum mode_t Mode;

void init_state();
void cleanup_state();

void set_intention(char *intention);
char *get_intention();
size_t get_intention_size();

Mode get_mode();
void set_mode(Mode mode);

// returns 1 for win, 0 for loss
U32 get_result(struct tm *time);
void set_result(struct tm *time, U32 result);

#endif
