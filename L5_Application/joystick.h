/*
 * joystick.h
 *
 *  Created on: Nov 23, 2018
 *      Author: Bharat & Vignesh
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_
#include<stdio.h>
#include "LPC17xx.h"
#include "wireless.h"
#include "nrf_stream.hpp"


class joystick
{
public:
    enum directions
    {
        nothing,
        up,
        down,
        left,
        right,
        reset_press,
    };


void initialize();

void joystick_actions();

void AccelInput();


private:

uint16_t x_axis, y_axis;

};



#endif /* JOYSTICK_H_ */
