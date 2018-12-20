/*
 * Lab_UART.hpp
 *
 *  Created on: Oct 6, 2018
 *      Author: Neeraj Dhavale
 */

#ifndef LAB_UART_HPP_
#define LAB_UART_HPP_

#include "LPC17xx.h"

typedef enum
{
    uart0 = 0,
    uart1 = 1,
    uart2 = 2,
    uart3 = 3,
}uart_no;

class LabUart
{
public:
    LabUart(uart_no num);

    // TODO: Fill in methods for Initialize(), Transmit(), Receive() etc.

        void set_baurd_rate_hz(uint32_t rate);
        bool tx_data(char ch);
        bool rx_queue_data(char *ch);


    private:
        uint8_t uart_type;
        uint32_t baurd_rate_hz=9600;
};



#endif /* LAB_UART_HPP_ */
