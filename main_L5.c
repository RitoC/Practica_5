////+++++++++++++++++++++++++++++++++++++| LIBRARIES / HEADERS |+++++++++++++++++++++++++++++++++++++
#include "device_config.h"
#include <stdint.h>
#include <math.h>

//+++++++++++++++++++++++++++++++++++++| DIRECTIVES |+++++++++++++++++++++++++++++++++++++
#define _XTAL_FREQ 8000000
#define SWEEP_FREQ 20

//+++++++++++++++++++++++++++++++++++++| DATA TYPES |+++++++++++++++++++++++++++++++++++++
enum por_ACDC {digital, analog};          // digital = 0, analog = 1

////+++++++++++++++++++++++++++++++++++++| ISRs |+++++++++++++++++++++++++++++++++++++
//// ISR for high priority
//void __interrupt ( high_priority ) high_isr( void );
//// ISR for low priority
//void __interrupt ( low_priority ) low_isr( void ); 

//+++++++++++++++++++++++++++++++++++++| FUNCTION DECLARATIONS |+++++++++++++++++++++++++++++++++++++
void portsInit(void);
uint8_t char_to_seg(uint8_t);
void send_to_disp(uint32_t);
char key_scanner(void);
//+++++++++++++++++++++++++++++++++++++| MAIN |+++++++++++++++++++++++++++++++++++++
void main(void){
    portsInit();
    uint8_t key;
    uint32_t key_anterior = 0;
    while(1){
        key = key_scanner();
        if (key!= 'x'){
            key_anterior=  key_anterior<< 8;
            key_anterior += key;
        }
        send_to_disp(key_anterior);

        
        
    }
}

//+++++++++++++++++++++++++++++++++++++| FUNCTIONS |+++++++++++++++++++++++++++++++++++++
void portsInit(void){
    ANSELB = digital;   // Set port B as Digital for keypad driving
    TRISB  = 0x0F;      // For Port B, set pins 4 to 7 as inputs (columns), and pins 0 to 3 as outputs (rows)
    ANSELC= digital;   // Set port C as Digital for 7 segment cathode selection (only 4 pins used)  
    TRISC  = 0x00;      // For Port C, set pins as outputs for cathode selection
    ANSELD = digital;   // Set port D as Digital for 7 segment anodes
    TRISD  = 0x00;      // for Port D, set all pins as outputs for 7 segment anodes
    OSCCON = 0x74;      // Set the internal oscillator to 8MHz and stable
    INTCON2 = 0;
}


char key_scanner(void){
        LATBbits.LB4 = 0;
        LATBbits.LB5 = 1;
        LATBbits.LB6 = 1;
        LATBbits.LB7 = 1;
        __delay_ms(SWEEP_FREQ);
        if      (PORTBbits.RB0 == 0) {__delay_ms(SWEEP_FREQ); return 1;}
        else if (PORTBbits.RB1 == 0) {__delay_ms(SWEEP_FREQ); return 4;}
        else if (PORTBbits.RB2 == 0) {__delay_ms(SWEEP_FREQ); return 7;}
        else if (PORTBbits.RB3 == 0) {__delay_ms(SWEEP_FREQ); return 15;}//*
        LATBbits.LB4 = 1;
        LATBbits.LB5 = 0;
        LATBbits.LB6 = 1;
        LATBbits.LB7 = 1;
        __delay_ms(SWEEP_FREQ);
        if      (PORTBbits.RB0 == 0) {__delay_ms(SWEEP_FREQ); return 2;}
        else if (PORTBbits.RB1 == 0) {__delay_ms(SWEEP_FREQ); return 5;}
        else if (PORTBbits.RB2 == 0) {__delay_ms(SWEEP_FREQ); return 8;}
        else if (PORTBbits.RB3 == 0) {__delay_ms(SWEEP_FREQ); return 0;}
        LATBbits.LB4 = 1;
        LATBbits.LB5 = 1;
        LATBbits.LB6 = 0;
        LATBbits.LB7 = 1;
        __delay_ms(SWEEP_FREQ);
        if      (PORTBbits.RB0 == 0) {__delay_ms(SWEEP_FREQ); return 3;}
        else if (PORTBbits.RB1 == 0) {__delay_ms(SWEEP_FREQ); return 6;}
        else if (PORTBbits.RB2 == 0) {__delay_ms(SWEEP_FREQ); return 9;}
        else if (PORTBbits.RB3 == 0) {__delay_ms(SWEEP_FREQ); return 14;}//#
        LATBbits.LB4 = 1;
        LATBbits.LB5 = 1;
        LATBbits.LB6 = 1;
        LATBbits.LB7 = 0;
        __delay_ms(SWEEP_FREQ);
        if      (PORTBbits.RB0 == 0) {__delay_ms(SWEEP_FREQ); return 10;}//A
        else if (PORTBbits.RB1 == 0) {__delay_ms(SWEEP_FREQ); return 11;}//B
        else if (PORTBbits.RB2 == 0) {__delay_ms(SWEEP_FREQ); return 12;}//C
        else if (PORTBbits.RB3 == 0) {__delay_ms(SWEEP_FREQ); return 13;}//D   
        
        return 'x';
}

void send_to_disp(uint32_t disp_word){
    for (char i = 0; i < 4; i++){
        int internal_sweep = (int) pow(2, i);
        if(internal_sweep==8){
            internal_sweep = 64 ; 
        }
        LATC = internal_sweep;
        uint8_t num_disp = 0x000000FF & (disp_word >> i*8);
        LATD = char_to_seg(num_disp);
        __delay_ms(SWEEP_FREQ);
    }
}

uint8_t char_to_seg(uint8_t num){
    uint8_t segments;
    switch(num){
        case 0:  segments = 0b00000011; break;
        case 1:  segments = 0b10011111; break;
        case 2:  segments = 0b00100101; break;
        case 3:  segments = 0b00001101; break;
        case 4:  segments = 0b10011001; break;
        case 5:  segments = 0b01001001; break;
        case 6:  segments = 0b01000001; break;
        case 7:  segments = 0b00011101; break;
        case 8:  segments = 0b00000001; break;
        case 9:  segments = 0b00001001; break;
        case 10: segments = 0b00010001; break;
        case 11: segments = 0b11000001; break;
        case 12: segments = 0b01100011; break;
        case 13: segments = 0b10000101; break;
        case 14: segments = 0b01100001; break;
        default: segments = 0b01110001; break;
    }
    return segments;
}