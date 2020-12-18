#include <cstdint>
#include "Tetris.h"
#include <iostream>

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

void update_gameplay(GameState* gState, const InputState* input) {
	PieceState pState = gState->pieceState;

	if (input->dleft > 0) {
		--pState.offset_col;
	}
	if (input->dright > 0) {
		std::cout << pState.offset_col;
		++pState.offset_col;
	}
	if (input->dup > 0) {
		pState.rotation = (pState.rotation + 1) % 4;
	}
	if (validMove(&pState, gState->board, WIDTH, HEIGHT)) {
		gState->pieceState = pState;
	}
}

void update_game(GameState* gState, const InputState* input) {
	switch (gState->phase) {
	case GAME_PHASE_PLAYING:
		return update_gameplay(gState, input);
	}
}


