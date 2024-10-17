#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <fstream>  // Pour ifstream et ofstream
#include <cstdlib>  // Pour abs()

#define BOARD_SIZE 8
#define TILE_SIZE 100 // Taille des cases du plateau

// Enumération pour les types de pièces
typedef enum { EMPTY, QUEEN, KNIGHT, ROOK, BISHOP } PieceType;

// Enumération pour les joueurs
typedef enum { PLAYER1, PLAYER2 } Player;

// Structure pour représenter une pièce
struct Piece {
    PieceType type;
    Player player;
    Piece(PieceType t, Player p) : type(t), player(p) {}
};

// Structure pour représenter le plateau
struct Board {
    Piece* grid[BOARD_SIZE][BOARD_SIZE];
};

// Structure pour représenter une partie
struct Game {
    Board board;
    Player currentPlayer;
    int selectedX; // Position X de la pièce sélectionnée
    int selectedY; // Position Y de la pièce sélectionnée
    bool pieceSelected; // Indique si une pièce est sélectionnée
};

// Fonction pour initialiser le plateau
void initialize_board(Board* board) {
    // Initialiser toutes les cases à nullptr (cases vides)
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board->grid[i][j] = nullptr; // Toutes les cases sont vides au départ
        }
    }

    // Placement des pièces
    board->grid[0][0] = new Piece(ROOK, PLAYER1);
    board->grid[0][1] = new Piece(KNIGHT, PLAYER1);
    board->grid[0][2] = new Piece(BISHOP, PLAYER1);
    board->grid[0][3] = new Piece(QUEEN, PLAYER1);

    board->grid[1][0] = new Piece(ROOK, PLAYER2);
    board->grid[1][1] = new Piece(KNIGHT, PLAYER2);
    board->grid[1][2] = new Piece(BISHOP, PLAYER2);
    board->grid[1][3] = new Piece(QUEEN, PLAYER2);
}

// Fonction pour afficher le plateau dans la console
void print_board(Board* board) {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board->grid[y][x] != nullptr) {
                std::cout << (board->grid[y][x]->type + 1) << " "; // Afficher le type de pièce
            } else {
                std::cout << "0 "; // Afficher 0 pour une case vide
            }
        }
        std::cout << std::endl; // Nouvelle ligne pour chaque rangée
    }
}

// Vérifier si un mouvement est valide
int is_valid_move(PieceType type, int fromX, int fromY, int toX, int toY) {
    // Validation basique des mouvements pour chaque type de pièce
    switch (type) {
        case QUEEN:
            return (fromX == toX || fromY == toY || abs(fromX - toX) == abs(fromY - toY)); // Déplacement en ligne ou en diagonale
        case KNIGHT:
            return (abs(fromX - toX) == 2 && abs(fromY - toY) == 1) || (abs(fromX - toX) == 1 && abs(fromY - toY) == 2); // Mouvement en "L"
        case ROOK:
            return (fromX == toX || fromY == toY); // Déplacement en ligne droite
        case BISHOP:
            return (abs(fromX - toX) == abs(fromY - toY)); // Déplacement en diagonale
        default:
            return 0; // Mouvement invalide
    }
}

// Enregistrer la partie dans un fichier
void save_game(Game* game, const char* filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                if (game->board.grid[y][x] != nullptr) {
                    file << game->board.grid[y][x]->type << " " << game->board.grid[y][x]->player << " ";
                } else {
                    file << "-1 "; // -1 pour une case vide
                }
            }
            file << std::endl;
        }
        file << game->currentPlayer << std::endl;
        file.close();
    } else {
        std::cerr << "Erreur d'ouverture du fichier pour la sauvegarde." << std::endl;
    }
}

// Charger une partie à partir d'un fichier
void load_game(Game* game, const char* filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                int type, player;
                file >> type >> player;
                if (type != -1) {
                    game->board.grid[y][x] = new Piece(static_cast<PieceType>(type), static_cast<Player>(player));
                } else {
                    game->board.grid[y][x] = nullptr; // Case vide
                }
            }
        }
        int currentPlayer;
        file >> currentPlayer;
        game->currentPlayer = static_cast<Player>(currentPlayer);
        file.close();
    } else {
        std::cerr << "Erreur d'ouverture du fichier pour le chargement." << std::endl;
    }
}

// Effectuer un mouvement
void make_move(Game* game, int fromX, int fromY, int toX, int toY) {
    Piece* piece = game->board.grid[fromY][fromX];
    if (piece != nullptr && piece->player == game->currentPlayer) {
        if (is_valid_move(piece->type, fromX, fromY, toX, toY)) {
            delete game->board.grid[toY][toX]; // Supprimer la pièce de destination s'il y en a une
            game->board.grid[toY][toX] = piece; // Déplacer la pièce
            game->board.grid[fromY][fromX] = nullptr; // Vider la case de départ
            game->currentPlayer = (game->currentPlayer == PLAYER1) ? PLAYER2 : PLAYER1; // Changer de joueur
        } else {
            std::cout << "Mouvement invalide!" << std::endl;
        }
    } else {
        std::cout << "Aucune pièce à déplacer ou mauvaise pièce!" << std::endl;
    }
}

// Fonction pour dessiner le plateau et les pièces
void draw_board(sf::RenderWindow& window, Board* board, sf::Sprite boardSprite, sf::Sprite pieceSprites[2][4]) {
    window.draw(boardSprite); // Dessiner le plateau

    // Dessiner les pièces
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board->grid[y][x] != nullptr) {
                int player = board->grid[y][x]->player;
                int type = board->grid[y][x]->type;

                // Dessiner la pièce
                sf::Sprite pieceSprite = pieceSprites[player][type];
                pieceSprite.setPosition(x * TILE_SIZE, y * TILE_SIZE); // Positionner la pièce
                window.draw(pieceSprite); // Dessiner la pièce
            }
        }
    }
}

