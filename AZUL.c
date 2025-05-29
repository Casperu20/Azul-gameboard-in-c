// MILESTONE 6
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
#define RESET "\x1b[0m"

typedef struct {
    int patternLines[MAX_PATTERN_LINES][MAX_PATTERN_LINES];
    int wall[WALL_SIZE][WALL_SIZE]; // 5x5 wall
    int floorLine[FLOOR_LINE_MAX]; // Floor lineCp
    int score;
} PLAYER_BOARD;

typedef struct {
    char player_name[MAX_LENGTH];
    PLAYER_BOARD board;
} PLAYER;

typedef struct {
    PLAYER players[2]; // Made this game only for 2 players
    int factoryDisplays[FACTORY_DISPLAYS][TILES_PER_FACTORY];
    int middleAreaHasFirstPlayerToken; // 0 = No, 1 = Yes
    int middleArea[MIDDLE_SIZE];
    int currentPlayer; // Track the current player
} GAME_STATE;

void initialize_player_board(PLAYER_BOARD* board) {
    for (int i = 0; i < MAX_PATTERN_LINES; ++i) {
        for (int j = 0; j <= i; ++j) {
            board->patternLines[i][j] = -1; // -1 - EMPTY
        }
    }
    memset(board->wall, 0, sizeof(board->wall)); // 0 - EMPTY
    memset(board->floorLine, -1, sizeof(board->floorLine)); // -1 - EMPTY
    board->score = 0;
}

void initialize_players(PLAYER* player1, PLAYER* player2) {
    printf("Player's 1 name: ");
    scanf("%99s", player1->player_name);
    printf("Player's 2 name: ");
    scanf("%99s", player2->player_name);
    //         After each name, I'll set up their boards:
    printf("\n");
    initialize_player_board(&player1->board);
    initialize_player_board(&player2->board);
}

void initialize_bag(int bag[]) {
    for (int type = 0; type < NUMBER_OF_TYPES; ++type) {
        for (int tile = 0; tile < TILES_PER_TYPE; ++tile) {
            bag[type * TILES_PER_TYPE + tile] = type;
        }
    }
}

void shuffle_bag(int bag[], int size) {
    for (int i = size - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int temp = bag[i];
        bag[i] = bag[j];
        bag[j] = temp;
    }
    //printf("Bag size: %d\n", size);
}

void print_tile(int tileType) {
    const char* colors[NUMBER_OF_TYPES] = {
        "\x1b[34m", // Blue
        "\x1b[33m", // Yellow
        "\x1b[31m", // Red
        "\x1b[38;5;16m", // Black
        "\x1b[37m"  // White
    };

    if (tileType >= 0 && tileType < NUMBER_OF_TYPES) {
        printf("%s[%s]\x1b[0m", colors[tileType], 
               (tileType == 0 ? "Blue" : tileType == 1 ? "Yellow" : tileType == 2 ? "Red" : tileType == 3 ? "Black" : "White"));
    } else {
        printf("   ");
    }
}

