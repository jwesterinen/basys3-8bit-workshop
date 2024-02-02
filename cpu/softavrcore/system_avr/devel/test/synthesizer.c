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

#include "../include/system_avr_b3.h"
#include "../include/system_avr_b3_lib.c"

int main(void)
{
    unsigned curFreq = 0;
    unsigned value, keyVal = 0, butVal = BUTTON_NONE;
    
    // init displays
    DISPLAY0 = 0;
    DISPLAY1 = 0;
    DISPLAY2 = 0;
    DISPLAY3 = 0;
            
    while (1)
    {
        // select oscillators to be mixed (SW15..SW12)
        MIXER_SEL = (SW_MSB & 0xf0) >> 4;
        
        // modulation depth (SW7..SW5)
        LFO_MOD_DEPTH = (SW_LSB & 0xe0) >> 5;
        
        // select oscillators to be modulated (SW3..SW0)
        MOD_SEL = (SW_LSB & 0x07);
        
        // choose the current osc
        value = ReadButtons(true);
        if (value)
        {
            butVal = value;
        }
        
        // choose the freq to be played
        value = ReadKeypad(true);
        if (value)
        {
            keyVal = value & 0x000f;
            curFreq = keyVal << 6;
            switch (butVal)
            {
                // VCO1
                case BUTTON_U:
                    DISPLAY0 = keyVal;
                    VCO1_FREQ_LO = curFreq & 0x00ff;
                    VCO1_FREQ_HI = (curFreq & 0xff00) >> 8;
                    break;
                    
                // VCO2
                case BUTTON_L:
                    DISPLAY1 = keyVal;
                    VCO2_FREQ_LO = curFreq & 0x00ff;
                    VCO2_FREQ_HI = (curFreq & 0xff00) >> 8;
                    break;
                    
                // noise
                case BUTTON_D:
                    DISPLAY2 = keyVal;
                    NOISE_FREQ_LO = curFreq & 0x00ff;
                    NOISE_FREQ_HI = (curFreq & 0xff00) >> 8;
                    break;
                    
                // LFO
                case BUTTON_R:
                    DISPLAY3 = keyVal;
                    curFreq = keyVal << 6;
                    LFO_FREQ_LO = curFreq & 0x00ff;
                    LFO_FREQ_LO = (curFreq & 0xff00) >> 8;
                    break;
                                
                default:
                    break;
            }
        }
    }
        
    return(0);
}

