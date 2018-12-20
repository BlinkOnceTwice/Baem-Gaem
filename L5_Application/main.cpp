/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          dhavale.neeraj@gmail.com
 */
#include "LPC17xx.h"
#include "tasks.hpp"
#include "FreeRTOS.h"
#include "stdio.h"
#include "utilities.h"
#include <string.h>
#include "Snake_Game.hpp"
#include "RGBmatrixPanel.h"
#include "uart0_min.h"
#include "wireless.h"

uint8_t flag=0;
Beim_Game Object;
TaskHandle_t MODE;
SemaphoreHandle_t MODE_SELECT_HANDLE = NULL;


void vtaskgame(void *p)
{
    Object.Setup();
    Object.StartScreen();
    while(!(Object.Mode()));
    while(1)
    {
        {
            if(((!Object.gameoverlevel()) && (!Object.shutdownlevel())))
            {
                Object.Draw();
                if(!Object.Pausegetlevel())
                    Object.Logic();
            }
        }
        vTaskDelay(50);
    }
}

/*
 * Accepts two types of inputs wireless or on board switches
 * Comment out either way of input
 */
void vtaskInput(void*p)
{
    while(1)
    {
        //        Object.Input();
        Object.vConsumer();
    }
}



int main()
{
    /*
     * while using Nordic wireless Always:
     * 1. Add wireless task using scheduler_add_task()
     * 2. start wireless task using scheduler_start()
     */
    MODE_SELECT_HANDLE = xSemaphoreCreateMutex();
    wireless_init();
    xTaskCreate(vtaskgame,"GAME",2048,NULL,PRIORITY_MEDIUM,NULL);
    xTaskCreate(vtaskInput,"INPUT",2048,NULL,PRIORITY_LOW,NULL);
    scheduler_add_task(new wirelessTask (PRIORITY_HIGH));
    scheduler_start();
    return 0;
}
