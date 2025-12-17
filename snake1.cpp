#include <SDL.h>
#include <iostream>
#undef main 

using namespace std; 

enum Direction
{
    DOWN = 0,
    LEFT = 1,
    RIGHT = 2, 
    UP = 3,
};

int main(int argc, char* argv[]) 
{
    // 1. Inicjalizacja z obsługą błędu
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "Błąd inicjalizacji SDL: " << SDL_GetError() << endl;
        return 1;
    }

    // 2. Tworzenie okna
    SDL_Window* window = SDL_CreateWindow("Test Renderowania",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, 
                                          1000, 1000, 
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "Błąd tworzenia okna: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    // 3. Wymuszenie Akceleracji (SDL_RENDERER_ACCELERATED) i VSync (SDL_RENDERER_PRESENTVSYNC)
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!renderer) {
       
        cerr << "Błąd tworzenia renderera. Sprawdź sterowniki graficzne: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Event e;
    bool running = true;
    int dir = RIGHT; // Startowy ruch w prawo
    SDL_Rect head {500, 500, 10, 10};

    while(running)
    {
        
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) { running = false; }
            if(e.type == SDL_KEYDOWN) {
                if(e.key.keysym.sym == SDLK_DOWN) { dir = DOWN; }
                else if(e.key.keysym.sym == SDLK_UP) { dir = UP; }
                else if(e.key.keysym.sym == SDLK_RIGHT) { dir = RIGHT; }
                else if(e.key.keysym.sym == SDLK_LEFT) { dir = LEFT; }
            }
        }
        switch(dir)
        {
            case DOWN:  head.y += 10; break;
            case UP:    head.y -= 10; break;
            case LEFT:  head.x -= 10; break;
            case RIGHT: head.x += 10; break;
        }

        // 4. Renderowanie (RÓŻOWA kropka na CIEMNOSZARYM tle)
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Ciemnoszary
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255); // RÓŻOWY
        SDL_RenderFillRect(renderer, &head); 

        SDL_RenderPresent(renderer);
        
        SDL_Delay(25);
    }

    // 5. Sprzątanie
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
