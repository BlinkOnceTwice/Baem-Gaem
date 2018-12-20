/*
 * Display_Matrix.cpp
 *
 *  Created on: Nov 7, 2018
 *      Author: Neeraj Dhavale
 */


#include "Display_Matrix.hpp"
#include "io.hpp"


//frame data for our start screen
uint32_t startScreenImg[32][3] =
{{0, 0, 0}, {0,0,0}, {0x0, 0x0, 0x55000}, {0,0,0xAA800},
 {0,0,0xAA800}, {0,0,0xAA800}, {0,0,0xAA800}, {0,0,0x80800},
 {0,0,0x140800}, {0,0,0x140800}, {0,0,0x100800}, {0,0,0xFF000},
 {0,0,0}, {0x3C000000,0x0000003C,0}, {0x43800000,0x00003FC2,0}, {0x4C400000,0x00004032,0},
 {0x47800000,0x3FE2,0}, {0x40400000,0x4002,0}, {0x47800000,0x3FE2,0}, {0x40400000,0x0202,0},
 {0x47800000,0x01E2,0}, {0x40400000,0x202,0}, {0x3F800000,0x1FC,0}, {0,0,0},
 {0,0,0}, {0x7DF7BEF8,0,0x7DF7BEF8}, {0x4044A220,0,0x4044A220}, {0x7C47AE20,0,0x7C47AE20},
 {0x0444A420,0,0x0444A420}, {0x7C44A220,0,0x7C44A220}, {0,0,0}, {0,0,0}
};

uint32_t roundImg[5] = {0x38C94B80, 0x25296A40, 0x39295A40, 0x29294A40, 0x24C64B80};

uint32_t testImg[32][3] = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},
        {0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},
        {0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},
        {0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},
        {0, 0, 0},{0, 0, 0},
};



//arrow
uint8_t arrowImg[3] = {0x2, 0x3, 0x2};


void Display_Matrix::init(){

    //select control lines
    addrA = new GPIO(P0_0);
    addrB = new GPIO(P0_1);
    addrC = new GPIO(P2_6);
    addrD = new GPIO(P2_7);
    latch = new GPIO(P1_29);
    oe = new GPIO(P1_28);
    clk = new GPIO(P1_19);
    //select data lines
    r1 = new GPIO(P1_22);
    g1 = new GPIO(P0_26);
    b1 = new GPIO(P1_23);
    r2 = new GPIO(P1_30);
    g2 = new GPIO(P1_31);
    b2 = new GPIO(P2_5);
    //switch for testing state
    SW.init();

    //configure dirs
    addrA->setAsOutput(); addrB->setAsOutput(); addrC->setAsOutput();
    addrD->setAsOutput(); latch->setAsOutput(); oe->setAsOutput();
    clk->setAsOutput(); r1->setAsOutput(); g1->setAsOutput();
    b1->setAsOutput(); r2->setAsOutput(); g2->setAsOutput();
    b2->setAsOutput();

    //set default states
    addrA->setLow(); addrB->setLow(); addrC->setLow();
    addrD->setLow(); latch->setLow(); oe->setHigh();
    clk->setLow();  r1->setLow();  g1->setLow();
    b1->setLow(); r2->setLow(); g2->setLow();
    b2->setLow();
    puts("init complete");
}

void Display_Matrix::setEn(bool set)
{
    (set) ? oe->setHigh() : oe->setLow();
}

void Display_Matrix::setLatch(bool set)
{
    (set) ? latch->setHigh() : latch->setLow();
}

void Display_Matrix::drawStartScreen()
{
    //TODO: hook up PCB's START switch to Port 0 or 2 external interrupt and have ISR give a semaphore that will exit this loop
    //while(!SW.getSwitch(1))
    //{
        //copy startScreenImg into our frameBuffer that will be drawn by updateDisplay
        memcpy(frameBuffer, testImg, sizeof(frameBuffer));
////        //dot animation
//        frameBuffer[29][0] &= ~(0xF);
//        frameBuffer[29][1] &= ~(0xF);
//        frameBuffer[29][2] &= ~(0xF);
//        delay_ms(25);
//        frameBuffer[29][0] |= (0x8);
//        frameBuffer[29][1] |= (0x8);
//        frameBuffer[29][2] |= (0x8);
//        delay_ms(25);
//        frameBuffer[29][0] |= (0xC);
//        frameBuffer[29][1] |= (0xC);
//        frameBuffer[29][2] |= (0xC);
//        delay_ms(25);
//        frameBuffer[29][0] |= (0xE);
//        frameBuffer[29][1] |= (0xE);
//        frameBuffer[29][2] |= (0xE);
//        delay_ms(25);

//        for(int i = 25; i < 30; i++)
//        {
//            frameBuffer[i][0] &= ~(0xFFFFFFFF);
//            frameBuffer[i][1] &= ~(0xFFFFFFFF);
//            frameBuffer[i][2] &= ~(0xFFFFFFFF);
//        }

/*        //draw arrow
        for(int i = 26; i < 29; i++)
        {
            (onePlayer) ? frameBuffer[i][0] |= (arrowImg[i-26] << 29) : frameBuffer[i][0] |= (arrowImg[i-26] << 11);
            (onePlayer) ? frameBuffer[i][1] |= (arrowImg[i-26] << 29) : frameBuffer[i][1] |= (arrowImg[i-26] << 11);
            (onePlayer) ? frameBuffer[i][2] |= (arrowImg[i-26] << 29) : frameBuffer[i][2] |= (arrowImg[i-26] << 11);
        }
            puts("Start Screen\n");

    //}*/
}

