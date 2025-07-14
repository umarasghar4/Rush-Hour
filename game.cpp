//============================================================================
// Name        : .cpp
// Author      : Dr. Sibt Ul Hussain
// Version     :
// Copyright   : (c) Reserved
// Description : Basic 2D game...
//============================================================================

#ifndef RushHour_CPP_
#define RushHour_CPP_
#include "util.h"
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <cstring>
using namespace std;

const int grid_size = 15;
const int cell_size = 40;

const char layout[grid_size][grid_size+1] = {
    "BBBBBBBBBBBBBBB",
    "BRRRRRRRRRRRRRB",
    "BRRRRRRRBBBRRBB",
    "BRRRRRRRRRRRRRB",
    "BRRRRRRRRRBBBRB",
    "BBBRRRRRRRRRBRB",
    "BRBRRRBRRRRRBRB",
    "BRBRRRBRRRRRRRB",
    "BRRRRRBBRRRRRRB",
    "BRRRRRRRRBRRRRB",
    "BRRRRRRRRRRRRRB",
    "BRRRRRRRRRRRRRB",
    "BRRRRRRRRRRRRRB",
    "BRRRRRRRRRRRRRB",
    "BBBBBBBBBBBBBBB"
};

struct HighscoreEntry {
    int score;
    int rank;
    string name;  
};

//]variables of the highscore
char playerName[20] = "Player";
//Class Object ---
class Object {
public:
    int x, y;
    bool visible;
    Object(int x, int y) : x(x), y(y), visible(true) {}
    virtual void draw(int Xset, int Yset) = 0;
    virtual char getType() const = 0;
    virtual ~Object() {}
};

// --- Static Objects ---
class Tree : public Object {
public:
    Tree(int x, int y) : Object(x, y) {}
    void draw(int Xset, int Yset) override {
        int px = Xset + x * cell_size;
        int py = Yset + y * cell_size;
        DrawSquare(px, py, cell_size, colors[WHITE]);
        DrawSquare(px + cell_size/2 - 3, py + 5, 6, colors[BROWN]);
        DrawCircle(px + cell_size/2, py + cell_size/2 + 7, 10, colors[GREEN]);
    }
    char getType() const override { return 'T'; }
};

class FuelStation : public Object {
public:
    FuelStation(int x, int y) : Object(x, y) {}
    void draw(int Xset, int Yset) override {
        int px = Xset + x * cell_size;
        int py = Yset + y * cell_size;
        DrawSquare(px, py, cell_size, colors[WHITE]);
        DrawTriangle(px + 5, py + 5,  px + cell_size - 5, py + 5, px + cell_size/2, py + cell_size - 5,  colors[RED]);
    }
    char getType() const override { return 'F'; }
};

class Garage : public Object {
public:
    Garage(int x, int y) : Object(x, y) {}
    void draw(int Xset, int Yset) override {
        int px = Xset + x * cell_size;
        int py = Yset + y * cell_size;
        DrawSquare(px, py, cell_size, colors[WHITE]);
        DrawTriangle(px + 5, py + 5, px + cell_size - 5, py + 5, px + cell_size/2, py + cell_size - 5, colors[GRAY]);
    }
    char getType() const override { return 'G'; }
};

class DropOff : public Object {
public:
    DropOff(int x, int y) : Object(x, y) {}
    void draw(int Xset, int Yset) override {
        int px = Xset + x * cell_size;
        int py = Yset + y * cell_size;
        DrawSquare(px, py, cell_size, colors[WHITE]);
        DrawSquare(px + 10, py + 10, cell_size - 20, colors[BLUE]);
    }
    char getType() const override { return 'D'; }
};

