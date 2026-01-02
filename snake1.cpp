#define SDL_MAIN_HANDLED // pozwala na uzywanie zwyklej funkcji main
#include <SDL2/SDL.h> //wieksze szanse ze program zadziala
//#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <deque> // pozwala na wyciaganie i wkladanie rzeczy z frontu i tylu 
#include <ctime>
#include <string>

using namespace std;

//wymiary okna
int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

int ART_NUM = 65; //ilosc artefaktow
int score;


//struktura zawierajaca wszystko co potrzebuje napis
struct TextElement {
    SDL_Texture* texture = nullptr;
    SDL_Rect rect = { 0, 0, 0, 0 };

    // Funkcja do zwalniania pamięci, gdy napis nie jest już potrzebny
    void clean() {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }
};

TextElement createText(SDL_Renderer* renderer, TTF_Font* font, string message, SDL_Color color, int y) {
    TextElement temp;

    SDL_Surface* surf = TTF_RenderText_Blended(font, message.c_str(), color);
    temp.texture = SDL_CreateTextureFromSurface(renderer, surf);


    temp.rect.y = y;
    temp.rect.w = surf->w;
    temp.rect.h = surf->h;
    temp.rect.x = (WINDOW_WIDTH - surf->w) / 2;

    SDL_FreeSurface(surf);

    return temp;
}

// licznik punktów w rogu 
TextElement createHUDText(SDL_Renderer* renderer, TTF_Font* font, string message, SDL_Color color, int x, int y) {
    TextElement temp;
    if (!font) return temp;
    SDL_Surface* surf = TTF_RenderText_Blended(font, message.c_str(), color);
    if (!surf) return temp;
    temp.texture = SDL_CreateTextureFromSurface(renderer, surf);
    temp.rect = { x, y, surf->w, surf->h };
    SDL_FreeSurface(surf);
    return temp;
}

enum Direction
{
    DOWN = 0,
    LEFT = 1,
    RIGHT = 2,
    UP = 3,
};

enum GameState //stany gry
{
    MENU,
    LEADERBOARD,
    PLAYING,
    GAMEOVER
};

void artefakty(vector<SDL_Rect>& artifacts)
{
    artifacts.clear(); //  Czyszczenie przy restarcie gry
    for (int i = 0; i < ART_NUM; i++)
    {
        artifacts.push_back({
            rand() % (WINDOW_WIDTH - 20),  // Losowy X w granicach okna
            rand() % (WINDOW_HEIGHT - 20), // Losowy Y w granicach okna
            20, 20
            });
    }
}

