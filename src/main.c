#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <raylib.h>
#include "window_state.h"
#include "common.h"

#define WINDOW_HEIGHT 700
#define WINDOW_WIDTH 550
#define OVERLAY_HEIGHT 400
#define OVERLAY_WIDTH 700

#define NUM_WEEKS 12

void switch_mode(Mode mode) {
  set_mode(mode);
  
  switch (mode) {
  case WINDOW:
    SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    break;
  case OVERLAY:
    SetWindowSize(OVERLAY_WIDTH, OVERLAY_HEIGHT);
    break;
  default:
    break;
  }
}

typedef struct cell_t {
  int row;
  int column;
} Cell;

int cell_number(Cell c) {
  return c.row + 1 + (c.column * 7);
}

int distance(Cell a, Cell b) {
  return cell_number(b) - cell_number(a);
}

#define CELL_SPACING 10
#define CELL_SIZE 20
time_t now;
struct tm local_time;

// Returns the time info struct which is a static variable within time.h stdlib 
struct tm *get_date_days_ago(int days_ago, char *buffer, size_t buffer_size) {
  time_t raw_time;
  struct tm *time_info;
  time(&raw_time);
  time_info = localtime(&raw_time); // caution: localtime() overwrites the static struct internally as opposed to localtime_r
  time_info->tm_mday -= days_ago;
  mktime(time_info);

  strftime(buffer, buffer_size, "%B %d", time_info);
  
  return time_info;
}

void draw_graph(int x, int y) {
  GuiEnableTooltip();

  int num_columns = NUM_WEEKS;
  int num_rows = 7; // days in a week

  int cell_number_today = cell_number((Cell) {local_time.tm_wday, num_columns - 1});

  for (int c = 0; c < num_columns; c++) {
    for (int r = 0; r < num_rows; r++) {
      Cell cell = { r, c };
      int curr_cell_number = cell_number(cell);
      if (curr_cell_number > cell_number_today) break;

      int days_ago = cell_number_today - curr_cell_number;
      char tooltip_text[128];
      struct tm *time = get_date_days_ago(days_ago, tooltip_text, sizeof(tooltip_text));

      U32 result = get_result(time);
      GuiSetTooltip(tooltip_text);
      
      // full size of a cell is spacing * 2 + size, and then need to go little more 
      int cell_x = CELL_SPACING + (((CELL_SPACING * 2) + CELL_SIZE) * c);
      int cell_y = CELL_SPACING + (((CELL_SPACING * 2) + CELL_SIZE) * r);

      if (result) {
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(GREEN));
      } else {
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(BLANK));
      }
      if (GuiButton((Rectangle) {cell_x + x, cell_y + y, CELL_SIZE, CELL_SIZE}, " ")) {
	set_result(time, abs(result - 1));
      }

      if (c == 0 && r == 1) {
	GuiLabel((Rectangle) {x-5, cell_y + y, 20, 20}, "M");
      } else if (c==0 && r== 3) {
	GuiLabel((Rectangle) {x-5, cell_y + y, 20, 20}, "W");
      } else if (c==0 && r == 5) {
	GuiLabel((Rectangle) {x-5, cell_y + y, 20, 20}, "F");
      }
    }
  }
  GuiDisableTooltip();
}

bool edit_mode = false;

void render_window() {
  ClearBackground(RAYWHITE);
  DrawFPS(WINDOW_WIDTH - 50, 10);

  Rectangle text_box_rect = {(WINDOW_WIDTH / 2) - 200, (WINDOW_HEIGHT / 2) - 200, 400, 50};
  Vector2 mouse_pos = GetMousePosition();
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (CheckCollisionRecs((Rectangle) {mouse_pos.x, mouse_pos.y, 2, 2}, text_box_rect)) {
      
      edit_mode = true;
    } else {
      edit_mode = false;
    }
  }
  if (GuiTextBox(text_box_rect, get_intention(), get_intention_size(), edit_mode)) {
    switch_mode(OVERLAY);
  }

  char label[2048];
  sprintf(label, "%d / %d characters | Hit enter to set", (int) strlen(get_intention()), (int) get_intention_size()); 
  GuiLabel((Rectangle){ WINDOW_WIDTH / 2 - 100, (WINDOW_HEIGHT / 2) - 150, 300, 50}, label);

  int graph_y_offset = (WINDOW_HEIGHT / 2) + 50;
  draw_graph(20, graph_y_offset);
}

void render_overlay() {
  ClearBackground(BLANK);
  DrawFPS(OVERLAY_WIDTH - 50, 10);

  const Rectangle button_rect = {OVERLAY_WIDTH - 100, 10, 100, 25};
  int active = 0;

  if (GuiToggleSlider(button_rect, "Toggle mode", &active)) {
    switch_mode(WINDOW);
  }

  GuiSetStyle(DEFAULT, TEXT_SIZE, 75);
  GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_CHAR);
  GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
  GuiLabel((Rectangle){0, 0, OVERLAY_WIDTH, OVERLAY_HEIGHT}, get_intention());
  GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, 0);
}

int main(int argc, char *argv[]) {
  time(&now);
  localtime_r(&now, &local_time);
  mktime(&local_time);
  
  init_window_state();
  
  printf("Welcome to Flowy.adhd\n");

  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Flowy.adhd");
 
  while (!WindowShouldClose()) {
    BeginDrawing();

    GuiSetStyle(DEFAULT, TEXT_SIZE, 15);

    if (get_mode() == WINDOW) render_window();
    else render_overlay();
    
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