// --- Carryable Entities ---
class Passenger : public Object {
public:
    Passenger(int x, int y) : Object(x, y) {}
    void draw(int Xset, int Yset) override {
        int px = Xset + x * cell_size;
        int py = Yset + y * cell_size;
        DrawSquare(px, py, cell_size, colors[WHITE]);
        DrawCircle(px + cell_size/2, py + cell_size/2 + 7, 5, colors[BLACK]);
        DrawLine(px + cell_size/2, py + cell_size/2 + 2, px + cell_size/2, py + cell_size/2 - 8, 2, colors[BLACK]);
        DrawLine(px + cell_size/2, py + cell_size/2, px + cell_size/2 - 6, py + cell_size/2 - 2, 2, colors[BLACK]);
        DrawLine(px + cell_size/2, py + cell_size/2, px + cell_size/2 + 6, py + cell_size/2 - 2, 2, colors[BLACK]);
        DrawLine(px + cell_size/2, py + cell_size/2 - 8, px + cell_size/2 - 4, py + cell_size/2 - 16, 2, colors[BLACK]);
        DrawLine(px + cell_size/2, py + cell_size/2 - 8, px + cell_size/2 + 4, py + cell_size/2 - 16, 2, colors[BLACK]);
    }
    char getType() const override { return 'P'; }
};

class Package : public Object {
public:
    Package(int x, int y) : Object(x, y) {}
    void draw(int Xset, int Yset) override {
        int px = Xset + x * cell_size;
        int py = Yset + y * cell_size;
        DrawSquare(px, py, cell_size, colors[WHITE]);
        DrawSquare(px + 10, py + 10, cell_size - 20, colors[BROWN]);
    }
    char getType() const override { return 'K'; }
};

class Board;

// Simplify NPCCar class
class NPCCar : public Object {
public:
    NPCCar(int x, int y) : Object(x, y) {}
    
    void draw(int Xset, int Yset) override {
        int px = Xset + x * cell_size;
        int py = Yset + y * cell_size;
        DrawRoundRect(px+5, py+10, cell_size-10, cell_size-20, colors[RED], 10);
        DrawCircle(px+12, py+10, 4, colors[BLACK]);
        DrawCircle(px+cell_size-12, py+10, 4, colors[BLACK]);
    }
    
    char getType() const override { return 'N'; }
};

// Update Player class
class Player : public Object {
public:
    int score, fuel, money, role; // role main, 0=taxi, 1=delivery
    bool carrying;
    int dropX, dropY;
    Player(int x, int y, int role) : Object(x, y), score(0), fuel(100), money(0), role(role), carrying(false), dropX(-1), dropY(-1) {}
    void draw(int Xset, int Yset) override {
        int px = Xset + x * cell_size;
        int py = Yset + y * cell_size;
        // Different colors for different roles
        float* carColor = (role == 0) ? colors[YELLOW] : colors[BLUE];
        DrawRoundRect(px+5, py+10, cell_size-10, cell_size-20, carColor, 10);
        DrawCircle(px+12, py+10, 4, colors[BLACK]);
        DrawCircle(px+cell_size-12, py+10, 4, colors[BLACK]);
    }
    char getType() const override { return 'C'; }
    void refuel(int cost) {
        if (money >= cost) {
            money -= cost;
            fuel = 100;
        }
    }
};

class Board{
public:
    Object* grid[grid_size][grid_size];
    Player* player;
    
    Board() { 
        for(int i=0;i<grid_size;i++) for(int j=0;j<grid_size;j++) grid[i][j]=nullptr; 
        player=nullptr; 
    }
    
    ~Board() { 
        for(int i=0;i<grid_size;i++) {
        for(int j=0;j<grid_size;j++) {
        delete grid[i][j]; 
        delete player;
        }}
    }
    
    void switch_role();
    void setup(int role);
    void draw(int Xset, int Yset);
    bool isRoad(int x, int y);
    void movePlayer(int dx, int dy);
    void spawnDropOff();
    void tryPickup();
    void tryDropoff();
    void getRandomRoadPosition(int &rx, int &ry);
};

// --- Game (Aggregates Board, manages state) ---
class Game {
public:
    Board* board;
    int Xset, Yset;
    int timeLeft;
    bool gameOver;
    Game() : board(nullptr), Xset(0), Yset(0), timeLeft(180), gameOver(false) {}
    ~Game() { 
            delete board;
            board = nullptr;
    }
    void start(int role);
    void draw();
    void updateTimer();
};