// Libérer la mémoire allouée aux pièces
void free_board(Board* board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            delete board->grid[i][j]; // Supprimer chaque pièce
            board->grid[i][j] = nullptr; // Réinitialiser la case
        }
    }
}

// Fonction pour dessiner le plateau avec les mouvements possibles
void draw_board_with_moves(sf::RenderWindow& window, Board* board, sf::Sprite boardSprite, sf::Sprite pieceSprites[2][4], std::vector<std::pair<int, int> > possibleMoves) {
    // Dessiner le plateau
    window.draw(boardSprite);

    // Mettre en surbrillance les cases possibles
    for (const std::pair<int, int>& move : possibleMoves) {
        sf::RectangleShape highlight(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        highlight.setFillColor(sf::Color(0, 255, 0, 100)); // Vert transparent
        highlight.setPosition(move.first * TILE_SIZE, move.second * TILE_SIZE); // Positionner
        window.draw(highlight); // Dessiner la case mise en évidence
    }

    // Dessiner les pièces
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board->grid[y][x] != nullptr) {
                sf::Sprite pieceSprite = pieceSprites[board->grid[y][x]->player][board->grid[y][x]->type]; // Récupérer le sprite de la pièce
                pieceSprite.setPosition(x * TILE_SIZE, y * TILE_SIZE); // Définir la position de la pièce
                window.draw(pieceSprite); // Dessiner la pièce
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Jeu d'Echecs");

    // Charger les textures pour le plateau et les pièces
    sf::Texture boardTexture;
    sf::Texture pieceTextures[2][4];

    // Chargement des textures
    boardTexture.loadFromFile("/Users/andreea/Documents/echec2/images/board.png");

    // Créer un sprite à partir de la texture du plateau
    sf::Sprite boardSprite(boardTexture);
    boardSprite.setScale(0.39f, 0.39f); // Ajuster la taille du plateau à 800x800 pixels

    // Charger les images des pièces (assurez-vous qu'elles existent)
    pieceTextures[PLAYER1][QUEEN].loadFromFile("/Users/andreea/Documents/echec2/images/player1_queen.png");
    pieceTextures[PLAYER1][KNIGHT].loadFromFile("/Users/andreea/Documents/echec2/images/player1_knight.png");
    pieceTextures[PLAYER1][ROOK].loadFromFile("/Users/andreea/Documents/echec2/images/player1_rook.png");
    pieceTextures[PLAYER1][BISHOP].loadFromFile("/Users/andreea/Documents/echec2/images/player1_bishop.png");

    pieceTextures[PLAYER2][QUEEN].loadFromFile("/Users/andreea/Documents/echec2/images/player2_queen.png");
    pieceTextures[PLAYER2][KNIGHT].loadFromFile("/Users/andreea/Documents/echec2/images/player2_knight.png");
    pieceTextures[PLAYER2][ROOK].loadFromFile("/Users/andreea/Documents/echec2/images/player2_rook.png");
    pieceTextures[PLAYER2][BISHOP].loadFromFile("/Users/andreea/Documents/echec2/images/player2_bishop.png");

    // Créer des sprites pour les pièces et les ajuster
    sf::Sprite pieceSprites[2][4];
    for (int player = 0; player < 2; player++) {
        for (int type = 0; type < 4; type++) {
            pieceSprites[player][type] = sf::Sprite(pieceTextures[player][type]);
            pieceSprites[player][type].setScale(0.25f, 0.25f); // Réduire la taille à 100x100 si les images originales sont 400x400
        }
    }

    // Initialiser le jeu
    Game game;
    initialize_board(&game.board);
    game.currentPlayer = PLAYER1;
    game.pieceSelected = false;

    // Vecteur pour stocker les mouvements possibles
    std::vector<std::pair<int, int> > possibleMoves; // Notez l'espace entre 'int' et '>'

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int x = event.mouseButton.x / TILE_SIZE; // Calculer la case X
                    int y = event.mouseButton.y / TILE_SIZE; // Calculer la case Y

                    if (game.pieceSelected) { // Si une pièce est déjà sélectionnée
                        // Essayer de faire le mouvement
                        make_move(&game, game.selectedX, game.selectedY, x, y);
                        game.pieceSelected = false; // Désélectionner la pièce
                    } else { // Sélectionner une pièce
                        Piece* piece = game.board.grid[y][x];
                        if (piece != nullptr && piece->player == game.currentPlayer) {
                            game.selectedX = x; // Stocker la position sélectionnée
                            game.selectedY = y;
                            game.pieceSelected = true; // Marquer comme pièce sélectionnée

                            // Ici, vous pouvez ajouter la logique pour déterminer les mouvements possibles
                            possibleMoves.clear(); // Effacer les mouvements possibles précédents
                            // Ajoutez les mouvements possibles pour la pièce sélectionnée (exemple fictif)
                            possibleMoves.push_back(std::make_pair(x + 1, y)); // Ajouter un mouvement possible (exemple)
                        }
                    }
                }
            }
        }

        // Effacer la fenêtre
        window.clear();

        // Dessiner le plateau avec les mouvements possibles
        draw_board_with_moves(window, &game.board, boardSprite, pieceSprites, possibleMoves); // Passer le vecteur des mouvements possibles

        // Afficher le contenu de la fenêtre
        window.display();
    }

    // Libérer la mémoire
    free_board(&game.board);

    return 0;
}
