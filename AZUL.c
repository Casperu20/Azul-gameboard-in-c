// MILESTONE 3:    BUNEA NICOLAE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LENGTH 100 // for name
#define TILES_PER_TYPE 20
#define NUMBER_OF_TYPES 5
#define BAG_SIZE (TILES_PER_TYPE * NUMBER_OF_TYPES)
#define FACTORY_DISPLAYS 5
#define TILES_PER_FACTORY 4
#define MAX_PATTERN_LINES 5
#define WALL_SIZE 5
#define FLOOR_LINE_MAX 7
#define MIDDLE_SIZE 101

typedef struct{
  int patternLines[MAX_PATTERN_LINES][MAX_PATTERN_LINES];
  int wall[WALL_SIZE][WALL_SIZE]; // 5x5 wall
  int floorLine[FLOOR_LINE_MAX]; // Floor line
  int score;
}PLAYER_BOARD;

typedef struct{
  char player_name[MAX_LENGTH];
  PLAYER_BOARD board;
}PLAYER;

typedef struct{
  PLAYER players[2]; // Made this game only for 2 players
  int factoryDisplays[FACTORY_DISPLAYS][TILES_PER_FACTORY];
  int middleAreaHasFirstPlayerToken; // 0 = No, 1 = Yes
  int middleArea[MIDDLE_SIZE];
}GAME_STATE;

void initialize_player_board(PLAYER_BOARD* board){
  // I'll set default values at the start:
  memset(board->patternLines, -1, sizeof(board->patternLines)); // -1 - EMPTY
  memset(board->wall, 0, sizeof(board->wall)); // 0 - EMPTY
  // Score marker & floor
  memset(board->floorLine, -1, sizeof(board->floorLine)); // -1 - EMPTY
  board->score = 0;
}

void initialize_players(PLAYER* player1, PLAYER* player2){
  printf("Player's 1 name: ");
  scanf("%99s", player1->player_name);
  printf("Player's 2 name: ");
  scanf("%99s", player2->player_name);
  // After each name, I'll set up their boards:
  initialize_player_board(&player1->board);
  initialize_player_board(&player2->board);
}

void initialize_bag(int bag[]){
  for (int type = 0; type < NUMBER_OF_TYPES; ++type){
    for (int tile = 0; tile < TILES_PER_TYPE; ++tile){
      bag[type * TILES_PER_TYPE + tile] = type;
    }
  }
}
// I tried to use rand() to randomised the tiles, seems to work fine
void shuffle_bag(int bag[], int size){
  printf("\n  DEBUG --> Bag contents after rand():  [SIZE = %d]\n", size);
  for (int i = size - 1; i > 0; --i){
    int j = rand() % (i + 1);
    int temp = bag[i];
    bag[i] = bag[j];
    bag[j] = temp;
    printf("%d, ", bag[i]);
  }
  printf("\n");
}

void print_tile(int tileType){
  const char* colors[NUMBER_OF_TYPES] = {
    "\x1b[34m", // Blue
    "\x1b[33m", // Yellow
    "\x1b[31m", // Red
    "\x1b[38;5;16m", // Black
    "\x1b[37m"  // White
  };
  
  if(tileType >= 0 && tileType < NUMBER_OF_TYPES){
    if(tileType == 0){
      printf("%s[Blue]\x1b[0m", colors[tileType]); 
    }
    else if(tileType == 1){
      printf("%s[Yellow]\x1b[0m", colors[tileType]); 
    }
    else if(tileType == 2){
      printf("%s[Red]\x1b[0m", colors[tileType]); 
    }
    else if(tileType == 3){
      printf("%s[Black]\x1b[0m", colors[tileType]); 
    }
    else{
      printf("%s[White]\x1b[0m", colors[tileType]); 
    }
  }
  else{
    printf("   ");
  }
}

void fill_factory_displays(GAME_STATE* game, int bag[], int* size){
  for (int i = 0; i < FACTORY_DISPLAYS; ++i){
    for (int j = 0; j < TILES_PER_FACTORY && *size > 0; ++j){
      game->factoryDisplays[i][j] = bag[*size - 1];
      // the size of bag will decrease after filling!
      (*size)--;
    }
  }
  // Printing:
  printf("\n");
  for (int i = 0; i < FACTORY_DISPLAYS; ++i){
    printf("Factory %d: ", i + 1);
    for (int j = 0; j < TILES_PER_FACTORY; ++j){
      //printf("%d ", game->factoryDisplays[i][j]);
      print_tile(game->factoryDisplays[i][j]);
      printf(" "); 
    }
    printf("\n");
  }
  /*printf("Middle Area: ");
  if(game->middleAreaHasFirstPlayerToken){
    printf("X\n");
  } 
  else{
    printf("NO 1st Player Token\n");
  }*/
  
  printf("\n  DEBUG --> Tiles remained in bag: %d\n", *size);
  printf("Remaining bag contents: ");
  for(int i = 0; i < *size; i++){
    printf("%d, ", bag[i]);
  }
  printf("\n\n");
  
}

