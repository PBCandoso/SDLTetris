#include "SDL.h"
#include "SDL_ttf.h"
#include "Tetris.h"
#include "Colors.h"

void draw_text(SDL_Renderer* renderer, TTF_Font* font, const char* text,
	int x, int y, TextAlign align, Color c) {
	SDL_Color sdl_color = SDL_Color { c.r, c.g, c.b, c.a };
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, sdl_color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_Rect rect;
	rect.w = surface->w;
	rect.h = surface->h;
	rect.y = y;
	switch (align) {
	case TextAlign::TEXT_ALIGN_LEFT:
		rect.x = x;
		break;
	case TextAlign::TEXT_ALIGN_RIGHT:
		rect.x = x - surface->w;
		break;
	case TextAlign::TEXT_ALIGN_CENTER:
		rect.x = x - surface->w / 2;
		break;
	}
	SDL_RenderCopy(renderer, texture, 0, &rect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

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
		draw_rect(renderer, x+2, y+2, GRID_SIZE-4, GRID_SIZE-4, base_color);
		return;
	}

	if (grid) {
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
				draw_cell(renderer, row, col, 0, offset_x, offset_y, false,true);
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

void render_game(const GameState* gState, SDL_Renderer* renderer, TTF_Font* font) {
	Color tColor = color(0xFF, 0xFF, 0xFF, 0xFF);
	int margin_x = 150;
	// Draw board background
	fill_rect(renderer, margin_x, 0, GRID_SIZE * WIDTH, GRID_SIZE * HEIGHT, GRID_COLOR);
	// Draw board outline and pieces
	draw_board(renderer, gState->board, WIDTH, HEIGHT, margin_x, 0);
	if (gState->phase == GamePhase::GAME_PHASE_PLAYING ||
		gState->phase == GamePhase::GAME_PHASE_LINE) {
		
		PieceState ghost = gState->pieceState;
		draw_piece(renderer, &gState->pieceState,margin_x,0);
		while (validMove(&ghost, gState->board, WIDTH,HEIGHT)) {
			ghost.offset_row++;
		}
		ghost.offset_row--;
		draw_piece(renderer, &ghost, margin_x, 0, true);

	}
	else if (gState->phase == GamePhase::GAME_PHASE_OVER) {
		int x = margin_x + (WIDTH * GRID_SIZE) / 2;
		int y = (HEIGHT * GRID_SIZE) / 2;
		draw_text(renderer, font, "GAME OVER", x, y, TextAlign::TEXT_ALIGN_CENTER, tColor);
	}
	else if (gState->phase == GamePhase::GAME_PHASE_START) {
		int x = margin_x + (WIDTH * GRID_SIZE) / 2;
		int y = (HEIGHT * GRID_SIZE) / 2;
		draw_text(renderer, font, "PRESS SPACE TO START", x, y, TextAlign::TEXT_ALIGN_CENTER, tColor);
	}
	fill_rect(renderer, margin_x, 0,
		WIDTH * GRID_SIZE, (HEIGHT - PLAYABLE_HEIGHT) * GRID_SIZE,
		color(0x00, 0x00, 0x00, 0x00));
}

int main(int argc, char* argv[]) {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return 1;
	}

	if (TTF_Init() < 0) {
		return 2;
	}

	const char* font_file = "sesquipedalian.ttf";

	TTF_Font* font = TTF_OpenFont(font_file, 20);

	SDL_Window* window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	
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
		render_game(&game, renderer, font);

		SDL_RenderPresent(renderer);

	}

	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();

	return 0;
}

