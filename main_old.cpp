#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <random>
#include <array>
#include <vector>

// Color variables
const sf::Color NORD_DARK(36, 36, 36, 255);
const sf::Color NORD_WHITE(236, 239, 244, 255);
const sf::Color NORD_BLUE(136, 192, 208, 255);

// Size of stuff
const int WINDOW_SIZE = 800;
const int NUM_OF_LINES = 40;
const int WALL_SIZE = 2;
const int CELL_SIZE = (WINDOW_SIZE/NUM_OF_LINES);
const int DELAY = 0;

// Stuff
sf::RectangleShape CELL_SQUARE({CELL_SIZE-(2*WALL_SIZE), CELL_SIZE-(2*WALL_SIZE)});
sf::RectangleShape BORDER_VERTICAL({WALL_SIZE, CELL_SIZE});
sf::RectangleShape BORDER_HORIZONTAL({CELL_SIZE+(2*WALL_SIZE), WALL_SIZE});

// Color of stuff
const sf::Color BG_COLOR = NORD_DARK;
const sf::Color CELL_COLOR = NORD_DARK;
const sf::Color BORDER_COLOR = NORD_WHITE;
const sf::Color HIGHLIGHT_COLOR = NORD_BLUE;
const sf::Color BORDER_HIGHLIGHT_COLOR = CELL_COLOR;

// Random number generation
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis;

int rand_num(int start, int end) {
    // Update the distribution range
    dis.param(std::uniform_int_distribution<>::param_type(start, end));
    return dis(gen);
}

// Cell class
class Cell{
  public:
    int x;
    int y;

    bool active{false};
    bool highlighted{false};
    bool wall_up{true};
    bool wall_down{true};
    bool wall_left{true};
    bool wall_right{true};

    void draw_cell(sf::RenderWindow* window, int x, int y){
      CELL_SQUARE.setPosition(x+WALL_SIZE, y+WALL_SIZE);
      if(this->highlighted){
        CELL_SQUARE.setFillColor(HIGHLIGHT_COLOR);
        BORDER_HORIZONTAL.setFillColor(BORDER_HIGHLIGHT_COLOR);
        BORDER_VERTICAL.setFillColor(BORDER_HIGHLIGHT_COLOR);
      }
      else{
        CELL_SQUARE.setFillColor(CELL_COLOR);
        BORDER_HORIZONTAL.setFillColor(BORDER_COLOR);
        BORDER_VERTICAL.setFillColor(BORDER_COLOR);
      }

      window->draw(CELL_SQUARE);

      if(wall_up){
        BORDER_HORIZONTAL.setPosition(x-WALL_SIZE, y);
        window->draw(BORDER_HORIZONTAL);
      }
      if(wall_down){
        BORDER_HORIZONTAL.setPosition(x-WALL_SIZE, y+(CELL_SIZE-WALL_SIZE));
        window->draw(BORDER_HORIZONTAL);
      }
      if(wall_left){
        BORDER_VERTICAL.setPosition(x, y);
        window->draw(BORDER_VERTICAL);
      }
      if(wall_right){
        BORDER_VERTICAL.setPosition(x+(CELL_SIZE-WALL_SIZE), y);
        window->draw(BORDER_VERTICAL);
      }

    }

};

class Maze{
  public:
    std::array<std::array<Cell, NUM_OF_LINES>, NUM_OF_LINES> matrix;

    Maze(){
      for(int y = 0; y < NUM_OF_LINES; y++){
        for(int x = 0; x < NUM_OF_LINES; x++){
          matrix[y][x].x = x;
          matrix[y][x].y = y;
        }
      }
    }

    void draw(sf::RenderWindow* window){
      for(int y = 0; y < NUM_OF_LINES; y++){
        for(int x = 0; x < NUM_OF_LINES; x++){
          if(this->matrix[y][x].active){
            this->matrix[y][x].draw_cell(window, x*CELL_SIZE, y*CELL_SIZE);
          }
        }
      }
    }