Game* game = nullptr;

// --- Board Implementation ---
void Board::getRandomRoadPosition(int &rx, int &ry) {
        do {
            rx = rand() % grid_size;
            ry = rand() % grid_size;
        } while (layout[ry][rx] == 'B' || grid[ry][rx] != nullptr);
    }
    
void Board::setup(int role) {
    for(int i=0;i<grid_size;i++) {
    for(int j=0;j<grid_size;j++) { 
    if(grid[i][j]) {
    delete grid[i][j]; 
    grid[i][j]=nullptr; }
    }}
  
    int rx, ry; 
    int numTrees = 3;
    int numPassengers = 3;
    int numPackages = 3;
    int numNPCs = 2;
    
    // trees
    for(int i = 0; i < numTrees; i++) {
        getRandomRoadPosition(rx, ry);
        grid[ry][rx] = new Tree(rx, ry);
    }
    
    //station
    grid[1][1] = new FuelStation(1, 1);
    
    // garage
    grid[grid_size-2][1] = new Garage(1, grid_size-2);
    
    // NPC cars
    for(int i = 0; i < numNPCs; i++) {
        getRandomRoadPosition(rx, ry);
        grid[ry][rx] = new NPCCar(rx, ry);
    }
    
    // passengers
    for(int i = 0; i < numPassengers; i++) {
        getRandomRoadPosition(rx, ry);
        grid[ry][rx] = new Passenger(rx, ry);
    }
    
     // packages
    for(int i = 0; i < numPackages; i++) {
        getRandomRoadPosition(rx, ry);
        grid[ry][rx] = new Package(rx, ry);
    }
    
    //player
    player = new Player(1, grid_size-2, role);
}

void Board::draw(int Xset, int Yset) {
    for(int y=0;y<grid_size;y++) {
        for(int x=0;x<grid_size;x++) {
            int px = Xset + x*cell_size;
            int py = Yset + y*cell_size;
            if(layout[y][x] == 'B')
                DrawSquare(px, py, cell_size, colors[BLACK]);
            else
                DrawSquare(px, py, cell_size, colors[WHITE]);
            if(grid[y][x] && grid[y][x]->visible)
                grid[y][x]->draw(Xset, Yset);
        }
    }
    if(player) player->draw(Xset, Yset);
}

bool Board::isRoad(int x, int y) {
    if(x<0||x>=grid_size||y<0||y>=grid_size) return false;
    return layout[y][x] != 'B'; // Only buildings are impassable
}

void Board::spawnDropOff() {
    int rx, ry;
    do {
        rx = rand() % grid_size;
        ry = rand() % grid_size;
    }
while (layout[ry][rx] == 'B' || grid[ry][rx] != nullptr || (player && player->x == rx && player->y == ry));
    grid[ry][rx] = new DropOff(rx, ry);
    player->dropX = rx;
    player->dropY = ry;
}

void Board::movePlayer(int dx, int dy) {
    if(!player) return;
    int nx = player->x + dx;
    int ny = player->y + dy;
    if(nx<0||nx>=grid_size||ny<0||ny>=grid_size||!isRoad(nx,ny)) return;
    
    player->x = nx; 
    player->y = ny; 
    player->fuel--;
    
    // out of fuel
    if(player->fuel <= 0) {
            game->gameOver = true;
        return;
    }
    
    // Check for collisions
    if(grid[ny][nx]) {
        char type = grid[ny][nx]->getType();
        
        // Collision with NPC car
        if(type == 'N') {
            if(player->role == 0) {
                player->score -= 3; // Taxi penalty
            }
else {
                player->score -= 5; // Delivery penalty
            }
        }
        // Collision with tree
        else if(type == 'T') {
            if(player->role == 0) {
                player->score -= 2; // Taxi penalty
            }
else {
                player->score -= 4; // Delivery penalty
            }
        }

        else if((player->role == 0 && type == 'P' && player->carrying) || 
                (player->role == 1 && type == 'K' && player->carrying)) {
            if(player->role == 0) {
                player->score -= 5;
            }
else {
                player->score -= 8; 
            }
        }
        else if((player->role == 0 && type == 'K') || (player->role == 1 && type == 'P')) {
            if(player->role == 0) {
                player->score -= 2; 
            }
else {
                player->score -= 4; 
            }
        }
        
        if(player->score < 0) {
            player->score = 0;
            if(game) {
                game->gameOver = true; 
            }
        }
    }
    
    if(grid[ny][nx] && grid[ny][nx]->getType()=='F') {
        player->fuel = 100; 
        player->money -= 20; 
    }
}

