#define SDL_MAIN_HANDLED // pozwala na uzywanie zwyklej funkcji main
#include <SDL2/SDL.h> //wieksze szanse ze program zadziala
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <deque> // pozwala na wyciaganie i wkladanie rzeczy z frontu i tylu 
#include <ctime>
#include <string>
#include <fstream>

using namespace std;

//wymiary okna
int WINDOW_WIDTH = 1920;
int WINDOW_HEIGHT = 1080;

int ART_NUM = 65; //ilosc artefaktow
int score;

struct Player
{
    string username;
    int HighScore;
};

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
    
    SDL_Surface* surf = TTF_RenderText_Blended(font, message.c_str(), color);
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
    GAMEOVER,
    SAVESCORE
};

int textRand = (rand() % 5);

void artefakty(vector<SDL_Rect>& artifacts)
{
    artifacts.clear();

        artifacts.push_back({
            rand() % (WINDOW_WIDTH - 32),  // Losowy X w granicach okna
            rand() % (WINDOW_HEIGHT - 32), // Losowy Y w granicach okna
            48, 48
            });
        textRand = (rand() % 5) + 1;
    

}

//FUNKCJE DO LEADEROBARDU
vector <Player> loadScores()
{
    vector <Player> loadedScores;

    fstream scoreboard;
    scoreboard.open("scores.txt", ios::in);

    string tempuser;
    int tempscore;

    while (scoreboard >> tempuser >> tempscore)
    {
        loadedScores.push_back({ tempuser, tempscore });
    }
    scoreboard.close();

    return loadedScores;
}

void saveScores(vector<Player>& scores)
{
    fstream scoreboard("scores.txt", ios::out);

    for (const auto& user : scores)
    {
        scoreboard << user.username << " " << user.HighScore << "\n";
    }
    scoreboard.close();
}