    Cell* random_unvisited_neighbor(Cell* cell){
      // Array to store neighbors
      std::vector<Cell*> neighbors;

      // Left neighbor
      if(cell->x > 0){
        if(this->matrix[cell->y][cell->x-1].active == false)
          neighbors.push_back(&(this->matrix[cell->y][cell->x-1]));
      }
      // Right neighbor
      if(cell->x < NUM_OF_LINES - 1){
        if(this->matrix[cell->y][cell->x+1].active == false)
          neighbors.push_back(&(this->matrix[cell->y][cell->x+1]));
      }
      // Up neighbor
      if(cell->y > 0){
        if(this->matrix[cell->y-1][cell->x].active == false)
          neighbors.push_back(&(this->matrix[cell->y-1][cell->x]));
      }
      // Down neighbor
      if(cell->y < NUM_OF_LINES - 1){
        if(this->matrix[cell->y+1][cell->x].active == false)
          neighbors.push_back(&(this->matrix[cell->y+1][cell->x]));
      }

      if(neighbors.empty()){
        return nullptr;
      }

      int random_index = rand_num(0, neighbors.size()-1);
      return neighbors[random_index];
    }

    bool is_deadend(Cell* cell){

      // Left neighbor
      if(cell->x > 0){
        if(this->matrix[cell->y][cell->x-1].active == false)
          return false;
      }
      // Right neighbor
      if(cell->x < NUM_OF_LINES - 1){
        if(this->matrix[cell->y][cell->x+1].active == false)
          return false;
      }
      // Up neighbor
      if(cell->y > 0){
        if(this->matrix[cell->y-1][cell->x].active == false)
          return false;
      }
      // Down neighbor
      if(cell->y < NUM_OF_LINES - 1){
        if(this->matrix[cell->y+1][cell->x].active == false)
          return false;
      }

      return true;
    }

    void mazefy_binary_tree(sf::RenderWindow* window){
      for(int y = 0; y < NUM_OF_LINES; y++){
        for(int x = 0; x < NUM_OF_LINES; x++){
          this->matrix[y][x].active = true;

          int random = rand_num(0, 1);

          if(random == 1 or x == 0 or y == 0){
            this->matrix[y][x].wall_up = false;
            if(y > 0){
              this->matrix[y-1][x].wall_down = false;
              this->matrix[y-1][x].draw_cell(window, x*CELL_SIZE, (y-1)*CELL_SIZE);
            }
          }

          if(random == 0 or x == 0 or y == 0){
            this->matrix[y][x].wall_left = false;
            if(x > 0){
              this->matrix[y][x-1].wall_right = false;
              this->matrix[y][x-1].draw_cell(window, (x-1)*CELL_SIZE, y*CELL_SIZE);
            }
          }

          this->matrix[y][x].draw_cell(window, x*CELL_SIZE, y*CELL_SIZE);

          window->display();

          sf::sleep(sf::milliseconds(DELAY));
        }
      }
    }

    void mazefy_depth_first_search(sf::RenderWindow* window, Cell* cell){
      cell->active = true;
      while(is_deadend(cell) == false){

        Cell* neighbor = this->random_unvisited_neighbor(cell);

        if(cell->x > neighbor->x){
          cell->wall_left = false;
          neighbor->wall_right = false;
        }
        else if(cell->x < neighbor->x){
          cell->wall_right = false;
          neighbor->wall_left = false;
        }
        else if(cell->y > neighbor->y){
          cell->wall_up = false;
          neighbor->wall_down = false;
        }
        else if(cell->y < neighbor->y){
          cell->wall_down = false;
          neighbor->wall_up = false;
        }

        cell->draw_cell(window, cell->x*CELL_SIZE, cell->y*CELL_SIZE);
        neighbor->draw_cell(window, neighbor->x*CELL_SIZE, neighbor->y*CELL_SIZE);
        window->display();

        mazefy_depth_first_search(window, neighbor);
      }
    };
};

int main(){
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "MazeTrix");
    window.setFramerateLimit(120);
    window.clear(BG_COLOR);

    Maze maze;
    maze.mazefy_depth_first_search(&window, &(maze.matrix[10][10]));


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(BG_COLOR);

        maze.draw(&window);

        window.display();
    }

    return 0;
}