void fill_factory_displays(GAME_STATE* game, int bag[], int* size) {
    for (int i = 0; i < FACTORY_DISPLAYS; ++i) {
        for (int j = 0; j < TILES_PER_FACTORY && *size > 0; ++j) {
            game->factoryDisplays[i][j] = bag[--(*size)];
        }
    }
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

void display_middle_area(GAME_STATE* game) {
    printf("Middle Area: ");
    if (game->middleAreaHasFirstPlayerToken == 1) {
        printf("X | ");
    }

    for (int i = 0; i < MIDDLE_SIZE; i++) {
        if (game->middleArea[i] != -1) {
            print_tile(game->middleArea[i]);
            printf(" ");
        }
    }
    printf("\n");
}


// RESOLVED THE ERRORS IF THE TILE CANT BE PLACED THERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!             [IMPORTANT ONE BUT OOK NOW]

int can_place_tile(PLAYER_BOARD* board, int row, int tileColor) {
    for (int col = 0; col < WALL_SIZE; col++) {
        if (board->wall[row][col] == tileColor + 1) {
            return 0; //THE TILE IS ALREADY IN THE WALL !!!!
        }
    }
    for (int i = 0; i <= row; i++) {
        if (board->patternLines[row][i] != -1 && board->patternLines[row][i] != tileColor) {
            return 0; // Different color 
        }
    }
    return 1; //Can place the tile there
}

void place_tiles(PLAYER_BOARD* board, int row, int tileColor, int count) {
    int placed = 0;
    for (int i = 0; i <= row && placed < count; i++) {
        if (board->patternLines[row][i] == -1) {
            board->patternLines[row][i] = tileColor;
            placed++;
        }
    }
    //Excess tiles transfer to floor line
    int excess = count - placed;
    if (excess > 0) {
        for (int i = 0; i < FLOOR_LINE_MAX && excess > 0; i++) {
            if (board->floorLine[i] == -1) {
                board->floorLine[i] = tileColor;
                excess--;
            }
        }
    }
}

void display_player_board(const PLAYER_BOARD* board);
void move_unselected_to_middle(GAME_STATE* game, int factoryDisplay);
void process_tile_placement(GAME_STATE* game, int playerIndex, int tileColor, int count);

int factory_is_empty(GAME_STATE* game, int factoryIndex) {
    for (int i = 0; i < TILES_PER_FACTORY; i++) {
        if (game->factoryDisplays[factoryIndex][i] != -1) {
            return 0;
        }
    }
    return 1;
}

int middle_area_is_empty(GAME_STATE* game) {
    for (int i = 0; i < MIDDLE_SIZE; i++) {
        if (game->middleArea[i] != -1 && game->middleArea[i] != PLAYER_TOKEN) {
            return 0; // IS NOT
        }
    }
    return 1; //IF MIDDLE AREA IS EMPTY
}

void player_choose_tiles(GAME_STATE* game, int playerIndex) {
    int choice = 0, tileColor = -1, count = 0;
    int validChoice = 0;

    while (!validChoice) {
        printf("\n%s, choose: 1-5 for Factory Displays / 6 for Middle Area\nChoice: ", game->players[playerIndex].player_name);
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); //CLEAR 
            continue;
        }

        if (choice < 1 || choice > 6) {
            printf("Invalid choice. Please select a valid factory display or the middle area.\n");
        } else if (choice == 6) {
            if (middle_area_is_empty(game)) {
                printf("The middle area is empty. Please select a different area.\n");
            } else {
                validChoice = 1;
            }
        } else if (factory_is_empty(game, choice - 1)) {
            printf("Selected factory display is empty. Please select a different one.\n");
        } else {
            validChoice = 1;
        }
    }

    if (choice == 6) {
        while (1) {
            printf("\n0 = Blue | 1 = Yellow | 2 = Red | 3 = Black | 4 = White\nSelect what tile you want (0-4): ");
            if (scanf("%d", &tileColor) != 1) {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n'); //CLEAR
                continue;
            }

            if (tileColor < 0 || tileColor >= NUMBER_OF_TYPES) {
                printf("Invalid choice. Please choose a tile color (0=Blue, 1=Yellow, 2=Red, 3=Black, 4=White).\n");
                continue;
            }

            int tileFound = 0;
            for (int i = 0; i < MIDDLE_SIZE; i++) {
                if (game->middleArea[i] == tileColor) {
                    tileFound = 1;
                    break;
                }
            }

            if (!tileFound) {
                printf("The chosen tile color is not in the middle area. Please select a different tile color.\n");
                continue;
            }

            if (game->middleAreaHasFirstPlayerToken) {
                game->players[playerIndex].board.floorLine[0] = PLAYER_TOKEN;
                game->middleAreaHasFirstPlayerToken = 0;
                game->currentPlayer = playerIndex; //uPDATE CURRENT PLAYER
                printf("First player token picked and will start next round!\n");
            }

            for (int i = 0; i < MIDDLE_SIZE; i++) {
                if (game->middleArea[i] == tileColor) {
                    count++;
                    game->middleArea[i] = -1;
                }
            }
            break;
        }
    } else if (choice >= 1 && choice <= 5) {
        choice -= 1;
        printf("\n0 = Blue | 1 = Yellow | 2 = Red | 3 = Black | 4 = White\nSelect what tile you want (0-4): ");
        if (scanf("%d", &tileColor) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); //clear input BUFF
            return;
        }

        while (tileColor < 0 || tileColor >= NUMBER_OF_TYPES || (game->factoryDisplays[choice][0] != tileColor && game->factoryDisplays[choice][1] != tileColor && game->factoryDisplays[choice][2] != tileColor && game->factoryDisplays[choice][3] != tileColor)) {
            printf("Invalid choice. Please choose a tile color (0=Blue, 1=Yellow, 2=Red, 3=Black, 4=White): ");
            if (scanf("%d", &tileColor) != 1) {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n'); //clear input BUFF
                return;
            }
        }

        for (int i = 0; i < TILES_PER_FACTORY; i++) {
            if (game->factoryDisplays[choice][i] == tileColor) {
                count++;
                game->factoryDisplays[choice][i] = -1;
            }
        }
        move_unselected_to_middle(game, choice);
    }

    printf("\n%s got %d tiles of color: ", game->players[playerIndex].player_name, count);
    print_tile(tileColor);
    printf(" tiles.\n");

    process_tile_placement(game, playerIndex, tileColor, count);
}


