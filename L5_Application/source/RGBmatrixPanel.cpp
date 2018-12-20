/*

This version uses a few tricks to achieve better performance and/or
lower CPU utilization:

- To control LED brightness, traditional PWM is eschewed in favor of
  Binary Code Modulation, which operates through a succession of periods
  each twice the length of the preceding one (rather than a direct
  linear count a PWM).  It's explained well here:

    http://www.batsocks.co.uk/readme/art_bcm_1.html

  I was initially skeptical, but it works exceedingly well in practice!
  And this uses considerably fewer CPU cycles than software PWM.

- Although many control pins are software-configurable in the user's
  code, a couple things are tied to specific PORT registers.  It's just
  a lot faster this way -- port lookups take time.  Please see the notes
  later regarding wiring on "alternative" Arduino boards.

- A tiny bit of inline assembly language is used in the most speed-
  critical section.  The C++ compiler wasn't making optimal use of the
  instruction set in what seemed like an obvious chunk of code.  Since
  it's only a few short instructions, this loop is also "unrolled" --
  each iteration is stated explicitly, not through a control loop.

Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon for
Adafruit Industries.
BSD license, all text above must be included in any redistribution.
 */

#include "RGBmatrixPanel.h"
#include "Adafruit_GFX.h"
#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

// A full PORT register is required for the data lines, though only the
// top 6 output bits are used.  For performance reasons, the port # cannot
// be changed via library calls, only by changing constants in the library.
// For similar reasons, the clock pin is only semi-configurable...it can
// be specified as any pin within a specific PORT register stated below.


// Ports for "standard" boards (Arduino Uno, Duemilanove, etc.)
uint8_t DATAPORT = 0;
#define DATADIR  DDRD
#define CLKPORT  PORTB
#define nPlanes 4

#define CALLOVERHEAD 60   // Actual value measured = 56
#define LOOPTIME     100


uint32_t count =0 ;

// The fact that the display driver interrupt stuff is tied to the
// singular Timer1 doesn't really take well to object orientation with
// multiple RGBmatrixPanel instances.  The solution at present is to
// allow instances, but only one is active at any given time, via its
// begin() method.  The implementation is still incomplete in parts;
// the prior active panel really should be gracefully disabled, and a
// stop() method should perhaps be added...assuming multiple instances
// are even an actual need.


#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

static RGBmatrixPanel *activePanel = NULL;

void IRQ_HANDLER() {
    activePanel->updateDisplay();   // Call refresh func for active display
}

// Code common to both the 16x32 and 32x32 constructors:
void RGBmatrixPanel::init(uint8_t rows, bool dbuf, uint8_t width)
{
    nRows = rows; // Number of multiplexed rows; actual height is 2X this
    // Allocate and initialize matrix buffer:
    int buffsize  = width * nRows * 3, // x3 = 3 bytes holds 4 planes "packed"
            allocsize = (dbuf == true) ? (buffsize * 2) : buffsize;
    if(NULL == (matrixbuff[0] = (uint8_t *)malloc(allocsize))) return;
    memset(matrixbuff[0], 0, allocsize);
    // If not double-buffered, both buffers then point to the same address:
    matrixbuff[1] = (dbuf == true) ? &matrixbuff[0][buffsize] : matrixbuff[0];

    /*   addrA = new GPIO(P2_7);
    addrB = new GPIO(P1_28);
    addrC = new GPIO(P2_2);
    addrD = new GPIO(P1_23);
    latch = new GPIO(P1_22);
    oe = new GPIO(P0_15);
    clk = new GPIO(P0_10);
    //select data lines
    r1 = new GPIO(P2_3);
    g1 = new GPIO(P0_26);
    b1 = new GPIO(P2_4);
    r2 = new GPIO(P2_5);
    g2 = new GPIO(P1_30);
    b2 = new GPIO(P2_6);
    gnd1 = new GPIO(P1_31);
    gnd2 = new GPIO(P1_29);
    gnd3 = new GPIO(P1_20);

    LPC_PINCON->PINSEL3 &= ~(3<<30);
    LPC_PINCON->PINSEL3 &= ~(3<<26);
    LPC_PINCON->PINSEL3 &= ~(3<<8);
    LPC_GPIO1->FIODIR |=(1<<31);
    LPC_GPIO1->FIODIR |=(1<<29);
    LPC_GPIO1->FIODIR |=(1<<20);*/


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


    //configure dirs
    addrA->setAsOutput(); addrB->setAsOutput(); addrC->setAsOutput();
    addrD->setAsOutput(); latch->setAsOutput(); oe->setAsOutput();
    clk->setAsOutput(); r1->setAsOutput(); g1->setAsOutput();
    b1->setAsOutput(); r2->setAsOutput(); g2->setAsOutput();
    b2->setAsOutput();
    gnd1->setAsOutput(); gnd2->setAsOutput(); gnd3->setAsOutput();

    //set default states
    addrA->setLow(); addrB->setLow(); addrC->setLow();
    addrD->setLow(); latch->setLow(); oe->setHigh();
    clk->setLow();  r1->setLow();  g1->setLow();
    b1->setLow(); r2->setLow(); g2->setLow();
    b2->setLow();
    gnd1->setLow();gnd2->setLow();gnd3->setLow();
    plane     = nPlanes - 1;
    row       = nRows   - 1;
    swapflag  = false;
    backindex = 0;     // Array index of back buffer
    printf("init\n");
}

