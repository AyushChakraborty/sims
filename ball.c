// making a bouncy ball simulation using SDL

#include <SDL.h>
#include <math.h>
#include <stdio.h>

#define WIDTH 1200
#define HEIGHT 600
#define TRJ_LEN 100
#define WHITE 0xffffffff
#define BLACK 0x00000000
#define BG 0x0f0f0f0f
#define TRJ_COLOR 0xe803fc

typedef struct circle {
  double x;
  double y;
  double r;
} circle;

void FillCircle(SDL_Surface *surface, circle c, Uint32 color) {
  double dist;
  for (double x = c.x - c.r; x <= c.x + c.r; x++) {
    for (double y = c.y - c.r; y <= c.y + c.r; y++) {
      dist = sqrt(pow(x - c.x, 2) + pow(y - c.y, 2));
      if (dist <= c.r) {
        SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};
        SDL_FillRect(surface, &pixel, color);
      }
    }
  }
}

void bounces(
    circle *c, double *vx,
    double *vy) { // need to pass it as a pointer, as we want exactly that
  // struct to be effected
  double g = 0.4;
  double damping = 0.8;
  *vy += g;

  if (c->x + *vx < WIDTH && c->x + *vx > 0)
    c->x += (*vx) * 1;
  else {
    *vx *= damping;
    *vx = -(*vx);
  }

  if (c->y + *vy < HEIGHT && c->y + *vy > 0)
    c->y += (*vy) * 1;
  else {
    // c->y = HEIGHT - c->r;
    *vy *= damping;
    *vy = -(*vy);
  }

  if (c->x + c->r == WIDTH || c->x - c->r == 0) {
    *vx *= damping;
    *vx = -(*vx);
  }
  if (c->y + c->r == HEIGHT || c->y - c->r == 0) {
    // c->y = HEIGHT - c->r;
    *vy *= damping;
    *vy = -(*vy);
  }
}

void FillTrajectory(SDL_Surface *surface, circle trj[TRJ_LEN],
                    int current_trj_index, Uint32 color) {
  for (int i = 0; i < current_trj_index; i++) {
    SDL_Rect trj_pixel = (SDL_Rect){trj[i].x, trj[i].y, 3, 3};
    SDL_FillRect(surface, &trj_pixel, color);
  }
}

void UpdateTrajectory(int current_trj_index, circle c, circle arr[TRJ_LEN]) {
  if (current_trj_index >= TRJ_LEN) {
    circle copy_arr[current_trj_index];
    for (int i = 0; i < current_trj_index; i++) {
      copy_arr[i] =
          arr[i + 1]; // last space in copy of array is left empty for new pixel
    }
    for (int i = 0; i < current_trj_index; i++) {
      arr[i] = copy_arr[i];
    }
    arr[current_trj_index] = c;
  } else {
    arr[current_trj_index] = c;
  }
}

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window = SDL_CreateWindow("bouncingball", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,
                                        SDL_WINDOW_BORDERLESS);

  SDL_Surface *surface = SDL_GetWindowSurface(window);
  circle c1 = {200, 200, 40};
  SDL_Rect erase_rect = (SDL_Rect){0, 0, WIDTH, HEIGHT};
  double vx = 7;
  double vy = 10;

  circle trj_arr[TRJ_LEN];
  int current_trj_index = 0;

  SDL_Event event;
  int sim_running = 1;
  while (sim_running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        sim_running = 0;
      }
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_SPACE) {
          sim_running = 0;
        }
      }
    }
    SDL_FillRect(surface, &erase_rect, BG);
    FillCircle(surface, c1, WHITE);
    bounces(&c1, &vx, &vy);
    UpdateTrajectory(current_trj_index, c1, trj_arr);
    FillTrajectory(surface, trj_arr, current_trj_index, TRJ_COLOR);
    // we can see the trj path even though the surface is filled with gray every
    // cycle of the while loop as the info about the circles trj is already
    // stored in the arr, and in the FillTrajectory() func, we fill it from the
    // very start every single time, so first cycle 1, next one after the grey
    // filling 1, 2, then after grey filling again 1,2,3 ... and so on, so it
    // seems that the individual pixels tracing the trj persist, but they are
    // actually redrawn again and again

    SDL_UpdateWindowSurface(window);

    if (current_trj_index < TRJ_LEN)
      ++current_trj_index;

    SDL_Delay(20);
  }

  for (size_t i = 0; i < 100; i++) {
    SDL_PumpEvents();
  }

  return 0;
}
