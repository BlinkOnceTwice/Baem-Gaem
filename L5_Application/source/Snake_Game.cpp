#include "Snake_Game.hpp"
#include "RGBmatrixPanel.h"
#include<iostream>
#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "uart0_min.h"
#include<string>
#include <cstdlib>
#include <sstream>
#include "printf_lib.h"
#include <bits/stdc++.h>

RGBmatrixPanel M(true,32);
PWM pwm(PWM::pwm1,1);
using namespace std;

void IRQ_HANDLER1() {
    M.updateDisplay();   // Call refresh func for active display
}

void Beim_Game::game_Speed()
{
    if(nTail % 10 == 5 && speed < 55)
    {
        speed += (nTail/5)*2;
        rit_enable(IRQ_HANDLER1,speed);
    }
}

/*Bitmap for the snake image*/
const unsigned char PROGMEM bitmap[128] =
{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x00,
        0x00, 0x3e, 0xc0, 0x00, 0x00, 0x4e, 0xe0, 0x00, 0x00, 0x4e, 0x60, 0x00, 0x00, 0x9e, 0x60, 0x00,
        0x00, 0x9e, 0x70, 0x00, 0x00, 0x9e, 0x70, 0x00, 0x00, 0x9c, 0xf0, 0x00, 0x00, 0x9c, 0xf0, 0x00,
        0x00, 0xfd, 0xf0, 0x00, 0x00, 0x7f, 0xf0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x7f, 0xc0, 0x00,
        0x00, 0x3f, 0xc0, 0x00, 0x00, 0xef, 0xf0, 0x00, 0x01, 0xcf, 0xf0, 0x00, 0x03, 0x8b, 0xf8, 0x00,
        0x01, 0x0f, 0xfe, 0x00, 0x01, 0x0f, 0xfe, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xff, 0x00,
        0x00, 0x0f, 0xbe, 0x00, 0x00, 0x0f, 0x9e, 0x00, 0x00, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char PROGMEM Eyes[128] =
{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x31, 0x80, 0x00, 0x00, 0x61, 0x80, 0x00,
        0x00, 0x61, 0x80, 0x00, 0x00, 0x61, 0x80, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00,
        0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*Bitmap for snake border*/

const unsigned char PROGMEM Border[128]=
{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x80, 0x00,
        0x00, 0x30, 0xC0, 0x00, 0x00, 0x4A, 0xA0, 0x00, 0x00, 0x4A, 0x20, 0x00, 0x00, 0x9A, 0x20, 0x00,
        0x00, 0x9A, 0x30, 0x00, 0x00, 0x9A, 0x30, 0x00, 0x00, 0x98, 0x10, 0x00, 0x00, 0x98, 0x10, 0x00,
        0x00, 0x98, 0x10, 0x00, 0x00, 0x60, 0x10, 0x00, 0x00, 0x60, 0xA0, 0x00, 0x00, 0x7F, 0x60, 0x00,
        0x00, 0x10, 0x60, 0x00, 0x00, 0x08, 0x70, 0x00, 0x00, 0x08, 0x50, 0x00, 0x00, 0x08, 0x88, 0x00,
        0x00, 0x08, 0x82, 0x00, 0x00, 0x08, 0x02, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00, 0x08, 0x61, 0x00,
        0x00, 0x08, 0xA2, 0x00, 0x00, 0x08, 0x92, 0x00, 0x00, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void Beim_Game::StartScreen()
{
    for(int i=38;i>10;i-=2)
    {
        M.drawChar(6,i-8,'B',M.Color333(6,6,4),0,1);
        M.drawChar(12,i-8,'A',M.Color333(6,6,4),0,1);
        M.drawChar(18,i-8,'E',M.Color333(6,6,4),0,1);
        M.drawChar(24,i-8,'M',M.Color333(6,6,4),0,1);
        M.drawChar(6,i,'G',M.Color333(6,6,4),0,1);
        M.drawChar(12,i,'E',M.Color333(6,6,4),0,1);
        M.drawChar(18,i,'I',M.Color333(6,6,4),0,1);
        M.drawChar(24,i,'M',M.Color333(6,6,4),0,1);
        M.swapBuffers(false);
        delay_ms(250);
        M.fillScreen(M.Color333(0,0,0));
        pwm.set(30);
        pwm.set(0);
        delay_ms(250);
        pwm.set(20);
        delay_ms(250);
    }
    pwm.set(0);

    /*Some pixel drawn to fill spaces*/
    M.fillScreen(M.Color333(1,1,1));
    M.drawBitmap(0, 0,Eyes,32,32,M.Color333(0,0,4));
    M.drawBitmap(0, 0,bitmap,32,32,M.Color333(0,3,0));
    M.drawBitmap(0, 0,Border,32,32,M.Color333(0,0,0));
    M.drawPixel(6,19,M.Color333(7,0,0));
    M.drawPixel(7,21,M.Color333(7,0,0));
    M.drawPixel(7,20,M.Color333(7,0,0));
    M.drawPixel(7,19,M.Color333(7,0,0));
    M.drawPixel(7,18,M.Color333(7,0,0));
    M.drawPixel(8,19,M.Color333(7,0,0));
    M.drawPixel(8,19,M.Color333(7,0,0));
    M.drawPixel(8,18,M.Color333(7,0,0));
    M.drawPixel(8,17,M.Color333(7,0,0));
    M.drawPixel(9,18,M.Color333(7,0,0));
    M.drawPixel(9,18,M.Color333(7,0,0));
    M.drawPixel(9,17,M.Color333(7,0,0));
    M.drawPixel(10,16,M.Color333(7,0,0));
    M.drawPixel(10,17,M.Color333(7,0,0));
    M.drawPixel(13,19,M.Color333(0,7,0));

    M.drawPixel(11,11,M.Color333(1,1,1));
    M.drawPixel(12,11,M.Color333(1,1,1));
    M.drawPixel(14,5,M.Color333(1,1,1));
    M.drawPixel(12,6,M.Color333(1,1,1));
    M.drawPixel(12,7,M.Color333(1,1,1));
    M.drawPixel(11,12,M.Color333(0,7,0));
    M.drawPixel(12,12,M.Color333(0,7,0));
    M.drawPixel(13,19,M.Color333(0,7,0));

    M.drawLine(12,5,12,12,M.Color333(0,7,0));

    M.swapBuffers(false);
    delay_ms(3000);
}

void Beim_Game::pauseScreen()
{
    M.fillScreen(0);
    M.drawChar(3,11,'P',M.Color333(6,6,4),0,1);
    M.drawChar(9,11,'A',M.Color333(6,6,4),0,1);
    M.drawChar(15,11,'U',M.Color333(6,6,4),0,1);
    M.drawChar(21,11,'S',M.Color333(6,6,4),0,1);
    M.drawChar(27,11,'E',M.Color333(6,6,4),0,1);
    M.swapBuffers(false);
}


bool Beim_Game::getLevel(int pin)
{
    bool level=(LPC_GPIO1->FIOPIN & (1<<pin));
    return level;
}

void Beim_Game::FruitSpawn()
{
    fruitX = rand() % (width-1);
    fruitY = rand() % (height-1);
}
void Beim_Game::BonusFruitSpawn()
{
    time1=rand()%100 + 50;
    timeout=50;
    BonusX = rand() % (width-1);
    BonusY = rand() % (height-1);
    global=0;
}

void Beim_Game:: EvilFruitSpawn()
{
    time2=rand()%100 + 50;
    timeout1 = 25;                     //Time for the Bonus fruit to be available
    EvilX = rand() % (width-1);        //Get row value where the fruit to be spawned
    EvilY = rand() % (height-1);       //Get column value where the fruit to be spawned
    global1 = 0;                       //Time for bonus fruit to reappear
}

bool Beim_Game::gameoverlevel()
{
    if(gameover)
        return true;
    else
        return false;
}

void Beim_Game::Setup()
{
    M.begin();                             //Function to initialize the LED Matrix setup
    //Pins chosen are 1.9, 1.10,
    LPC_GPIO1->FIODIR &= ~(1<<9);
    LPC_GPIO1->FIODIR &= ~(1<<10);
    LPC_GPIO1->FIODIR &= ~(1<<14);
    LPC_GPIO1->FIODIR &= ~(1<<15);
    pause = false;
    gameover = false;                      //Flag to indicate if game is over
    temp = RIGHT;
    dir = RIGHT;                           //Initial Direction of the Snake to move
    x = width / 2;                        //Row value for snake head to start with
    y = height / 2;                      //Column value for snake head to start with
    FruitSpawn();                       //Spawn the fruit
    EvilFruitSpawn();
    BonusFruitSpawn();                  //Spawn the Bonus fruit
    score = 0;                                          //Total score of the user
    nTail=3;                                            //Initial body length of the snake
}

/*
 * This function selects the game mode between Arcade or Normal
 */
bool Beim_Game::Mode()
{
    M.fillScreen(M.Color333(0,0,0));
    M.drawChar(1,4,'S',M.Color333(7,0,0),0,1);
    M.drawChar(6,4,'E',M.Color333(7,0,0),0,1);
    M.drawChar(11,4,'L',M.Color333(7,0,0),0,1);
    M.drawChar(16,4,'E',M.Color333(7,0,0),0,1);
    M.drawChar(21,4,'C',M.Color333(7,0,0),0,1);
    M.drawChar(26,4,'T',M.Color333(7,0,0),0,1);
    M.drawChar(7,14,'M',M.Color333(7,0,0),0,1);
    M.drawChar(13,14,'O',M.Color333(7,0,0),0,1);
    M.drawChar(19,14,'D',M.Color333(7,0,0),0,1);
    M.drawChar(25,14,'E',M.Color333(7,0,0),0,1);
    M.swapBuffers(false);

    int delay=500; //(nTail%5);
    (void)wireless_get_rx_pkt(&rx, delay);
    u0_dbg_printf("DATA0:%d\n", rx.data[0]);
    u0_dbg_printf("DATA1:%d\n", rx.data[1]);
    M.fillScreen(M.Color333(0,0,0));
    //        if(getLevel(9))
    if(rx.data[0] == UP || getLevel(9))
    {
        uart0_puts("entered\n");
        MODE=true;
        M.drawChar(1,14,'A',M.Color333(0,7,2),0,1);
        M.drawChar(6,14,'R',M.Color333(0,7,2),0,1);
        M.drawChar(11,14,'C',M.Color333(0,7,2),0,1);
        M.drawChar(16,14,'A',M.Color333(0,7,2),0,1);
        M.drawChar(21,14,'D',M.Color333(0,7,2),0,1);
        M.drawChar(26,14,'E',M.Color333(0,7,2),0,1);
        M.swapBuffers(false);
        delay_ms(2000);
        M.fillScreen(M.Color333(0,0,0));
        return true;
    }

    else if(rx.data[0] == DOWN || getLevel(10))
    {
        uart0_puts("entered\n");
        MODE=false;
        M.drawChar(1,14,'N',M.Color333(3,4,3),0,1);
        M.drawChar(6,14,'O',M.Color333(3,4,3),0,1);
        M.drawChar(11,14,'R',M.Color333(3,4,3),0,1);
        M.drawChar(16,14,'M',M.Color333(3,4,3),0,1);
        M.drawChar(21,14,'A',M.Color333(3,4,3),0,1);
        M.drawChar(26,14,'L',M.Color333(3,4,3),0,1);
        M.swapBuffers(false);
        delay_ms(2000);
        M.fillScreen(M.Color333(0,0,0));
        return true;
    }
    else{
    }
    return false;
}

/*
 * This function receives a wireless packet from either
 * joystick or accelerometer.
 */

void Beim_Game::vConsumer()
{
    // int delay=10; //(nTail%5);
    u0_dbg_printf("temp:%d\n",temp);
    u0_dbg_printf("direction:%d\n",dir);
    (void)wireless_get_rx_pkt(&rx, 100);
    if(rx.data[0] == UP)
    {
        if(temp!=DOWN)
        {
            dir = UP;
            temp = UP;
        }
        pause = false;
        uart0_puts("up\n");
    }

    else if(rx.data[0] == DOWN )
    {
        if(temp!=UP)
        {
            dir = DOWN;
            temp = DOWN;
        }
        pause = false;
        uart0_puts("down\n");
    }

    else if(rx.data[0] == LEFT )
    {
        if(temp!=RIGHT)
        {
            dir = LEFT;
            temp = LEFT;
        }
        pause = false;
        uart0_puts("left\n");
    }

    else if(rx.data[0] == RIGHT)
    {
        if(temp!=LEFT)
        {
            dir = RIGHT;
            temp = RIGHT;
        }
        pause = false;
        uart0_puts("right\n");
    }

    else if(rx.data[0] == RESET)                          //Condition to reset the game
    {
        if(!gameoverlevel()&&!pause)
        {
            COPY++;
            dir=STOP;
            pause=true;
            dir= temp;
        }
        else if(gameoverlevel())
        {
            M.fillScreen(M.Color333(0,0,0));                //Clear the screen
            delay_ms(250);
            M.swapBuffers(false);
            shutDown=true;
            StartScreen();
            Setup();

            for(uint8_t i= 0; i < nTail ; i++)
            {
                Logic();
            }
            while(!Mode());
            shutDown=false;
            M.swapBuffers(false);
        }
    }
    else if(rx.data[0] == STOP)
    {
    }

    else{
    }

}

/*
 * This function draws the updated snake game screen
 * Uses all the RGBMatrix class methods
 */

void Beim_Game::Draw()
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (i == y && j == x)      //If row and column is same as snake head index
            {
                if(dir==DOWN)
                {
                    M.drawPixel(i,j,M.Color444(0,7,0));
                }
                else if(dir==UP)
                {
                    M.drawPixel(i,j,M.Color444(0,7,0));
                }
                else if(dir==RIGHT)
                {
                    M.drawPixel(i,j,M.Color444(0,7,0));
                }
                else if(dir==LEFT)
                {
                    M.drawPixel(i,j,M.Color444(0,7,0));
                }
            }
            else if (i == fruitY && j == fruitX)
            {
                if(MODE)
                {
                    if(((fruitX==0&&(fruitY>=0 && fruitY<=31)) ||(fruitY==0&&(fruitX>=0 && fruitX<=31)) ||(fruitY==31&&(fruitX>=0 && fruitX<=31)) || (fruitX==31&&(fruitY>=0 && fruitY<=31))))

                    {
                        FruitSpawn();
                    }
                    else
                        M.drawPixel(i,j,M.Color333(7,0,7));
                }
                else
                    M.drawPixel(i,j,M.Color333(7,0,7));
            }
            else if(i == BonusY && j == BonusX &&timeout)
            {
                if((BonusX!=fruitX || BonusY!=fruitY) && ((BonusX!= EvilX || BonusY!= EvilY)))
                {
                    if(MODE)
                    {
                        if(((BonusX==0&&(BonusY>=0 && BonusY<=31)) ||(BonusY==0&&(BonusX>=0 && BonusX<=31)) ||(BonusY==31&&(BonusX>=0 && BonusX<=31)) || (BonusX==31&&(BonusY>=0 && BonusY<=31))))
                        {
                            BonusFruitSpawn();
                        }
                        else
                        {
                            M.drawPixel(i,j,M.Color333(0,0,7));
                            timeout--;
                        }
                    }
                    else{
                        M.drawPixel(i,j,M.Color444(0,0,7));
                        timeout--;
                    }
                }
            }

            else if(i == EvilY && j == EvilX &&timeout1)   //If row and column is same as bonus fruit
            {
                if((EvilX!=fruitX || EvilY!=fruitY)&&(EvilX!=BonusX || EvilY!=BonusY))      //If bonus fruit doesnot overlay fruit
                {
                    if(MODE)
                    {
                        if(((EvilX==0&&(EvilY>=0 && EvilY<=31))
                                ||(EvilY==0&&(EvilX>=0 && EvilX<=31)) ||(EvilY==31&&(EvilX>=0 && EvilX<=31))
                                || (EvilX==31&&(EvilY>=0 && EvilY<=31))))
                        {
                            EvilFruitSpawn();              //If mode is arcade, Spawn the fruit apart from boundary
                        }
                        else
                        {
                            M.drawPixel(i,j,M.Color444(7,0,0));
                            timeout1--;
                        }
                    }
                    else{
                        M.drawPixel(i,j,M.Color444(7,0,0));
                        timeout1--;
                    }
                }
            }

            else {
                bool print = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        M.drawPixel(i,j,M.Color333(0,7,3));
                        print = true;
                    }
                }
                if(!print)
                    M.drawPixel(i,j,0);
            }
        }
    }
    if(MODE==true)
    {
        M.drawLine(0,0,31,0,M.Color333(6,2,0));
        M.drawLine(31,0,31,31,M.Color333(6,2,0));
        M.drawLine(31,31,0,31,M.Color333(6,2,0));
        M.drawLine(0,31,0,0,M.Color333(6,2,0));
    }
    pwm.set(0);
    M.swapBuffers(false);

}

