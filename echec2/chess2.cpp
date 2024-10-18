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

    board->grid[7][7] = new Piece(ROOK, PLAYER2);
    board->grid[7][6] = new Piece(KNIGHT, PLAYER2);
    board->grid[7][5] = new Piece(BISHOP, PLAYER2);
    board->grid[7][4] = new Piece(QUEEN, PLAYER2);
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

    float squareSize = static_cast<float>(TILE_SIZE); // Taille d'une case

    // Dessiner les pièces
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board->grid[y][x] != nullptr) {
                int player = board->grid[y][x]->player;
                int type = board->grid[y][x]->type - 1; // Ajuster l'index pour correspondre au tableau

                // Ajuster la taille et la position de la pièce
                sf::Sprite& pieceSprite = pieceSprites[player][type];
                float scaleFactor = squareSize / std::max(pieceSprite.getTexture()->getSize().x, pieceSprite.getTexture()->getSize().y);
                pieceSprite.setScale(scaleFactor, scaleFactor);

                // Centrer la pièce dans la case
                float pieceWidth = pieceSprite.getGlobalBounds().width;
                float pieceHeight = pieceSprite.getGlobalBounds().height;
                float offsetX = (squareSize - pieceWidth) / 2;
                float offsetY = (squareSize - pieceHeight) / 2;

                pieceSprite.setPosition(x * squareSize + offsetX, y * squareSize + offsetY);
                window.draw(pieceSprite);
            }
        }
    }
}

// Libérer la mémoire allouée aux pièces
void free_board(Board* board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            delete board->grid[i][j]; // Libérer chaque pièce
        }
    }
}

// Fonction pour afficher le menu principal
void displayMenu() {
    std::cout << "1. Nouvelle partie (2 joueurs)" << std::endl;
    std::cout << "2. Nouvelle partie contre l'IA" << std::endl;
    std::cout << "3. Charger une partie" << std::endl;
    std::cout << "4. Quitter" << std::endl;
    std::cout << "Choisissez une option : ";
}

// Fonction pour vérifier si un joueur a encore des pièces
bool hasRemainingPieces(Board* board, Player player) {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board->grid[y][x] != nullptr && board->grid[y][x]->player == player) {
                return true;
            }
        }
    }
    return false;
}

