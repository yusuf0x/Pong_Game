#include <SDL2/SDL.h>


typedef struct 
{
	SDL_Rect rect;
	int score;
	int controls[2];
}Paddle;

typedef struct
{
	SDL_Rect rect;
	int x;
	int y;
}Ball;

int Init(SDL_Window **window,SDL_Renderer **renderer,TTF_Font** font);
void init_game(Paddle *player1,Paddle *AI_player,Ball *ball);
void start_or_quit(SDL_Event event, int* running,int* begin);
void draw_menu(SDL_Renderer *renderer, TTF_Font* font);
void player_event(SDL_Event event, Paddle* player1, int* running, int* paused);
void move_ai_paddle(Paddle *ai_player, Ball* ball);
void move_ball(Ball* ball, Paddle* player, Paddle* ai_player, char* score_str);
void move_player_paddle(Paddle* player);
void one_player(SDL_Event event, int* running, Ball* ball, Paddle* player, Paddle* ai_player, char* score_str, int* paused);
void draw_score(SDL_Renderer *renderer, const char *score_str, TTF_Font *font, SDL_Rect *rect, SDL_Color *color) ;
void game_paused(SDL_Renderer *renderer, TTF_Font* font);
void draw_game(SDL_Renderer *renderer, Paddle player, Paddle ai_player, Ball ball);
void update_game(SDL_Renderer* renderer);
void quit(SDL_Renderer* renderer, SDL_Window* window, TTF_Font* font);