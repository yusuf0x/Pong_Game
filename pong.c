
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "pong.h"
/*
sudo apt install libsdl2-dev
sudo apt install libsdl2-ttf-dev 

gcc pong.c `sdl2-config --cflags --libs` -lSDL2_ttf -o pong

*/
#define WIDTH 640
#define HEIGHT 480
#define BALL_X (WIDTH/2)
#define BALL_Y (HEIGHT/2)

int main(int argc, char const *argv[])
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	TTF_Font *font;
	Init(&window,&renderer,&font);

	Paddle player1,AI_player;
	Ball ball;
	init_game(&player1,&AI_player,&ball);

	SDL_Color White = {255, 255, 255};
	char score_str[5];
	sprintf(score_str, "%d  %d", player1.score, AI_player.score);
	SDL_Rect score_rect;

	int running = 0;
	int paused = -1;
	SDL_Event event;
	int begin = 0;
	while(!running){
		if (begin == 0)
		{
			start_or_quit(event, &running, &begin);
			draw_menu(renderer,font);
		}
		else{
			one_player(event, &running, &ball, &player1, &AI_player, score_str, &paused);
			draw_score(renderer, score_str, font, &score_rect, &White);
			draw_game(renderer, player1, AI_player, ball);
			if(paused == 1)
				game_paused(renderer, font);
		}
		update_game(renderer);
	}


	printf("HEllo\n");
	quit(renderer, window, font);
	return 0;
}

int Init(SDL_Window **window,SDL_Renderer **renderer,TTF_Font** font){
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("Error in Init \n");
		return 1;
	}
	*window = SDL_CreateWindow("pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if(NULL == *window)
	{
		fprintf(stderr, "Erreur SDL_CreateWindow : %s",SDL_GetError());
		SDL_Quit();
		return 1;
	}
	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(NULL == *renderer)
	{
		fprintf(stderr, "Erreur SDL_CreateRenderer : %s",SDL_GetError());
		SDL_DestroyWindow(*window);
		SDL_Quit();
		return 1;
	}
	TTF_Init();
	*font = TTF_OpenFont("Press_Start_2P/PressStart2P-Regular.ttf", 24);
	if(*font == NULL){
		printf("Error initializing font: %s\n", SDL_GetError());
		SDL_DestroyWindow(*window);
		SDL_DestroyRenderer(*renderer);
		SDL_Quit();
		return 1;
	}
	SDL_Surface* icon = SDL_LoadBMP("img/pong.png");
	SDL_SetWindowIcon(*window, icon);
}	
void init_game(Paddle *player1,Paddle *AI_player,Ball *ball){

	SDL_Rect player_rect = {0,HEIGHT/2,16,64};
	player1->rect = player_rect;
	player1->score = 0;
	player1->controls[0]=player1->controls[1]=0;

	SDL_Rect AI_player_rect = {WIDTH-16,HEIGHT/2,16,64};
	AI_player->rect = AI_player_rect;
	AI_player->score = 0;
	AI_player->controls[0]=AI_player->controls[1]=0;

	
	ball->x = 5;
	ball->y = 5;
	SDL_Rect ball_rect = {BALL_X,BALL_Y,16,16};
	ball->rect = ball_rect;
	return;
}
void start_or_quit(SDL_Event event, int* running,int* begin){
	while(SDL_PollEvent(&event))
		switch(event.type){
			case SDL_QUIT:
				*running = 1;
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym){
					case SDLK_SPACE:
						*begin = 1;
						break;
					default:
						break;
				}
				break;
		}
}

void draw_menu(SDL_Renderer *renderer, TTF_Font* font){
	SDL_Color color;
	SDL_Color White = { 255, 255, 255 };
	SDL_Surface * surface = TTF_RenderText_Solid(font, "Pong Game", White);
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect pong_rect = {(WIDTH / 2) - 150, 0, 300, 200};
	SDL_RenderCopy(renderer, texture, NULL, &pong_rect);
	surface = TTF_RenderText_Solid(font, "space to play", White);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect oneplayer_rect = {(WIDTH / 2) - 150, HEIGHT/2,400, 200};
	SDL_RenderCopy(renderer, texture, NULL, &oneplayer_rect);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}

