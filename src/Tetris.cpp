#include <iostream>
#include <cstdint>
#include <cstdlib>
#include "Tetris.h"

Tetromino tetrominos[] = {
	Tetromino(tetrominoBar,4),
	Tetromino(tetrominoSquare,2),
	Tetromino(tetrominoL,3),
	Tetromino(tetrominoRL,3),
	Tetromino(tetrominoT,3),
	Tetromino(tetrominoSkew,3),
	Tetromino(tetrominoRSkew,3),
};

uint8_t xy_get(const uint8_t* values, int width, int x, int y) {
	return values[x * width + y];
}

void xy_set(uint8_t *values, int width, int x, int y, uint8_t val) {
	values[x * width + y] = val;
}

uint8_t tetromino_get(const Tetromino* t, int x, int y, int rotation) {
	int size = t->mSize;
	switch (rotation) {
	case 0: // No rotation
		return t->data[x * size + y];
	case 1: // 90 rotation
		return t->data[(size - y - 1) * size + x];
	case 2: // 180 rotation
		return t->data[(size - x - 1) * size + (size - y - 1)];
	case 3: // 270 rotation
		return t->data[y * size + (size - x - 1)];
	}
	return 0;
}

int random_int(int min, int max) { 
	int range = max - min;
	return min + rand() % range;
}

uint8_t check_full_line(const uint8_t* board, int width, uint8_t row) { 
	for (int col = 0; col < width; ++col) { 
		if (!xy_get(board, width, row, col)) {
			return 0;
		}
	}
	return 1;
}

uint8_t check_empty_line(const uint8_t* board, int width, uint8_t row) { 
	for (int col = 0; col < width; ++col) { 
		if (xy_get(board, width, row, col)) {
			return 0;
		}
	}
	return 1;
}

int get_full_lines(const uint8_t* board, int width, int height, uint8_t* lines) {
	int count = 0;
	for (int row = 0; row < height; ++row) {
		uint8_t filled = check_full_line(board, width, row);
		lines[row] = filled;
		count += filled;
	}
	return count;
}

void clear_lines(uint8_t* board, int width, int height, const uint8_t* lines) {
	int bRow = height - 1;
	for (int dstRow = height-1; dstRow >= 0; --dstRow) {
		while (bRow > 0 && lines[bRow]) {
			--bRow;
		}
		if (bRow < 0) {
			memset((board + dstRow * width), 0, width);
		}
		else {
			if (bRow != dstRow) {
				memcpy((board + dstRow * width), (board + bRow * width), width);
			}
			--bRow;
		}
	}
}

bool validMove(PieceState* piece, const uint8_t* board, int width, int height) {
	const Tetromino* t = tetrominos + piece->tIndex;
	for (int row = 0; row < t->mSize; ++row) {
		for (int col = 0; col < t->mSize; ++col) {
			uint8_t value = tetromino_get(t, row, col, piece->rotation);
			if (value > 0) {
				int board_row = piece->offset_row + row;
				int board_col = piece->offset_col + col;
				// Is out of bounds
				if (board_row < 0 || board_row >= height) {
					return false;
				}
				if (board_col < 0 || board_col >= width) {
					return false;
				}
				// Colides with board pieces
				if (xy_get(board, width, board_row, board_col)) {
					return false;
				}
			}
		}
	}
	return true;
}

double get_next_drop_time() {
	return 43 * SECONDS_PER_FRAME;
}

void spawn_piece(GameState* game) {
	game->pieceState = {};
	game->pieceState.tIndex = (uint8_t)random_int(0, ARRAY_COUNT(tetrominos));;
	game->pieceState.offset_col = WIDTH / 2;
	game->next_drop_time = game->time + get_next_drop_time();
}

void place_piece(GameState* game) {
	const Tetromino* t = tetrominos + game->pieceState.tIndex;
	for (int row = 0; row < t->mSize; ++row) {
		for (int col = 0; col < t->mSize; ++col) {
			uint8_t value = tetromino_get(t, row, col, game->pieceState.rotation);
			if (value) {
				int board_row = game->pieceState.offset_row + row;
				int board_col = game->pieceState.offset_col + col;
				xy_set(game->board, WIDTH, board_row, board_col, value);
			}
		}
	}
}

bool drop_piece(GameState* game) {
	++game->pieceState.offset_row;
	if (!validMove(&game->pieceState, game->board,WIDTH,HEIGHT)) {
		// Colided with board, move piece back up
		--game->pieceState.offset_row;
		place_piece(game);
		spawn_piece(game);
		return false;
	}
	
	game->next_drop_time = game->time + get_next_drop_time();
	return true;
}

void update_game_start(GameState* gState, const InputState* input)
{
	if (input->dbtn1 > 0)
	{
		memset(gState->board, 0, WIDTH * HEIGHT);
		gState->line_count = 0;
		gState->score = 0;
		spawn_piece(gState);
		gState->phase = GamePhase::GAME_PHASE_PLAYING;
	}
}

void update_game_over(GameState* gState, const InputState* input)
{
	if (input->dbtn1 > 0)
	{
		gState->phase = GamePhase::GAME_PHASE_START;
	}
}

void update_clear_lines(GameState* gState) {
	if (gState->time >= gState->clear_lines_time) {
		clear_lines(gState->board, WIDTH, HEIGHT, gState->full_lines);
		gState->line_count += gState->lines_to_clear;
		gState->score += gState->point_values[gState->lines_to_clear];
		gState->phase = GamePhase::GAME_PHASE_PLAYING;
	}
}

void update_gameplay(GameState* gState, const InputState* input) {
	PieceState pState = gState->pieceState;

	if (input->dleft > 0) {
		--pState.offset_col;
	}
	if (input->dright > 0) {
		++pState.offset_col;
	}
	if (input->dup > 0) {
		pState.rotation = (pState.rotation + 1) % 4;
	}
	if (validMove(&pState, gState->board, WIDTH, HEIGHT)) {
		gState->pieceState = pState;
	}

	if (input->ddown > 0) {
		drop_piece(gState);
	}

	if (input->dbtn1 > 0) {
		while (drop_piece(gState));
	}

	while (gState->time >= gState->next_drop_time) {
		drop_piece(gState);
	}

	gState->lines_to_clear = get_full_lines(gState->board, WIDTH, HEIGHT, gState->full_lines);

	if (gState->lines_to_clear > 0) {
		gState->phase = GamePhase::GAME_PHASE_LINE;
		gState->clear_lines_time = gState->time + 0.1f;
	}

	// It's game over when the two invisible rows are filled
	if (!check_empty_line(gState->board, WIDTH, HEIGHT - PLAYABLE_HEIGHT)) {
		gState->phase = GamePhase::GAME_PHASE_OVER;
	}
}

void update_game(GameState* gState, const InputState* input) {
	switch (gState->phase) {
	case GamePhase::GAME_PHASE_PLAYING:
		update_gameplay(gState, input);
		break;
	case GamePhase::GAME_PHASE_LINE:
		update_clear_lines(gState);
		break;
	case GamePhase::GAME_PHASE_OVER:
		update_game_over(gState, input);
		break;
	case GamePhase::GAME_PHASE_START:
		update_game_start(gState, input);
		break;
	}
}