void Board::tryPickup() {
    if(!player || player->carrying) return;
    
    Object* Object = grid[player->y][player->x];
    if(!Object) return;
    
    char type = Object->getType();
    if((player->role == 0 && type == 'P') || (player->role == 1 && type == 'K')) {
        player->carrying = true;
        delete Object;
        grid[player->y][player->x] = nullptr;
        spawnDropOff();
    }
}

void Board::tryDropoff() {
    if(!player || !player->carrying) return;
    
    Object* Object = grid[player->y][player->x];
    if(!Object || Object->getType() != 'D') return;
    
    player->carrying = false;
    if(player->role == 0) {
        player->score += 10;
        player->money += 10;
    }

    else {
        player->score += 20;
        player->money += 20;
    }
    
    delete Object;
    grid[player->y][player->x] = nullptr;
    player->dropX = -1;
    player->dropY = -1;
    
    
    // adding passenger/package after dropoff
    int rx, ry;
    do {
        rx = rand() % grid_size;
        ry = rand() % grid_size;
    }
while (layout[ry][rx] == 'B' || grid[ry][rx] != nullptr || 
            (player && player->x == rx && player->y == ry));
    
    if(player->role == 0) {
        grid[ry][rx] = new Passenger(rx, ry);
    }
else {
        grid[ry][rx] = new Package(rx, ry);
    }
}

void Board::switch_role() {
    Object* Object = grid[player->y][player->x];
    if(Object && Object->getType() == 'G') {
      if (player->role == 0) {
        player->role = 1;}
        else if (player->role == 1) {
        player->role  = 0; }                        // Toggle role
        player->carrying = false; // Drop any object jo carry kiya wa  
    }
}

// --- Game Implementation ---
void Game::start(int role) {
    board = new Board();
    board->setup(role);
    timeLeft = 180;
    gameOver = false;
}
void Game::draw() {
    int winW = glutGet(GLUT_WINDOW_WIDTH);
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    Xset = (winW - grid_size*cell_size) / 2;
    Yset = (winH - grid_size*cell_size) / 2;
    if(board) board->draw(Xset, Yset);
    
    // Draw stats
    if(board && board->player) {
        int infoX = Xset - 180;
        int infoY = Yset + grid_size*cell_size - 30;

        // Draw game stats
        DrawString(infoX, infoY + 30, "Player: " + string(playerName), colors[BLACK]);
        DrawString(infoX, infoY, "Score: " + to_string(board->player->score), colors[BLACK]);
        DrawString(infoX, infoY - 30, "Fuel: " + to_string(board->player->fuel) + "%", colors[BLACK]);
        DrawString(infoX, infoY - 60, "Money: " + to_string(board->player->money), colors[BLACK]);
        DrawString(infoX, infoY - 90, "Time: " + to_string(timeLeft) + "s", colors[BLACK]);
        
        // Draw role
        string roleText = board->player->role == 0 ? "Role: Taxi Driver" : "Role: Delivery Driver";
        DrawString(infoX, infoY - 120, roleText, colors[BLACK]);
    }
    
    if(gameOver) {
        if(board && board->player && board->player->score >= 100 && timeLeft == 0) {
            DrawString(Xset + grid_size*cell_size/2 - 60, Yset + grid_size*cell_size/2, "YOU WIN!", colors[GREEN]);
        }
else {
            DrawString(Xset + grid_size*cell_size/2 - 60, Yset + grid_size*cell_size/2, "GAME OVER", colors[RED]);
        }
    }
}
void Game::updateTimer() {
    if(!gameOver) {
        static int frame = 0;
        frame++;
        if(frame%10==0) {
            timeLeft--;
            if(timeLeft<=0) { 
                timeLeft=0; 
                if(board && board->player && (board->player->score >= 100) || (timeLeft==0)) {
                    gameOver = true;
                }
else {
                    gameOver = true;
                }
            }
            glutPostRedisplay();
        }
    }
}

