/* starfied.cpp*/

/*
 *
 * this is an attempt to re-create starfield coding challenge by
 * The coding train, but rendered in cpp
 *
 */

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

/* debugging stuff, remove later */
#include <iostream>

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

constexpr int win_width = 640;
constexpr int win_height = 640;

constexpr unsigned int stars_count = 1000;
static SDL_FPoint stars_init_coordinates[stars_count];
static SDL_FPoint stars_final_coordinate[stars_count];
int stars_speed_factor[stars_count];

SDL_FPoint center_it(const SDL_FPoint point) {
  return SDL_FPoint{point.x + (win_width / 2), point.y + (win_height / 2)};
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_SetAppMetadata("StarField", "1.0", "com.example.start-field");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("StarField", win_width, win_height, 0,
                                   &window, &renderer)) {
    SDL_Log("Couldn't create StarField windows and renderer: %s",
            SDL_GetError());
    return SDL_APP_FAILURE;
  }

  for (auto i = 0u; i < stars_count; ++i) {
    /*  coordinates are from -screen limit to +screen limit */
    stars_init_coordinates[i].x = SDL_rand(2 * win_width) - (win_width);
    stars_init_coordinates[i].y = SDL_rand(2 * win_height) - (win_height);
    //    stars_init_coordinates[i] = center_it(stars_init_coordinates[i]);
    stars_speed_factor[i] = SDL_rand(win_width) + 1;
  }

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }
  return SDL_APP_CONTINUE; /* carry on with the program! */
}

///////// program stuff

// bool draw_circle(void *renderer, const SDL_FPoint center, const float radius,
//                  const Uint8 r, const Uint8 g, const Uint8 b) {}

float map(float i, float x1, float y1, float x2, float y2) {
  return ((y2 - x2) * ((i - x1) / (y1 - x1)));
}

////////////////////////

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {

  /* move the stars */
  for (auto i = 0u; i < stars_count; ++i) {
    stars_final_coordinate[i].x =
        map(stars_init_coordinates[i].x / stars_speed_factor[i], 0, 1, 0,
            win_width);
    stars_final_coordinate[i].y =
        map(stars_init_coordinates[i].y / stars_speed_factor[i], 0, 1, 0,
            win_height);
    stars_final_coordinate[i] = center_it(stars_final_coordinate[i]);
    stars_speed_factor[i]--;

    /* reset stars which are out of window */
    if (stars_speed_factor[i] < 1) {
      stars_init_coordinates[i].x = SDL_rand(2 * win_width) - (win_width);
      stars_init_coordinates[i].y = SDL_rand(2 * win_height) - (win_height);
      stars_speed_factor[i] = SDL_rand(win_width) + 1;
    }
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0,
                         SDL_ALPHA_OPAQUE); /* black, full alpha */
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255,
                         SDL_ALPHA_OPAQUE); /* white, full alpha */
  SDL_RenderPoints(renderer, stars_final_coordinate,
                   SDL_arraysize(stars_final_coordinate));

  /* put the newly-cleared rendering on the screen. */
  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  /* SDL will clean up the window/renderer for us. */
}
