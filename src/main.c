#include "base.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#define WINDOWWIDTH 1000
#define WINDOWHEIGHT 1000
#define CELLWIDTH 10
#define CELLHEIGHT 10

#define CELLSINHEIGHT (WINDOWHEIGHT / CELLHEIGHT)
#define CELLSINWIDTH (WINDOWWIDTH / CELLWIDTH)

void init(arena_t *arena);
void execute_rule();
void move_cells();
void render_state();
void parse_rule_file();

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;

  SDL_Rect *cells[CELLSINHEIGHT][CELLSINWIDTH];
  bool cell_states[CELLSINHEIGHT][CELLSINWIDTH];

  // First entry is arr 2 is state, and 2nd is outcome.
  struct {
    bool rule[3];
    bool center_outcome;
  } rule_definitions[100];
  int rules_size;
} state_t;

state_t *state;

void init(arena_t *arena) {
  state = (state_t *)arena_push(arena, sizeof(state_t));
  state->rules_size = 0;
  SDL_Init(SDL_INIT_EVENTS);
  SDL_version compiled_version, linked_version;
  SDL_VERSION(&compiled_version);
  SDL_GetVersion(&linked_version);
  printf("Compiled SDL version: %i.%i.%i\n", compiled_version.major,
         compiled_version.minor, compiled_version.patch);
  printf("Linked SDL version: %i.%i.%i\n", linked_version.major,
         linked_version.minor, linked_version.patch);

  state->window = SDL_CreateWindow("rule 30", SDL_WINDOWPOS_CENTERED_DISPLAY(1),
                                   SDL_WINDOWPOS_CENTERED_DISPLAY(1),
                                   WINDOWWIDTH, WINDOWHEIGHT, SDL_WINDOW_SHOWN);

  state->renderer = SDL_CreateRenderer(
      state->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(state->renderer);
  SDL_RenderPresent(state->renderer);

  SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  for (int y = 0; y < CELLSINHEIGHT; ++y) {
    for (int x = 0; x < CELLSINWIDTH; ++x) {
      state->cells[y][x] = (SDL_Rect *)arena_push(arena, sizeof(SDL_Rect));
      state->cells[y][x]->x = x * CELLWIDTH;
      state->cells[y][x]->y = y * CELLHEIGHT;
      state->cells[y][x]->h = CELLHEIGHT;
      state->cells[y][x]->w = CELLWIDTH;
      state->cell_states[y][x] = false;
      SDL_RenderFillRect(state->renderer, state->cells[y][x]);
    }
  }
}

void move_cells() {
  for (int y = CELLSINHEIGHT - 1; y > 0; --y) {
    for (int x = 0; x < CELLSINWIDTH; ++x) {
      state->cell_states[y][x] = state->cell_states[y - 1][x];
    }
  }
}

void parse_rule_file() {
  FILE *file;
  file =
      fopen("/home/owner/projects/2d-cellular-automata-builder/rule.txt", "r");
  if (!file) {
    printf("Couldn't read file.");
    exit(0);
  }

  char c;
  int size = 0;
  while ((c = getc(file)) != EOF) {
    printf("Char found: %c\n", c);
    if (c == '\n') {
      size = 0;
      continue;
    }

    if (size == 0) {
      if (c == '1')
        state->rule_definitions[state->rules_size].rule[size] = true;
      else
        state->rule_definitions[state->rules_size].rule[size] = false;
    }
    if (size == 1) {
      if (c == '1')
        state->rule_definitions[state->rules_size].rule[size] = true;
      else
        state->rule_definitions[state->rules_size].rule[size] = false;
    }
    if (size == 2) {
      if (c == '1')
        state->rule_definitions[state->rules_size].rule[size] = true;
      else
        state->rule_definitions[state->rules_size].rule[size] = false;
    }
    if (size == 4) {
      if (c == '1')
        state->rule_definitions[state->rules_size].center_outcome = true;
      else
        state->rule_definitions[state->rules_size].center_outcome = false;

      ++state->rules_size;
    }

    size++;
  }

  fclose(file);
}

void execute_rule() {
  for (int x = 0; x < CELLSINWIDTH; ++x) {
    bool left = state->cell_states[1][x - 1];
    bool center = state->cell_states[1][x];
    bool right = state->cell_states[1][x + 1];

    for (int j = 0; j < state->rules_size; ++j) {
      bool left_rule = state->rule_definitions[j].rule[0];
      bool center_rule = state->rule_definitions[j].rule[1];
      bool right_rule = state->rule_definitions[j].rule[2];

      printf("Checking rule: %i%i%i\n", left_rule, center_rule, right_rule);

      if (left == left_rule && center == center_rule && right == right_rule) {
        state->cell_states[0][x] = state->rule_definitions[j].center_outcome;
      }
    }
  }
}

void render_state() {
  for (int y = 0; y < CELLSINHEIGHT; ++y) {
    for (int x = 0; x < CELLSINWIDTH; ++x) {
      if (state->cell_states[y][x]) {
        SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      } else {
        SDL_SetRenderDrawColor(state->renderer, 255, 255, 255,
                               SDL_ALPHA_OPAQUE);
      }

      SDL_RenderFillRect(state->renderer, state->cells[y][x]);
    }
  }

  SDL_RenderPresent(state->renderer);
}

int main(void) {
  arena_t *arena = arena_create();
  if (!arena) {
    return 1;
  }

  init(arena);
  parse_rule_file();
  bool running = 1;

  state->cell_states[0][CELLSINWIDTH / 2] = true;

  while (running) {
    SDL_Event event;

    move_cells();
    execute_rule();
    render_state();

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = 0;
      }
    }
  }
}
