#define SDL_MAIN_HANDLED // pozwala na uzywanie zwyklej funkcji main
#include <SDL2/SDL.h> //wieksze szanse ze program zadziala
#include <iostream>
#include <vector>
#include <algorithm>
#include <deque> // pozwala na wyciaganie i wkladanie rzeczy z frontu i tylu 
#include <ctime>

using namespace std;

//wymiary okna
int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

int ART_NUM = 65; //ilosc artefaktow
int score;

enum Direction
{
    DOWN = 0,
    LEFT = 1,
    RIGHT = 2,
    UP = 3,
};

enum GameState //stany gry
{
    MENU = 0,
    LEADERBOARD = 1,
    PLAYING = 2,
    GAMEOVER = 3
};

void artefakty(vector<SDL_Rect> &artifacts)
{
    for (int i = 0;i < ART_NUM;i++)
    {
        artifacts.push_back({
            rand() % (WINDOW_WIDTH - 20),  // Losowy X w granicach okna
            rand() % (WINDOW_HEIGHT - 20), // Losowy Y w granicach okna
            20, 20
            });
    }
}

int main()
{
    srand(time(NULL)); //ziarno losowosci
    // Inicjalizacja z obsługa błędu
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "Błąd inicjalizacji SDL: " << SDL_GetError() << endl;
        return 1;
    }

    // Tworzenie okna
    SDL_Window* window = SDL_CreateWindow("Indiana Snake",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, //dodanie wymiarow jako zmiennej globalnej
        SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "Błąd tworzenia okna: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    // Wymuszenie Akceleracji (SDL_RENDERER_ACCELERATED) i VSync (SDL_RENDERER_PRESENTVSYNC)
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
    deque<SDL_Rect> rq; //cialo weza
    int snake_size = 3; //rozmiar snake'a
    vector<SDL_Rect> artifacts;
    SDL_Rect head{ WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 20, 20 }; //skalowanie z wymiarami ktore sa teraz zmienna globalna, zwiekszone wymiary snake'a
    score = 0;

    artefakty(artifacts);

    while (running)
    {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running = false; }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) { running = false; } //dodano linijke zamykajaca program po wcisnieciu escape
                else if (e.key.keysym.sym == SDLK_DOWN) { dir = DOWN; }
                else if (e.key.keysym.sym == SDLK_UP) { dir = UP; }
                else if (e.key.keysym.sym == SDLK_RIGHT) { dir = RIGHT; }
                else if (e.key.keysym.sym == SDLK_LEFT) { dir = LEFT; }
             
            }
        }
        switch (dir)
        {
        case DOWN:  head.y += 20; break;
        case UP:    head.y -= 20; break;
        case LEFT:  head.x -= 20; break;
        case RIGHT: head.x += 20; break;
        }

        //Kolizje
        //Artefakty
        for (auto& artif : artifacts) {
            if (SDL_HasIntersection(&head, &artif)) //jesli sie pokryje z artefaktem
            {
                snake_size += 2; //jak je artefakt to rosnie o 2
                artif.x = -100;//wywala artefakt za mape
                artif.y = -100;
                score++;
                cout << score << endl;
            }
        };

        //Sam ze soba - zakoncz gre
        for_each(rq.begin(), rq.end(), [&](auto& snake_segment) {
            if (head.x == snake_segment.x && head.y == snake_segment.y)
            {
                running = false;
            }
            });

        //Ze sciana - zakoncz gre
        if (head.x < 0 || head.x > WINDOW_WIDTH - 20 || head.y < 0 || head.y > WINDOW_HEIGHT - 20) 
        {
            running = false;
        }

        //dodanie najnowszej glowy na front
        rq.push_front(head);

        while (rq.size() > snake_size)
        {
            rq.pop_back();
        }


        // Renderowanie (rozowa kropka na ciemnoszarym tle)
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Ciemnoszary
        SDL_RenderClear(renderer);

        // Wyrysowanie ciala snake'a
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255); // Rozowy
        for_each(rq.begin(), rq.end(), [&](auto& snake_segment)
            {
                SDL_RenderFillRect(renderer, &snake_segment);
            });

        // Artefakty - wyrysowanie
        SDL_SetRenderDrawColor(renderer, 211, 175, 55, 0);
        for_each(artifacts.begin(), artifacts.end(), [&](auto &artif) {
            SDL_RenderFillRect(renderer, &artif);
            });

        SDL_RenderPresent(renderer);

        SDL_Delay(30);
    }

    // Zamykanie programu
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

