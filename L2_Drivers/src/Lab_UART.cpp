/*
 * Lab_UART.cpp
 *
 *  Created on: Oct 6, 2018
 *      Author: Neeraj Dhavale
 */




#include <stdio.h>

#include "tasks.hpp"
#include "task.h"
#include "utilities.h"
#include "LPC17xx.h"
#include "Lab_UART.hpp"

#define PCLOCK 96000000

QueueHandle_t uart_rx_queue;


void uart2_rx_intr(void)
{
    char data;
    data = LPC_UART2->RBR;
//    Display.setNumber(data);
    xQueueSend(uart_rx_queue, &data, 0);
}

void uart3_rx_intr(void)
{
    char data;
    data = LPC_UART3->RBR;
    xQueueSend(uart_rx_queue, &data, 0);
}

LabUart::LabUart(uart_no num)
{
    printf("In uart_driver\n");
    uart_type = num;
    uart_rx_queue = xQueueCreate(100, sizeof(char));

    switch(uart_type)
    {
        case uart2:
            // 1)Powers up UART2 peripheral
            LPC_SC->PCONP |= 1 << pconp_uart2;
            printf("PCONP = 0x%lx\n",LPC_SC->PCONP);

            // 2)Set peripheral clock rate to CLOCK/1
            LPC_SC->PCLKSEL1 &= ~( 0x3UL << 16 );
            LPC_SC->PCLKSEL1 |= ( 0x1UL << 16 );
            printf("PCLKSEL1 = 0x%lx\n",LPC_SC->PCLKSEL1);

            // 3)Set frame size to 8(pay load)+2(start & stop bit)
            LPC_UART2->LCR |= ( 0x3 << 0 );
            LPC_UART2->LCR &= ~( 0x1 << 2 );
            printf("LCR = 0x%x\n",LPC_UART2->LCR);

            // 4)Enable FIFO and reset it
            LPC_UART2->FCR |= ( 0x1 << 0 ); //Enable FIFO
            LPC_UART2->FCR |= ( 0x1 << 1 ); //Reset RX FIFO
            LPC_UART2->FCR |= ( 0x1 << 2 ); //Reset TX FIFO
            LPC_UART2->FCR &= ~( 0x3 << 6 ); // Set RX interrupt trigger to 1 byte

            // 5)Set PINSEL to select UART2 at PORT2 pin 8&9
            LPC_PINCON->PINSEL4 &= ~((0x3 << 16));
            LPC_PINCON->PINSEL4 |= ((0x2 << 16)); //TXD2
            LPC_PINCON->PINMODE4 &= ~((0x3 << 16)); //pull-up resistor enabled

            LPC_PINCON->PINSEL4 &= ~((0x3 << 18));
            LPC_PINCON->PINSEL4 |= ((0x2 << 18)); //RXD2
            LPC_PINCON->PINMODE4 &= ~((0x3 << 18)); //pull-up resistor enabled

            // 6)Enable UART interrupts
            LPC_UART2->IER |= ( 0x1 << 0 ); //Enables the Receive Data Available interrupt

            // Init UART Rx interrupt (TX interrupt is optional)
            isr_register( UART2_IRQn , uart2_rx_intr);
            NVIC_EnableIRQ(UART2_IRQn);
            break;

        case uart3:
            // 1)Powers up UART3 peripheral
            LPC_SC->PCONP |= 1 << pconp_uart3;
            printf("PCONP = 0x%lx\n",LPC_SC->PCONP);


            // 2)Set peripheral clock rate to CLOCK/1
            LPC_SC->PCLKSEL1 &= ~( 0x3UL << 18 );
            LPC_SC->PCLKSEL1 |= ( 0x1UL << 18 );
            printf("PCLKSEL1 = 0x%lx\n",LPC_SC->PCLKSEL1);


            // 3)Set frame size to 8(pay load)+2(start & stop bit)
            LPC_UART3->LCR |= ( 0x3 << 0 );
            LPC_UART3->LCR &= ~( 0x1 << 2 );
            printf("LCR = 0x%x\n",LPC_UART3->LCR);

            // 4)Enable FIFO and reset it
            LPC_UART3->FCR |= ( 0x1 << 0 ); //Enable FIFO
            LPC_UART3->FCR |= ( 0x1 << 1 ); //Reset RX FIFO
            LPC_UART3->FCR |= ( 0x1 << 2 ); //Reset TX FIFO
            LPC_UART3->FCR &= ~( 0x3 << 6 ); // Set RX interrupt trigger to 1 byte


            // 5)Set PINSEL to select UART at PORT0 pin 25&26
            LPC_PINCON->PINSEL9 |= (0x03<<24); //TXD3
            LPC_PINCON->PINMODE9 &= ~((0x3 << 24)); //pull-up resistor enabled

            LPC_PINCON->PINSEL9 |= (0x03<<26); //RXD2
            LPC_PINCON->PINMODE9 &= ~((0x3 << 26)); //pull-up resistor enabled


            // 6)Enable UART interrupts
            LPC_UART3->IER |= ( 0x1 << 0 ); //Enables the Receive Data Available interrupt


            // Init UART Rx interrupt (TX interrupt is optional)
            isr_register( UART3_IRQn , uart3_rx_intr);
            NVIC_EnableIRQ(UART3_IRQn);
            break;

        default:
            printf("ERROR: unknown uart port\n");
            exit(1);
            break;
    }

    set_baurd_rate_hz(9600);
}

