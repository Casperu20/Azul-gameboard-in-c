// MILESTONE 2:    BUNEA NICOLAE

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
}GAME_STATE;

void initialize_player_board(PLAYER_BOARD* board){
  // I'll set default values at the start:
  memset(board->patternLines, -1, sizeof(board->patternLines)); // -1 - EMPTY
  memset(board->wall, 0, sizeof(board->wall)); // 0 - EMPTY
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
  printf("\nBag contents after rand():  [SIZE = %d]\n", size);
  for (int i = size - 1; i > 0; --i){
    int j = rand() % (i + 1);
    int temp = bag[i];
    bag[i] = bag[j];
    bag[j] = temp;
    printf("%d, ", bag[i]);
  }
  printf("\n");
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
  printf("\n0 = Blue | 1 = Yellow | 2 = Red | 3 = Black | 4 = White | X = First player token\n");
  for (int i = 0; i < FACTORY_DISPLAYS; ++i){
    printf("Factory %d: ", i + 1);
    for (int j = 0; j < TILES_PER_FACTORY; ++j){
      printf("%d ", game->factoryDisplays[i][j]);
    }
    printf("\n");
  }
  printf("  --> Tiles remained in bag: %d\n", *size);
}

void player_choose_tiles(GAME_STATE* game, int playerIndex){
  int factoryDisplay, tileColor;
  printf("\n%s, choose a factory display (1-5): ", game->players[playerIndex].player_name);
  scanf("%d", &factoryDisplay);
  while (factoryDisplay < 1 || factoryDisplay > 5){
    printf("WRONG NUMBER! Please choose a factory display (1-5): ");
    scanf("%d", &factoryDisplay);
  }
  
  printf("Select what tile you want: ");
  scanf("%d", &tileColor);
  while (tileColor < 0 || tileColor >= NUMBER_OF_TYPES){
    printf("Invalid choice. Please choose a tile color (0=Blue, 1=Yellow, 2=Red, 3=Black, 4=White): ");
    scanf("%d", &tileColor);
  }
  
  printf("\n%s got from factory display %d the tiles with color %d.\n", game->players[playerIndex].player_name, factoryDisplay, tileColor);
}

void display_player_board(const PLAYER_BOARD* board){
  // All this mass is for putting the pattern lines near the wall
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
      for (int j = 0; j < WALL_SIZE; j++) {
        if (board->wall[i][j] == 0)
          printf("%2d ", board->wall[i][j]);
        else 
          printf(" x ");
      }
    }
    printf("\n");
  }
  printf("\n(Pattern lines  |   Wall)\n");
}

void initialize_game_state(GAME_STATE* game){
  // Initialize players
  initialize_players(&game->players[0], &game->players[1]);
  // Place the 1st player token
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
  printf("Middle Area: ");
  if(game.middleAreaHasFirstPlayerToken){
    printf("X\n");
  } 
  else{
    printf("NO 1st Player Token\n");
  }
    
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

