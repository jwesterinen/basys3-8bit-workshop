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
*/

#include "../../include/avr_b3.h"
#include "../../include/avr_b3_lib.h"
#include "../../include/avr_b3_stdio.h"

int main(void)
{
    uint8_t value, keyVal = KEY_NONE, butVal = BUTTON_NONE;
    uint8_t vco1 = 0, vco2 = 0, noise = 0, lfo = 0;
    uint16_t switches;
    
    stdout = &mystdout;

    // set UART baud rate to 115200
    UBRR0 = 13-1;

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

        // init all fields controlled by the switches but retain the frequencies
        vco1 &= FREQ_MASK;
        vco2 &= FREQ_MASK;
        noise &= FREQ_MASK;
        lfo &= FREQ_MASK;
        
        // select oscillators to be mixed (SW15..SW12)
        lfo   |= ((switches & 0x8000) >> 15) << MIXER_SEL;
        noise |= ((switches & 0x4000) >> 14) << MIXER_SEL;
        vco2  |= ((switches & 0x2000) >> 13) << MIXER_SEL;
        vco1  |= ((switches & 0x1000) >> 12) << MIXER_SEL;
        
        // modulation depth (SW7..SW5)
        lfo |= (((switches & 0x00e0) >> 5) << LFO_SHIFT);
        
        // select oscillators to be modulated (SW3..SW0)
        noise |= ((switches & 0x0004) >> 2) << MOD_SEL;
        vco2  |= ((switches & 0x0002) >> 1) << MOD_SEL;
        vco1  |= ((switches & 0x0001) >> 0) << MOD_SEL;
        
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
                    vco1 &= ~FREQ_MASK;
                    vco1 |= keyVal & FREQ_MASK;
                    break;
                    
                // VCO2
                case BUTTON_L:
                    DISPLAY1 = keyVal;
                    vco2 &= ~FREQ_MASK;
                    vco2 |= keyVal & FREQ_MASK;
                    break;
                    
                // noise
                case BUTTON_D:
                    DISPLAY2 = keyVal;
                    noise &= ~FREQ_MASK;
                    noise |= keyVal & FREQ_MASK;
                    break;
                    
                // LFO
                case BUTTON_R:
                    DISPLAY3 = keyVal;
                    lfo &= ~FREQ_MASK;
                    lfo |= keyVal & FREQ_MASK;
                    break;
                                
                default:
                    break;
            }
        }

        // write the regs
        VCO1 = vco1;
        VCO2 = vco2;
        NOISE = noise;
        LFO = lfo;

//#define VERBOSE
#ifdef VERBOSE        
        // display reg values on the console
        printf("vco1 = %x, vco2 = %x, noise = %x, lfo = %x\r\n", vco1, vco2, noise, lfo);
        msleep(500);
#endif        
    }
        
    return(0);
}

