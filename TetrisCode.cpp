
#include <iostream>
#include <vector>
#include <windows.h>
#include <conio.h>
#include <chrono>
#include <thread>
#include <random>
#include <string>

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

// Constants for game dimensions
const int WIDTH = 10;
const int HEIGHT = 20;
const int INFO_WIDTH = 20;

// Console handle for Windows API
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

// Game state variables
int score = 0;
int level = 1;
int linesCleared = 0;
bool gameOver = false;

// Tetromino pieces
vector<vector<vector<int>>> tetrominoes = {
    // I piece
    {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // O piece
    {
        {1, 1},
        {1, 1}
    },
    // T piece
    {
        {0, 1, 0},
        {1, 1, 1},
        {0, 0, 0}
    },
    // S piece
    {
        {0, 1, 1},
        {1, 1, 0},
        {0, 0, 0}
    },
    // Z piece
    {
        {1, 1, 0},
        {0, 1, 1},
        {0, 0, 0}
    },
    // J piece
    {
        {1, 0, 0},
        {1, 1, 1},
        {0, 0, 0}
    },
    // L piece
    {
        {0, 0, 1},
        {1, 1, 1},
        {0, 0, 0}
    }
};

class Tetromino {
public:
    int x, y;                      // Position of the tetromino
    int type;                      // Type of tetromino (0 to 6)
    vector<vector<int>> shape;     // 2D matrix representing the tetromino shape
    
    Tetromino() {                  // Constructor
        random_device rd;
        mt19937 gen(rd());             
        uniform_int_distribution<> dist(0, 6);  

        type = dist(gen);          // Randomly select a tetromino type (0 to 6)
        shape = tetrominoes[type]; // Assign the shape from the predefined list

        x = WIDTH / 2 - shape[0].size() / 2; // Center the tetromino horizontally
        y = 0;                    // Start at the top of the board
    }
    
    void rotate() {
        vector<vector<int>> rotated(shape[0].size(), vector<int>(shape.size()));
        for (size_t i = 0; i < shape.size(); ++i) {
            for (size_t j = 0; j < shape[0].size(); ++j) {
                rotated[j][shape.size() - 1 - i] = shape[i][j];
            }
        }
        shape = rotated;
    }
};

vector<vector<int>> grid(HEIGHT, vector<int>(WIDTH, 0)); // repeats this row HEIGHT times, forming a 2D grid.
Tetromino currentPiece;
Tetromino nextPiece;

// Initialize console settings
void initConsole() {
    CONSOLE_CURSOR_INFO cursorInfo;  // Structure to hold cursor information
    cursorInfo.dwSize = 1;           // Set cursor size (not relevant here)
    cursorInfo.bVisible = FALSE;     // Hide the cursor (FALSE means invisible)
    SetConsoleCursorInfo(hConsole, &cursorInfo); // Apply settings to the console
}

// Function to draw the game board with double buffering
void draw() {
    static COORD cursorPos = {0, 0};
    SetConsoleCursorPosition(hConsole, cursorPos);
    
    string buffer;
    buffer.reserve((WIDTH + INFO_WIDTH + 3) * (HEIGHT + 1));
    
    // Add 5 blank lines at the top
    for (int i = 0; i < 5; i++) {
        buffer += "\n";
    }
    // Draw the game board
    for (int i = 0; i < HEIGHT; ++i) {
        buffer += "|";
        for (int j = 0; j < WIDTH; ++j) {
            if (grid[i][j] != 0) {
                buffer += "#";
            } else if (i >= currentPiece.y && i < currentPiece.y + currentPiece.shape.size() &&
                       j >= currentPiece.x && j < currentPiece.x + currentPiece.shape[0].size() &&
                       currentPiece.shape[i - currentPiece.y][j - currentPiece.x] != 0) {
                buffer += "#";
            } else {
                buffer += " ";
            }
        }
        buffer += "|";
        
        // Draw game info on the right
        if (i == 1) {
            buffer += "   SCORE: " + to_string(score);
        } else if (i == 3) {
            buffer += "   LEVEL: " + to_string(level);
        } else if (i == 5) {
            buffer += "   LINES: " + to_string(linesCleared);
        } else if (i == 8) {
            buffer += "   CONTROLS:";
        } else if (i == 10) {
            buffer += "   < > or A/D: Move";
        } else if (i == 11) {
            buffer += "   ^ or W: Rotate";
        } else if (i == 12) {
            buffer += "   v  or S: Soft Drop";
        } else if (i == 13) {
            buffer += "   Space: Hard Drop";
        } else if (i == 14) {
            buffer += "   ESC: Pause/Quit";
        } else if (i == 17) {
            buffer += "   NEXT PIECE:";
        } else if (i >= 18 && i < 18 + nextPiece.shape.size()) {
            buffer += "   ";
            for (size_t j = 0; j < nextPiece.shape[0].size(); ++j) {
                if (nextPiece.shape[i - 18][j] != 0) {
                    buffer += "#";
                } else {
                    buffer += " ";
                }
            }
        }
        
        buffer += "\n";
    }
    
    // Draw the bottom border
    buffer += "+";
    for (int j = 0; j < WIDTH; ++j) {
        buffer += "-";
    }
    buffer += "+\n";
    
    // Output the entire frame at once
    cout << buffer;
}

// Check for collisions
bool checkCollision() {
    for (size_t i = 0; i < currentPiece.shape.size(); ++i) {
        for (size_t j = 0; j < currentPiece.shape[0].size(); ++j) {
            if (currentPiece.shape[i][j] != 0) {
                int newX = currentPiece.x + j;
                int newY = currentPiece.y + i;
                
                if (newX < 0 || newX >= WIDTH || newY >= HEIGHT) {
                    return true;
                }
                
                if (newY >= 0 && grid[newY][newX] != 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Merge the current piece into the grid
void mergePiece() {
    for (size_t i = 0; i < currentPiece.shape.size(); ++i) {
        for (size_t j = 0; j < currentPiece.shape[0].size(); ++j) {
            if (currentPiece.shape[i][j] != 0) {
                int gridX = currentPiece.x + j;
                int gridY = currentPiece.y + i;
                if (gridY >= 0) {
                    grid[gridY][gridX] = currentPiece.type + 1;
                }
            }
        }
    }
}

// Clear completed lines
void clearLines() {
    int linesToClear = 0;
    for (int i = HEIGHT - 1; i >= 0; --i) {
        bool lineComplete = true;
        for (int j = 0; j < WIDTH; ++j) {
            if (grid[i][j] == 0) {
                lineComplete = false;
                break;
            }
        }
        
        if (lineComplete) {
            grid.erase(grid.begin() + i);
            grid.insert(grid.begin(), vector<int>(WIDTH, 0));
            linesToClear++;
            i++; // Check the same row again as we've moved everything down
        }
    }
    
    // Update score based on lines cleared
    if (linesToClear > 0) {
        linesCleared += linesToClear;
        switch (linesToClear) {
            case 1: score += 100 * level; break;
            case 2: score += 300 * level; break;
            case 3: score += 500 * level; break;
            case 4: score += 800 * level; break;
        }
        
        // Increase level every 10 lines
        level = 1 + linesCleared / 10;
    }
}

// Handle user input
void handleInput() {
    if (_kbhit()) {
        int key = _getch();
        
        // Handle arrow keys (which come as two key presses)
        if (key == 224 || key == 0) {
            key = _getch();
        }
        
        switch (key) {
            case 72:    // Up arrow
            case 'w':   // W key
            case 'W':   // W key (uppercase)
                currentPiece.rotate();
                if (checkCollision()) {
                    // Undo rotation if it causes collision
                    for (int i = 0; i < 3; i++) currentPiece.rotate();
                }
                break;
            case 75:    // Left arrow
            case 'a':   // A key
            case 'A':   // A key (uppercase)
                currentPiece.x--;
                if (checkCollision()) currentPiece.x++;
                break;
            case 77:    // Right arrow
            case 'd':   // D key
            case 'D':   // D key (uppercase)
                currentPiece.x++;
                if (checkCollision()) currentPiece.x--;
                break;
            case 80:    // Down arrow
            case 's':   // S key
            case 'S':   // S key (uppercase)
                currentPiece.y++;
                if (checkCollision()) {
                    currentPiece.y--;
                    mergePiece();
                    clearLines();
                    currentPiece = nextPiece;
                    nextPiece = Tetromino();
                    if (checkCollision()) {
                        gameOver = true;
                    }
                }
                break;
            case 32: // Space - hard drop
                while (!checkCollision()) {
                    currentPiece.y++;
                }
                currentPiece.y--;
                mergePiece();
                clearLines();
                currentPiece = nextPiece;
                nextPiece = Tetromino();
                if (checkCollision()) {
                    gameOver = true;
                }
                break;
            case 27: // ESC - pause or quit
                cout << "Game Paused. Press any key to continue or ESC to quit...";
                key = _getch();
                if (key == 27) {
                    gameOver = true;
                }
                break;
        }
    }
}

int main() {
    // Initialize console settings
    initConsole();
    
    // Initialize game
    currentPiece = Tetromino();
    nextPiece = Tetromino();
    
    // Game loop
    auto lastTime = chrono::steady_clock::now();
    while (!gameOver) {
        // Handle input
        handleInput();
        
        // Game logic
        auto currentTime = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(currentTime - lastTime).count();
        
        // Adjust speed based on level
        int fallSpeed = max(50, 500 - (level - 1) * 50);
        
        if (elapsed > fallSpeed) {
            currentPiece.y++;
            if (checkCollision()) {
                currentPiece.y--;
                mergePiece();
                clearLines();
                currentPiece = nextPiece;
                nextPiece = Tetromino();
                if (checkCollision()) {
                    gameOver = true;
                }
            }
            lastTime = currentTime;
        }
        
        // Draw the game
        draw();
        
        // Platform-independent delay
        #ifdef _WIN32
            Sleep(16);  // Windows version
        #else
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Standard version
        #endif
    }
}