int score = 0;
int fuel = 100;
bool carryingPassenger = false;
int dropOffX = -1, dropOffY = -1;
bool gameOver = false;
int timeLeft = 180; 

int Xset = 1, Yset = 1;

bool inMenu = true;
int menuState = 0; // 0: main menu, 1: name input, 2: role select, 3: leaderboard
int playerRole = 0; // 0: taxi, 1: delivery
string inputName = "";
bool isTypingName = false;

// Menu display
void drawMenu() {
    int winW = glutGet(GLUT_WINDOW_WIDTH);
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    int cx = winW/2 - 120;
    int cy = winH/2 + 60;
    DrawString(cx, cy, "RUSH HOUR GAME", colors[RED]);
    DrawString(cx, cy-40, "1. Start Game", colors[BLACK]);
    DrawString(cx, cy-80, "2. Leaderboard", colors[BLACK]);
    DrawString(cx, cy-120, "3. Exit", colors[BLACK]);
    DrawString(cx, cy-180, "Use 1, 2, or 3 to select.", colors[GRAY]);
}

// Name input menu
void drawNameInputMenu() {
    int winW = glutGet(GLUT_WINDOW_WIDTH);
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    int cx = winW/2 - 120;
    int cy = winH/2 + 60;
    
    DrawString(cx, cy, "Enter Your Name", colors[RED]);
    DrawString(cx, cy-40, "Name: " + inputName, colors[BLACK]);
    DrawString(cx, cy-80, "Press Enter to continue", colors[GRAY]);
}

// Role selection menu
void drawRoleMenu() {
    int winW = glutGet(GLUT_WINDOW_WIDTH);
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    int cx = winW/2 - 120;
    int cy = winH/2 + 60;
    DrawString(cx, cy, "Select Role", colors[RED]);
    DrawString(cx, cy-40, "1. Taxi Driver", colors[BLACK]);
    DrawString(cx, cy-80, "2. Delivery Driver", colors[BLACK]);
    DrawString(cx, cy-120, "3. Random Role", colors[BLACK]);
    DrawString(cx, cy-160, "4. Main Menu", colors[BLACK]);
    DrawString(cx, cy-220, "Use 1, 2, 3, or 4 to select.", colors[GRAY]);
}

// Draw leaderboard
void drawLeaderboardMenu() {
    int winW = glutGet(GLUT_WINDOW_WIDTH);
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    int cx = winW/2 - 120;
    int cy = winH/2 + 60;
    
    DrawString(cx, cy, "LEADERBOARD", colors[RED]);
   DrawString(cx, cy-100, "High Scores Logic Nahin Bani TA Bhai :(", colors[BLACK]);
    
    DrawString(cx, cy-370, "Press 'M' for Main Menu", colors[GRAY]);
}

// Update GameDisplay to show menu or game
void GameDisplay() {
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    if (inMenu) {
        if (menuState == 0) drawMenu();
        else if (menuState == 1) drawNameInputMenu();
        else if (menuState == 2) drawRoleMenu();
        else if (menuState == 3) drawLeaderboardMenu();
    }
else {
        game->draw();
    }
    glutSwapBuffers();
}

void SetCanvasSize(int width, int height) {
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}

void NonPrintableKeys(int key, int x, int y) {
    if (!inMenu && game && game->board && !game->gameOver) {
        if (key == GLUT_KEY_LEFT) game->board->movePlayer(-1, 0);
        if (key == GLUT_KEY_RIGHT) game->board->movePlayer(1, 0);
        if (key == GLUT_KEY_UP) game->board->movePlayer(0, 1);
        if (key == GLUT_KEY_DOWN) game->board->movePlayer(0, -1);
        glutPostRedisplay();
    }
}