// CLEARING THE FACOTORY AFTER THE TILE WAS CHOOSEN!

void move_unselected_to_middle(GAME_STATE* game, int factoryDisplay) {
    int indexMiddleArea = 0;
    while (game->middleArea[indexMiddleArea] != -1 && indexMiddleArea < MIDDLE_SIZE) {
        indexMiddleArea++;
    }
    for (int i = 0; i < TILES_PER_FACTORY; i++) {
        if (game->factoryDisplays[factoryDisplay][i] != -1) {
            game->middleArea[indexMiddleArea++] = game->factoryDisplays[factoryDisplay][i];
            game->factoryDisplays[factoryDisplay][i] = -1;
        }
    }
}

// going through error possible situations   [done]

void process_tile_placement(GAME_STATE* game, int playerIndex, int tileColor, int count) {
    printf("\nInitial state of %s's board:\n", game->players[playerIndex].player_name);
    display_player_board(&game->players[playerIndex].board);

    int row = -1;
    int canPlace = 0;
    while (!canPlace) {
        printf("Choose a row (1-5) to place your tiles ( SELECT 0 IF THERE'S NO AVAILABLE CHOISE): ");
        if (scanf("%d", &row) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); //clear input BUFF
            continue;
        }
        
        if (row == 0) {
            break;  //skip placing if the pattern line is full
        }

        row -= 1;
        if (row < 0 || row >= MAX_PATTERN_LINES) {
            printf("Invalid row. Please choose a row (1-5) or 0 to skip: ");
            continue;  // error message
        }

        if (can_place_tile(&game->players[playerIndex].board, row, tileColor)) {
            canPlace = 1;
        } else {
            printf("Cannot place tiles in row %d due to restrictions. Please choose another row or 0 to skip.\n", row + 1);
        }
    }

    if (canPlace) {
        place_tiles(&game->players[playerIndex].board, row, tileColor, count);
        printf("Tiles placed successfully.\n");
    } else {
        printf("Cannot place selected tiles on any pattern line. Moving all tiles to the floor line.\n");
        for (int i = 0; i < count; i++) {
            for (int j = 0; j < FLOOR_LINE_MAX; j++) {
                if (game->players[playerIndex].board.floorLine[j] == -1) {
                    game->players[playerIndex].board.floorLine[j] = tileColor;
                    break;
                }
            }
        }
    }

    printf("\n%s's board after placement:\n", game->players[playerIndex].player_name);
    display_player_board(&game->players[playerIndex].board);
    display_middle_area(game);
}


const int wall_color_pattern[WALL_SIZE][WALL_SIZE] = {
    {0, 1, 2, 3, 4},
    {4, 0, 1, 2, 3},
    {3, 4, 0, 1, 2},
    {2, 3, 4, 0, 1},
    {1, 2, 3, 4, 0}
};