int main()
{
    srand(time(NULL)); //ziarno losowosci

    // Inicjalizacja z obsługa błędu
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "Błąd inicjalizacji SDL: " << SDL_GetError() << endl;
        return 1;
    }
    TTF_Init();

    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

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

    //Inicjalizacja dzwieku
    SDL_Init(SDL_INIT_AUDIO);
    int initted = Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    // Wymuszenie Akceleracji (SDL_RENDERER_ACCELERATED) i VSync (SDL_RENDERER_PRESENTVSYNC)
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {

        cerr << "Błąd tworzenia renderera. Sprawdź sterowniki graficzne: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    //OBSLUGA NAPISOW I CZCIONEK ORAZ KOLOROW

    const char* fontPath = "blocky.ttf"; 
    TTF_Font* main_font = TTF_OpenFont(fontPath, 120);
    TTF_Font* main_font2 = TTF_OpenFont(fontPath, 60);
    TTF_Font* main_font3 = TTF_OpenFont(fontPath, 120);
    TTF_Font* hud_font = TTF_OpenFont(fontPath, 30); // czcionka licznika
    SDL_Color Menu_Color = { 28, 33, 31, 255 };
    SDL_Color Menu_Color2 = { 58, 68, 68, 255 };
    SDL_Color Menu_Color3 = { 97, 124, 115, 255 };
    SDL_Color HUD_Color = { 255, 255, 255, 255 }; //  kolor licznika podczas gry

    SDL_Event e;
    bool running = true;
    int dir = RIGHT; // Startowy ruch w prawo
    deque<SDL_Rect> rq; //cialo weza
    int snake_size = 3; //rozmiar snake'a
    vector<SDL_Rect> artifacts;
    SDL_Rect head{ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 32, 32 };
    score = 0;

    //NAPISY
    TextElement GOtxt = createText(renderer, main_font, "GAME OVER", Menu_Color, WINDOW_HEIGHT / 4);
    TextElement ScoreHUD; //  Element licznika punktow na zywo
    TextElement GameName = createText(renderer, main_font, "INDIANA SNAKE", Menu_Color,  WINDOW_HEIGHT / 4);
    TextElement GameName2 = createHUDText(renderer, main_font3, "INDIANA SNAKE", Menu_Color3, WINDOW_WIDTH / 8 - 10, WINDOW_HEIGHT / 4-5);
    TextElement Option1 = createText(renderer, main_font2, "Tryb Klasyczny", Menu_Color, WINDOW_HEIGHT / 4 + 250);
    TextElement Option2 = createText(renderer, main_font2, "Leaderboard", Menu_Color, WINDOW_HEIGHT / 4 + 375);
    TextElement Option3 = createText(renderer, main_font2, "Wyjscie", Menu_Color, WINDOW_HEIGHT / 4 + 500);
    TextElement GO_Option1 = createText(renderer, main_font2, "Rozpocznij od nowa", Menu_Color, WINDOW_HEIGHT / 4 + 325);
    TextElement GO_Option2 = createText(renderer, main_font2, "Zapisz swoj wynik", Menu_Color, WINDOW_HEIGHT / 4 + 425);
    TextElement GO_Option3 = createText(renderer, main_font2, "Powroc do menu", Menu_Color, WINDOW_HEIGHT / 4 + 525);
    TextElement ScoreInfo = createText(renderer, main_font2, "Wpisz swoja nazwe:", Menu_Color, WINDOW_HEIGHT / 4);
    TextElement UsernameTxt;
   

    //HIGHLIGHTED TEXT
    TextElement Option1H = createText(renderer, main_font2, "Tryb Klasyczny", HUD_Color, WINDOW_HEIGHT / 4 + 250);
    TextElement Option2H = createText(renderer, main_font2, "Leaderboard",HUD_Color, WINDOW_HEIGHT / 4 + 375);
    TextElement Option3H = createText(renderer, main_font2, "Wyjscie", HUD_Color, WINDOW_HEIGHT / 4 + 500);
    TextElement GO_Option1H = createText(renderer, main_font2, "Rozpocznij od nowa", HUD_Color, WINDOW_HEIGHT / 4 + 325);
    TextElement GO_Option2H = createText(renderer, main_font2, "Zapisz swoj wynik", HUD_Color, WINDOW_HEIGHT / 4 + 425);
    TextElement GO_Option3H = createText(renderer, main_font2, "Powroc do menu", HUD_Color, WINDOW_HEIGHT / 4 + 525);

    //MUZYKA
    Mix_Music* gamebgm = Mix_LoadMUS("snake_main_music.mp3");
    Mix_Music* menubgm = Mix_LoadMUS("snake_intro_theme.mp3");
    Mix_Chunk* eatSound = Mix_LoadWAV("bite.ogg");

    //GRAFIKA
    SDL_Texture* artifactTexture1 = IMG_LoadTexture(renderer, "graphic_assets/artf1.png");
    SDL_Texture* artifactTexture2 = IMG_LoadTexture(renderer, "graphic_assets/artf2.png");
    SDL_Texture* artifactTexture3 = IMG_LoadTexture(renderer, "graphic_assets/artf3.png");
    SDL_Texture* artifactTexture4 = IMG_LoadTexture(renderer, "graphic_assets/artf4.png");
    SDL_Texture* artifactTexture5 = IMG_LoadTexture(renderer, "graphic_assets/artf5.png");
    SDL_Texture* menuBackground = IMG_LoadTexture(renderer, "graphic_assets/bgmenu.png");

    SDL_Rect bgRect;
    bgRect.x = 0;             
    bgRect.y = 0;            
    bgRect.w = 1920;          
    bgRect.h = 1080;

    //OBSLUGA WPISU DO LEADERBOARDA
    string usernameinput = "";
    bool typing = false;

    GameState current_state = MENU;

    artefakty(artifacts);
    
    //OBSLUGA MENU INTERAKTYWNYCH
    int chosenIndex = 0;
    const int OPTIONS_NUM = 3; 

    int GameOverIndex = 0;
    const int GO_OPTIONS = 3;
    Mix_PlayMusic(menubgm, -1);

   

    while (running)
    {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running = false; }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) { running = false; } //zamykanie programu po wcisnieciu escape
            if (current_state == SAVESCORE)
            {
                if (typing == false)
                {
                    SDL_StartTextInput(); //aktywacja pisania
                    usernameinput = "";
                    typing = true;
                }
                else {
                    if (e.type == SDL_TEXTINPUT) //przyjmuje tekst i dodaje do inputa
                    {
                        usernameinput += e.text.text;
                    }
                    else if (e.type == SDL_KEYDOWN)
                    {
                        if (e.key.keysym.sym == SDLK_BACKSPACE && usernameinput.length() > 0) //dzialajacy klawisz usuwania
                        {
                            usernameinput.pop_back();
                        }
                        else if (e.key.keysym.sym == SDLK_RETURN) //zapis gracza do pliku i przejscie do leaderboarda
                        {
                            vector<Player> scores = loadScores();
                            scores.push_back({ usernameinput, score });

                            sort(scores.begin(), scores.end(), [](const Player& a, const Player& b) {
                                return a.HighScore > b.HighScore;
                                });

                            if (scores.size() > 10)
                            {
                                scores.resize(10);
                            }
                            
                            saveScores(scores);
                            SDL_StopTextInput();
                            typing = false;
                            current_state = LEADERBOARD;
                        }
                    }
                }
            }
            else if (e.type == SDL_KEYDOWN) {
                if (current_state == MENU)
                {
                    
                    if(e.key.keysym.sym == SDLK_UP) 
                    {
                        chosenIndex = (chosenIndex - 1 + OPTIONS_NUM) % OPTIONS_NUM;
                    }
                    else if (e.key.keysym.sym == SDLK_DOWN)
                    {
                        chosenIndex = (chosenIndex + 1) % OPTIONS_NUM;
                    }
                    else if (e.key.keysym.sym == SDLK_RETURN)
                    {
                        switch(chosenIndex)
                            {
                                case 0:
                                    current_state = PLAYING;              
                                    Mix_HaltMusic();
                                    Mix_PlayMusic(gamebgm, -1);

                                    score = 0;
                                    snake_size = 3;
                                    rq.clear();

                                    head = { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 32, 32 };
                                    dir = RIGHT;

                                    artefakty(artifacts);
                                    ScoreHUD.clean();

                                    break;
                                case 1:
                                    current_state = LEADERBOARD;
                                    break;
                                case 2:
                                    running = false;

                            }
                    }
                }
                else if (current_state == PLAYING)
                {
                    
                    //  Blokada zawracania o 180 stopni
                    if (e.key.keysym.sym == SDLK_DOWN && dir != UP) { dir = DOWN; }
                    else if (e.key.keysym.sym == SDLK_UP && dir != DOWN) { dir = UP; }
                    else if (e.key.keysym.sym == SDLK_RIGHT && dir != LEFT) { dir = RIGHT; }
                    else if (e.key.keysym.sym == SDLK_LEFT && dir != RIGHT) { dir = LEFT; }
                
                }
                else if (current_state == LEADERBOARD)
                {
                    if (e.key.keysym.sym == SDLK_m)
                    {
                        current_state = MENU;
                    }
                }
                else if (current_state == GAMEOVER) {
                    if (e.key.keysym.sym == SDLK_UP)
                    {
                        GameOverIndex = (GameOverIndex - 1 + GO_OPTIONS) % GO_OPTIONS;
                    }
                    else if (e.key.keysym.sym == SDLK_DOWN)
                    {
                        GameOverIndex = (GameOverIndex + 1) % GO_OPTIONS;
                    }
                    else if (e.key.keysym.sym == SDLK_RETURN)
                    {
                        
                        switch (GameOverIndex)
                        {
                        case 0:
                            Mix_HaltMusic();
                            Mix_PlayMusic(gamebgm, -1);
                            current_state = PLAYING;
                            score = 0; snake_size = 3; rq.clear();
                            head = { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 32, 32 };
                            dir = RIGHT;
                            artefakty(artifacts);
                            ScoreHUD.clean();
                            break;
                        case 1:
                            current_state = SAVESCORE;
                            break;
                        case 2:
                            current_state = MENU;

                        }
                    }
                    
                }
               


            }
        }

        if (current_state == PLAYING) {

            switch (dir)
            {
            case DOWN:  head.y += 32; break;
            case UP:    head.y -= 32; break;
            case LEFT:  head.x -= 32; break;
            case RIGHT: head.x += 32; break;
            }

            //Kolizje
            //Artefakty
            for (auto& artif : artifacts) {
                if (SDL_HasIntersection(&head, &artif)) //jesli sie pokryje z artefaktem
                {
                    Mix_PlayChannel(-1, eatSound, 0);
                    snake_size += 2; //jak je artefakt to rosnie o 2
                    //losujemy nowa lokalizacje artefaktu
                    artif.x = rand() % (WINDOW_WIDTH - 48);
                    artif.y = rand() % (WINDOW_HEIGHT - 48);
                    score++;
                    textRand = (rand() % 5) + 1;
                }
            };

            //Sam ze soba - zakoncz gre
            for_each(rq.begin(), rq.end(), [&](auto& snake_segment) {
                if (head.x == snake_segment.x && head.y == snake_segment.y)
                {
                    Mix_HaltMusic();
                    Mix_PlayMusic(menubgm, -1);
                    current_state = GAMEOVER;
                }
                });

            //Ze sciana - zakoncz gre
            if (head.x < 0 || head.x > WINDOW_WIDTH - 32 || head.y < 0 || head.y > WINDOW_HEIGHT - 32)
            {
                Mix_HaltMusic();
                Mix_PlayMusic(menubgm, -1);
                current_state = GAMEOVER;
            }

            //dodanie najnowszej glowy na front
            rq.push_front(head);

            while (rq.size() > (size_t)snake_size)
            {
                rq.pop_back();
            }

            // Renderowanie
            SDL_SetRenderDrawColor(renderer, 47, 55, 52, 255); //zmieniono kolor tla
            SDL_RenderClear(renderer);

            // Wyrysowanie ciala snake'a
            SDL_SetRenderDrawColor(renderer, 62, 79, 73, 255); // oraz snake'a
            for_each(rq.begin(), rq.end(), [&](auto& snake_segment)
                {
                    SDL_RenderFillRect(renderer, &snake_segment);
                });

            // Artefakty - wyrysowanie
            SDL_SetRenderDrawColor(renderer, 211, 175, 55, 0);
            for_each(artifacts.begin(), artifacts.end(), [&](auto& artif) {          
                switch(textRand)
                {
                case 1:
                    SDL_RenderCopy(renderer, artifactTexture1, NULL, &artif);
                    break;
                case 2:
                    SDL_RenderCopy(renderer, artifactTexture2, NULL, &artif);
                    break;
                case 3:
                    SDL_RenderCopy(renderer, artifactTexture3, NULL, &artif);
                    break;
                case 4:
                    SDL_RenderCopy(renderer, artifactTexture4, NULL, &artif);
                    break;
                case 5:
                    SDL_RenderCopy(renderer, artifactTexture5, NULL, &artif);
                    break;
                }
                });

            //  Odswiezanie licznika punktow w rogu podczas gry
            ScoreHUD.clean();
            ScoreHUD = createHUDText(renderer, hud_font, "Wynik: " + to_string(score), HUD_Color, 20, 20);
            SDL_RenderCopy(renderer, ScoreHUD.texture, NULL, &ScoreHUD.rect);


        }
        else if (current_state == GAMEOVER)
        {
            string yourscore = "Twoj wynik to " + to_string(score) + ". ";
            TextElement ScoreTxt = createText(renderer, main_font2, yourscore, HUD_Color, WINDOW_HEIGHT/4 + 200);
            SDL_SetRenderDrawColor(renderer, 117, 138, 136, 255);
            SDL_RenderClear(renderer);

            SDL_RenderCopy(renderer, GOtxt.texture, NULL, &GOtxt.rect);
            SDL_RenderCopy(renderer, ScoreTxt.texture, NULL, &ScoreTxt.rect);

            if (GameOverIndex == 0) SDL_RenderCopy(renderer, GO_Option1H.texture, NULL, &GO_Option1H.rect);
            else SDL_RenderCopy(renderer, GO_Option1.texture, NULL, &GO_Option1.rect);

            if (GameOverIndex == 1) SDL_RenderCopy(renderer, GO_Option2H.texture, NULL, &GO_Option2H.rect);
            else SDL_RenderCopy(renderer, GO_Option2.texture, NULL, &GO_Option2.rect);

            if (GameOverIndex == 2) SDL_RenderCopy(renderer, GO_Option3H.texture, NULL, &GO_Option3H.rect);
            else SDL_RenderCopy(renderer, GO_Option3.texture, NULL, &GO_Option3.rect);
            

            ScoreTxt.clean(); // usuwa stary napis, aby nie zapychac pamieci
        }
        else if (current_state == MENU)
        {
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, menuBackground, NULL, &bgRect);
                SDL_RenderCopy(renderer, GameName.texture, NULL, &GameName.rect);
                SDL_RenderCopy(renderer, GameName2.texture, NULL, &GameName2.rect);

                if (chosenIndex == 0) SDL_RenderCopy(renderer, Option1H.texture, NULL, &Option1H.rect);
                else SDL_RenderCopy(renderer, Option1.texture, NULL, &Option1.rect);
                
                if (chosenIndex == 1) SDL_RenderCopy(renderer, Option2H.texture, NULL, &Option2H.rect);
                else SDL_RenderCopy(renderer, Option2.texture, NULL, &Option2.rect);
                    
                if (chosenIndex == 2) SDL_RenderCopy(renderer, Option3H.texture, NULL, &Option3H.rect);
                else SDL_RenderCopy(renderer, Option3.texture, NULL, &Option3.rect);
         
        }
        else if (current_state == LEADERBOARD)
        {
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 117, 138, 136, 255);

            TextElement top10 = createText(renderer, main_font3, "TOP 10 graczy" ,Menu_Color, 30);
            SDL_RenderCopy(renderer, top10.texture, NULL, &top10.rect);
            top10.clean();

            vector<Player> displayScores = loadScores();

            int pos = 200;
            int i=1;
            for (const auto& user:displayScores){
                string scoreline = to_string(i) + ". " + user.username + " - " + to_string(user.HighScore);
                TextElement scoreLine = createHUDText(renderer, main_font2, scoreline, Menu_Color2,150, pos);

                SDL_RenderCopy(renderer, scoreLine.texture, NULL, &scoreLine.rect);
                scoreLine.clean();
                pos += 75;
                i++;
            }
            TextElement backtomenu = createText(renderer, main_font2, "Aby powrocic do menu uzyj 'M'", Menu_Color, WINDOW_HEIGHT - 100);
            SDL_RenderCopy(renderer, backtomenu.texture, NULL, &backtomenu.rect);
            backtomenu.clean();



        }
        else if (current_state == SAVESCORE)
        {
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 117, 138, 136, 255);

            string displayString = usernameinput;
            if (displayString.empty()) displayString = " ";
            
            UsernameTxt.clean();
            UsernameTxt = createText(renderer, main_font2, displayString, Menu_Color, WINDOW_HEIGHT / 4 + 200);
            SDL_RenderCopy(renderer, UsernameTxt.texture, NULL, &UsernameTxt.rect);
            
            SDL_RenderCopy(renderer, ScoreInfo.texture, NULL, &ScoreInfo.rect);
            
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(40);
    }

    // Zamykanie programu
    ScoreHUD.clean(); // restartowanie licznika
    GOtxt.clean(); // zwalnianie miejsca
   

    Mix_FreeMusic(gamebgm);
    Mix_FreeMusic(menubgm);
    Mix_CloseAudio();

    TTF_CloseFont(main_font);
    TTF_CloseFont(main_font2);
    TTF_CloseFont(hud_font);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
