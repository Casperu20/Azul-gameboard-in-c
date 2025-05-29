  // MILESTONE 4:    BUNEA NICOLAE

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
  #define PLAYER_TOKEN 5

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
      // printf("%d, ", bag[i]);
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
        // print_tile(game->factoryDisplays[i][j]);
        printf(" "); 
      }
      printf("FILLED!\n");
    }
    
    printf("\n  DEBUG --> Tiles remained in bag AFTER putting in facotry displays: %d\n", *size);
    /*printf("Remaining bag contents: ");
    for(int i = 0; i < *size; i++){
      printf("%d, ", bag[i]);
    }*/
    printf("\n");
    
  }

  void display_factory_displays(GAME_STATE* game) {
      for (int i = 0; i < FACTORY_DISPLAYS; i++) {
          printf("Factory %d: ", i + 1);
          int empty = 1;
          for (int j = 0; j < TILES_PER_FACTORY; j++) {
              if (game->factoryDisplays[i][j] != -1) {
                  print_tile(game->factoryDisplays[i][j]);
                  printf(" ");
                  empty = 0;
              }
          }
          if (empty) {
              printf("[Empty]");
          }
          printf("\n");
      }
  }

  void display_middle_area(GAME_STATE* game){
    printf("Middle Area: ");
    if(game->middleAreaHasFirstPlayerToken == 1){
      printf("X |");
    } 
    else{
      printf(" ");
    }
    
    for (int i = 0; i < MIDDLE_SIZE; i++) {
      if (game->middleArea[i] != -1) {  // empty spot
        print_tile(game->middleArea[i]);
        printf(" ");
      }
    }
    printf("\n");
  }

  int can_place_tile(PLAYER_BOARD *board, int row, int tileColor){
    if (board->wall[row][tileColor] != 0){
      return 0; // if the tile is already in the wall
    }
    for (int i = 0; i <= row; i++){
      if (board->patternLines[row][i] != -1 && board->patternLines[row][i] != tileColor){
        return 0; // if there s already other color on the patttern line
      }
    }
    return 1; // IF OK
  }

  void place_tiles(PLAYER_BOARD *board, int row, int tileColor, int count){
    int placed = 0;
    for (int i = 0; i <= row && placed < count; i++){
      if (board->patternLines[row][i] == -1){
        board->patternLines[row][i] = tileColor;
        placed++;
      }
    }
      // if PATTERN LINE full => in floor line
    int excess = count - placed;
    if (excess > 0){
      for (int i = 0; i < FLOOR_LINE_MAX && excess > 0; i++){
        if (board->floorLine[i] == -1){
          board->floorLine[i] = tileColor;
          excess--;
        }
      }
    }
  }

  void display_player_board(const PLAYER_BOARD* board);
  void move_unselected_to_middle(GAME_STATE* game, int factoryDisplay);
  void process_tile_placement(GAME_STATE* game, int playerIndex, int tileColor, int count);

  int factory_is_empty(GAME_STATE* game, int factoryIndex){
    for (int i = 0; i < TILES_PER_FACTORY; i++) {
      if (game->factoryDisplays[factoryIndex][i] != -1) {
        return 0;
      }
    }
    return 1; // e gol
  }

  void player_choose_tiles(GAME_STATE* game, int playerIndex){
    int choice, tileColor, count = 0;
    int validChoice = 0;

    while (!validChoice){
      printf("\n%s, choose:  1-5 for Factory Displays / 6 for Middle Area\nChoice: ", game->players[playerIndex].player_name);
      scanf("%d", &choice);

      if (choice < 1 || choice > 6){
        printf("Invalid choice. Please select a valid factory display or the middle area.\n");
      } 
      else if (choice == 6){
        validChoice = 1;
      }
      else if (factory_is_empty(game, choice - 1)){
        printf("Selected factory display is empty. Please select a different one.\n");
      }
      else{
        validChoice = 1;
      }
    }
    
    if ((choice < 1 || choice > 6) || (choice != 6 && factory_is_empty(game, choice - 1))){
      printf("Invalid input or empty factory display, returning to selection.\n");
      return;
    }
      
    if (choice == 6){  // if middleArea
      printf("\n0 = Blue | 1 = Yellow | 2 = Red | 3 = Black | 4 = White\nSelect what tile you want (0-4): ");
      scanf("%d", &tileColor);
      while (tileColor < 0 || tileColor >= NUMBER_OF_TYPES){
        printf("Invalid choice. Please choose a tile color (0=Blue, 1=Yellow, 2=Red, 3=Black, 4=White): ");
        scanf("%d", &tileColor);
      }

          // to pick up the 1st player token
      if (game->middleAreaHasFirstPlayerToken){
        game->players[playerIndex].board.floorLine[0] = PLAYER_TOKEN;
        game->middleAreaHasFirstPlayerToken = 0;
        printf("First player token picked and will start next round!\n");
      }

      for (int i = 0; i < MIDDLE_SIZE; i++){
        if (game->middleArea[i] == tileColor){
          count++;
          game->middleArea[i] = -1; // To remove the tiles remained in middle
        }
      }
    } 
    else if (choice >= 1 && choice <= 5 && game->factoryDisplays[choice][0] != -1){  // if factory
      choice -= 1;
      printf("\n0 = Blue | 1 = Yellow | 2 = Red | 3 = Black | 4 = White\nSelect what tile you want (0-4): ");
      scanf("%d", &tileColor);
      
      while (tileColor < 0 || tileColor >= NUMBER_OF_TYPES || (game->factoryDisplays[choice][0] != tileColor && game->factoryDisplays[choice][1] != tileColor && game->factoryDisplays[choice][2] != tileColor && game->factoryDisplays[choice][3] != tileColor)){
        printf("Invalid choice. Please choose a tile color (0=Blue, 1=Yellow, 2=Red, 3=Black, 4=White): ");
        scanf("%d", &tileColor);
      }
          
          // clear selected tiles
      for (int i = 0; i < TILES_PER_FACTORY; i++){
        if (game->factoryDisplays[choice][i] == tileColor){
          count++;
          game->factoryDisplays[choice][i] = -1;
        }
      }
      // !!!
      move_unselected_to_middle(game, choice);
      }
      else{
        printf("Invalid input, returning to selection.\n");
        return;
      }

      printf("\n%s got %d tiles of color: ", game->players[playerIndex].player_name, count);
      print_tile(tileColor);
      printf(" tiles.\n");

      // Next placing onto board:
      process_tile_placement(game, playerIndex, tileColor, count);
  }

  void move_unselected_to_middle(GAME_STATE* game, int factoryDisplay){
    int indexMiddleArea = 0;
    while (game->middleArea[indexMiddleArea] != -1 && indexMiddleArea < MIDDLE_SIZE){
      indexMiddleArea++;
    }
    for (int i = 0; i < TILES_PER_FACTORY; i++) {
      if (game->factoryDisplays[factoryDisplay][i] != -1) {
        game->middleArea[indexMiddleArea++] = game->factoryDisplays[factoryDisplay][i];
        game->factoryDisplays[factoryDisplay][i] = -1;
      }
    }
  }

  void process_tile_placement(GAME_STATE* game, int playerIndex, int tileColor, int count){
      printf("\nInitial state of %s's board:\n", game->players[playerIndex].player_name);
      display_player_board(&game->players[playerIndex].board);

      int row;
      while (1){
        printf("Choose a row (1-5) to place your tiles: ");
        scanf("%d", &row);
        row -= 1; 
        if (row < 0 || row >= MAX_PATTERN_LINES){
          printf("Invalid row. Please choose a row (1-5): ");
          continue;  // ask for input again
        }

        if (can_place_tile(&game->players[playerIndex].board, row, tileColor)){
          place_tiles(&game->players[playerIndex].board, row, tileColor, count);
          printf("Tiles placed successfully.\n");
          break;
        }
        else{
          printf("Cannot place tiles in row %d due to restrictions. Please choose another row.\n", row + 1);
        }
      }

      printf("\n%s's board after placement:\n", game->players[playerIndex].player_name);
      display_player_board(&game->players[playerIndex].board);
      display_middle_area(game);
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
    
    #define RESET "\x1b[0m"
    printf("\nWall:\n");
    int wall_color_pattern[WALL_SIZE][WALL_SIZE] = {
      {0, 1, 2, 3, 4},
      {4, 0, 1, 2, 3},
      {3, 4, 0, 1, 2},
      {2, 3, 4, 0, 1},
      {1, 2, 3, 4, 0}
    };
    const char* colors[NUMBER_OF_TYPES] = {"\x1b[44m", "\x1b[43m", "\x1b[41m", "\x1b[40m", "\x1b[47m"};
                                            // BLUE        YELLOW      RED       BLACK       WHITE
    for (int i = 0; i < WALL_SIZE; i++){
      for (int j = 0; j < WALL_SIZE; j++) {
        if (board->wall[i][j] == 0){ // only if is empty
          printf("%s  %2d %s", colors[wall_color_pattern[i][j]], board->wall[i][j], RESET);
        }
        else{
          print_tile(board->wall[i][j]);
        }   
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
        if(board->floorLine[i] == PLAYER_TOKEN){
          printf("[X] ");
        }
        else{
          print_tile(board->floorLine[i]);
          printf(" ");
        }
      }
    }
    printf("\n\n");
    // Next will be the tile placing section
  }

  int tiles_remaining(GAME_STATE* game){
    // IF ARE TILES REMAINING IN FACOTRY DISPLAYS!!!!!!!!!!
    for (int i = 0; i < FACTORY_DISPLAYS; i++) {
      for (int j = 0; j < TILES_PER_FACTORY; j++){
        if (game->factoryDisplays[i][j] != -1){
          return 1; // STILL ARE
        }
      }
    }
    // SAME FOR MIDDLE
    for (int i = 0; i < MIDDLE_SIZE; i++){
      if (game->middleArea[i] != -1 && game->middleArea[i] != PLAYER_TOKEN) {
        return 1; 
      }
    }
    return 0; // NO TILES REMAINED
  }

  void transfer_tiles_to_wall_and_bag(GAME_STATE* game, int bag[]){
    for (int p = 0; p < 2; p++){ // for each player
      PLAYER* player = &game->players[p];
      for (int row = 0; row < MAX_PATTERN_LINES; row++){
        int patternLineSize = row + 1;  // check if the line is completed
        int tileType = player->board.patternLines[row][0];  // check what tile requirment
        int isComplete = 1; // line
        for (int i = 0; i < patternLineSize; i++){
          if (player->board.patternLines[row][i] != tileType){
            isComplete = 0;
            break;
          }
        }
        // Place one tile on the wall
        if (isComplete && tileType != -1){
          player->board.wall[row][tileType] = tileType + 1; // + 1 = tile placed

          for (int i = 0; i < patternLineSize; i++) {
            player->board.patternLines[row][i] = -1;
          }
          int excessTiles = patternLineSize - 1;
          for (int i = 0; i < excessTiles; i++){
            int index = 0;
            while (bag[index] != -1)
              index++; // for empty spots in bag
            bag[index] = tileType;  
          }
        }
      }
    }
  }

  // loop THE GAME INTO ROUNDS!!!!
  void play_round(GAME_STATE* game){
    int currentPlayer = 0;  
    int bag[BAG_SIZE];
    initialize_bag(bag);
    while (tiles_remaining(game)){
      display_factory_displays(game);  //The remaining facotry displays
      display_middle_area(game);
      
      player_choose_tiles(game, currentPlayer);
      currentPlayer = 1 - currentPlayer;  // to switch them
    }
    
    transfer_tiles_to_wall_and_bag(game, bag);
    printf("All tiles have been taken.\n");
  }

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
    
    // display_middle_area(&game);
    /*for (int i = 0; i < 2; ++i){
      player_choose_tiles(&game, i);
    }*/
    play_round(&game);
    
    // For gameboards:
    for (int i = 0; i < 2; ++i){
      printf("\n%s's Board at the end of the round:\n", game.players[i].player_name);
      display_player_board(&game.players[i].board);
    }

    return 0;
  }