// Constructor for 16x32 panel:


// Constructor for 32x32 or 32x64 panel:
RGBmatrixPanel::RGBmatrixPanel(bool dbuf, uint8_t width) : Adafruit_GFX(width, 32)
{
    init(16,dbuf, width);
    // Init a few extra 32x32-specific elements:

}


void RGBmatrixPanel::begin(void) {

    backindex   = 0;                         // Back buffer
    buffptr     = matrixbuff[1 - backindex]; // -> front buffer
    activePanel = this;
    // For interrupt hander
    rit_enable(IRQ_HANDLER,20);
}

// Original RGBmatrixPanel library used 3/3/3 color.  Later version used
// 4/4/4.  Then Adafruit_GFX (core library used across all Adafruit
// display devices now) standardized on 5/6/5.  The matrix still operates
// internally on 4/4/4 color, but all the graphics functions are written
// to expect 5/6/5...the matrix lib will truncate the color components as
// needed when drawing.  These next functions are mostly here for the
// benefit of older code using one of the original color formats.

// Promote 3/3/3 RGB to Adafruit_GFX 5/6/5
uint16_t RGBmatrixPanel::Color333(uint8_t r, uint8_t g, uint8_t b) {
    // RRRrrGGGgggBBBbb
    return ((r & 0x7) << 13) | ((r & 0x6) << 10) |
            ((g & 0x7) <<  8) | ((g & 0x7) <<  5) |
            ((b & 0x7) <<  2) | ((b & 0x6) >>  1);
}

// Promote 4/4/4 RGB to Adafruit_GFX 5/6/5
uint16_t RGBmatrixPanel::Color444(uint8_t r, uint8_t g, uint8_t b) {
    // RRRRrGGGGggBBBBb
    return ((r & 0xF) << 12) | ((r & 0x8) << 8) |
            ((g & 0xF) <<  7) | ((g & 0xC) << 3) |
            ((b & 0xF) <<  1) | ((b & 0x8) >> 3);
}

// Demote 8/8/8 to Adafruit_GFX 5/6/5
// If no gamma flag passed, assume linear color
uint16_t RGBmatrixPanel::Color888(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (b >> 3);
}

