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

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;

  SDL_Rect *cells[CELLSINHEIGHT][CELLSINWIDTH];
  bool cell_states[CELLSINHEIGHT][CELLSINWIDTH];

  // First entry is arr 2 is state, and 2nd is outcome.
  bool rules[100][2][3];
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

  state->renderer =
      SDL_CreateRenderer(state->window, -1, SDL_RENDERER_ACCELERATED);
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

  state->rules[0][0][0] = false;
  state->rules[0][0][1] = true;
  state->rules[0][0][2] = false;

  state->rules[0][1][0] = false;
  state->rules[0][1][1] = false;
  state->rules[0][1][2] = true;

  state->rules[1][0][0] = true;
  state->rules[1][0][1] = true;
  state->rules[1][0][2] = true;

  state->rules[1][1][0] = false;
  state->rules[1][1][1] = true;
  state->rules[1][1][2] = false;
}

void execute_rule() {
  for (int x = 0; x < CELLSINWIDTH; ++x) {
    bool left = state->cell_states[1][x - 1];
    bool center = state->cell_states[1][x];
    bool right = state->cell_states[1][x + 1];

    for (int j = 0; j < state->rules_size; ++j) {
      bool left_rule = state->rules[j][0][0];
      bool center_rule = state->rules[j][0][1];
      bool right_rule = state->rules[j][0][2];
      if (left == left_rule && center == center_rule && right == right_rule) {
        state->cell_states[0][x - 1] = state->rules[j][1][0];
        state->cell_states[0][x] = state->rules[j][1][1];
        state->cell_states[0][x + 1] = state->rules[j][1][2];

        if (state->cell_states[0][x - 1]) {
          SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        } else {
          SDL_SetRenderDrawColor(state->renderer, 255, 255, 255,
                                 SDL_ALPHA_OPAQUE);
        }
        SDL_RenderFillRect(state->renderer, state->cells[0][x - 1]);

        if (state->cell_states[0][x]) {
          SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        } else {
          SDL_SetRenderDrawColor(state->renderer, 255, 255, 255,
                                 SDL_ALPHA_OPAQUE);
        }
        SDL_RenderFillRect(state->renderer, state->cells[0][x]);

        if (state->cell_states[0][x + 1]) {
          SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        } else {
          SDL_SetRenderDrawColor(state->renderer, 255, 255, 255,
                                 SDL_ALPHA_OPAQUE);
        }
        SDL_RenderFillRect(state->renderer, state->cells[0][x + 1]);
      }
    }
  }

  SDL_RenderPresent(state->renderer);
}

int main(void) {
  arena_t *arena = arena_create();
  if (!arena) {
    printf("Failed to init memory arena");
    return 1;
  }

  init(arena);
  bool running = 1;
  while (running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      execute_rule();
      SDL_Delay(50);
      switch (event.type) {
      case SDL_QUIT:
        running = 0;
      }
    }
  }
}
