
#include "joystick.h"
#include "LPC17xx.h"
#include "freeRTOS.h"
#include<stdio.h>
#include"utilities.h"
#include"task.h"
#include "io.hpp"


// x_axis pin number is 1.30
//y_axis pin number is 1.31

void joystick::initialize()
{
    LPC_SC->PCONP |= (1<<12); //Power
    LPC_ADC->ADCR |= (1<<21); //ADC is operational | PDN is 1
    LPC_SC->PCLKSEL0 &= ~(3<<24); //PCLK set to 00, clk will be 24 MHZ
    LPC_ADC->ADCR &= ~(0xFF<<8);
    LPC_ADC->ADCR |= (1<<9); //clock set to 12MHZ
    LPC_ADC->ADCR &= ~(7<<24); //Start bits 000
    LPC_ADC->ADCR |=(3<<4); //Select channel 4 & 5
    LPC_PINCON->PINSEL3 |= (0XF<<28);
    LPC_ADC->ADCR |= (1<<16); //Burst mode
    LPC_PINCON->PINSEL1 &= ~(1<<26)|(1<<27);  // making 0.29 as GPIO
    LPC_GPIO0->FIODIR &= ~(1<<29);  // making 0.29 as input
}

void joystick::joystick_actions()
{
    uint8_t var1;
    if(!(LPC_GPIO0->FIOPIN & (1 << 29)))
    {
        printf("Pressed\n");
        var1=directions::reset_press;
        wireless_send(150, mesh_pkt_nack, &var1, 1, 0);
        delay_ms(250);
    }
    /*Configure ADC for burst mode
     * */
    x_axis = LPC_ADC->ADDR4;
    x_axis = x_axis>>4;
    x_axis = x_axis&0x0FFF;
    y_axis = LPC_ADC->ADDR5;
    y_axis = y_axis>>4;
    y_axis = y_axis&0x0FFF;
    printf("x= %d\n",x_axis);
    printf("y= %d\n",y_axis);
    if(x_axis>3800)
    {
        printf("Right\n");
        var1 = directions::right;
        wireless_send(150, mesh_pkt_nack, &var1, 1, 0); //send value to receiver
    }
    else if(x_axis<200)
    {
        printf("Left\n");
        var1 = directions::left;
        wireless_send(150, mesh_pkt_nack, &var1, 1, 0);
    }
    else if(y_axis>3800)

    {
        printf("Up\n");
        var1 = directions::up;
        wireless_send(150, mesh_pkt_nack, &var1, 1, 0);
    }
    else if(y_axis<200)
    {
        printf("Down\n");
        var1 = directions::down;
        wireless_send(150, mesh_pkt_nack, &var1, 1, 0);
    }
    else
    {
        var1 = directions::nothing;
        wireless_send(150, mesh_pkt_nack, &var1, 1, 0);
    }
}


void joystick::AccelInput()
{
    int16_t Ax = AS.getX();
    int16_t Ay = AS.getY();
    int16_t Az = AS.getZ();
    uint8_t var1;
    printf("Before queue send\n");
    printf("Ax: %d\nAy:%d\nAz:%d\n",Ax,Ay,Az);

    if((Ax <= -900))//&& (Ay<100))
    {
        printf("Left\n");
        var1 = directions::left;
        wireless_send(150, mesh_pkt_nack, &var1, 1, 0);
    }

    else if((Ax >= 900))//&&(Ay<100))
    {
        printf("Right\n");
        var1 = directions::right;
        wireless_send(150, mesh_pkt_nack, &var1, 1, 0);
    }

    else if(Ay>900)
    {
        printf("Up\n");
        var1 = directions::up;
        wireless_send(150, mesh_pkt_nack, &var1, 1, 0);
    }

    else if(Ay< -900)
    {

        printf("Down\n");
        var1 = directions::down;
        wireless_send(150, mesh_pkt_nack, &var1, 1, 0);
    }
    else if((Ax > -50)&&(Az > 0))
    {
        var1 = directions::nothing;
        wireless_send(150, mesh_pkt_nack, &var1, 1, 0);
    }
    else
    {}
}