// Fonction simple pour le mouvement de l'IA (à améliorer pour une vraie IA)
void ai_move(Game* game) {
    std::vector<std::pair<int, int>> pieces;
    std::vector<std::pair<int, int>> moves;

    // Trouver toutes les pièces de l'IA
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (game->board.grid[y][x] != nullptr && game->board.grid[y][x]->player == PLAYER2) {
                pieces.push_back({x, y});
            }
        }
    }

    // Trouver tous les mouvements possibles
    for (const auto& piece : pieces) {
        for (int toY = 0; toY < BOARD_SIZE; toY++) {
            for (int toX = 0; toX < BOARD_SIZE; toX++) {
                if (is_valid_move(game->board.grid[piece.second][piece.first]->type, piece.first, piece.second, toX, toY)) {
                    moves.push_back({toX, toY});
                }
            }
        }
    }

    // Choisir un mouvement aléatoire
    if (!moves.empty()) {
        int randomPieceIndex = rand() % pieces.size();
        int randomMoveIndex = rand() % moves.size();
        make_move(game, pieces[randomPieceIndex].first, pieces[randomPieceIndex].second, moves[randomMoveIndex].first, moves[randomMoveIndex].second);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(BOARD_SIZE * TILE_SIZE, BOARD_SIZE * TILE_SIZE), "Jeu d'Echecs");

    // Chargement des textures pour le plateau et les pièces
    sf::Texture boardTexture, pieceTextures[2][4];
    boardTexture.loadFromFile("/Users/andreea/Documents/echec2/images/board.png");
    
    pieceTextures[PLAYER1][QUEEN-1].loadFromFile("/Users/andreea/Documents/echec2/images/player1_queen.png");
    pieceTextures[PLAYER1][KNIGHT-1].loadFromFile("/Users/andreea/Documents/echec2/images/player1_knight.png");
    pieceTextures[PLAYER1][ROOK-1].loadFromFile("/Users/andreea/Documents/echec2/images/player1_rook.png");
    pieceTextures[PLAYER1][BISHOP-1].loadFromFile("/Users/andreea/Documents/echec2/images/player1_bishop.png");
    
    pieceTextures[PLAYER2][QUEEN-1].loadFromFile("/Users/andreea/Documents/echec2/images/player2_queen.png");
    pieceTextures[PLAYER2][KNIGHT-1].loadFromFile("/Users/andreea/Documents/echec2/images/player2_knight.png");
    pieceTextures[PLAYER2][ROOK-1].loadFromFile("/Users/andreea/Documents/echec2/images/player2_rook.png");
    pieceTextures[PLAYER2][BISHOP-1].loadFromFile("/Users/andreea/Documents/echec2/images/player2_bishop.png");

    sf::Sprite boardSprite(boardTexture);
    boardSprite.setScale(
        (float)(BOARD_SIZE * TILE_SIZE) / boardTexture.getSize().x,
        (float)(BOARD_SIZE * TILE_SIZE) / boardTexture.getSize().y
    );

    sf::Sprite pieceSprites[2][4];
    for (int player = 0; player < 2; player++) {
        for (int type = 0; type < 4; type++) {
            pieceSprites[player][type] = sf::Sprite(pieceTextures[player][type]);
        }
    }

    Game game;
    bool gameStarted = false;
    bool playingAgainstAI = false;

    while (!gameStarted) {
        displayMenu();
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1: // Nouvelle partie (2 joueurs)
                initialize_board(&game.board);
                game.currentPlayer = PLAYER1;
                gameStarted = true;
                break;
            case 2: // Nouvelle partie contre l'IA
                initialize_board(&game.board);
                game.currentPlayer = PLAYER1;
                gameStarted = true;
                playingAgainstAI = true;
                break;
            case 3: { // Charger une partie
                std::string filename;
                std::cout << "Entrez le nom du fichier de sauvegarde : ";
                std::cin >> filename;
                load_game(&game, filename.c_str());
                gameStarted = true;
                break;
            }
            case 4: // Quitter
                return 0;
            default:
                std::cout << "Option invalide. Veuillez réessayer." << std::endl;
        }
    }

    // Variables pour gérer la sélection et le déplacement des pièces
    bool isPieceSelected = false;
    int selectedX = -1, selectedY = -1;

    // Boucle principale du jeu
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int x = event.mouseButton.x / TILE_SIZE;
                    int y = event.mouseButton.y / TILE_SIZE;

                    if (!isPieceSelected) {
                        // Sélectionner une pièce
                        if (game.board.grid[y][x] != nullptr && game.board.grid[y][x]->player == game.currentPlayer) {
                            selectedX = x;
                            selectedY = y;
                            isPieceSelected = true;
                        }
                    } else {
                        // Déplacer la pièce sélectionnée
                        make_move(&game, selectedX, selectedY, x, y);
                        isPieceSelected = false;
                    }
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::S) {
                    // Sauvegarder la partie
                    std::string filename;
                    std::cout << "Entrez le nom du fichier pour la sauvegarde : ";
                    std::cin >> filename;
                    save_game(&game, filename.c_str());
                    std::cout << "Partie sauvegardée." << std::endl;
                }
            }
        }

        // Vérifier s'il y a un gagnant
        if (!hasRemainingPieces(&game.board, PLAYER1)) {
            std::cout << "Le joueur 2 a gagné !" << std::endl;
            window.close();
        } else if (!hasRemainingPieces(&game.board, PLAYER2)) {
            std::cout << "Le joueur 1 a gagné !" << std::endl;
            window.close();
        }

        // Mouvement de l'IA si c'est son tour
        if (playingAgainstAI && game.currentPlayer == PLAYER2) {
            ai_move(&game);
        }

        window.clear();
        draw_board(window, &game.board, boardSprite, pieceSprites);

        // Dessiner un indicateur pour la pièce sélectionnée
        if (isPieceSelected) {
            sf::RectangleShape selector(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            selector.setFillColor(sf::Color(255, 255, 0, 128)); // Jaune semi-transparent
            selector.setPosition(selectedX * TILE_SIZE, selectedY * TILE_SIZE);
            window.draw(selector);
        }

        window.display();
    }

    // Libérer la mémoire
    free_board(&game.board);

    return 0;
}
