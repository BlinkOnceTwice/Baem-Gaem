/*
 * LabGPIO.cpp
 *
 *  Created on: Sep 3, 2018
 *      Author: Neeraj Dhavale
 */

#include "LabGPIO.hpp"
#include "tasks.hpp"
uint8_t SwitchState1=0;
uint8_t SwitchState2=0;

void LabGPIO_1::setAsInput(void)
{
    ptrLabGPIO->FIODIR &= ~(1 << PinNum);
}
void LabGPIO_1::setAsOutput(void)
{
    ptrLabGPIO->FIODIR |= (1 << PinNum);
}

void LabGPIO_1::setDirection(bool output)
{
    if(output == 1)
    {
        setHigh();
    }

    else
    {
        setLow();
    }

}



void vControlLED()
{

        /* Define Local Variables and Objects */

        /* Initialization Code */
        LPC_PINCON->PINSEL2 |= 0x00000000;
        LPC_GPIO1->FIODIR1 |= (1<<0);
        LPC_GPIO1->FIOCLR1 |= (1<<0);

}

void vControlLEDExter(void *p)
{
        /* Get Parameter */
       // uint32_t param = (uint32_t)(LPC_GPIO1);
        /* Define Constants Here */

        /* Define Local Variables and Objects */

        /* Initialization Code */
        LPC_PINCON->PINSEL0 = 0x00000000;
        LPC_GPIO0->FIODIR0 |= (1<<1);
        LPC_GPIO0->FIOSET0 |= (1<<1);


        /*Only necessary if above loop has a condition */
        //xTaskDelete(NULL);
}

void vReadSwitch(uint8_t PortNum,uint8_t PinNum)
{
    /* Get Parameter */
    //uint32_t param = (uint32_t)(pvParameters);
    /* Define Constants Here */

    /* Define Local Variables and Objects */

    /* Initialization Code */
    /*Make the pin input*/
     LPC_PINCON->PINSEL0 = 0x00000000;
     LPC_GPIO0->FIODIR0 &= ~(1<<0);

     LPC_PINCON->PINSEL2 = 0x00000000;
     LPC_GPIO1->FIODIR1 &= ~(1<<10);
     /*Read switch status*/



    while(1)
    {
        /* Insert Loop Code */

        if((LPC_GPIO0->FIOPIN>>0)&0x01)
        {

            SwitchState1++;
            vTaskDelay(100);
        }

        if((LPC_GPIO1->FIOPIN>>10)&0x01)
        {
            SwitchState2++;
        }


        SwitchState1 = SwitchState1%2;
        SwitchState2 = SwitchState2%2;


        if(SwitchState2 == 1)
        {
           // vControlLED(PortNum,PinNum);
            vTaskDelay(100);
        }

        if(SwitchState2 == 0)
        {
            LPC_GPIO1->FIOCLR1 |= (1<<0);
        }

        //SwitchState1 = SwitchState1%2;
        /*Check if switch is pressed*/

        if(SwitchState1 == 1)
        {
            //vControlLEDExter(void);
            vTaskDelay(100);
        }

        if(SwitchState1 == 0)
        {
            LPC_GPIO0->FIOCLR0 |= (1<<1);
        }
        vTaskDelay(100);
    }
    /* Only necessary if above loop has a condition */
    //xTaskDelete(NULL);
}


