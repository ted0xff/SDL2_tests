/*  package: libsdl2-dev libsdl2-ttf-dev
gcc sdl_rect.c -o sdl_rect -lSDL2 -lSDL2_ttf

'fleches' : déplacement
'RGB': active RGB channel
'rgb': désactive RGB channel

*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


#define D_WINDOW_WIDTH  (800)
#define D_WINDOW_HEIGHT (600)

/* déplacement step */
int speed= 20;

/* pour debug */
int fps=0;




TTF_Font *verdanaFont=NULL;
SDL_Texture *textTexture=NULL;

// Est  appelé de my_callbackfunc(), sur mise à jour du calcul ...
void regenere_string_fps(SDL_Renderer* renderer) {
    char str[64];
    snprintf(str, 64, "FPS: %d",fps);
    SDL_Color textColor = { 0, 255, 0, 128 };
    SDL_Surface *textSurface = TTF_RenderText_Solid(verdanaFont,  str, textColor);
    if(!textSurface) {
        printf("textSurface: %s\n", TTF_GetError());
        return ;
    }

    /* evite leak : dealloue previous */
    if(! textTexture)
        SDL_DestroyTexture(textTexture);

    /* texture stockée pour appliquer */
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if(!textTexture) {
        printf("textTexture: %s\n",  SDL_GetError() );
        return ;
    }
    // on a tout ce qu'il faut, plus besoin de TTF :-p
    SDL_FreeSurface(textSurface);
}

// show FPS
unsigned int my_callbackfunc(Uint32 interval, void *param) {
    printf("FPS: %d\n", fps);
    regenere_string_fps( (SDL_Renderer*) param);

    fps = 0;
    return(interval);
}



int main (int argc, char** argv)
{
    int close = 0;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Rect r;
    SDL_Color m_color;

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    // need exact path : copy to application !!!
    verdanaFont = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 128);
    if(!verdanaFont) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        return -1;
    }

    window = SDL_CreateWindow(
        "test pixels", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        D_WINDOW_WIDTH,
        D_WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN );

    // Setup renderer
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

    //start rect
    r.x = 50;
    r.y = 50;
    r.w = 50;
    r.h = 50;
    // start color
    m_color.r = 0;
    m_color.g = 0;
    m_color.b = 255;
    m_color.a = 255;

    // timer FPS : dooit etre a 1000ms pour FP[S], sinon faut div !
    Uint32 delay = 1000;
    SDL_TimerID my_timer_id = SDL_AddTimer(delay, my_callbackfunc, renderer /* renderer as param*/);



    SDL_Rect textRect; /* position du texte (fixe) */
    /* TODO FIXME: refaire les maths ... pour pas de déformation et bonne taille*/
    textRect.x = 0/*D_WINDOW_WIDTH - textSurface->w * 0.5*/;
    textRect.y = 0/*D_WINDOW_HEIGHT - textSurface->h * 0.5*/;
    textRect.w = 100/*textSurface->w*/;
    textRect.h = 20/*textSurface->h*/;


	// annimation loop 
	while (!close) { 
		SDL_Event event; 

		// Events mangement 
		while (SDL_PollEvent(&event)) { 
			switch (event.type) { 

			case SDL_QUIT: 
				// handling of close button 
				close = 1; 
				break; 

			case SDL_KEYDOWN: 
				// keyboard API for key pressed move
				switch (event.key.keysym.scancode) { 
				case SDL_SCANCODE_W: 
				case SDL_SCANCODE_UP: 
					r.y -= speed; 
                    printf("(%3d,%3d)\n", r.x,r.y);
					break; 
				case SDL_SCANCODE_A: 
				case SDL_SCANCODE_LEFT: 
					r.x -= speed;
                    printf("(%3d,%3d)\n", r.x,r.y);
					break; 
				case SDL_SCANCODE_S: 
				case SDL_SCANCODE_DOWN: 
					r.y += speed; 
                    printf("(%3d,%3d)\n", r.x,r.y);
					break; 
				case SDL_SCANCODE_D: 
				case SDL_SCANCODE_RIGHT: 
					r.x += speed; 
                    printf("(%3d,%3d)\n", r.x,r.y);
					break; 

                // keypress color
				case SDL_SCANCODE_R: 
                    if( event.key.keysym.mod & KMOD_SHIFT )
                        m_color.r = 255;
                    else
                        m_color.r = 0;
					break; 
				case SDL_SCANCODE_G:
                    if( event.key.keysym.mod & KMOD_SHIFT )
                        m_color.g = 255;
                    else
                        m_color.g = 0;
					break;
				case SDL_SCANCODE_B: 
                    if( event.key.keysym.mod & KMOD_SHIFT )
                        m_color.b = 255;
                    else
                        m_color.b = 0;
					break;  
				} 
			} 
		} 

		// right boundary 
		if (r.x + r.w > D_WINDOW_WIDTH) 
			r.x = D_WINDOW_WIDTH - r.w; 

		// left boundary 
		if (r.x < 0) 
			r.x = 0; 

		// bottom boundary 
		if (r.y + r.h > D_WINDOW_HEIGHT) 
			r.y = D_WINDOW_HEIGHT - r.h; 

		// upper boundary 
		if (r.y < 0) 
			r.y = 0; 


        // Set render color to black ( background will be rendered in this color )
        SDL_SetRenderDrawColor( renderer, 0, 0, 0,   255 );
        SDL_RenderClear( renderer );

        // Set render color to blue ( rect will be rendered in this color ) for rect draw
        SDL_SetRenderDrawColor( renderer, m_color.r, m_color.g, m_color.b, m_color.a);
        SDL_RenderFillRect( renderer, &r );

        // c'est précalculé et copié dans le renderer
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        // Render the scene to the screen
        SDL_RenderPresent(renderer);

        //calcul FPS
        fps++;

		// calculates to 30 fps 
    	SDL_Delay(1000 / 30); 
	} 

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