void player_event(SDL_Event event, Paddle* player1, int* running, int* paused){
	while(SDL_PollEvent(&event))
		switch(event.type){
		case SDL_QUIT:
			*running = 1;
			break;
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym){
			case SDLK_w:
				player1->controls[0]=1;
				break;
			case SDLK_s:
				player1->controls[1]=1;
				break;
			case SDLK_ESCAPE:
				*paused *= -1;
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
				switch(event.key.keysym.sym){
				case SDLK_w:
					player1->controls[0]=0;
					break;
				case SDLK_s:
					player1->controls[1]=0;
					break;
				default:
					break;
				}
		}
}
void move_ai_paddle(Paddle *ai_player, Ball* ball){
	if(ball->rect.y >= ai_player->rect.y && ball->rect.x > WIDTH - WIDTH / 2) ai_player->rect.y += 5;
	else {
		ai_player->rect.y -= 5;
	}
	if(ai_player->rect.y <= 0) ai_player->rect.y = 0;
	else if(ai_player->rect.y + ai_player->rect.h >= HEIGHT) ai_player->rect.y = HEIGHT - ai_player->rect.h;
}
void move_ball(Ball* ball, Paddle* player, Paddle* ai_player, char* score_str){
	ball->rect.x += ball->x;
	ball->rect.y += ball->y;
	// ai_player win 
	if(ball->rect.x <= 0){
		ball->rect.x = BALL_X;
		ball->rect.y = BALL_Y;
		ball->x *= -1;
		ai_player->score++;
		sprintf(score_str, "%d  %d", player->score, ai_player->score);
	}
	// player win 
	if(ball->rect.x + ball->rect.w >= WIDTH){
		ball->rect.x = BALL_X;
		ball->rect.y = BALL_Y;
		ball->x *= -1;
		player->score++;
		sprintf(score_str, "%d  %d", player->score, ai_player->score);
	}
	if(ball->rect.y <= 0){
				ball->rect.y = 0;
				ball->y *= -1;
			}
	if(ball->rect.y + ball->rect.h >= HEIGHT){
				ball->rect.y = HEIGHT - ball->rect.h;
				ball->y *= -1;
			}
	//check for collision with the paddle
	if(ball->rect.y + ball->rect.h >= player->rect.y && ball->rect.y <= player->rect.y + player->rect.h && ball->rect.x >= player->rect.x && ball->rect.x <= player->rect.x + player->rect.w){
		ball->rect.x = player->rect.x + player->rect.w;
		ball->x *= -1;
	}
	if(ball->rect.y + ball->rect.h >= ai_player->rect.y && ball->rect.y - ball->rect.h <= ai_player->rect.y + ai_player->rect.h && ball->rect.x + ball->rect.w >= ai_player->rect.x && ball->rect.x <= ai_player->rect.x + ai_player->rect.w){
		ball->rect.x = ai_player->rect.x - ball->rect.w;
		ball->x *= -1;
	}
	return;
}
void move_player_paddle(Paddle* player){
	if(player->controls[0] && !player->controls[1]) player->rect.y -= 5;
	if(player->controls[1] && !player->controls[0]) player->rect.y += 5;
	if(player->rect.y <= 0) player->rect.y = 0;
	if(player->rect.y + player->rect.h >= HEIGHT) player->rect.y = HEIGHT - player->rect.h;
	return;
}
void one_player(SDL_Event event, int* running, Ball* ball, Paddle* player, Paddle* ai_player, char* score_str, int* paused){
	///check player event 
	player_event(event, player,running, paused);
	if(*paused == -1){
		move_player_paddle(player);
		move_ball(ball, player, ai_player, score_str);
		move_ai_paddle(ai_player, ball);
		if(player->score >= 5 || ai_player->score >= 5){
			sprintf(score_str, "%d  %d", player->score, ai_player->score);
			player->score = 0;
			ai_player->score = 0;
		}
	}
}

void draw_score(SDL_Renderer *renderer, const char *score_str, TTF_Font *font, SDL_Rect *rect, SDL_Color *color) {
    SDL_Surface *surface;
    SDL_Texture *texture;
    surface = TTF_RenderText_Solid(font, score_str, *color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect->x = (WIDTH / 2) - 40;
    rect->y = 0;
    rect->w = surface->w;
    rect->h = surface->h;
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_DestroyTexture(texture);
}
void game_paused(SDL_Renderer *renderer, TTF_Font* font){
	SDL_Color White = { 255, 255, 255 };
	SDL_Surface * surface = TTF_RenderText_Solid(font, "Paused", White);
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect pause_rect = {(WIDTH / 2) - 150, HEIGHT/2 - 150, 300, 200};
	SDL_RenderCopy(renderer, texture, NULL, &pause_rect);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}
void draw_game(SDL_Renderer *renderer, Paddle player, Paddle ai_player, Ball ball){
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255 );
	SDL_Rect* player_rect = &player.rect;
	SDL_Rect* ai_player_rect = &ai_player.rect;
	SDL_Rect* ball_rect = &ball.rect;
	SDL_RenderFillRect(renderer, player_rect);
	SDL_RenderFillRect(renderer, ai_player_rect);
	SDL_RenderFillRect(renderer, ball_rect);
	SDL_Rect line;
	line.x =  WIDTH / 2;
	line.y = 20;
	line.w = 5;
	line.h = 15;
	int i,r;
	for(i = 0; i < 16; i++) {
		r = SDL_RenderFillRect(renderer,&line);
		line.y = line.y + 30;
	}
	return;
}

void update_game(SDL_Renderer* renderer){
	SDL_RenderPresent(renderer);
	//Clear screen
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_Delay(1000/60);
	return;
}
void quit(SDL_Renderer* renderer, SDL_Window* window, TTF_Font* font){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(font);
	SDL_Quit();
	TTF_Quit();
	return;
}