// CONSTANT LOOP FOR EACH PLAYER AFTER TILE SELECTING!

void display_player_board(const PLAYER_BOARD* board) {
    printf("  --> Score marker: %d\n\n", board->score);

    printf("Pattern Lines:\n");
    for (int i = 0; i < MAX_PATTERN_LINES; i++) {
        for (int j = 0; j <= i; j++) {
            if (board->patternLines[i][j] == -1)
                printf("[-1] "); // empty
            else
                print_tile(board->patternLines[i][j]);
        }
        printf("\n");
    }

    printf("\nWall:\n");
    const char* colors[NUMBER_OF_TYPES] = {"\x1b[44m", "\x1b[43m", "\x1b[41m", "\x1b[40m", "\x1b[47m"};
           // for better showing on the wall |BLUE      YELLOW     RED         BLACK          WHITE
    for (int i = 0; i < WALL_SIZE; i++) {
        for (int j = 0; j < WALL_SIZE; j++) {
            if (board->wall[i][j] == 0) {
                printf("%s  %2d %s", colors[wall_color_pattern[i][j]], board->wall[i][j], RESET);
            } else {
                print_tile(board->wall[i][j] - 1);  //To show the tile type
            }
        }
        printf("\n");
    }

    printf("\nFloor Line: ");
    for (int i = 0; i < FLOOR_LINE_MAX; i++) {
        if (board->floorLine[i] == -1) {
            printf("[EMPTY] ");
        } else {
            if (board->floorLine[i] == PLAYER_TOKEN) {
                printf("[X] ");
            } else {
                print_tile(board->floorLine[i]);
                printf(" ");
            }
        }
    }
    printf("\n\n");
}

int tiles_remaining(GAME_STATE* game) {
    for (int i = 0; i < FACTORY_DISPLAYS; i++) {
        for (int j = 0; j < TILES_PER_FACTORY; j++) {
            if (game->factoryDisplays[i][j] != -1) {
                return 1; // Tiles remained
            }
        }
    }
    for (int i = 0; i < MIDDLE_SIZE; i++) {
        if (game->middleArea[i] != -1 && game->middleArea[i] != PLAYER_TOKEN) {
            return 1; // Tiles remained
        }
    }
    return 0; //No tiles remaining
}

// SCORING PART WHEN TILES TRANSFER TO WALL:::: !!!


void update_score(PLAYER_BOARD* board, int row, int col) {
    int horizontal_count = 1; // include the newly placed tile for both
    int vertical_count = 1;   

    //horizontally linked
    for (int i = col - 1; i >= 0 && board->wall[row][i] != 0; i--) {
        horizontal_count++;
    }
    for (int i = col + 1; i < WALL_SIZE && board->wall[row][i] != 0; i++) {
        horizontal_count++;
    }

    // Vertical linked
    for (int i = row - 1; i >= 0 && board->wall[i][col] != 0; i--) {
        vertical_count++;
    }
    for (int i = row + 1; i < WALL_SIZE && board->wall[i][col] != 0; i++) {
        vertical_count++;
    }

    // calculate the  points: (horizontal and vertical is scored separately )
    int points = 0;
    if (horizontal_count > 1) points += horizontal_count;
    if (vertical_count > 1) points += vertical_count;

    //1 point if no adjiacent tile!
    if (points == 0) points = 1;

    //Update after the score with all the bonus points then substract the penalties!!!!!
    board->score += points;
    if (board->score < 0) board->score = 0; // score cant be < 0 !!!!
}

void deduct_floor_line_points(PLAYER_BOARD* board) {
    int minus_points = 0;
    for (int i = 0; i < FLOOR_LINE_MAX; i++) {
        if (board->floorLine[i] != -1) {
            // penalties
            if (i == 0) minus_points += 1;
            else if (i == 1) minus_points += 1;
            else if (i == 2) minus_points += 2;
            else if (i == 3) minus_points += 2;
            else if (i == 4) minus_points += 2;
            else if (i == 5) minus_points += 3;
            else if (i == 6) minus_points += 3;
        }
    }

    //Aplying the penalties from the floor line!!!!!!
    board->score -= minus_points;
    if (board->score < 0) board->score = 0; //  !! score cant be < 0
}