/*
 * This function is used if user wants to give inputs using on-board switches
 */
void Beim_Game::Input()
{
    if(getLevel(9))
    {
        if(temp!=RIGHT)
        {
            dir = LEFT;
            temp = LEFT;
        }
    }
    else if(getLevel(10))
    {
        if(temp!=DOWN)
        {
            dir = UP;
            temp = UP;
        }
    }
    else if(getLevel(14))
    {
        if(temp!=UP)
        {
            dir = DOWN;
            temp = DOWN;
        }
    }
    else if(getLevel(15))
    {
        if(temp!=LEFT)
        {
            dir = RIGHT;
            temp = RIGHT;
        }
    }
}


void Beim_Game::Gameover()
{
    int temp1=score;
    ostringstream ostr;
    ostr<<score;
    string theString = ostr.str();
    M.fillScreen(M.Color333(0,0,0));
    pwm.set(10);
    delay_ms(2000);
    for(int i=-6;i<14;i+=2)
    {
        M.drawChar(6,i+8,'O',M.Color333(7,0,0),0,1);
        M.drawChar(12,i+8,'V',M.Color333(7,0,0),0,1);
        M.drawChar(18,i+8,'E',M.Color333(7,0,0),0,1);
        M.drawChar(24,i+8,'R',M.Color333(7,0,0),0,1);
        M.drawChar(6,i,'G',M.Color333(7,0,0),0,1);
        M.drawChar(12,i,'A',M.Color333(7,0,0),0,1);
        M.drawChar(18,i,'M',M.Color333(7,0,0),0,1);
        M.drawChar(24,i,'E',M.Color333(7,0,0),0,1);
        M.swapBuffers(false);
        delay_ms(250);
        M.fillScreen(M.Color333(0,0,0));
        delay_ms(250);
    }
    delay_ms(2000);
    M.swapBuffers(false);
    M.fillScreen(M.Color333(0,0,0));

    const char *ptr=theString.c_str();
    int length=sizeof(theString),index=0,i=0,j=1;

    while(temp1>=10)
    {
        temp1=temp1%10;
        j++;
    }
    M.drawChar(2,2,'S',M.Color333(7,0,0),0,1);
    M.drawChar(8,2,'C',M.Color333(7,0,0),0,1);
    M.drawChar(14,2,'O',M.Color333(7,0,0),0,1);
    M.drawChar(20,2,'R',M.Color333(7,0,0),0,1);
    M.drawChar(26,2,'E',M.Color333(7,0,0),0,1);
    while(j>=0)
    {
        M.drawChar(index+6,15,ptr[i++],M.Color333(7,0,0),0,1);
        j--;
        index+=6;
    }

    M.swapBuffers(false);
    delay_ms(200);
    pwm.set(0);
}

