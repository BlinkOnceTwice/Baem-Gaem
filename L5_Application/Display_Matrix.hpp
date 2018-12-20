/*
 * Display_Matrix.hpp
 *
 *  Created on: Nov 7, 2018
 *      Author: Neeraj Dhavale
 */

#ifndef DISPLAY_MATRIX_HPP_
#define DISPLAY_MATRIX_HPP_


#include <stdio.h>
#include "FreeRTOS.h"
#include "gpio.hpp"
#include "utilities.h"
#include "string.h"


class Display_Matrix
{
public:

    void init();
    void setEn(bool set);
    void setLatch(bool set);
    void drawStartScreen();
    void updateDisplay();
    void clearScreen();
    void selRow(uint8_t row);
    void fillBuffer();

private:

    bool onePlayer;
    GPIO *addrA, *addrB, *addrC, *addrD, *latch, *oe, *clk, *r1, *g1, *b1, *r2, *g2, *b2;
    uint32_t frameBuffer[32][3] = {{0}}; //32 rows, 3 "color" columns, each entry is 32 bits for each row pixel
};


#endif /* DISPLAY_MATRIX_HPP_ */