int main(int argc, char* argv[])
{
    srand(time(NULL)); //ziarno losowosci
    // Inicjalizacja z obsługa błędu
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "Błąd inicjalizacji SDL: " << SDL_GetError() << endl;
        return 1;
    }
    TTF_Init();

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

    // czcionka 
    const char* fontPath = "C:\\Gry\\blocky.ttf"; // (Justyna - musiałam folder sobie utworzyc bo mi nie działało inaczej, wiec trzeba bedzie to zmienic) 
    TTF_Font* main_font = TTF_OpenFont(fontPath, 70);
    TTF_Font* main_font2 = TTF_OpenFont(fontPath, 30);
    TTF_Font* hud_font = TTF_OpenFont(fontPath, 30); // czcionka licznika
    SDL_Color MenuGameOver_Color = { 194, 197, 170, 255 }; //kolor gameovera
    SDL_Color HUD_Color = { 255, 255, 255, 255 }; //  kolor licznika podczas gry

    SDL_Event e;
    bool running = true;
    int dir = RIGHT; // Startowy ruch w prawo
    deque<SDL_Rect> rq; //cialo weza
    int snake_size = 3; //rozmiar snake'a
    vector<SDL_Rect> artifacts;
    SDL_Rect head{ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 20, 20 };
    score = 0;

    TextElement GOtxt = createText(renderer, main_font, "GAME OVER", MenuGameOver_Color, WINDOW_HEIGHT / 4);
    TextElement ScoreHUD; //  Element licznika punktow na zywo

    GameState current_state = PLAYING;

    artefakty(artifacts);

    while (running)
    {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running = false; }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) { running = false; } //zamykanie programu po wcisnieciu escape
                if (current_state == PLAYING)
                {
                    //  Blokada zawracania o 180 stopni
                    if (e.key.keysym.sym == SDLK_DOWN && dir != UP) { dir = DOWN; }
                    else if (e.key.keysym.sym == SDLK_UP && dir != DOWN) { dir = UP; }
                    else if (e.key.keysym.sym == SDLK_RIGHT && dir != LEFT) { dir = RIGHT; }
                    else if (e.key.keysym.sym == SDLK_LEFT && dir != RIGHT) { dir = LEFT; }
                }
                //  Restart gry po nacisnieciu SPACJI
                if (current_state == GAMEOVER && e.key.keysym.sym == SDLK_SPACE) {
                    current_state = PLAYING;
                    score = 0; snake_size = 3; rq.clear();
                    head = { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 20, 20 };
                    dir = RIGHT;
                    artefakty(artifacts);
                    ScoreHUD.clean();
                }

            }
        }

        if (current_state == PLAYING) {
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
                }
            };

            //Sam ze soba - zakoncz gre
            for_each(rq.begin(), rq.end(), [&](auto& snake_segment) {
                if (head.x == snake_segment.x && head.y == snake_segment.y)
                {
                    current_state = GAMEOVER;
                }
                });

            //Ze sciana - zakoncz gre
            if (head.x < 0 || head.x > WINDOW_WIDTH - 20 || head.y < 0 || head.y > WINDOW_HEIGHT - 20)
            {
                current_state = GAMEOVER;
            }

            //dodanie najnowszej glowy na front
            rq.push_front(head);

            while (rq.size() > (size_t)snake_size)
            {
                rq.pop_back();
            }


            // Renderowanie
            SDL_SetRenderDrawColor(renderer, 71, 44, 27, 255); //zmieniono kolor tla
            SDL_RenderClear(renderer);

            // Wyrysowanie ciala snake'a
            SDL_SetRenderDrawColor(renderer, 101, 109, 74, 255); // oraz snake'a
            for_each(rq.begin(), rq.end(), [&](auto& snake_segment)
                {
                    SDL_RenderFillRect(renderer, &snake_segment);
                });

            // Artefakty - wyrysowanie
            SDL_SetRenderDrawColor(renderer, 211, 175, 55, 0);
            for_each(artifacts.begin(), artifacts.end(), [&](auto& artif) {
                SDL_RenderFillRect(renderer, &artif);
                });

            //  Odswiezanie licznika punktow w rogu podczas gry
            ScoreHUD.clean();
            ScoreHUD = createHUDText(renderer, hud_font, "Wynik: " + to_string(score), HUD_Color, 20, 20);
            SDL_RenderCopy(renderer, ScoreHUD.texture, NULL, &ScoreHUD.rect);

        }
        else if (current_state == GAMEOVER)
        {
            string yourscore = "Twoj wynik to " + to_string(score) + ". (SPACJA - Restart)";
            TextElement ScoreTxt = createText(renderer, main_font2, yourscore, MenuGameOver_Color, 300);
            SDL_SetRenderDrawColor(renderer, 65, 72, 51, 255);
            SDL_RenderClear(renderer);

            SDL_RenderCopy(renderer, GOtxt.texture, NULL, &GOtxt.rect);
            SDL_RenderCopy(renderer, ScoreTxt.texture, NULL, &ScoreTxt.rect);

            ScoreTxt.clean(); // usuwa stary napis, aby nie zapychac pamieci
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(50);
    }

    // Zamykanie programu
    ScoreHUD.clean(); // restartowanie licznika
    GOtxt.clean(); // zwalnianie miejsca
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    TTF_CloseFont(main_font);
    TTF_CloseFont(main_font2);
    TTF_CloseFont(hud_font);
    TTF_Quit();

    return 0;
}
