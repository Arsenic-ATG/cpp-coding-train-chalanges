#ifndef SNAKE_H
#define SNAKE_H

#include <memory>
#include <random>
#include <vector>

constexpr unsigned int default_grid_size = 11;

/* grid coordinates */
struct grid_coords_t {
  unsigned int x, y;

  grid_coords_t(const unsigned int x = 0, const unsigned int y = 0)
      : x(x), y(y) {}
  bool operator==(const grid_coords_t &other) const {
    return ((x == other.x) && (y == other.y));
  }
};

enum Direction { north = 0, east, south, west };

class Snake {
  grid_coords_t head; /* coordinates of the head on game grid*/
  std::vector<grid_coords_t> body;
  Direction direction;

public:
  Snake(const grid_coords_t head_coords) : head(head_coords) {
    body.push_back(head);
  }
  Snake(const int x, const int y) : Snake(grid_coords_t(x, y)) {}

  auto get_body() const { return body; }
  auto get_head() const { return head; }
  auto get_direction() const { return direction; }
  auto get_next_head_location() const {
    auto next_coords = head;
    switch (direction) {
    case north:
      next_coords.y--;
      break;
    case east:
      next_coords.x++;
      break;
    case south:
      next_coords.y++;
      break;
    case west:
      next_coords.x--;
      break;
    }
    return next_coords;
  }

  void set_direction(const Direction dir) { direction = dir; }

  /* move the snake to the next location. NOTE: this fuction doesn't check for
   * collision and food eating, they must be handled seperately. */
  void move(const bool has_eaten_food = false) {
    head = get_next_head_location();

    /* update the body of the snake (remember, head is at the end) */
    for (auto i = 1u; i < body.size(); ++i) {
      body[i - 1] = body[i];
    }

    /* also adjust the length of the snake if food has been eaten*/
    if (has_eaten_food)
      body.push_back(head);
    else
      body.back() = head;
  }

  // check if the snake body existin in the given coordnate or not
  bool has_snake(const grid_coords_t here) const {
    /*  linear search snake body for this coordinate */
    bool found = false;
    for (auto i = 0u; i < body.size(); ++i) {
      if (here == body[i]) {
        found = true;
        break;
      }
    }
    return found;
  }
};

class Board {
  unsigned int grid_size;
  std::vector<grid_coords_t> grid;
  grid_coords_t food_loc;
  grid_coords_t init_snake_coords;
  std::unique_ptr<Snake> snake;

  /* spawn food at a random location */
  void spawn_new_food() {
    std::random_device rd;
    std::mt19937 gen(rd());
    // Define a uniform integer distribution for numbers between 0 and grid_size
    std::uniform_int_distribution<> distrib(0, grid_size - 1);

    /* make sure the food doesn't spawn on positions where snake would collide
     */
    do {
      food_loc.x = distrib(gen);
      food_loc.y = distrib(gen);
    } while (is_collision(food_loc));
  }

public:
  Board(const unsigned int grid_size = default_grid_size,
        const grid_coords_t init_snake_coords = {0, 0})
      : grid_size(grid_size), init_snake_coords(init_snake_coords),
        snake(std::make_unique<Snake>(Snake(init_snake_coords))) {
    spawn_new_food();
  }

  // Getters
  auto get_grid_size() const { return grid_size; }
  auto get_snake() { return *snake; }
  auto get_food_loc() const { return food_loc; }
  auto get_grid() const { return grid; }

  void update_snake_dir(Direction dir) { snake->set_direction(dir); }

  bool is_collision(const grid_coords_t next_loc) const {

    return (
        /* check collision with walls */
        (( next_loc.x <= 0 || next_loc.x >= grid_size) ||
         (next_loc.y <= 0 || next_loc.y >= grid_size)) ||
        /*  check collision with body */
        snake->has_snake(next_loc));
  }

  bool update() {
    /* get next head location */
    auto next_head_location = snake->get_next_head_location();
    /* check for bad collision */
    if (is_collision(next_head_location))
      return false;

    /* check for collision with food */
    bool has_eaten_food = false;
    if (next_head_location == food_loc) {
      spawn_new_food();
      has_eaten_food = true;
    }
    snake->move(has_eaten_food);
    return true;
  }

  bool reset() {
    auto new_snake = std::make_unique<Snake>(Snake(init_snake_coords));
    snake = std::move(new_snake);
    spawn_new_food();
    return true;
  }
};
#endif /* SNAKE_H */
