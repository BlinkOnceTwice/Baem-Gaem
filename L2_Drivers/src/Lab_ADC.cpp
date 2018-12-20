/*
 * Lab_ADC.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: Neeraj Dhavale
 */

#include "Lab_ADC.hpp"

uint8_t ADC_BIT_SET = 12;


void LabAdc::AdcInitBurstMode()
{
    LPC_SC->PCONP |= (1<<ADC_BIT_SET);
    LPC_ADC->ADCR |= (1<<21);
    LPC_ADC->ADCR &= ~((1<<24)|(1<<25)|(1<<26));
    LPC_SC->PCLKSEL0 &= ~((1<<24)|(1<<25));
    LPC_ADC->ADCR |= (0x08<<8);
    LPC_ADC->ADCR |= (1<<16);
}

void LabAdc::AdcSelectPin(LabAdc::Pin pin)
{
    if(pin == LabAdc::k0_25)
    {
        LPC_ADC->ADCR |= (1<<2);
        //LPC_PINCON->PINSEL1 |= ((1<<18));
    }

    else if(pin == LabAdc::k0_26)
    {
        LPC_ADC->ADCR |= (1<<3);
        LPC_PINCON->PINSEL1 |= ((1<<20));
        //LPC_PINCON->PINSEL1 &= ~((1<<21));
    }

    else if(pin == LabAdc::k1_30)
    {
        LPC_ADC->ADCR |= (1<<4);
        LPC_PINCON->PINSEL3 |= ((1<<28)| (1<<29));
    }

    else if(pin == LabAdc::k1_31)
    {
        LPC_ADC->ADCR |= (1<<5);
        LPC_PINCON->PINSEL3 |= ((1<<30)| (1<<31));
    }
    else
    {
        ;
    }
}

float LabAdc::ReadAdcVoltageByChannel(uint8_t channel)
{
    switch(channel)
    {
        case 2:
            return LPC_ADC->ADDR2;
            break;
        case 3:
            return LPC_ADC->ADDR3;
            break;
        case 4:
            return LPC_ADC->ADDR4;
            break;
        case 5:
            return LPC_ADC->ADDR5;
            break;
        default:
            return float(0x0000);
    }
}


long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