void Display_Matrix::updateDisplay()
{
    //only need to iterate up to 16 because we're doing 2 rows at a time
    for(int i = 0; i < 16; i++)
    {
//        setEn(true);
//        setLatch(false);
        selRow(i); //will select i and i+16 rows at same time
        //this inner loop will shift MSB of frameBuffer[i] into corresponding shift register
        for(int j = 31; j >= 0; j--)
        {
            ((frameBuffer[i][0] >> j) & 1) ? r1->setHigh() : r1->setLow(); //shift in r1 data with msb getting shifted in first
            ((frameBuffer[i][1] >> j) & 1) ? g1->setHigh() : g1->setLow(); //g1 data
            ((frameBuffer[i][2] >> j) & 1) ? b1->setHigh() : b1->setLow(); //b1 data

            //bottom half for corresponding row; rows 0/16, 1/17 etc
            ((frameBuffer[i + 16][0] >> j) & 1) ? r2->setHigh() : r2->setLow(); //r2
            ((frameBuffer[i + 16][1] >> j) & 1) ? g2->setHigh() : g2->setLow(); //g2
            ((frameBuffer[i + 16][2] >> j) & 1) ? b2->setHigh() : b2->setLow(); //b2
            clk->setHigh(); clk->setLow();  //shift in all 3 color bits at once for top half/bottom half regs
        }

        printf("i =%d\n",i);
        if (i == 15)
        {
            printf("Nci");
            clearScreen();
            delay_ms(2000);
        }
        //at this point, all 3 shift registers should be filled with corresponding row data in frameBuffer
//        setEn(true); //turn off display before updating output registers
//        setLatch(true); //push shift register contents down to output registers
//        setEn(false); //turn on display
        delay_ms(800); //let the row display for a small time
    }
}

void Display_Matrix::selRow(uint8_t row)
{
    switch(row)
    {
        case 0: //0000
        case 16:
        {
            addrA->setLow(); addrB->setLow();
            addrC->setLow(); addrD->setLow();
            break;
        }

        case 1: //0001
        case 17:
        {
            addrA->setHigh(); addrB->setLow();
            addrC->setLow(); addrD->setLow();
            break;
        }
        case 2: //0010
        case 18:
        {
            addrA->setLow(); addrB->setHigh();
            addrC->setLow(); addrD->setLow();
            break;
        }
        case 3: //0011
        case 19:
        {
            addrA->setHigh(); addrB->setHigh();
            addrC->setLow();  addrD->setLow();
            break;
        }
        case 4: //0100
        case 20:
        {
            addrA->setLow(); addrB->setLow();
            addrC->setHigh();addrD->setLow();
            break;
        }
        case 5: //0101
        case 21:
        {
            addrA->setHigh(); addrB->setLow();
            addrC->setHigh(); addrD->setLow();
            break;
        }
        case 6: //0110
        case 22:
        {
            addrA->setLow(); addrB->setHigh();
            addrC->setHigh();addrD->setLow();
            break;
        }
        case 7: //0111
        case 23:
        {
            addrA->setHigh(); addrB->setHigh();
            addrC->setHigh(); addrD->setLow();
            break;
        }
        case 8: //1000
        case 24:
        {
            addrA->setLow(); addrB->setLow();
            addrC->setLow(); addrD->setHigh();
            break;
        }
        case 9: //1001
        case 25:
        {
            addrA->setHigh(); addrB->setLow();
            addrC->setLow();  addrD->setHigh();
            break;
        }
        case 10:  //1010
        case 26:
        {
            addrA->setLow(); addrB->setHigh();
            addrC->setLow(); addrD->setHigh();
            break;
        }
        case 11:  //1011
        case 27:
        {
            addrA->setHigh(); addrB->setHigh();
            addrC->setLow();  addrD->setHigh();
            break;
        }
        case 12: //1100
        case 28:
        {
            addrA->setLow(); addrB->setLow();
            addrC->setHigh();addrD->setHigh();
            break;
        }
        case 13:  //1101
        case 29:
        {
            addrA->setHigh(); addrB->setLow();
            addrC->setHigh(); addrD->setHigh();
            break;
        }
        case 14:  //1110
        case 30:
        {
            addrA->setLow(); addrB->setHigh();
            addrC->setHigh();addrD->setHigh();
            break;
        }
        case 15:  //1111
        case 31:
        {
            addrA->setHigh(); addrB->setHigh();
            addrC->setHigh(); addrD->setHigh();
            break;
        }
        default:
        {
            puts("selRow out of bounds");
            return;
            break;
        }

    }
}


void Display_Matrix::clearScreen()
{
    printf("\n clear screen\n");
    //size of one element * # of rows * # of columns
    memset(frameBuffer, 0, sizeof(frameBuffer[0][0]) * 3 * 32);
    updateDisplay();
}

void Display_Matrix::fillBuffer()
{
    //all 3 colors for all 32 rows are filled so should be white
    for(uint8_t i = 0; i < 32; i++)
    {
        frameBuffer[i][0] = 0xFFFFFFFF; //r
        frameBuffer[i][1] = 0x00000000; //g
        frameBuffer[i][2] = 0x00000000; //b
        //printf("Row %i Data: %" PRIu32 "  %" PRIu32 "  %" PRIu32 "\n", i, frameBuffer[i][0], frameBuffer[i][1], frameBuffer[i][2]);
    }

//    r1->setHigh();
//    b2->setHigh();
//    clk->setHigh();
//    delay_us(100);
//    clk->setLow();
}

