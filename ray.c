// making a very basic ray simulation in c using SDL library

#include <SDL.h>
#include <math.h>
#include <stdio.h>

#define HEIGHT 600
#define WIDTH 1200
#define PI 3.14159
#define WHITE 0xffffffff // 32 bit color
#define BLACK 0x00000000
#define GRAY 0xefefefef
#define GOLD 0xffd43b
#define NUM_RAYS 520

typedef struct circle {
  double x;
  double y;
  double r;
} circle;

typedef struct rays {
  double x_start, y_start;
  // double x_end, y_end;
  double angle;
} rays;

void GenerateRays(circle c, rays arr[NUM_RAYS]) {
  for (int i = 0; i < NUM_RAYS; i++) {
    double angle = ((double)i / NUM_RAYS) * 2 * PI;
    rays ray = {c.x, c.y, angle};
    arr[i] = ray;
  }
}

void FillRays(SDL_Surface *surface, rays arr[NUM_RAYS], circle c,
              Uint32 color) {
  for (int i = 0; i < NUM_RAYS; i++) {
    rays ray = arr[i];

    int end_of_screen = 0;
    int obj_hit = 0;

    double step = 1;
    double x_draw = ray.x_start;
    double y_draw = ray.y_start;

    while (!end_of_screen && !obj_hit) {
      x_draw += step * cos(ray.angle);
      y_draw += step * sin(ray.angle);
      // above method to get the x, y coords where a single pixel rect can be
      // filled is very clever, as we go by steps, so for a single ray with a
      // certain angle within a while loop for that ray only, we step wise
      // increment x by cos(angle) and stepwise increment y by sin(angle) and
      // this way for each step of the while loop, we get the appropriate points
      // of the ray, and can fill a rect for that pixel

      SDL_Rect pixel = (SDL_Rect){x_draw, y_draw, 1, 1};
      SDL_FillRect(surface, &pixel, color);

      if (x_draw < 0 || x_draw > WIDTH) {
        end_of_screen = 1;
      }
      if (y_draw < 0 || y_draw > HEIGHT) {
        end_of_screen = 1;
      }

      if (sqrt(pow(x_draw - c.x, 2) + pow(y_draw - c.y, 2)) <= c.r) {
        obj_hit = 1;
      } // if any of the pixels of the rays fall within the circle, then
      // stop the rendering of all the rays therein
    }
  }
}

void FillCircle(SDL_Surface *surface, circle c, Uint32 color) {
  // the approach used here is that we define a restricting square around the
  // circle and if the points(representing pixels) are within the radius of the
  // search area, then we fill that pixel only at that x, y by a square, so,
  // circles are also define by squres in SDL
  double dist;
  // defining the square search area
  for (double x = c.x - c.r; x <= c.x + c.r;
       x++) { // we do skip over 1s as we let the standard
    // pixel size in any dimension to be 1 unit
    for (double y = c.y - c.r; y <= c.y + c.r;
         y++) { // this is equilavent to searching for
      // points in the search area top to bottom for each pixel width from left
      // to right
      dist = sqrt(pow(x - c.x, 2) + pow(y - c.y, 2));
      if (dist <= c.r) {
        SDL_Rect pixel =
            (SDL_Rect){x, y, 1, 1}; // fill 1 unit area of square at x,y
        SDL_FillRect(surface, &pixel, color);
      }
    }
  }
}

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window = SDL_CreateWindow("raytracing", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,
                                        SDL_WINDOW_BORDERLESS);

  SDL_Surface *surface = SDL_GetWindowSurface(window);

  // SDL_Rect rect = (SDL_Rect) {200, 200, 200, 200};  //so {} is a compound
  // literal/instance
  // //of a struct, and its type caseted to SDL_Rect, its filled as x,y, width,
  // height

  // SDL_FillRect(surface, &rect, WHITE);
  SDL_Rect erase_rect = (SDL_Rect){0, 0, WIDTH, HEIGHT};
  circle c1 = {200, 0, 55};    // is the light source
  circle c2 = {650, 300, 130}; // is the fixed circle
  // FillCircle(surface, c1, WHITE);

  rays rays[NUM_RAYS];
  GenerateRays(
      c1,
      rays); // needed as we want that light source to shine "light" from the
  // very first instance, not just from the point where we move it with the
  // mouse

  // dynamic animations
  int sim_running = 1;
  int speed = 3;
  SDL_Event event;
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
      if (event.type == SDL_MOUSEMOTION && event.motion.state != 0) {
        c1.x = event.motion.x;
        c1.y = event.motion.y;
        GenerateRays(c1, rays);
      }
    }
    SDL_FillRect(surface, &erase_rect,
                 BLACK); // used to fill the whole screen with
    // black before filling in the new circle in the surface, so that it seems
    // that the circle is moving around with the mouse
    FillCircle(surface, c1, WHITE);
    FillCircle(surface, c2, WHITE);
    FillRays(surface, rays, c2, GOLD);

    // making the other circle move up and down
    c2.y += speed;
    if (c2.y + c2.r >= HEIGHT || c2.y - c2.r <= 0) {
      speed = -speed;
    }

    SDL_UpdateWindowSurface(window);
    SDL_Delay(20);
  }

  //
  for (size_t i = 0; i < 100; i++) {
    SDL_PumpEvents(); // have to pump the event queue before display
  }
  //
  return 0;
}