void LabUart::set_baurd_rate_hz(uint32_t rate=9600)
{

    baurd_rate_hz = rate;
    uint32_t cal=0;
    cal = PCLOCK/(rate*16);

    switch(uart_type)
    {
        case uart2:
            LPC_UART2->LCR |= ( 0x1 << 7 ); //enable DLAB reg to access DLL & DLM
            printf("LCR = 0x%x\n",LPC_UART2->LCR);

            LPC_UART2->DLL = cal & 0xFF;
            LPC_UART2->DLM = ( cal >> 8 ) & 0xFF;
            LPC_UART2->LCR &= ~( 0x1 << 7 ); //Disable DLAB reg to access DLL & DLM
            printf("LCR = 0x%x\n",LPC_UART2->LCR);
            break;

        case uart3:

            LPC_UART3->LCR |= ( 0x1 << 7 ); //enable DLAB reg to access DLL & DLM
            printf("LCR = 0x%x\n",LPC_UART3->LCR);

            LPC_UART3->DLL = cal & 0xFF;
            LPC_UART3->DLM = ( cal >> 8 ) & 0xFF;
            LPC_UART3->LCR &= ~( 0x1 << 7 ); //Disable DLAB reg to access DLL & DLM
            printf("LCR = 0x%x\n",LPC_UART3->LCR);

            break;

        default:
            printf("ERROR: unknown uart port\n");
            exit(1);
            break;
    }
}

bool LabUart::tx_data(char data)
{
    bool result=true;

    switch(uart_type)
    {
        case uart2:
            while(!( LPC_UART2->LSR >> 5) & 0x1);
            LPC_UART2->THR = data;
            printf("\nD:%c",data);
            break;
        case uart3:
            while(!( LPC_UART3->LSR >> 5) & 0x1);
            LPC_UART3->THR = data;
            printf("\nD:%c",data);
            break;
        default:
            printf("ERROR: unknown uart port\n");
            result=false;
            break;
    }
    return result;
}

bool LabUart::rx_queue_data(char *data)
{
    bool result = false;
    if ( xQueueReceive( uart_rx_queue, data, 100) )
    {
        result = true;
    }
    return result;
}


/*uint8_t get_char_value(char *c)
{
    uint8_t value,temp_val;
    char oper;

    if((c == '+')||(c=='-')||(c=='*')||(c== '/'))
    {
        oper = c;
    }

    if(c >= 0 && c<= 9)
    {
        temp_val = c - '0';
        return temp_val;
    }

    else
    {
        printf("Incorrect input, enter values from 0-9");
        return -1;
    }
}*/

