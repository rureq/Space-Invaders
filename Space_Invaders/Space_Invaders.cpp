#define _WIN32_WINNT 0x0500
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_SPACE 32

#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <dos.h>
#include <chrono>
#include <conio.h>
#include <vector>
#include <functional>
#include <cstdlib>

using namespace std;
using namespace chrono;

void gotoxy(int x, int y)
{
    COORD p = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), p);

}

class Character{
 public:
    Character(int a, int b, int c) : PosX(a), PosY(b), HP(c) {}
    ~Character() {}
    virtual void display() = 0;
    virtual void move() = 0;

    void operator-- (int) {
        HP-=1;
    }

    double PosX;
    double PosY;
    int HP;
};

class Player : public Character{
public:
    Player(int a, int b,int c) : Character(a, b, c), points(0) {}
    ~Player() {}
    virtual void move() {}
    int points;

    virtual void display() {
        gotoxy(PosX, PosY);
        cout << "^";
        
    }
};

class Enemy : public Character{
public:
    Enemy(int a, int b, int c) : Character(a,b,c) {}
    ~Enemy() {}
    virtual void move() {}
     
    virtual void display() {
        if (HP == 1) {
            gotoxy(PosX, PosY);
            cout << "W";
        }
    }


};

class Bullet : public Character {
public:
    Bullet(int a, int b, int c, int d) : Character(a, b, c), dir(d) {}
    ~Bullet() {}
    virtual void display() {
        gotoxy(PosX, PosY);
            cout << "o";
    }
    virtual void move() {
        PosY += dir;
    }

    int dir;


};


class GameHandler{

public:
    GameHandler() :state(1), EnemyCountX(8), EnemyCountY(4), FirstPosX(4), FirstPosY(2), move(10), dir(1), space_trig(0), space_trig_prev(0), crt_bullet(0), random_select(0), key(0), random(rand()), max_enemy_pos(0){}
    ~GameHandler() {}
    bool get_state() {
        return state;
    }
    void set_state(BOOL a) {
        state = a;
    }
    void game_over() {
    
    
    }
    


    //zmienne
    int EnemyCountX; //liczba wrogów na kierunku poziomym
    int EnemyCountY; //liczba wrogów na kierunku pionowym
    int EnemyCount; //liczba wszystkich wrogów
    int FirstPosX; //pozycja pierwszego wroga na kierunku poziomym (lewy górny róg)
    int FirstPosY; //pozycja pierwszego wroga na kierunku pionowym (lewy górny róg)
    int move; //liczba ruchow na boki zanim nastapi przesuniecie w dol
    int dir; //aktualny kierunek poruszania się wrógów 1-prawo, -1 lewo
    bool space_trig; //przechowuje informacje o tym czy w aktualnej petli programu zostala wcisnieta spacja
    bool space_trig_prev;//przechowuje informacje czy w poprzedniej petli programu zostala wcisnieta spacja
    bool crt_bullet; //przechowuje informacje czy stworzyc pocisk od gracza
    int random_select;//przechowuje informację o tym które przeciwnika wybrać do wystrzelenia pocisku
    int key; //przechowuje wcisniety klawisz
    int random; //przechowuje aktualnie wylosowaną liczbę
    int max_enemy_pos; //przechowuje informacje o najnizej znajdujacym sie przeciwniku


private:
    bool state;
   
};

void redraw(vector <Character*> &A, Player &B, vector<Character*> &C) {
    system("cls");
    
    auto it = A.begin();
    for (it; it != A.end(); it++) {
        (*it)->display();
    }
    
    B.display();
    
    if (C.size() != 0) {
        for (auto it2 = C.begin(); it2 != C.end(); it2++) {
            gotoxy((*it2)->PosX, (*it2)->PosY);
            (*it2)->display();
        }
    }

    gotoxy(0, 0);
    cout <<"HP:"<< B.HP;
    gotoxy(44, 0);
    cout <<"Points:"<< B.points;
}



