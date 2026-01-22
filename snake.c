#include <stdio.h>
#include <conio.h>   // Required for _kbhit() and _getch()
#include <windows.h> // Required for Sleep()
#include <stdlib.h>  // Required for rand(), srand()
#include <time.h>    // Required for time()
#include <string.h>

// --- GAME CONFIGURATION ---
#define WIDTH 20      // Fixed width [cite: 38]
#define HEIGHT 10     // Fixed height [cite: 38]
#define CHAR_WALL '#' // [cite: 39]
#define CHAR_SNAKE 'O'// [cite: 49]
#define CHAR_FOOD '*' // [cite: 60]
#define CHAR_EMPTY ' '// [cite: 40]

// --- GLOBAL VARIABLES ---
int x, y;                // Snake Head coordinates
int foodX, foodY;        // Food coordinates
int score;               // Current score
int gameOver;            // Game state flag
int tailX[100], tailY[100]; // Arrays to store tail coordinates
int nTail;               // Current length of tail
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN }; // Directions
enum eDirection dir;

// --- LEADERBOARD STRUCTURE ---
struct Player {
    char name[50];
    int score;
};

// --- FUNCTION PROTOTYPES ---
void Setup();
void Draw();
void Input();
void Logic();
void HandleLeaderboard();

int main() {
    Setup();
    
    // --- START SCREEN ---
    Draw(); // Draw board once so user sees it
    printf("\n --- SNAKE GAME --- \n");
    printf(" Controls: W, A, S, D  OR  Arrow Keys\n");
    printf(" Press ANY KEY to start running...");
    
    // This waits for a click before the loop starts
    if (_getch() == 224) _getch(); // Clear buffer if arrow key pressed
    // --------------------

    // Game Loop
    while (!gameOver) {
        Draw();
        Input();
        Logic();
        Sleep(100); // [cite: 64]
    }

    // End Game
    printf("\nGAME OVER!\n");
    printf("Final Score: %d\n", score); // [cite: 75]
    HandleLeaderboard();

    printf("\nPress any key to exit...");
    _getch();
    return 0;
}

// --- 1. SETUP ---
void Setup() {
    srand(time(0)); 
    gameOver = 0;
    dir = RIGHT;    // [cite: 51]
    
    // Start in the middle
    x = WIDTH / 2;
    y = HEIGHT / 2;

    // Initial Length = 3 [cite: 50]
    nTail = 2; 
    tailX[0] = x - 1; tailY[0] = y;
    tailX[1] = x - 2; tailY[1] = y;

    score = 0;
    
    // Place first food
    foodX = rand() % (WIDTH - 2) + 1;
    foodY = rand() % (HEIGHT - 2) + 1;
}

// --- 2. DRAW ---
void Draw() {
    system("cls"); 

    printf("SCORE: %d\n", score); // [cite: 74]

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            
            // Walls
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                printf("%c", CHAR_WALL);
            }
            // Snake Head
            else if (i == y && j == x) {
                printf("%c", CHAR_SNAKE);
            }
            // Food
            else if (i == foodY && j == foodX) {
                printf("%c", CHAR_FOOD);
            }
            // Snake Body
            else {
                int print = 0;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        printf("%c", CHAR_SNAKE); 
                        print = 1;
                    }
                }
                if (!print) {
                    printf("%c", CHAR_EMPTY); 
                }
            }
        }
        printf("\n");
    }
}

// --- 3. INPUT (Updated for WASD + ARROW KEYS) ---
void Input() {
    if (_kbhit()) { 
        int key = _getch();
        
        // Check for Arrow Keys (they return 224 first)
        if (key == 224) {
            key = _getch(); // Get the actual arrow code
            switch (key) {
                case 75: if(dir != RIGHT) dir = LEFT; break;  // Left Arrow
                case 77: if(dir != LEFT) dir = RIGHT; break;  // Right Arrow
                case 72: if(dir != DOWN) dir = UP; break;     // Up Arrow
                case 80: if(dir != UP) dir = DOWN; break;     // Down Arrow
            }
        } 
        // Standard WASD Keys
        else {
            switch (key) {
                case 'a': case 'A': if(dir != RIGHT) dir = LEFT; break; // [cite: 57]
                case 'd': case 'D': if(dir != LEFT) dir = RIGHT; break; // [cite: 58]
                case 'w': case 'W': if(dir != DOWN) dir = UP; break;    // [cite: 54]
                case 's': case 'S': if(dir != UP) dir = DOWN; break;    // [cite: 56]
                case 'x': gameOver = 1; break; 
            }
        }
    }
}

// --- 4. LOGIC ---
void Logic() {
    // Update Tail positions
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;

    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    // Move Head
    switch (dir) {
        case LEFT:  x--; break;
        case RIGHT: x++; break;
        case UP:    y--; break;
        case DOWN:  y++; break;
        default: break;
    }

    // Wall Collision [cite: 71]
    if (x <= 0 || x >= WIDTH - 1 || y <= 0 || y >= HEIGHT - 1) {
        gameOver = 1;
    }

    // Self Collision [cite: 73]
    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == x && tailY[i] == y) {
            gameOver = 1;
        }
    }

    // Eating Food
    if (x == foodX && y == foodY) {
        score += 10; // [cite: 68]
        nTail++;     // [cite: 65]
        
        // Respawn food
        int valid = 0;
        while (!valid) {
            valid = 1;
            foodX = rand() % (WIDTH - 2) + 1;
            foodY = rand() % (HEIGHT - 2) + 1;
            
            if(foodX == x && foodY == y) valid = 0;
            for(int i=0; i<nTail; i++) {
                if(foodX == tailX[i] && foodY == tailY[i]) valid = 0;
            }
        }
    }
}

// --- 5. LEADERBOARD ---
void HandleLeaderboard() {
    FILE *fp;
    struct Player players[4]; 
    int count = 0;
    
    struct Player currentPlayer;
    printf("Enter your name: ");
    scanf("%s", currentPlayer.name);
    currentPlayer.score = score;

    fp = fopen("leaderboard.txt", "r");
    if (fp != NULL) {
        while (count < 3 && fscanf(fp, "%s %d", players[count].name, &players[count].score) == 2) {
            count++;
        }
        fclose(fp);
    }

    players[count] = currentPlayer;
    count++;

    // Bubble Sort
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (players[j].score < players[j + 1].score) {
                struct Player temp = players[j];
                players[j] = players[j + 1];
                players[j + 1] = temp;
            }
        }
    }

    fp = fopen("leaderboard.txt", "w");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }
    
    printf("\n--- TOP 3 PLAYERS ---\n"); // [cite: 76]
    for (int i = 0; i < count && i < 3; i++) {
        fprintf(fp, "%s %d\n", players[i].name, players[i].score);
        printf("%d. %s - %d\n", i + 1, players[i].name, players[i].score);
    }
    fclose(fp);
}