void transfer_tiles_to_wall_and_bag(GAME_STATE* game, int bag[]) {
    int bagIndex = BAG_SIZE - 1;

    // each player loop:
    for (int p = 0; p < 2; p++) { 
        PLAYER* player = &game->players[p];
        printf("\nTransferring tiles for %s\n", player->player_name);
        for (int row = 0; row < MAX_PATTERN_LINES; row++) {
            // if line is completed 
            int patternLineSize = row + 1; 
            int tileType = player->board.patternLines[row][0]; // DONT FORGET TO CHECK THE TYLE
            int isComplete = 1; // IM REFERING TO THE LINE 

            for (int i = 0; i < patternLineSize; i++) {
                if (player->board.patternLines[row][i] == -1) {
                    isComplete = 0;
                    break;
                }
            }
            if (isComplete && tileType != -1) {
                int column = -1;
                for (int col = 0; col < WALL_SIZE; col++) {
                    if (wall_color_pattern[row][col] == tileType) {
                        column = col;
                        break;
                    }
                }

                if (column != -1) {
                    player->board.wall[row][column] = tileType + 1; //If the assigment of the type was correct HERE!
                    printf("Placed tile %d in row %d, column %d on the wall.\n", tileType, row, column);
                    update_score(&player->board, row, column); //Updating
                } else {
                    printf("Error: Could not find column for tile %d in row %d.\n", tileType, row);
                }

                for (int i = 0; i < patternLineSize; i++) {
                    player->board.patternLines[row][i] = -1;
                }
                int excessTiles = patternLineSize - 1;
                for (int i = 0; i < excessTiles; i++) {
                    if (bagIndex >= 0) {
                        bag[bagIndex--] = tileType;
                    }
                }
            } else {
                printf("Pattern line %d is not complete or has an invalid tile type.\n", row);
            }
        }
    }
}


// -------------------------------------------------------------------------------| All available tiles have been transfered to the wall.

void calculate_final_score(PLAYER_BOARD* board) {
    //checker for completed rows:

    for (int i = 0; i < WALL_SIZE; i++) {
        int row_complete = 1;
        for (int j = 0; j < WALL_SIZE; j++) {
            if (board->wall[i][j] == 0) {
                row_complete = 0;
                break;
            }
        }
        if (row_complete) {
            board->score += 2; //Score for a completed ROW  ===)  2 POINTS
        }
    }

    //Check if a column is completed:
    for (int j = 0; j < WALL_SIZE; j++) {
        int column_complete = 1;
        for (int i = 0; i < WALL_SIZE; i++) {
            if (board->wall[i][j] == 0) {
                column_complete = 0;
                break;
            }
        }
        if (column_complete) {
            board->score += 7; //A completed COLUMN  ===)  7 POINTS
        }
    }

    // Each 5 colors on a set func here:

    for (int color = 1; color <= NUMBER_OF_TYPES; color++) {
        int color_count = 0;
        for (int i = 0; i < WALL_SIZE; i++) {
            for (int j = 0; j < WALL_SIZE; j++) {
                if (board->wall[i][j] == color) {
                    color_count++;
                }
            }
        }
        if (color_count == 5) {
            board->score += 10; //A completed set of 5 tiles of the same color on the wall  ===)  10 POINTS
        }
    }
}



// Round changer with its settings (everything comes back!)

void reset_factory_displays(GAME_STATE* game) {
    for (int i = 0; i < FACTORY_DISPLAYS; i++) {
        for (int j = 0; j < TILES_PER_FACTORY; j++) {
            game->factoryDisplays[i][j] = -1;
        }
    }
}