/*
 * This function will implement the game algorithm
 * with corresponding input values.
 * The movement of snake is determined on the current position
 * and new with resp to inputs
 */
void Beim_Game::Logic()
{
    uint16_t indexX,indexY;
    int prevX = tailX[0];
    int prevY = tailY[0];
    tailX[0] = x;
    tailY[0] = y;

    int prev2X, prev2Y;

    //values of the body of snake
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;

    }
    switch (dir)
    {
        case LEFT:
            y--;
            if(y<0)
            {
                y=width-1;
            }

            break;

        case RIGHT:
            y++;

            if(y>width-1)
            {
                y=0;
            }
            break;

        case UP:
            x--;
            if(x<0)
            {
                x=width-1;
            }
            break;

        case DOWN:
            x++;
            if(x>width-1)
            {
                x=0;
            }
            break;
        case STOP:
            indexX = x;
            indexY = y;
    }

    //The logic and end game conditions change according to the mode

    // The logic for arcade mode end game
    if(MODE)
    {
        if(x==0 || y==0 || x==31 || y== 31)
        {
            pwm.set(40);
            gameover = true;
            for(uint8_t j= 0; j<5; j++)
            {
                M.fillScreen(0);
                M.swapBuffers(false);
                Draw();
                delay_ms(750);
            }
            Gameover();
        }

        // The logic for normal mode end game
        for (int i = 1; i < nTail; i++) {
            if (tailX[i] == x && tailY[i] == y)
            {
                pwm.set(40);
                gameover = true;
                M.swapBuffers(false);
                for(uint8_t j= 0; j<5; j++)
                {
                    M.fillScreen(0);
                    M.swapBuffers(false);
                    Draw();
                    delay_ms(750);
                }
                Gameover();
            }
        }
    }
    else
    {
        for (int i = 1; i < nTail; i++) {
            if (tailX[i] == x && tailY[i] == y)
            {
                pwm.set(40);
                gameover = true;
                M.swapBuffers(false);
                for(uint8_t j= 0; j<5; j++)
                {
                    M.fillScreen(0);
                    M.swapBuffers(false);
                    Draw();
                    delay_ms(750);
                }
                Gameover();
            }
        }

    }
    if(timeout==0)
    {
        global++;
        if(global==time1)
            BonusFruitSpawn();
    }
    if(timeout1==0)
    {
        global1++;
        if(global1== time2)
            EvilFruitSpawn();
    }
    if (x == fruitX && y == fruitY) {
        score++;
        nTail++;
        FruitSpawn();
        pwm.set(10);
    }
    else if ((x == BonusX && y == BonusY) && timeout) {
        score+=6;
        nTail+=2;
        timeout=0;
        pwm.set(30);
        delay_ms(30);
    }
    else if ((x == EvilX && y == EvilY) && timeout1) {
        score-=8;
        nTail+=2;
        pwm.set(40);
        timeout1 = 0;
    }
}

bool Beim_Game::shutdownlevel()
{
    return shutDown;
}

/* Return pause flag*/
bool Beim_Game::Pausegetlevel()
{
    return pause;
}


