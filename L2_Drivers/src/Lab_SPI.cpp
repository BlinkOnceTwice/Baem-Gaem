/*
 * Lab_SPI.cpp
 *
 *  Created on: Sep 22, 2018
 *      Author: Neeraj Dhavale
 */

#include "Lab_SPI.hpp"



bool LabSpi::initialize(uint8_t data_size_select, FrameModes format, uint8_t divide)
{
    /*Init the exact SPI config to talk to the SPI flash */

    LPC_SC->PCONP |= (1<<10);
    LPC_SC->PCLKSEL1 |=(1<<20);
    LPC_PINCON->PINSEL0 |= (1<<15)|(1<<17)|(1<<19);
    LPC_PINCON->PINSEL0 &= ~((1<<14) | (1<<16) | (1<<18));

    if(divide ==0)
    {
        uart0_puts("0 in divide field not allowed,\nUse only even numbers,\nUse even values less than 254");
        return 0;
    }
    else if(divide%2 != 0)
    {
        uart0_puts("Use only even numbers");
        return 0;
    }
    else if(divide > 254)
    {
        uart0_puts("Use even values less than 254");
        return 0;
    }
    else
    {
        LPC_SSP1->CPSR |= (divide);
    }
    LPC_SSP1->CR0 |= (data_size_select-1)<<0;
    LPC_SSP1->CR0 |= format<<4;
    LPC_SSP1->CR1 |= 2<<0;
    LPC_GPIO0->FIODIR0 |= (1<<6);
    return 1;
}



uint8_t LabSpi::transfer(uint8_t data)
{
    LPC_SSP1->DR = data;
    uint32_t SSP_Status = 0x4003000C;
    //while(!((SSP1_Status_reg(SSP_Status).RNE)&0x01))

    while(!(LPC_SSP1->SR>>2)&0x01)
     {
         ;
     }
     return (LPC_SSP1->DR);
}


void LabSpi::read_sig(uint8_t op_code)
{
  uint8_t d[10];

  // The simplest test is to try to read the signature of the Adesto flash and print it out
  adesto_cs();
  d[0] = transfer(op_code); // TODO: Find what to send to read Adesto flash signature
  d[1] = transfer(0xFF);
  d[2] = transfer(0xFF);
  d[3] = transfer(0xFF);
  d[4] = transfer(0xFF);
  d[5] = transfer(0xFF);
  adesto_ds();

  printf("Returned data: %x %x %x %x %x %x\n", d[0],d[1],d[2],d[3],d[4],d[5]);
}


void LabSpi::adesto_cs(void)
{
    LPC_GPIO0->FIOCLR0 |= (1<<6);
}

void LabSpi::adesto_ds(void)
{
    LPC_GPIO0->FIOSET0 |= (1<<6);
}