void transition_to_next_round(GAME_STATE* game, int bag[], int* size) {
    // apply the minus points for each tile onto floor

    for (int p = 0; p < 2; p++) {
        PLAYER* player = &game->players[p];
        deduct_floor_line_points(&player->board); // Deducting points from the floor line to compute the substracting

        for (int i = 0; i < FLOOR_LINE_MAX; i++) {
            int tile = player->board.floorLine[i];
            if (tile != -1) {
                bag[(*size)++] = tile;
                player->board.floorLine[i] = -1; //To reset the floor
            }
        }

        // printf("%s lost points from the floor line.\n", player->player_name);
    }

    // Reset and refill at the start of the round :

    reset_factory_displays(game);
    fill_factory_displays(game, bag, size);

    // token back to middle !!!!!!!!!                                   (carefull here, the empty floor isnt shown exactly after the round start!!!!1)
    game->middleAreaHasFirstPlayerToken = 1;
    for (int i = 0; i < MIDDLE_SIZE; ++i) {
        if (game->middleArea[i] == -1) {
            game->middleArea[i] = PLAYER_TOKEN;
            break;
        }
    }

    //here will determine the starting player
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < FLOOR_LINE_MAX; j++) {
            if (game->players[i].board.floorLine[j] == PLAYER_TOKEN) {
                game->players[i].board.floorLine[j] = -1;
                game->currentPlayer = i; //Set the player to start the round !!!! (1st player token)
                printf("%s will start the next round.\n", game->players[i].player_name);
                return;
            }
        }
    }
}



//CORE FUNCS:



void play_round(GAME_STATE* game) {
    while (tiles_remaining(game)) {
        display_factory_displays(game);
        display_middle_area(game);

        player_choose_tiles(game, game->currentPlayer);
        game->currentPlayer = 1 - game->currentPlayer; //To switch the player at the start of the round (1st player token)
    }

    int bag[BAG_SIZE];
    initialize_bag(bag); //The tiles was randomized
    transfer_tiles_to_wall_and_bag(game, bag);
    printf("\n-------------------- All tiles have been taken --------------------\n");

    printf("\n%s's board:\n", game->players[0].player_name);
    display_player_board(&game->players[0].board);
    printf("\n%s's board:\n", game->players[1].player_name);
    display_player_board(&game->players[1].board);
}

void play_game(GAME_STATE* game) {
    int bag[BAG_SIZE];
    initialize_bag(bag);
    int size = BAG_SIZE;
    shuffle_bag(bag, size);

    while (1) {
        play_round(game);
        //HERE IS THE END GAME CONDITION (AFTER THAT THE POINTS FOR EACH PLAYER IS SHOWN!!!!!!!!!!!!)
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < WALL_SIZE; j++) {
                int row_complete = 1;
                for (int k = 0; k < WALL_SIZE; k++) {
                    if (game->players[i].board.wall[j][k] == 0) {
                        row_complete = 0;
                        break;
                    }
                }
                if (row_complete) {
                    printf("End of game!\n");
                    for (int p = 0; p < 2; p++) {
                        calculate_final_score(&game->players[p].board);
                        printf("\n%s's final score: %d\n", game->players[p].player_name, game->players[p].board.score);
                    }
                    return;
                }
            }
        }
        transition_to_next_round(game, bag, &size);
    }
}

void initialize_game_state(GAME_STATE* game) {
    initialize_players(&game->players[0], &game->players[1]);
    //Place the 1st player token
    for (int i = 0; i < MIDDLE_SIZE; ++i) {
        game->middleArea[i] = -1; //it should be empty after
    }

    game->middleArea[0] = PLAYER_TOKEN;
    game->middleAreaHasFirstPlayerToken = 1;
    game->currentPlayer = 0; //starting player

    //initialize and randomize the bag, fill bags after
    int bag[BAG_SIZE];
    initialize_bag(bag);
    int size = BAG_SIZE;
    shuffle_bag(bag, size);
    fill_factory_displays(game, bag, &size);
}


//Be carefull 

int main() {
    printf("\x1b[31m ------------------------     AZUL    ----------------------\n");
    printf("         \n");
    printf(" RULES:    HAVE FUN & DON'T FLAME IF U LOSE **** (noob)!!!\n\n");
    srand(time(NULL));

    GAME_STATE game;
    initialize_game_state(&game);

    play_game(&game);

    return 0;
}
