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

    double PosX;
    double PosY;
    int HP;
        
private:
    
    
};

class Player : public Character{
public:
    Player(int a, int b,int c) : Character(a, b, c) {}

    void display() {
        gotoxy(PosX, PosY);
        cout << "^";
        
    }

private:
    double velocity = 800;

};

class Enemy : public Character{
public:
    Enemy(int a, int b, int c) : Character(a,b,c) {}
    void operator-- () {
        HP = 0;
    }
     
    void display() {
        if (HP == 1) {
            gotoxy(PosX, PosY);
            cout << "W";
        }
    }

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

void redraw(vector <Character*> A, Player B) {
    system("cls");
    auto it = A.begin();
    for (it; it != A.end(); it++) {
        (*it)->display();
    }
    B.display();
    gotoxy(0, 0);
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
    //inicjalizacja gracza
    Player Hero(47,28,10);

    //inicjalizacja przeciwnikow
    vector<Character*> Invaders;
    for (int i = 0; i < Game.EnemyCountX; i++) {
        for (int j = 0; j < Game.EnemyCountY; j++) {
            Invaders.push_back(new Enemy(Game.FirstPosX + 7*i,Game.FirstPosY + 4*j,1));
        }
    }

    Invaders[1]--; //wiem ze to nie ma sensu ale juz probowalem co sie dalo
    
    
    //inicjalizacja pomiaru czasu
    auto t1000 = high_resolution_clock::now();
    auto t100 = high_resolution_clock::now();
    auto tnow = high_resolution_clock::now();
    auto elapsed_time_1000 = duration_cast<chrono::milliseconds>(tnow - t1000).count();
    auto elapsed_time_100 = duration_cast<chrono::milliseconds>(tnow - t100).count();
   
    
    //glowna petla gry
    while (Game.get_state() == TRUE) {
        //pomiar czasu
        tnow = high_resolution_clock::now();
        elapsed_time_1000 = duration_cast<chrono::milliseconds>(tnow - t1000).count();
        elapsed_time_100 = duration_cast<chrono::milliseconds>(tnow - t100).count();
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
            auto it = Invaders.begin();
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
        //zapewnienie FPS
        if (elapsed_time_100 >= 50) {
            redraw(Invaders, Hero);
            t100 = high_resolution_clock::now();

        }
        
        //przesuwanie gracza
        if (key == KEY_RIGHT && Hero.PosX<94) {
            Hero.PosX += 1;
        }
        if (key == KEY_LEFT && Hero.PosX>0) {
            Hero.PosX -= 1;
        }
        
    }
    
}