// 8/8/8 -> gamma -> 5/6/5
uint16_t RGBmatrixPanel::Color888(
        uint8_t r, uint8_t g, uint8_t b, bool gflag) {
    if(gflag) { // Gamma-corrected color?
        return ((uint16_t)r << 12) | ((uint16_t)(r & 0x8) << 8) | // 4/4/4->5/6/5
                ((uint16_t)g <<  7) | ((uint16_t)(g & 0xC) << 3) |
                (          b <<  1) | (           b        >> 3);
    } // else linear (uncorrected) color
    return ((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (b >> 3);
}

uint16_t RGBmatrixPanel::ColorHSV(
        long hue, uint8_t sat, uint8_t val, bool gflag) {

    uint8_t  r, g, b, lo;
    uint16_t s1, v1;

    // Hue
    hue %= 1536;             // -1535 to +1535
    if(hue < 0) hue += 1536; //     0 to +1535
    lo = hue & 255;          // Low byte  = primary/secondary color mix
    switch(hue >> 8) {       // High byte = sextant of colorwheel
        case 0 : r = 255     ; g =  lo     ; b =   0     ; break; // R to Y
        case 1 : r = 255 - lo; g = 255     ; b =   0     ; break; // Y to G
        case 2 : r =   0     ; g = 255     ; b =  lo     ; break; // G to C
        case 3 : r =   0     ; g = 255 - lo; b = 255     ; break; // C to B
        case 4 : r =  lo     ; g =   0     ; b = 255     ; break; // B to M
        default: r = 255     ; g =   0     ; b = 255 - lo; break; // M to R
    }

    // Saturation: add 1 so range is 1 to 256, allowig a quick shift operation
    // on the result rather than a costly divide, while the type upgrade to int
    // avoids repeated type conversions in both directions.
    s1 = sat + 1;
    r  = 255 - (((255 - r) * s1) >> 8);
    g  = 255 - (((255 - g) * s1) >> 8);
    b  = 255 - (((255 - b) * s1) >> 8);

    // Value (brightness) & 16-bit color reduction: similar to above, add 1
    // to allow shifts, and upgrade to int makes other conversions implicit.
    v1 = val + 1;
    if(gflag) { // Gamma-corrected color?
        r = pgm_read_byte((r * v1) >> 8); // Gamma correction table maps
        g = pgm_read_byte((g * v1) >> 8); // 8-bit input to 4-bit output
        b = pgm_read_byte((b * v1) >> 8);
    } else { // linear (uncorrected) color
        r = (r * v1) >> 12; // 4-bit results
        g = (g * v1) >> 12;
        b = (b * v1) >> 12;
    }
    return (r << 12) | ((r & 0x8) << 8) | // 4/4/4 -> 5/6/5
            (g <<  7) | ((g & 0xC) << 3) |
            (b <<  1) | ( b        >> 3);
}

void RGBmatrixPanel::drawPixel(int16_t x, int16_t y, uint16_t c) {
    uint8_t r, g, b, bit, limit, *ptr;

    if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;

    switch(rotation) {
        case 1:
            _swap_int16_t(x, y);
            x = WIDTH  - 1 - x;
            break;
        case 2:
            x = WIDTH  - 1 - x;
            y = HEIGHT - 1 - y;
            break;
        case 3:
            _swap_int16_t(x, y);
            y = HEIGHT - 1 - y;
            break;
    }

    // Adafruit_GFX uses 16-bit color in 5/6/5 format, while matrix needs
    // 4/4/4.  Pluck out relevant bits while separating into R,G,B:
    r =  c >> 12;        // RRRRrggggggbbbbb
    g = (c >>  7) & 0xF; // rrrrrGGGGggbbbbb
    b = (c >>  1) & 0xF; // rrrrrggggggBBBBb

    // Loop counter stuff
    bit   = 2;
    limit = 1 << nPlanes;

    if(y < nRows) {
        // Data for the upper half of the display is stored in the lower
        // bits of each byte.
        ptr = &matrixbuff[backindex][y * WIDTH * (nPlanes - 1) + x]; // Base addr
        // Plane 0 is a tricky case -- its data is spread about,
        // stored in least two bits not used by the other planes.
        ptr[WIDTH*2] &= ~0b00000011;           // Plane 0 R,G mask out in one op
        if(r & 1) ptr[WIDTH*2] |=  0b00000001; // Plane 0 R: 64 bytes ahead, bit 0
        if(g & 1) ptr[WIDTH*2] |=  0b00000010; // Plane 0 G: 64 bytes ahead, bit 1
        if(b & 1) ptr[WIDTH]   |=  0b00000001; // Plane 0 B: 32 bytes ahead, bit 0
        else      ptr[WIDTH]   &= ~0b00000001; // Plane 0 B unset; mask out
        // The remaining three image planes are more normal-ish.
        // Data is stored in the high 6 bits so it can be quickly
        // copied to the DATAPORT register w/6 output lines.
        for(; bit < limit; bit <<= 1) {
            *ptr &= ~0b00011100;            // Mask out R,G,B in one op
            if(r & bit) *ptr |= 0b00000100; // Plane N R: bit 2
            if(g & bit) *ptr |= 0b00001000; // Plane N G: bit 3
            if(b & bit) *ptr |= 0b00010000; // Plane N B: bit 4
            ptr  += WIDTH;                 // Advance to next bit plane
        }
    } else {
        // Data for the lower half of the display is stored in the upper
        // bits, except for the plane 0 stuff, using 2 least bits.
        ptr = &matrixbuff[backindex][(y - nRows) * WIDTH * (nPlanes - 1) + x];
        *ptr &= ~0b00000011;                  // Plane 0 G,B mask out in one op
        if(r & 1)  ptr[WIDTH] |=  0b00000010; // Plane 0 R: 32 bytes ahead, bit 1
        else       ptr[WIDTH] &= ~0b00000010; // Plane 0 R unset; mask out
        if(g & 1) *ptr        |=  0b00000001; // Plane 0 G: bit 0
        if(b & 1) *ptr        |=  0b00000010; // Plane 0 B: bit 0
        for(; bit < limit; bit <<= 1) {
            *ptr &= ~0b11100000;            // Mask out R,G,B in one op
            if(r & bit) *ptr |= 0b00100000; // Plane N R: bit 5
            if(g & bit) *ptr |= 0b01000000; // Plane N G: bit 6
            if(b & bit) *ptr |= 0b10000000; // Plane N B: bit 7
            ptr  += WIDTH;                 // Advance to next bit plane
        }
    }
}

void RGBmatrixPanel::fillScreen(uint16_t c) {
    if((c == 0x0000) || (c == 0xffff)) {
        // For black or white, all bits in frame buffer will be identically
        // set or unset (regardless of weird bit packing), so it's OK to just
        // quickly memset the whole thing:
        memset(matrixbuff[backindex], c, WIDTH * nRows * 3);
    } else {
        // Otherwise, need to handle it the long way:
        Adafruit_GFX::fillScreen(c);
    }
}

// Return address of back buffer -- can then load/store data directly
uint8_t *RGBmatrixPanel::backBuffer() {
    return matrixbuff[backindex];
}

// For smooth animation -- drawing always takes place in the "back" buffer;
// this method pushes it to the "front" for display.  Passing "true", the
// updated display contents are then copied to the new back buffer and can
// be incrementally modified.  If "false", the back buffer then contains
// the old front buffer contents -- your code can either clear this or
// draw over every pixel.  (No effect if double-buffering is not enabled.)
void RGBmatrixPanel::swapBuffers(bool copy) {
    if(matrixbuff[0] != matrixbuff[1]) {
        // To avoid 'tearing' display, actual swap takes place in the interrupt
        // handler, at the end of a complete screen refresh cycle.
        swapflag = true;                  // Set flag here, then...
        while(swapflag == true) delay_ms(1); // wait for interrupt to clear it
        if(copy == true)
            memcpy(matrixbuff[backindex], matrixbuff[1-backindex], WIDTH * nRows * 3);
    }
}

// Dump display contents to the Serial Monitor, adding some formatting to
// simplify copy-and-paste of data as a PROGMEM-embedded image for another
// sketch.  If using multiple dumps this way, you'll need to edit the
// output to change the 'img' name for each.  Data can then be loaded
// back into the display using a pgm_read_byte() loop.
void RGBmatrixPanel::dumpMatrix(void) {

    //    int i, buffsize = WIDTH * nRows * 3;
    //
    //    printf(("\n\n"
    //            "#include <avr/pgmspace.h>\n\n"
    //            "static const uint8_t PROGMEM img[] = {\n  "));
    //
    //    for(i=0; i<buffsize; i++) {
    //        printf(("0x"));
    //        if(matrixbuff[backindex][i] < 0x10) write('0');
    //        //        printf(matrixbuff[backindex][i],HEX);
    //        if(i < (buffsize - 1)) {
    //            if((i & 7) == 7) printf((",\n  "));
    //            else             write(',');
    //        }
    //    }
    //    printf(("\n};"));
}

// -------------------- Interrupt handler stuff --------------------





// The "on" time for bitplane 0 (with the shortest BCM interval) can
// then be estimated as LOOPTIME + CALLOVERHEAD * 2.  Each successive
// bitplane then doubles the prior amount of time.  We can then
// estimate refresh rates from this:
// 4 bitplanes = 320 + 640 + 1280 + 2560 = 4800 ticks per row.
// 4800 ticks * 16 rows (for 32x32 matrix) = 76800 ticks/frame.
// 16M CPU ticks/sec / 76800 ticks/frame = 208.33 Hz.
// Actual frame rate will be slightly less due to work being done
// during the brief "LEDs off" interval...it's reasonable to say
// "about 200 Hz."  The 16x32 matrix only has to scan half as many
// rows...so we could either double the refresh rate (keeping the CPU
// load the same), or keep the same refresh rate but halve the CPU
// load.  We opted for the latter.
// Can also estimate CPU use: bitplanes 1-3 all use 320 ticks to
// issue data (the increasing gaps in the timing invervals are then
// available to other code), and bitplane 0 takes 920 ticks out of
// the 2560 tick interval.
// 320 * 3 + 920 = 1880 ticks spent in interrupt code, per row.
// From prior calculations, about 4800 ticks happen per row.
// CPU use = 1880 / 4800 = ~39% (actual use will be very slightly
// higher, again due to code used in the LEDs off interval).
// 16x32 matrix uses about half that CPU load.  CPU time could be
// further adjusted by padding the LOOPTIME value, but refresh rates
// will decrease proportionally, and 200 Hz is a decent target.

// The flow of the interrupt can be awkward to grasp, because data is
// being issued to the LED matrix for the *next* bitplane and/or row
// while the *current* plane/row is being shown.  As a result, the
// counter variables change between past/present/future tense in mid-
// function...hopefully tenses are sufficiently commented.

void RGBmatrixPanel::updateDisplay(void) {
    uint8_t  i ,*ptr;
    uint16_t t, duration;

    count++;
    if(count == 64000)
    {
        puts("update display");
        printf("plane:%d\n",plane);
        count = 0;
    }

    oe->setHigh();// Disable LED output during row/plane switchover
    latch->setHigh();// Latch data loaded during *prior* interrupt
    clk->setLow();
    // Calculate time to next interrupt BEFORE incrementing plane #.
    // This is because duration is the display time for the data loaded
    // on the PRIOR interrupt.  CALLOVERHEAD is subtracted from the
    // result because that time is implicit between the timer overflow
    // (interrupt triggered) and the initial LEDs-off line at the start
    // of this method.
    //    t = (nRows > 8) ? LOOPTIME : (LOOPTIME * 2);
    //    duration = ((t + CALLOVERHEAD * 2) << plane) - CALLOVERHEAD;

    // Borrowing a technique here from Ray's Logic:
    // www.rayslogic.com/propeller/Programming/AdafruitRGB/AdafruitRGB.html
    // This code cycles through all four planes for each scan-line before
    // advancing to the next line.  While it might seem beneficial to
    // advance lines every time and interleave the planes to reduce
    // vertical scanning artifacts, in practice with this panel it causes
    // a green 'ghosting' effect on black pixels, a much worse artifact.

    if(++plane >= nPlanes) {      // Advance plane counter.  Maxed out?
        plane = 0;                  // Yes, reset to plane 0, and
        if(++row >= nRows) {        // advance row counter.  Maxed out?
            row     = 0;              // Yes, reset row counter, then...
            if(swapflag == true) {    // Swap front/back buffers if requested
                backindex = 1 - backindex;
                swapflag  = false;
            }
            buffptr = matrixbuff[1-backindex]; // Reset into front buffer
        }
    } else if(plane == 1) {
        // Plane 0 was loaded on prior interrupt invocation and is about to
        // latch now, so update the row address lines before we do that:
        if(row & 0x1)   addrA->setHigh();
        else            addrA->setLow();
        if(row & 0x2)   addrB->setHigh();
        else            addrB->setLow();
        if(row & 0x4)   addrC->setHigh();
        else            addrC->setLow();
        if(nRows > 8) {
            if(row & 0x8) addrD->setHigh();
            else          addrD->setLow();
        }
    }

    // buffptr, being 'volatile' type, doesn't take well to optimization.
    // A local register copy can speed some things up:
    ptr = (uint8_t *)buffptr;

    oe->setLow();// Re-enable output
    latch->setLow();// Latch down

    // Record current state of CLKPORT register, as well as a second
    // copy with the clock bit set.  This makes the innnermost data-
    // pushing loops faster, as they can just set the PORT state and
    // not have to load/modify/store bits every single time.  It's a
    // somewhat rude trick that ONLY works because the interrupt
    // handler is set ISR_BLOCK, halting any other interrupts that
    // might otherwise also be twiddling the port at the same time
    // (else this would clobber them). only needed for AVR's where you
    // cannot set one bit in a single instruction

    if(plane > 0)
    { // 188 ticks from TCNT1=0 (above) to end of function
        // Planes 1-3 copy bytes directly from RAM to PORT without unpacking.
        // The least 2 bits (used for plane 0 data) are presumed masked out
        // by the port direction bits.

        // Loop is unrolled for speed:

        //        if(WIDTH == 64) {
        //
        //        }
        for (i=0; i < WIDTH; i++) {
            (ptr[i] & 0x04) ? r1->setHigh() : r1->setLow();      //R1
            (ptr[i] & 0x08) ? g1->setHigh() : g1->setLow();       //G1
            (ptr[i] & 0x10) ? b1->setHigh() : b1->setLow();       //B1
            (ptr[i] & 0x20) ? r2->setHigh() : r2->setLow();       //R2
            (ptr[i] & 0x40) ? g2->setHigh() : g2->setLow();       //G2
            (ptr[i] & 0x80) ? b2->setHigh() : b2->setLow();   //B2
            clk->setHigh(); // Clock lo
            clk->setLow(); // Clock hi

        }
        //buffptr = ptr; //+= 32;
        buffptr += WIDTH;
    }

    else
    {
        for(i=0; i<WIDTH; i++) {
            DATAPORT =
                    ( ptr[i]         << 6)         |
                    ((ptr[i+WIDTH]   << 4) & 0x30) |
                    ((ptr[i+WIDTH*2] << 2) & 0x0C);
            (DATAPORT & 0x04) ? r1->setHigh() : r1->setLow();      //R1
            (DATAPORT & 0x08) ? g1->setHigh() : g1->setLow();       //G1
            (DATAPORT & 0x10) ? b1->setHigh() : b1->setLow();       //B1
            (DATAPORT & 0x20) ? r2->setHigh() : r2->setLow();       //R2
            (DATAPORT & 0x40) ? g2->setHigh() : g2->setLow();       //G2
            (DATAPORT & 0x80) ? b2->setHigh() : b2->setLow();       //B2
            clk->setHigh(); // Clock lo
            clk->setLow(); // Clock hi
        }
    }

}


