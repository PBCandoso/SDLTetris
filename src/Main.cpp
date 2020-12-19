#include "SDL.h"
#include "Tetris.h"
#include "Colors.h"

void fill_rect(SDL_Renderer* renderer,
	int x, int y, int width, int height, Color color)
{
	SDL_Rect rect = {};
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

void draw_rect(SDL_Renderer* renderer,
	int x, int y, int width, int height, Color color)
{
	SDL_Rect rect = {};
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawRect(renderer, &rect);
}

void draw_cell(SDL_Renderer* renderer, int row, int col, uint8_t value, 
	int offset_x, int offset_y, bool outline = false, bool grid = false) {

	Color base_color = BASE_COLORS[value];
	Color light_color = LIGHT_COLORS[value];
	Color dark_color = DARK_COLORS[value];

	int edge = GRID_SIZE / 8;

	int x = col * GRID_SIZE + offset_x;
	int y = row * GRID_SIZE + offset_y;

	if (outline)
	{
		draw_rect(renderer, x, y, GRID_SIZE, GRID_SIZE, base_color);
		return;
	}

	if (grid) {
		edge /= 2;
		draw_rect(renderer, x, y, GRID_SIZE, GRID_SIZE, GRID_OUTLINE);
		return;
	}

	fill_rect(renderer, x, y, GRID_SIZE, GRID_SIZE, dark_color);
	fill_rect(renderer, x + edge, y,
		GRID_SIZE - edge, GRID_SIZE - edge, light_color);
	fill_rect(renderer, x + edge, y + edge,
		GRID_SIZE - edge * 2, GRID_SIZE - edge * 2, base_color);
}

void draw_board(SDL_Renderer* renderer, const uint8_t* board, int width, int height,
	int offset_x, int offset_y) {
	for (int row = 0; row < height; ++row) {
		for (int col = 0; col < width; ++col) {
			uint8_t value = xy_get(board, width, row, col);
			if (value) {
				draw_cell(renderer, row, col, value, offset_x, offset_y);
			}
			else {
				draw_cell(renderer, row, col, offset_x,offset_y, false,true);
			}
		}
	}
}

void draw_piece(SDL_Renderer* renderer, const PieceState* piece, int offset_x, int offset_y, bool outline = false) {
	const Tetromino* t = tetrominos + piece->tIndex;
	for (int row = 0; row < t->mSize; ++row) {
		for (int col = 0; col < t->mSize; ++col) {
			uint8_t value = tetromino_get(t, row, col, piece->rotation);
			if (value) {
				draw_cell(renderer, row + piece->offset_row, col + piece->offset_col, value, offset_x, offset_y, outline);
			}
		}
	}
}

void render_game(const GameState* gState, SDL_Renderer* renderer) {
	draw_board(renderer, gState->board, WIDTH, HEIGHT, 0 ,0);
	draw_piece(renderer, &gState->pieceState,0,0);
	PieceState ghost = gState->pieceState;
	while (validMove(&ghost, gState->board, WIDTH,HEIGHT)) {
		ghost.offset_row++;
	}
	ghost.offset_row--;
	draw_piece(renderer, &ghost, 0, ghost.offset_col, true);
}

int main(int argc, char* argv[]) {

	/*
	if (SDL_Init(SDL_INIT_VIDEO) < 1) {
		return 1;
	}
	*/
	SDL_Window* window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	GameState game = {};
	InputState input = {};

	bool quit = false;
	while (!quit) {

		game.time = SDL_GetTicks() / 1000.0f;

		SDL_Event e;
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}
		int key_count;
		const uint8_t* key_states = SDL_GetKeyboardState(&key_count);
		
		if (key_states[SDL_SCANCODE_ESCAPE]) {
			quit = true;
		}
	
		InputState previous_input = input;
		input.left = key_states[SDL_SCANCODE_LEFT];
		input.right = key_states[SDL_SCANCODE_RIGHT];
		input.up = key_states[SDL_SCANCODE_UP];
		input.down = key_states[SDL_SCANCODE_DOWN];
		input.btn1 = key_states[SDL_SCANCODE_SPACE];

		input.dleft = input.left - previous_input.left;
		input.dright = input.right - previous_input.right;
		input.dup = input.up - previous_input.up;
		input.ddown= input.down - previous_input.down;
		input.dbtn1 = input.btn1 - previous_input.btn1;

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		update_game(&game, &input);
		render_game(&game, renderer);

		SDL_RenderPresent(renderer);

	}

	SDL_DestroyRenderer(renderer);
	SDL_Quit();

	return 0;
}

