#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SIZE 8

// Enums for piece types and players
typedef enum { EMPTY, QUEEN, KNIGHT, ROOK, BISHOP } PieceType;
typedef enum { PLAYER1, PLAYER2 } PlayerType;

// Struct to represent a piece
typedef struct {
    PieceType type;
    PlayerType player;
} Piece;

// Struct to represent the board
typedef struct {
    Piece* grid[BOARD_SIZE][BOARD_SIZE];
} Board;

// Struct to represent the game state
typedef struct {
    Board board;
    PlayerType currentPlayer;
} Game;

// Function prototypes
void initialize_board(Board* board);
void print_board(Board* board);
int is_valid_move(PieceType type, int fromX, int fromY, int toX, int toY);
void save_game(Game* game, const char* filename);
void load_game(Game* game, const char* filename);
void make_move(Game* game, int fromX, int fromY, int toX, int toY);

int main() {
    Game game;
    initialize_board(&game.board);
    game.currentPlayer = PLAYER1;  // Player 1 starts the game

    int choice;
    printf("1: New Game\n2: Load Game\nChoose an option: ");
    scanf("%d", &choice);

    if (choice == 2) {
        char filename[100];
        printf("Enter the filename to load: ");
        scanf("%s", filename);
        load_game(&game, filename);
    }

    char save_filename[100] = "";  // Array to hold the save filename
    int save_filename_set = 0; // Flag to check if the filename is set

    // Game loop
    // Game loop
while (1) {
    print_board(&game.board);
    printf("Player %d's turn. Enter your move (fromX fromY toX toY):\n", game.currentPlayer + 1);
    int fromX, fromY, toX, toY;
    scanf("%d %d %d %d", &fromX, &fromY, &toX, &toY);

    make_move(&game, fromX, fromY, toX, toY);

    // Ask to save the game after a successful move
    printf("Do you want to save the game? (1: Yes, 0: No): ");
    int save;
    scanf("%d", &save);
    
    if (save) {
        if (!save_filename_set) {
            printf("Enter the filename to save: ");
            scanf("%s", save_filename);
            save_filename_set = 1; // Set the filename flag
        }
        save_game(&game, save_filename);
        printf("Game saved!\n");
    }

    // Switch player
    game.currentPlayer = (game.currentPlayer == PLAYER1) ? PLAYER2 : PLAYER1;
}

    return 0;
}



// Initialize the board with pieces in the starting positions
void initialize_board(Board* board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board->grid[i][j] = NULL;
        }
    }

    // Player 1 pieces
    board->grid[0][0] = malloc(sizeof(Piece)); board->grid[0][0]->type = ROOK; board->grid[0][0]->player = PLAYER1;
    board->grid[0][1] = malloc(sizeof(Piece)); board->grid[0][1]->type = KNIGHT; board->grid[0][1]->player = PLAYER1;
    board->grid[0][2] = malloc(sizeof(Piece)); board->grid[0][2]->type = BISHOP; board->grid[0][2]->player = PLAYER1;
    board->grid[0][3] = malloc(sizeof(Piece)); board->grid[0][3]->type = QUEEN; board->grid[0][3]->player = PLAYER1;

    // Player 2 pieces
    board->grid[7][0] = malloc(sizeof(Piece)); board->grid[7][0]->type = ROOK; board->grid[7][0]->player = PLAYER2;
    board->grid[7][1] = malloc(sizeof(Piece)); board->grid[7][1]->type = KNIGHT; board->grid[7][1]->player = PLAYER2;
    board->grid[7][2] = malloc(sizeof(Piece)); board->grid[7][2]->type = BISHOP; board->grid[7][2]->player = PLAYER2;
    board->grid[7][3] = malloc(sizeof(Piece)); board->grid[7][3]->type = QUEEN; board->grid[7][3]->player = PLAYER2;
}

// Print the current board state
void print_board(Board* board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board->grid[i][j] == NULL) {
                printf(". ");
            } else {
                switch (board->grid[i][j]->type) {
                    case QUEEN: printf("Q "); break;
                    case KNIGHT: printf("N "); break;
                    case ROOK: printf("R "); break;
                    case BISHOP: printf("B "); break;
                    default: printf(". ");
                }
            }
        }
        printf("\n");
    }
}

// Basic function to check if a move is valid (implement piece-specific movement rules)
int is_valid_move(PieceType type, int fromX, int fromY, int toX, int toY) {
    if (type == ROOK) {
        // Rook can move horizontally or vertically any number of squares
        return (fromX == toX || fromY == toY);
    }
    // Implement similar checks for other pieces (e.g., QUEEN, KNIGHT, etc.)
    
    return 1;  // Placeholder for now
}


// Move a piece on the board
void make_move(Game* game, int fromX, int fromY, int toX, int toY) {
    // Check if the from-square contains a piece
    if (game->board.grid[fromX][fromY] == NULL) {
        printf("No piece at the selected position!\n");
        return;
    }

    // Check if the destination square contains a piece of the same player
    if (game->board.grid[toX][toY] != NULL && 
        game->board.grid[toX][toY]->player == game->board.grid[fromX][fromY]->player) {
        printf("Invalid move! You cannot move onto your own piece.\n");
        return;
    }

    // Print debug information
    printf("Attempting to move from (%d, %d) to (%d, %d)\n", fromX, fromY, toX, toY);

    // Validate the move (e.g., based on piece type)
    if (is_valid_move(game->board.grid[fromX][fromY]->type, fromX, fromY, toX, toY)) {
        // Move the piece
        game->board.grid[toX][toY] = game->board.grid[fromX][fromY]; // Move the piece to the new location
        game->board.grid[fromX][fromY] = NULL; // Set the old location to NULL

        // Print the updated board immediately after the move
        printf("Move successful!\n");
        print_board(&game->board);
    } else {
        printf("Invalid move based on piece rules!\n");
    }
}




// Save the game to a file
void save_game(Game* game, const char* filename) {
    FILE* file = fopen(filename, "wb");  // Use "wb" for binary write
    if (!file) {
        printf("Error saving the game!\n");
        return;
    }

    fwrite(&game->currentPlayer, sizeof(PlayerType), 1, file);

    // Save the board
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (game->board.grid[i][j] == NULL) {
                Piece emptyPiece = { EMPTY, PLAYER1 };  // Placeholder for empty space
                fwrite(&emptyPiece, sizeof(Piece), 1, file);
            } else {
                fwrite(game->board.grid[i][j], sizeof(Piece), 1, file);
            }
        }
    }

    fclose(file);
}

// Load the game from a file
void load_game(Game* game, const char* filename) {
    FILE* file = fopen(filename, "rb");  // Use "rb" for binary read
    if (!file) {
        printf("Error loading the game!\n");
        return;
    }

    fread(&game->currentPlayer, sizeof(PlayerType), 1, file);

    // Load the board
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            Piece* piece = (Piece*)malloc(sizeof(Piece));
            fread(piece, sizeof(Piece), 1, file);

            if (piece->type == EMPTY) {
                free(piece);
                game->board.grid[i][j] = NULL;
            } else {
                game->board.grid[i][j] = piece;
            }
        }
    }

    fclose(file);
}
