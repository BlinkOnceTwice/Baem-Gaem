/*
 * Lab_SPI.hpp
 *
 *  Created on: Sep 22, 2018
 *      Author: Neeraj Dhavale
 */

#ifndef LAB_SPI_HPP_
#define LAB_SPI_HPP_
#include "LPC17xx.h"
#include "uart0_min.h"
#include "stdio.h"

enum FrameModes
{
    /* Fill this out based on the datasheet. */
    SPI,
    TI,
    Microwave,
};

typedef union
{
    uint8_t byte;
    struct
    {
        uint8_t TFE:1;
        uint8_t TNF:1;
        uint8_t RNE:1;
        uint8_t RFF:1;
        uint8_t BSY:1;
        uint32_t Reserved:26;
    } __attribute__((packed));
} SSP1_SR_reg;


#define SSP1_Status_reg(reg) (*((SSP1_SR_reg*)(&reg)))


class LabSpi
{
public:


    /**
     * 1) Powers on SPPn peripheral
     * 2) Set peripheral clock
     * 3) Sets pins for specified peripheral to MOSI, MISO, and SCK
     *
     * @param data_size_select transfer size data width; To optimize the code, look for a pattern in the datasheet
     * @param format is the code format for which synchronous serial protocol you want to use.
     * @param divide is the how much to divide the clock for SSP; take care of error cases such as the value of 0, 1, and odd numbers
     *
     * @return true if initialization was successful
     */
    uint32_t Status_SSP1 = LPC_SSP1->SR;

    bool initialize(uint8_t data_size_select, FrameModes format, uint8_t divide);

    /**
     * Transfers a byte via SSP to an external device using the SSP data register.
     * This region must be protected by a mutex static to this class.
     *
     * @return received byte from external device via SSP data register.
     */
    uint8_t transfer(uint8_t data);

    /*Function to read the SPI data*/
    void read_sig(uint8_t op_code);
    /*Function to select adesto Flash*/
    void adesto_cs(void);
    /*Function to de-select adesto Flash*/
    void adesto_ds(void);

private:

};

#endif /* LAB_SPI_HPP_ */
