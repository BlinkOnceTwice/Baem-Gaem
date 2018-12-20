#ifndef RGBMATRIXPANEL_H
#define RGBMATRIXPANEL_H

#include "Adafruit_GFX.h"
#include "gpio.hpp"
#include "utilities.h"
#include "lpc_rit.h"

typedef uint32_t PortType; // Formerly 'RwReg' but interfered w/CMCIS header

class RGBmatrixPanel : public Adafruit_GFX {

 public:

    uint16_t Delay;
  // Constructor for 16x32 panel:
    RGBmatrixPanel();
//    virtual ~RGBmatrixPanel()
//      {
//
//      }
  // Constructor for 32x32 panel (adds 'd' pin):
  RGBmatrixPanel(bool dbuf, uint8_t width=32);

  void
    begin(void),
    drawPixel(int16_t x, int16_t y, uint16_t c),
    fillScreen(uint16_t c),
    updateDisplay(void),
    swapBuffers(bool),
    dumpMatrix(void);
  uint8_t
    *backBuffer(void);
  uint16_t
    Color333(uint8_t r, uint8_t g, uint8_t b),
    Color444(uint8_t r, uint8_t g, uint8_t b),
    Color888(uint8_t r, uint8_t g, uint8_t b),
    Color888(uint8_t r, uint8_t g, uint8_t b,     bool gflag),
    ColorHSV(long hue,  uint8_t sat, uint8_t val, bool gflag);

 private:

  uint8_t         *matrixbuff[2];
  uint8_t          nRows;
  volatile uint8_t backindex;
  volatile bool    swapflag;

  // Init/alloc code common to both constructors:
  void init(uint8_t rows,bool dbuf,uint8_t width);


  GPIO *addrA, *addrB, *addrC, *addrD, *latch, *oe, *clk, *r1, *g1, *b1, *r2, *g2, *b2,*gnd1,*gnd2,*gnd3;
  // Counters/pointers for interrupt handler:
  volatile uint8_t row, plane;
  volatile uint8_t *buffptr;
};

#endif // RGBMATRIXPANEL_H
