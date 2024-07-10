/*
*   synthesizer.c
*
*   This is a system AVR application that implements a synthesizer by writing to the 
*   sound I/O peripheral of the system.  It allows the control of the oscillator 
*   frequencies, which ocillators will be modulated, the modulation depth, and which 
*   oscillators will be mixed into the output signal.  
*
*   Frequencies are controlled by the buttons and the keypad as follows:
*     - button UP selects VCO1
*     - button LEFT selects VCO2
*     - button DOWN selects the noise oscillator
*     - button RIGHT selects the LFO
*   In order to specify a frequency, press the button corresponding to the desired
*   oscillator then use the keypad to select the frequency. 
*
*   The mixer, LFO depth and which oscillators will be modulated is controlled as follows:
*     - sw[2:0] selects which oscillators will be modulated {noise, VCO2, VCO1}
*     - sw[7:5] selects the modulation depth
*     - sw[15:12] selects with oscillators will be mixed to the output signal: {LFO, noise, VCO2, VCO1}
*
*   The values of each oscillator's frequency is shown on the display as follows:
*     - display 0: VCO1
*     - display 1: VC02
*     - display 2: noise oscillator
*     - display 3: LFO
*
*   Test procedure:
*       1. Press UP.
*       2. Press 8.
*       3. Press LEFT.
*       4. Press 1.
*       5. Press RIGHT.
*       6. Press 9.
*       7. Turn on switches 0, 1, 5, 6, 12, 13.
*       8. A warbling sound should be heard.
*/

#include <stdlib.h>
#include "../../include/avr_b3.h"
#include "../../include/avr_b3_stdio.h"
#include "../../include/avr_b3_console.h"

int main(void)
{
    uint8_t value, keyVal = KEY_NONE, butVal = BUTTON_NONE;
    uint16_t switches;
    
    stdout = &mystdout;

    // set UART baud rate to 115200
    UBRR0 = 54-1;

    // init displays
    DISPLAY0 = 0;
    DISPLAY1 = 0;
    DISPLAY2 = 0;
    DISPLAY3 = 0;
    DP = DP_NONE;
            
    while (1)
    {
        // cache the switches
        switches = SW;

        // select oscillators to be mixed (SW15..SW12)
        MIXER = (switches & 0xf000) >> 12;
        
        // modulation depth (SW7..SW5)
        MOD_DEPTH = (switches & 0x00e0) >> 5;
        
        // select oscillators to be modulated (SW2..SW0)
        MOD_SEL = switches & 0x0007;
        
        // choose the current osc
        value = ReadButtons(false, 0, 0);
        if (value)
        {
            butVal = value;
            
            // indicate the current oscillator by turning on the DP for that display
            switch (butVal)
            {
                // VCO1
                case BUTTON_U:
                    DP = (1<<DP0);
                    break;
                    
                // VCO2
                case BUTTON_L:
                    DP = (1<<DP1);
                    break;
                    
                // noise
                case BUTTON_D:
                    DP = (1<<DP2);
                    break;
                    
                // LFO
                case BUTTON_R:
                    DP = (1<<DP3);
                    break;
                                
                default:
                    break;
            }
        }
        
        // choose the oscillator frequency
        value = ReadKeypad(false, 0, 0);
        if (value)
        {
            keyVal = value & 0x000f;
            switch (butVal)
            {
                // VCO1
                case BUTTON_U:
                    DISPLAY0 = keyVal;
                    VCO1_FREQ = keyVal << 5;
                    break;
                    
                // VCO2
                case BUTTON_L:
                    DISPLAY1 = keyVal;
                    VCO2_FREQ = keyVal << 5;
                    break;
                    
                // noise
                case BUTTON_D:
                    DISPLAY2 = keyVal;
                    NOISE_FREQ = keyVal << 3;
                    break;
                    
                // LFO
                case BUTTON_R:
                    DISPLAY3 = keyVal;
                    LFO_FREQ = keyVal << 5;
                    break;
                                
                default:
                    break;
            }
        }

//#define VERBOSE
#ifdef VERBOSE        
        // display reg values on the console
        printf("vco1 freq = %x, vco2 freq = %x, noise freq = %x, lfo freq = %x\r\n", VCO1_FREQ, VCO2_FREQ, NOISE_FREQ, LFO_FREQ);
        msleep(50);
#endif        
    }
        
    return(0);
}

