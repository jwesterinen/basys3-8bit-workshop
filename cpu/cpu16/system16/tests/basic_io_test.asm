/*  basic_io.asm
 *  
 *  This program tests the basic I/O of the system16 as follows:
 *   - the switches are shown in the LEDs
 *   - the buttons are shown in the display segments
 *
 */ 

#include "../system16.h"

Begin:
    mov ax,#1                   // set display mode to raw
    mov DISPLAY_CTRL_REG,ax     
    
Loop:
    //mov ax,SWITCH_REG           // store switch values @ 0x0100
    //mov 0x100,ax
    mov   bx,@SWITCH_REG
    mov   ax,[bx]
    
    //mov ax,0x0100               // load LEDs from 0x0100
    //mov LED_REG,ax    
    mov   bx,@LED_REG
    mov   [bx],ax

    mov ax,BUTTON_REG           // store button values @ 0x0101
    mov 0x101,ax

    mov ax,0x0101               // load all displays from 0x0101
    mov bx,@DISPLAY1_REG
    mov [bx],ax
    inc bx
    mov [bx],ax
    inc bx
    mov [bx],ax
    inc bx
    mov [bx],ax
    
    jmp Loop

