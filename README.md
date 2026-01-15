# INDIANA SNAKE: snake, który gromadzi artefakty poukrywane w roznorakich kryptach
## Jak uruchomić grę?
**Jeśli posiadasz wszystkie biblioteki (SDL2, SDL_Mixer, SDL_TTF, SDL_Image):**
1. Pobierz repozytorium na swój komputer;
2. Uruchom projekt - IndianaSnake_Official;
3. Upewnij się, że umieściłeś folder z assetami w folderze projektu.
4. Uruchom projekt poprzez wciśnięcie F5.
   
**Jeśli nie posiadasz wymaganych bibliotek:**
1. Pobierz program Git;
2. Otwórz wiersz poleceń. W głównym katalogu dysku uruchom komendę: `git clone https://github.com/microsoft/vcpkg`;
3. Wejdź do folderu i uruchom `.\bootstrap-vcpkg.bat`;
4. Wpisz `.\vcpkg integrate install`, aby nie musieć ustawiać nic w Visual Studio - program sam zauważy doinstalowane biblioteki;
5. Wpisz `.\vcpkg install sdl2 sdl2-image sdl2-mixer sdl2-ttf --triplet=x64-windows`;
6. Odpal projekt, umieść assety i kliknij F5.

# Twórcy projektu
## Justyna Sielska
- Poprawki w kodzie;
- Mechanika artefaktów;
- Podstawowa mechanika Game Over (wylaczanie gry oraz restart spacja) i nazwanie stanów gry;
- Wyswietlanie wyniku w trakcie gry;
- Brak Game Over przy cofaniu.
## Marta Lipień
- Podstawowe ciało gry;
- Czuwanie nad fabułą i assetami gry;
- Przygotowanie kolorów snake'a i modulu ich zmiany wraz z wyodrebnieniem koloru glowy.
## Aleksandra Woźniak
- Obsluga błędów;
- Udźwiękowienie programu;
- Praca nad Leaderboardem - wyniki, zapis;
- Stworzenie ekranu Game Over z podanymi wynikami;
- Opieka techniczna nad bibliotekami.
