#pragma once

#define WIDTH 10
#define HEIGHT 22
#define PLAYABLE_HEIGHT 20
#define GRID_SIZE 30

#define ARRAY_COUNT(x) (sizeof(x) / sizeof((x)[0]))

const double SECONDS_PER_FRAME = 1.f / 60.f;

struct Tetromino {
	const uint8_t* data;
	const int mSize;

	Tetromino(const uint8_t* matrix, int size) : data(matrix), mSize(size) {};
};

// Tetromino piece matrices
// Bar tetromino
const uint8_t tetrominoBar[] = {
	0,0,0,0,
	1,1,1,1,
	0,0,0,0,
	0,0,0,0
};
// Square tetromino
const uint8_t tetrominoSquare[] = {
	2,2,
	2,2
};
// L tetromino
const uint8_t tetrominoL[] = {
	0,0,3,
	3,3,3,
	0,0,0
};
// Reverse L tetromino
const uint8_t tetrominoRL[] = {
	4,0,0,
	4,4,4,
	0,0,0
};
// T tetromino
const uint8_t tetrominoT[]{
	0,0,0,
	5,5,5,
	0,5,0
};
// Skew tetromino
const uint8_t tetrominoSkew[]{
	0,6,6,
	6,6,0,
	0,0,0
};
// Reverse Skew tetromino
const uint8_t tetrominoRSkew[]{
	7,7,0,
	0,7,7,
	0,0,0
};

Tetromino tetrominos[];

struct InputState {

	int8_t left;
	int8_t right;
	int8_t up;
	int8_t down;
	int8_t btn1;

	int8_t dleft;
	int8_t dright;
	int8_t dup;
	int8_t ddown;
	int8_t dbtn1;
};

struct PieceState {
	// Type of piece
	uint8_t tIndex;
	int offset_row;
	int offset_col;
	int rotation;
};

enum struct GamePhase {
	GAME_PHASE_PLAYING,
	GAME_PHASE_LINE
};

struct GameState {
	uint8_t board[WIDTH * HEIGHT];
	uint8_t full_lines[HEIGHT];
	PieceState pieceState;
	GamePhase phase;
	int score;
	// Points for 0,1,2,3,4 lines cleared
	int point_values[5] = { 0, 40, 100, 300, 1200 };
	int line_count;
	int lines_to_clear;

	double time;
	double next_drop_time;
	double clear_lines_time;
};

// Get value from 1D array based on (x,y) coordinates
uint8_t xy_get(const uint8_t* values, int width, int x, int y);

// Set value from 1D array based on (x,y) coordinates
void xy_set(uint8_t* values, int width, int x, int y, uint8_t val);

uint8_t tetromino_get(const Tetromino* t, int x, int y, int rotation);

bool validMove(PieceState* piece,const uint8_t* board, int width, int height);

void spawn_piece(GameState* game);

void place_piece(GameState* game);

bool drop_piece(GameState* game);

void update_gameplay(GameState* gState, InputState* iState);

void update_game(GameState* gState, const InputState* input);

