Imię i nazwisko
Justyna Sielska
Aleksandra Woźniak
Marta Lipień	Kierunek
Inżynieria Obliczeniowa	Rok studiów i grupa
I rok, Grupa 2
Data zajęć
15.01.2026r.	Numer i temat sprawozdania
12. Projekt – Indiana Snake

1.Cel i opis projektu
Cel projektu:  Głównym założeniem było stworzenie Indiana Snake – autorskiej wersji klasycznego "Węża" w przygodowym klimacie. Chcieliśmy sprawdzić nasze umiejętności w C++ i zobaczyć, jak od podstaw zbudować silnik gry przy użyciu biblioteki SDL2. Skupiliśmy się na tym, aby gra działała płynnie (dzięki synchronizacji klatek), poprawnie odczytywała ruchy gracza i sprawnie zarządzała tekstami wyświetlanymi na ekranie.
Opis gry i mechaniki:  W naszej grze nie sterujesz zwykłym wężem, ale poszukiwaczem przygód, który przemierza zapomniane ruiny oraz krypty.
•	Zbieranie skarbów: Zamiast standardowych punktów, gracz poluje na złote artefakty (złote kwadraty na mapie). Każdy zebrany przedmiot dodaje 1 punkt do wyniku, ale sprawia też, że Indiana Snake wydłuża się o 2 segmenty. Przez to z każdym ruchem robi się coraz ciaśniej i trudniej.
•	Zasady przetrwania: Gra jest bezlitosna – misja kończy się w momencie uderzenia w ścianę lub gdy wąż zaplącze się we własny ogon. Wymaga to od gracza planowania trasy z wyprzedzeniem.
•	Klimat przygody: Całość utrzymaliśmy w stylistyce retro. Wykorzystaliśmy kolory kojarzące się z dżunglą i ekspedycją (brązy, zielenie, złoto). Po każdej porażce pojawia się ekran „GAME OVER” z Twoim ostatecznym wynikiem, co motywuje do kolejnego podejścia.
Zastasowanie technik programistycznych: Do stworzenia ciała węża użyliśmy kolejki std::deque, ponieważ pozwala ona bardzo łatwo dodawać nową głowę i usuwać stary ogon przy każdym ruchu. Logika gry opiera się na przełączniku trybów, dzięki któremu program w odpowiednim momencie decyduje, czy ma pokazywać aktywną rozgrywkę, czy zatrzymać wszystko i wyświetlić ekran z napisem o przegranej. Zadbaliśmy też o porządek w pamięci komputera – stworzyliśmy funkcję clean(), która usuwa stare napisy, żeby program nie zwalniał. Kolizje ze skarbami sprawdza gotowa funkcja SDL_HasIntersection, która wykrywa moment, w którym dwa obiekty na siebie nachodzą.
Wykorzystane biblioteki:   Główną biblioteką jest SDL2, która odpowiada za okno gry i sterowanie. Do wyświetlania tekstów użyliśmy dodatku SDL2_ttf. Korzystaliśmy też ze standardowych narzędzi C++, takich jak vector do trzymania listy skarbów oraz string do tworzenia napisów z wynikami.


2.  Kod:








3.  Wnioski:
Praca nad projektem pokazała nam, że najważniejsza jest poprawna konfiguracja bibliotek oraz pilnowanie, aby wszystkie pliki, takie jak czcionki czy pliki DLL, znajdowały się w folderze z grą. Nauczyliśmy się pisać kod tak, aby był odporny na błędy braku danych, co zapobiegło nagłemu wyłączaniu się programu. Zrozumieliśmy też, jak ważne jest zwalnianie pamięci po nieużywanych obrazkach i tekstach, żeby gra działała płynnie. Okazało się, że nawet prosta gra staje się dużo fajniejsza, jeśli nada się jej ciekawy motyw graficzny i spójną kolorystykę.