void PrintableKeys(unsigned char key, int x, int y) {
    if (inMenu) {
        if (menuState == 0) { // Main menu
            if (key == '1') { 
                menuState = 1; 
                glutPostRedisplay();
            }
            else if (key == '2') { 
                menuState = 3; // Go to leaderboard
                glutPostRedisplay();
            }
            else if (key == '3') { 
                exit(0); 
            }
        }
        else if (menuState == 1) { // Name input menu
            if (key == 13) { 
                    strncpy(playerName, inputName.c_str(), 19);
                    playerName[19] = '\0';
                    menuState = 2; 
                    glutPostRedisplay();
            }
           
            else if (key >= 32 && key <= 126 && inputName.length() < 19) { 
                inputName += key;
                glutPostRedisplay();
            }
        }
        else if (menuState == 2) { // Role selection
            if (key == '1' || key == '2' || key == '3') { 
              if (key =='1') { playerRole = 0;}
              if (key =='2') { playerRole = 1;}
              if (key =='3') { playerRole = rand() % 2;;}
                game = new Game();
                game->start(playerRole);
                inMenu = false;
                glutPostRedisplay();
            }
            else if (key == '4') { 
                menuState = 1; 
                glutPostRedisplay();
            }
        }
        else if (menuState == 3) { // Leaderboard
            if (key == 'm' || key == 'M') { 
                menuState = 0; 
                drawMenu();
                glutPostRedisplay();
            }
        }
    }
    else {
        if (key == 27/* Escape key ASCII */) { 
            // Return to main menu
            inMenu = true;
            menuState = 0;
            glutPostRedisplay();
        }
        else if (key == ' ') {
            // Pickup/dropoff
            if(game && game->board && game->board->player) {
                if(game->board->player->carrying) {
                    game->board->tryDropoff();
                }
else {
                    game->board->tryPickup();
                }
                glutPostRedisplay();
            }
        }
        
        else if (key == 'p' || key == 'P') {
            if(game && game->board) {
                game->board->switch_role();
                glutPostRedisplay();
            }
        }
    }
}

void Timer(int m) {
    if (game && !game->gameOver) {
        game->updateTimer();
    }
    glutTimerFunc(100, Timer, 0); 
}

void MousePressedAndMoved(int x, int y) {
	glutPostRedisplay();
}
void MouseMoved(int x, int y) {
	glutPostRedisplay();
}
void MouseClicked(int button, int state, int x, int y) {
	glutPostRedisplay();
}

int main(int argc, char*argv[]) {

	int width = 1020, height = 840; // i have set my window size to be 800 x 600

	InitRandomizer(); // seed the random number generator...
	glutInit(&argc, argv); // initialize the graphics library...
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
	glutInitWindowPosition(50, 50); // set the initial position of our window
	glutInitWindowSize(width, height); // set the size of our window
	glutCreateWindow("OOP Project\nby Umar Asghar"); // set the title of our game window
	SetCanvasSize(width, height); // set the number of pixels...

	// Register your functions to the library,
	// you are telling the library names of function to call for different tasks.
	//glutDisplayFunc(display); // tell library which function to call for drawing Canvas.

	glutDisplayFunc(GameDisplay); // tell library which function to call for drawing Canvas.
	glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
	glutKeyboardFunc(PrintableKeys); // tell library which function to call for printable ASCII characters
	// This function tells the library to call our Timer function after 1000.0/FPS milliseconds...
	glutTimerFunc(1000.0, Timer, 0);

	glutMouseFunc(MouseClicked);
	glutPassiveMotionFunc(MouseMoved); // Mouse
	glutMotionFunc(MousePressedAndMoved); // Mouse

	// now handle the control to library and it will call our registered functions when
	// it deems necessary...
	glutMainLoop();
	return 1;
}
#endif /* RushHour_CPP_ */