void display_middle_area(GAME_STATE* game){
  printf("Middle Area: ");
  if(game->middleAreaHasFirstPlayerToken == 1){
    printf("X\n");
  } 
  else{
    printf("NO 1st Player Token\n");
  }
}

void player_choose_tiles(GAME_STATE* game, int playerIndex){
  int factoryDisplay, tileColor;
  printf("\n0 = Blue | 1 = Yellow | 2 = Red | 3 = Black | 4 = White | X = First player token\n");
  printf("\n%s, choose a factory display (1-5): ", game->players[playerIndex].player_name);
  scanf("%d", &factoryDisplay);
  while (factoryDisplay < 1 || factoryDisplay > 5){
    printf("WRONG NUMBER! Please choose a factory display (1-5): ");
    scanf("%d", &factoryDisplay);
  }
  
  printf("Select what tile you want (0-4): ");
  scanf("%d", &tileColor);
  while (tileColor < 0 || tileColor >= NUMBER_OF_TYPES){
    printf("Invalid choice. Please choose a tile color (0=Blue, 1=Yellow, 2=Red, 3=Black, 4=White): ");
    scanf("%d", &tileColor);
  }
  
  printf("\n%s got from factory display %d the tiles with color %d.\n", game->players[playerIndex].player_name, factoryDisplay, tileColor);
}

void display_player_board(const PLAYER_BOARD* board){
  printf("  --> Score marker: %d\n\n", board->score);

  printf("Pattern Lines:\n");
  for (int i = 0; i < MAX_PATTERN_LINES; i++){
    for (int j = 0; j <= i; j++){
      if (board->patternLines[i][j] == -1)
        printf("%2d ", board->patternLines[i][j]); // empty yet
      else 
        print_tile(board->patternLines[i][j]);
    }
    printf("\n");
  }
  printf("\nWall:\n");
  for (int i = 0; i < WALL_SIZE; i++){
    for (int j = 0; j < WALL_SIZE; j++) {
      if (board->wall[i][j] == 0)
        printf("%2d ", board->wall[i][j]);
      else 
        print_tile(board->wall[i][j]);
    }
    printf("\n");
  }
  
  printf("\nFloor Line: ");
  for (int i = 0; i < FLOOR_LINE_MAX; i++){
    if (board->floorLine[i] == -1){
      printf(" [EMPTY] ");
      //printf("%d ", board->floorLine[i]);
    } 
    else{
      print_tile(board->floorLine[i]);
      printf(" ");
    }
  }
  printf("\n\n(Here will be the tile placing section)\n");
}

#define PLAYER_TOKEN 5
void initialize_game_state(GAME_STATE* game){
  // Initialize players
  initialize_players(&game->players[0], &game->players[1]);
  // Place the 1st player token
  for (int i = 0; i < MIDDLE_SIZE; ++i) {
    game->middleArea[i] = -1; // Initializiing empty
  }
  
  game->middleArea[0] = PLAYER_TOKEN;
  game->middleAreaHasFirstPlayerToken = 1;
  // Intiliaze and randomize the bag, after it fills the facotry displays
  int bag[BAG_SIZE];
  initialize_bag(bag);
  int size = BAG_SIZE;
  shuffle_bag(bag, size);
  fill_factory_displays(game, bag, &size);
}

int main(){
  srand(time(NULL));
    
  GAME_STATE game;
  initialize_game_state(&game);
  
  display_middle_area(&game);
    
  for (int i = 0; i < 2; ++i){
    player_choose_tiles(&game, i);
  }
    
  // For gameboards:
  for (int i = 0; i < 2; ++i){
    printf("\n%s's Board:\n", game.players[i].player_name);
    display_player_board(&game.players[i].board);
  }

  return 0;
}

/* OLD DISPLAYING FOR PATTERN LINE + WALL
int maxRows = WALL_SIZE > MAX_PATTERN_LINES ? WALL_SIZE : MAX_PATTERN_LINES;
  for (int i = 0; i < maxRows; i++){
    if (i < MAX_PATTERN_LINES){
      for (int j = 0; j <= i; j++){
        if (board->patternLines[i][j] == -1)
          printf("%2d ", board->patternLines[i][j]);
        else 
           printf(" x "); // will continue
      }
      for (int k = i + 1; k < MAX_PATTERN_LINES; k++){
        printf("   "); // 3 spaces for 1 missing tile
      }
    } 
    else{
      for (int k = 0; k < MAX_PATTERN_LINES; k++){
        printf("   ");
      }
    }
    printf(" | ");
    if (i < WALL_SIZE){
      for (int j = 0; j < WALL_SIZE; j++){
        if (board->wall[i][j] == 0)
          printf("%2d ", board->wall[i][j]);
        else 
          printf(" x ");
      }
    }
    printf("\n");
  }
*/
