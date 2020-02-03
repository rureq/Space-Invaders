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
        HP=0;
    }

    double PosX;
    double PosY;
    int HP;
        
private:
    
    
};

class Player : public Character{
public:
    Player(int a, int b,int c) : Character(a, b, c) {}
    virtual void move() {}


    virtual void display() {
        gotoxy(PosX, PosY);
        cout << "^";
        
    }

private:
    double velocity = 800;

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

//pauza gry, koniec gry itd, wyswietlanie gry
public:
    GameHandler() :state(1), EnemyCountX(8), EnemyCountY(4), FirstPosX(4), FirstPosY(2) {}
    bool get_state() {
        return state;
    }
    void set_state(BOOL a) {
        state = a;
    }
    int EnemyCountX;
    int EnemyCountY;
    int EnemyCount;
    int FirstPosX;
    int FirstPosY;

   
private:
    int FPS = 50;
    bool state;
    int Key;
   
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
    //inicjalizacja gracza
    Player Hero(47,28,10);

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

    //(*Invaders[0])--; //wiem ze to nie ma sensu ale juz probowalem co sie dalo, jak sie wykomentuje to mozna latac statkiem na boki :)
    
    
    //inicjalizacja pomiaru czasu
    auto tnow = high_resolution_clock::now();

    auto t1000 = high_resolution_clock::now();
    auto t100 = high_resolution_clock::now();
    auto t50 = high_resolution_clock::now();
    
    auto elapsed_time_1000 = duration_cast<chrono::milliseconds>(tnow - t1000).count();
    auto elapsed_time_100 = duration_cast<chrono::milliseconds>(tnow - t100).count();
    auto elapsed_time_50 = duration_cast<chrono::milliseconds>(tnow - t50).count();
   
    
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
        if (elapsed_time_1000 >= 1000) {
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
        }
       
        
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

        //przesuwanie pocisków
        
        //wykrywanie kolizji i wyjechania za ekran
        if (Bullets.size() != 0) {
            for (auto iter = Invaders.begin(); iter != Invaders.end(); iter++) {
                for (auto iter2 = Bullets.begin(); iter2 != Bullets.end(); ) {
                    if ((*iter)->PosX == (*iter2)->PosX && (*iter)->PosY == (*iter2)->PosY)  {
                        //(*Invaders[iter])--;
                        (**iter)--;//tutaj
                        iter2 = Bullets.erase(iter2);
                        
                    }
                    else
                        ++iter2;
                }

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

        //zrzucanie bomby losowo

        //punkty

        //game over

        //zapewnienie FPS i przesuwania pocisków
        if (elapsed_time_100 >= 50 ) {
            if (Bullets.size() != 0) {
                it_Bullets = Bullets.begin();
                for (it_Bullets; it_Bullets != Bullets.end(); it_Bullets++) {
                    (*it_Bullets)->move();
                }
            }
            
            redraw(Invaders, Hero, Bullets);
            t100 = high_resolution_clock::now();

        }
    }
    
}
