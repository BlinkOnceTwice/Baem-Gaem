/*
 * Snake_Game.hpp
 *
 *  Created on: Nov 23, 2018
 *      Author: sdrsh
 */

#ifndef SNAKE_GAME_HPP_
#define SNAKE_GAME_HPP_
#include "RGBmatrixPanel.h"
#include "wireless.h"
#include "nrf_stream.hpp"
#include<string.h>
#include "lpc_pwm.hpp"
#include "lpc_rit.h"

enum eDirection {
    STOP,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    RESET
};



class Beim_Game
{
private:
    bool gameover;
    bool shutDown;
    bool pause;
    bool MODE;
    uint16_t time1,time2;
    const int width = 32;
    const int height = 32;
    int tailX[1024], tailY[1024], nTail,mTail,x,y,score,global, global1;
    int fruitX, fruitY,BonusX,BonusY,EvilX,EvilY;;
    int timeout, timeout1;
    uint8_t COPY = 0;
    mesh_packet_t rx;
    eDirection dir,temp;
    uint8_t speed= 10;

public:
    bool gameoverlevel();
    void vConsumer();
    void FruitSpawn();
    void Setup();
    void Draw();
    void joystick();
    void Logic();
    void StartScreen();
    bool getLevel(int pin);
    void Input();
    bool Mode();
    void Gameover();
    void BonusFruitSpawn();
    bool shutdownlevel();
    bool Pausegetlevel();
    void pauseScreen();
    void EvilFruitSpawn();
    void game_Speed();
};




#endif /* SNAKE_GAME_HPP_ */