int main()
{
    //inicjalizacja okna gry
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, 800, 600, TRUE);

    //ekran powitalny
    gotoxy(30, 20);
    cout<<"Welcome to Space Invaders"<<endl;
    system("pause");

    //inicjalizacja zmienych gry 
    GameHandler Game; //inicjalizacja klasy z danymi gry
       
    //inicjalizacja gracza
    Player Hero(47,28,4); //pozycja X:47, Y: 28, HP 4

    //inicjalizacja wektora pocisków
    vector<Character*> Bullets;
    auto it_Bullets = Bullets.begin(); //iterator dla wektora pocisków

    //inicjalizacja przeciwnikow
    vector<Character*> Invaders;
    for (int i = 0; i < Game.EnemyCountX; i++) {
        for (int j = 0; j < Game.EnemyCountY; j++) {
            Invaders.push_back(new Enemy(Game.FirstPosX + 7*i,Game.FirstPosY + 4*j,1));
        }
    }

    auto it = Invaders.begin(); //iterator dla wektora przeciwników

    //inicjalizacja pomiaru czasu
    auto tnow = high_resolution_clock::now(); //przechowuje aktualny czas - uaktualniana w kazdej petli

    auto t1000 = high_resolution_clock::now();//przechowuje czas odniesienia dla wydarzen odbywajacych sie co 1000ms
    auto t100 = high_resolution_clock::now();//przechowuje czas odniesienia dla wydarzen odbywajacych sie co 100ms
    auto t50 = high_resolution_clock::now();//przechowuje czas odniesienia dla wydarzen odbywajacych sie co 50ms
    
    auto elapsed_time_1000 = duration_cast<chrono::milliseconds>(tnow - t1000).count(); //przechowuje czas ktory minal od rozpoczecia odliczaina dla wydarzen co 1000ms
    auto elapsed_time_100 = duration_cast<chrono::milliseconds>(tnow - t100).count();//przechowuje czas ktory minal od rozpoczecia odliczaina dla wydarzen co 100ms
    auto elapsed_time_50 = duration_cast<chrono::milliseconds>(tnow - t50).count();//przechowuje czas ktory minal od rozpoczecia odliczaina dla wydarzen co 50ms
   
    srand(time(nullptr)); //ziarno dla generowania liczb pseudolosowych
       
    
    //glowna petla gry - wykonuje sie caly czas dopoki gracz nie wcisnie "esc", przegra albo wygra
    while (Game.get_state() == TRUE) {
        //pomiar czasu
        tnow = high_resolution_clock::now(); //aktualizacja czasu

        elapsed_time_1000 = duration_cast<chrono::milliseconds>(tnow - t1000).count();
        elapsed_time_100 = duration_cast<chrono::milliseconds>(tnow - t100).count();
        elapsed_time_50 = duration_cast<chrono::milliseconds>(tnow - t50).count();
        Game.key = 0;

        //jesli w buforze klawiatury znajduja sie dane, odczytaj jaki klawisz wcisnieto
        while (_kbhit())
        {
            Game.key = _getch();
        }

        //wyłączenie programu jesli wcisniety zostal esc
        if (Game.key == 27) {
           Game.set_state(FALSE);
        }

        //wydarzenia odbywajace sie co 1000ms
        
        if (elapsed_time_1000 >= 200) {
            
            //przesuwanie wrogów
            it = Invaders.begin();
            for (it; it != Invaders.end(); it++) {
                (*it)->PosX+=Game.dir;
               
            }
            t1000 = high_resolution_clock::now();
            Game.move++;
            if (modulus <int>()(Game.move, 10) == 0) {
                Game.dir *= -1;
                it = Invaders.begin();
                for (it; it != Invaders.end(); it++) {
                    (*it)->PosY += 1;
                    (*it)->PosX += Game.dir;
                }
            }

            //zrzucanie bomby losowo - pilnuje zeby bombe puscil wrog na samym dole wyolosowanej kolumny
            Game.random = rand()%(Game.EnemyCountX*Game.EnemyCountY) + 1;
            if (Invaders[Game.random-1]->HP == 1) {
                int xd = Game.EnemyCountY - modulus <int>()(Game.random, Game.EnemyCountY);
                
                for (int a = 0; a <= xd; a++) {
                    
                    if (Game.random<=32){
                        if (Invaders[Game.random-1]->HP == 1) {
                            Game.random_select = Game.random-1;
                        }
                    }
                    Game.random++;
                    
                }
                Bullets.push_back(new Bullet(Invaders[Game.random_select]->PosX, Invaders[Game.random_select]->PosY+1, 1, 1)); //bomba spawnuje sie pod wylosowanym przeciwnikiem z kierunkiem ruchu w dol ekranu
            }

        }
        
        //przesuwanie gracza
        if (Game.key == KEY_RIGHT && Hero.PosX<94) {
            Hero.PosX += 1; //w prawo
        }
        if (Game.key == KEY_LEFT && Hero.PosX>0) {
            Hero.PosX -= 1; //w lewo
        }

        //wystrzelenie pocisku
        Game.space_trig = 0;

        if (Game.key == KEY_SPACE) {
            Game.space_trig = 1;    //zapisanie informacji o wcisnieciu spacji    
        }
       
        if (Game.space_trig == 1 && Game.space_trig_prev == 0) {
            t50 = high_resolution_clock::now();     
            Game.crt_bullet = 1; //informacja o stworzeniu nowego pocisku
        }

        Game.space_trig_prev = Game.space_trig; //zapisanie informacji o wcisnietej spacji do nastepnej petli programu

        if (elapsed_time_50 >= 50 && Game.crt_bullet == 1) {
            Bullets.push_back(new Bullet(Hero.PosX,Hero.PosY-1,1,-1)); //dodanie nowego obiektu do wektora pociskow - o jedną kratkę wyżej niż znajduje się gracz i z kierunkiem ruchu do góry
            Game.crt_bullet = 0; //usuniecie informacji o stworzeniu nowego pocisku
        }

        
        
        //wykrywanie kolizji kosmitów z pociskami
        if (Bullets.size() != 0) { //wykonuje się tylko jesli istnieją pociski
            for (auto iter = Invaders.begin(); iter != Invaders.end(); iter++) {
                for (auto iter2 = Bullets.begin(); iter2 != Bullets.end(); ) {
                    if ((*iter)->PosX == (*iter2)->PosX && (*iter)->PosY == (*iter2)->PosY && (*iter)->HP > 0){ //jesli zgadza sie pozycja i przeciwnik istnieje (HP=1)
                            
                            (**iter)--; //zmniejszenie HP przeciwnika
                            Hero.points++; //zwiekszenie liczby punktow gracza
                            iter2 = Bullets.erase(iter2); //usuniecie pocisku
                    }
                    else
                        ++iter2;
                }

            }
        }

        //wykrywanie kolizji gracza z pociskami
        if (Bullets.size() != 0) {
            for (auto iter2 = Bullets.begin(); iter2 != Bullets.end(); ) {
                if (Hero.PosX == (*iter2)->PosX && Hero.PosY == (*iter2)->PosY) {

                    Hero--; //zmniejszenie HP gracza
                                       
                    iter2 = Bullets.erase(iter2); //usuniecie pocisku
                }
                else
                    ++iter2;
            }
        }

        //wykrywanie wyjechania za ekran
        if (Bullets.size() != 0) {
            for (auto iter2 = Bullets.begin(); iter2 != Bullets.end(); ) {
                if ((*iter2)->PosY == 0 || (*iter2)->PosY > 30) {
                    iter2 = Bullets.erase(iter2); //usuniecie pocisku jesli wyjedzie poza ekran
                }
                else
                    ++iter2;
            }
        }

        
        
        
        //wydarzenia dziejace sie co 50 ms
        //zapewnienie FPS i przesuwania pocisków
        if (elapsed_time_100 >= 50 ) {
            //przesuwanie pociskow
            if (Bullets.size() != 0) {
                it_Bullets = Bullets.begin();
                for (it_Bullets; it_Bullets != Bullets.end(); it_Bullets++) {
                    (*it_Bullets)->move();
                }
            }

            //rysowanie na ekranie
            redraw(Invaders, Hero, Bullets);
           
            t100 = high_resolution_clock::now();//zerowanie licznika dla wydarzen co 50ms

        }

        //sprawdzenie pozycji najniższego wroga
        
        for (auto it2 = Invaders.begin(); it2 != Invaders.end(); it2++) {
            if ((*it2)->HP == 1 && (*it2)->PosY > Game.max_enemy_pos) {
                Game.max_enemy_pos = (*it2)->PosY;

            }
        }

        //koniec gry
        if (Hero.HP == 0 || Game.max_enemy_pos == Hero.PosY) { //przegrana
            system("cls");
            gotoxy(30, 10);
            cout << "GAME OVER"<<endl<<endl<<endl<<endl<<endl;
            Game.set_state(0);
            system("pause");
        }

        if (Hero.points == Game.EnemyCountX * Game.EnemyCountY) { //wygrana
            system("cls");
            gotoxy(30, 10);
            cout << "YOU WON" << endl << endl << endl << endl << endl;
            Game.set_state(0);
            system("pause");
        }
    }
    
}
