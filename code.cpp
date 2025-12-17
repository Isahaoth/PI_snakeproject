#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include <deque>

using namespace std;

// --- STA£E GRY ---
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int TILE_SIZE = 20; // Rozmiar ka¿dego segmentu wê¿a

// --- DEKLARACJE GLOBALNE ---
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

// Struktura reprezentuj¹ca pozycjê segmentu wê¿a
struct Segment {
    int x;
    int y;
};

deque<Segment> snake;
int snake_length = 5; // Pocz¹tkowa d³ugoœæ wê¿a

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

// --- FUNKCJE STARTOWE I KOÑCZ¥CE ---

bool init()
{
    // 1. Inicjalizacja SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cerr << "SDL nie móg³ zostaæ zainicjalizowany! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    // 2. Tworzenie okna
    gWindow = SDL_CreateWindow(
        "Snake Game Prototype",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (gWindow == nullptr)
    {
        cerr << "Okno nie mog³o zostaæ utworzone! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    // 3. Tworzenie Renderer
    // Zmieniono flagê z SDL_RENDERER_PRESENTVSYNC na 0, aby nie wymuszaæ VSync
    gRenderer = SDL_CreateRenderer(
        gWindow,
        -1,
        SDL_RENDERER_ACCELERATED
    );
    if (gRenderer == nullptr)
    {
        cerr << "Renderer nie móg³ zostaæ utworzony! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    // Ustawienie pocz¹tkowej pozycji wê¿a
    snake.push_front({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 });

    return true;
}

void close()
{
    // 1. Zwalnianie Renderer
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

// --- LOGIKA GRY ---

void update_game()
{
    if (current_dir == NONE) return;

    // 1. Obliczenie nowej pozycji g³owy
    Segment new_head = snake.front();

    switch (current_dir)
    {
    case DOWN: new_head.y += TILE_SIZE; break;
    case UP: new_head.y -= TILE_SIZE; break;
    case LEFT: new_head.x -= TILE_SIZE; break;
    case RIGHT: new_head.x += TILE_SIZE; break;
    default: break;
    }

    // 2. Dodanie nowej g³owy na przód
    snake.push_front(new_head);

    // 3. Utrzymanie d³ugoœci (usuwamy ogon, jeœli w¹¿ jest za d³ugi)
    while (snake.size() > snake_length)
    {
        snake.pop_back();
    }

    SDL_Delay(30);
}

void render_game()
{
    // 1. Czyszczenie ekranu (czarny kolor t³a)
    SDL_SetRenderDrawColor(gRenderer, 106, 153, 78, 255);
    SDL_RenderClear(gRenderer);

    // 2. Ustawienie koloru wê¿a (np. zielony)
    SDL_SetRenderDrawColor(gRenderer, 167, 201, 87, 0);

    // 3. Rysowanie ka¿dego segmentu
    for (const auto& segment : snake)
    {
        SDL_Rect segment_rect = { segment.x, segment.y, TILE_SIZE, TILE_SIZE };
        SDL_RenderFillRect(gRenderer, &segment_rect);
    }

    // 4. Prezentacja (wyœwietlenie narysowanej klatki)
    SDL_RenderPresent(gRenderer);
}


int main()
{
    // Sprawdzenie, czy inicjalizacja siê powiod³a
    if (!init())
    {
        cerr << "Program nie móg³ wystartowaæ. Wychodzenie." << endl;
        close();
        return 1;
    }

    SDL_Event e;
    bool quit = false;

    // --- PÊTLA GRY (Game Loop) ---
    while (!quit)
    {
        // 1. PRZETWARZANIE WEJŒCIA (Obs³uga zdarzeñ)
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
                    // Zmiana kierunku (zabezpieczenie przed cofaniem siê w miejscu)
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

    // Zwolnienie zasobów i wyjœcie
    close();
    return 0;
}