#define SDL_MAIN_HANDLED //ten define pozwala nam wywolywac normalna petlę main, bez tego wywala blędy i ogolnie 

#include <SDL2/SDL.h> //biblioteka graficzna
#include <iostream>
#include <string>
#include <vector>
#include <deque> //podobne do listy, ale pozwala na wkladanie i usuwanie danych z obu koncow, so far uzywam tego - Alex

using namespace std;

//STALE GRY
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int TILE_SIZE = 20; // Rozmiar kazdego "kawalka" segmentu weza

//DEKLARACJE GLOBALNE 
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

// Struktura reprezentujaca pozycje segmentu weza  - UWAGA: warto objasnic w sprawozdaniu czym sa structy
struct Segment {
    int x;
    int y;
};

deque<Segment> snake; // to jest lista zawierajaca wspolrzedne kazdego kawalka weza
int snake_length = 5; // Poczatkowa dlugosc snake'a

// Kierunki
enum Direction
{
    NONE = 0,
    DOWN,
    UP,
    LEFT,
    RIGHT
};

Direction current_dir = NONE; // Aktualny kierunek ruchu

//FUNKCJE STARTOWE I KONCZACE

//boolowska funkcja inicjalizacyjna
bool init() 
{
    // 1. Inicjalizacja SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cerr << "SDL nie mógl zostaæ zainicjalizowany! SDL_Error: " << SDL_GetError() << endl; //cerr wywala bledy na konsole bledow a nie jak cout
        return false;
    }

    // 2. Tworzenie okna
    gWindow = SDL_CreateWindow(
        "Snake Game Prototype", //tutaj nazwa okienka 
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, //wymiary
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (gWindow == nullptr)
    {
        cerr << "Okno nie moglo zostac utworzone! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    // 3. Tworzenie Renderer
    // Zmieniono flage z SDL_RENDERER_PRESENTVSYNC na 0, aby nie wymuszac VSync
    gRenderer = SDL_CreateRenderer(
        gWindow, //gdzie rysuje
        -1, //jaki sterownik
        SDL_RENDERER_ACCELERATED //uzywaj karty graficznej
    );
    if (gRenderer == nullptr)
    {
        cerr << "Renderer nie mógl zostac utworzony! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    // Ustawienie poczatkowej pozycji weza (srodek ekranu)
    snake.push_front({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 });

    return true;
}

void close()
{
    // 1. Zwalnianie Renderera
    if (gRenderer != nullptr)
    {
        SDL_DestroyRenderer(gRenderer);
        gRenderer = nullptr;
    }

    // 2. Zwalnianie okna
    if (gWindow != nullptr)
    {
        SDL_DestroyWindow(gWindow);
        gWindow = nullptr;
    }

    // 3. Zamykanie podsystemów SDL
    SDL_Quit();
}

//LOGIKA GRY

void update_game()
{
    if (current_dir == NONE) return;

    // Obliczenie nowej pozycji glowy
    Segment new_head = snake.front();

    switch (current_dir)
    {
    case DOWN: new_head.y += TILE_SIZE; break;
    case UP: new_head.y -= TILE_SIZE; break;
    case LEFT: new_head.x -= TILE_SIZE; break;
    case RIGHT: new_head.x += TILE_SIZE; break;
    default: break;
    }

    // Dodanie nowej glowy na przód
    snake.push_front(new_head);

    //Utrzymanie dlugosci (usuwamy ogon, jezeli waz jest za dlugi)
    while (snake.size() > snake_length)
    {
        snake.pop_back();
    }

    SDL_Delay(30); //spowolnienie snake'a - upewnia sie ze w przypadku szybszego komputera nie zwieje z ekranu
}

void render_game()
{
    // 1. Czyszczenie ekranu (zielony (obecnie?) kolor tla)
    SDL_SetRenderDrawColor(gRenderer, 106, 153, 78, 255);
    SDL_RenderClear(gRenderer);

    // 2. Ustawienie koloru weza
    SDL_SetRenderDrawColor(gRenderer, 167, 201, 87, 0);

    // 3. Rysowanie kazdego segmentu
    for (const auto& segment : snake)
    {
        SDL_Rect segment_rect = { segment.x, segment.y, TILE_SIZE, TILE_SIZE };
        SDL_RenderFillRect(gRenderer, &segment_rect);
    }

    // 4. Prezentacja (wyswietlenie narysowanej klatki)
    SDL_RenderPresent(gRenderer);
}


int main()
{
    // Sprawdzenie, czy inicjalizacja sie powiodla
    if (!init())
    {
        cerr << "Program nie mógl wystartowac. Wychodzenie." << endl;
        close();
        return 1;
    }

    SDL_Event e;
    bool quit = false;

    //petla gry
    while (!quit)
    {
        // OBSLUGA ZDARZEN
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }

            if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                    // Zmiana kierunku (zabezpieczenie przed cofaniem sie w miejscu)
                case SDLK_DOWN: if (current_dir != UP) current_dir = DOWN; break;
                case SDLK_UP: if (current_dir != DOWN) current_dir = UP; break;
                case SDLK_LEFT: if (current_dir != RIGHT) current_dir = LEFT; break;
                case SDLK_RIGHT: if (current_dir != LEFT) current_dir = RIGHT; break;
                }
            }
        }

        // 2. AKTUALIZACJA LOGIKI GRY
        update_game();

        // 3. RENDEROWANIE
        render_game();

    }

    // Zwolnienie zasobów i wyjscie
    close();
    return 0;

}
