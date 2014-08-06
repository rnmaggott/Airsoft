/*
 * File:   ctf.c
 * Author: Ryan
 *
 * Created on 04 August 2014, 9:53 PM
 */

 // PIC16F88 Configuration Bit Settings

#include <xc.h>
#include <math.h>

// CONFIG1
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode disabled)

void startUp();
void initializeInt();
void initializeTimer1();
void initializeTimer0();

void main(void) {

    for(;;){
        
    }
    return;
}

void startUp(){
    OSCCON = 0x70;

    TRISB = 0b0001111;
    PORTB = 0;

    initializeInt();
    initializeTimer0();
    initializeTimer1();
}

/*
 *	This is the interrupt service routine. The interrupt handling
 * 	occurs in this section of the code.
 */
void interrupt isr(void){

	//Timer1 Interrupt
    if(TMR1IF){
        TMR1IF = 0; 			// clear interrupt flag
        if (tflag){
            
        }
        else{
            
        }
    }
    //timer0 interrupt
    if(TMR0IF){
        
    }
}

void initializeInt(){
    TMR1IE = 1;
    TMR0IE = 1;
    ei();
    INTCONbits.PEIE = 1;
    PIR1bits.RCIF = 0;
    PIE1bits.RCIE = 0;
}

void initializeTimer1(){
    T1CONbits.T1CKPS = 0b00;    //Find the proper value for this (Prescaler)
    T1CONbits.T1OSCEN = 1;      //Oscillator Enabled
    T1CONbits.TMR1CS = 0;       //Internal Clock Source (FOSC/4)
    T1CONbits.TMR1ON = 1;       //Enable timer 1
}

void initializeTimer0(){
    OPTION_REGbits.T0CS = 0;	//Select internal instruction cycle
    OPTION_REGbits.PSA = 0;     //Prescaler on timer0
    OPTION_REGbits.PS = 0b100;	//set prescaler value (1:32)
}