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
    GameHandler() :state(1), EnemyCountX(8), EnemyCountY(4), FirstPosX(4), FirstPosY(2) {}
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
}



int main()
{
    //inicjalizacja okna gry
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, 800, 600, TRUE);
          
    //inicjalizacja zmienych gry - dac do klasy zeby sie nie dalo zmieniac xd
    int key = 0;
    GameHandler Game;
    int move = 0;
    int dir = 1;
    bool space_trig = 0;
    bool space_trig_prev = 0;
    bool crt_bullet = 0;
    int rand_counter = 0;
    int random_select = 0;

    //inicjalizacja gracza
    Player Hero(47,28,4);

    //inicjalizacja wektora pocisków
    vector<Character*> Bullets;
    auto it_Bullets = Bullets.begin();

    //inicjalizacja przeciwnikow
    vector<Character*> Invaders;
    for (int i = 0; i < Game.EnemyCountX; i++) {
        for (int j = 0; j < Game.EnemyCountY; j++) {
            Invaders.push_back(new Enemy(Game.FirstPosX + 7*i,Game.FirstPosY + 4*j,1));
        }
    }

    auto it = Invaders.begin();

    //inicjalizacja pomiaru czasu
    auto tnow = high_resolution_clock::now();

    auto t1000 = high_resolution_clock::now();
    auto t100 = high_resolution_clock::now();
    auto t50 = high_resolution_clock::now();
    
    auto elapsed_time_1000 = duration_cast<chrono::milliseconds>(tnow - t1000).count();
    auto elapsed_time_100 = duration_cast<chrono::milliseconds>(tnow - t100).count();
    auto elapsed_time_50 = duration_cast<chrono::milliseconds>(tnow - t50).count();
   
    srand(time(nullptr));
    int random = rand();
    
    
    //glowna petla gry
    while (Game.get_state() == TRUE) {
        //pomiar czasu
        tnow = high_resolution_clock::now();
        elapsed_time_1000 = duration_cast<chrono::milliseconds>(tnow - t1000).count();
        elapsed_time_100 = duration_cast<chrono::milliseconds>(tnow - t100).count();
        elapsed_time_50 = duration_cast<chrono::milliseconds>(tnow - t50).count();
        key = 0;

        //jesli w buforze klawiatury znajduja sie dane, odczytaj jaki klawisz wcisnieto
        while (_kbhit())
        {
            key = _getch();
        }

        //wyłączenie programu jesli wcisniety zostal esc
        if (key == 27) {
           Game.set_state(FALSE);
        }

        //przesuwanie wrogów
        if (elapsed_time_1000 >= 300) {
            it = Invaders.begin();
            for (it; it != Invaders.end(); it++) {
                (*it)->PosX+=dir;
               
            }
            t1000 = high_resolution_clock::now();
            move++;
            if (modulus <int>()(move, 10) == 0) {
                dir *= -1;
                it = Invaders.begin();
                for (it; it != Invaders.end(); it++) {
                    (*it)->PosY += 1;
                    (*it)->PosX += dir;
                }
            }
            //zrzucanie bomby losowo
        
       
            random = rand()%(Game.EnemyCountX*Game.EnemyCountY) + 1;
            if (Invaders[random-1]->HP == 1) {
                int xd = Game.EnemyCountY - modulus <int>()(random, Game.EnemyCountY);
                
                for (int a = 0; a <= xd; a++) {
                    
                    if (random<=32){
                        if (Invaders[random-1]->HP == 1) {
                            random_select = random-1;
                        }
                    }
                    random++;
                    
                }
                Bullets.push_back(new Bullet(Invaders[random_select]->PosX, Invaders[random_select]->PosY+1, 1, 1));
            }

        }
        
        //}
       
        
        //przesuwanie gracza
        if (key == KEY_RIGHT && Hero.PosX<94) {
            Hero.PosX += 1;
        }
        if (key == KEY_LEFT && Hero.PosX>0) {
            Hero.PosX -= 1;
        }

        //wystrzelenie pocisku
        space_trig = 0;

        if (key == KEY_SPACE) {
            space_trig = 1;            
        }
        else {
            space_trig = 0;
        }

        if (space_trig == 1 && space_trig_prev == 0) {
            t50 = high_resolution_clock::now();     
            crt_bullet = 1;
        }

        space_trig_prev = space_trig;

        if (elapsed_time_50 >= 50 && crt_bullet == 1) {
            Bullets.push_back(new Bullet(Hero.PosX,Hero.PosY-1,1,-1));
            crt_bullet = 0;
        }

        
        
        //wykrywanie kolizji kosmitów z pociskami
        if (Bullets.size() != 0) {
            for (auto iter = Invaders.begin(); iter != Invaders.end(); iter++) {
                for (auto iter2 = Bullets.begin(); iter2 != Bullets.end(); ) {
                    if ((*iter)->PosX == (*iter2)->PosX && (*iter)->PosY == (*iter2)->PosY && (*iter)->HP > 0){
                        
                            (**iter)--;
                            Hero.points++;
                            iter2 = Bullets.erase(iter2);
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

                    Hero--;
                    
                    
                    iter2 = Bullets.erase(iter2);
                }
                else
                    ++iter2;
            }
        }

        //wykrywanie wyjechania za ekran
        if (Bullets.size() != 0) {
            for (auto iter2 = Bullets.begin(); iter2 != Bullets.end(); ) {
                if ((*iter2)->PosY == 0 || (*iter2)->PosY > 30) {
                    iter2 = Bullets.erase(iter2);
                }
                else
                    ++iter2;
            }
        }

        
        
        

        //zapewnienie FPS i przesuwania pocisków
        if (elapsed_time_100 >= 50 ) {
            if (Bullets.size() != 0) {
                it_Bullets = Bullets.begin();
                for (it_Bullets; it_Bullets != Bullets.end(); it_Bullets++) {
                    (*it_Bullets)->move();
                }
            }
            
            redraw(Invaders, Hero, Bullets);
            gotoxy(0, 0);
            cout << Hero.HP;
            gotoxy(44, 0);
            cout << Hero.points;
            t100 = high_resolution_clock::now();

        }

        //sprawdzenie pozycji najniższego wroga
        int max = 0;
        for (auto it2 = Invaders.begin(); it2 != Invaders.end(); it2++) {
            if ((*it2)->HP == 1 && (*it2)->PosY > max) {
                max = (*it2)->PosY;

            }
        }

        //koniec gry
        if (Hero.HP == 0 || max == Hero.PosY) {
            system("cls");
            gotoxy(30, 10);
            cout << "GAME OVER"<<endl<<endl<<endl<<endl<<endl;
            Game.set_state(0);
            system("pause");
        }
    }
    
}
