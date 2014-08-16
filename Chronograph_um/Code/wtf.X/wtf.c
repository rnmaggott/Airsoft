/*
 * File:   ctf.c
 * Author: Ryan
 *
 * Created on 12 August 2014, 7:59 PM
 */
// PIC16F88 Configuration Bit Settings

#include <xc.h>
#include <math.h>

// CONFIG1
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = ON      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode enabled)

//Function prototypes
void startUp();
void initializeInt();
void initializeTimer1();
void initializeTimer0();
void initializeLCD();
void fLCD_Start();
void fLCD_RawSend(char in, char mask);
void fLCD_Clear();
void fLCD_PrintASCII(char Character);
void fLCD_Command(char in);
void fLCD_Cursor(char x, char y);
void fLCD_PrintNumber(short Number);
void fLCD_PrintString(char* String, char MSZ_String);
void fLCD_ScrollDisplay(char Direction, char Num_Positions);
void fLCD_ClearLine(char Line);


//Defines
#define _XTAL_FREQ 8000000      // Internal Oscillator set to 8Mhz
#define LCD_0 PORTAbits.RA0
#define LCD_1 PORTBbits.RB1
#define LCD_2 PORTBbits.RB2
#define LCD_3 PORTBbits.RB3
#define LCD_RS PORTBbits.RB4
#define LCD_E PORTBbits.RB5
#define delay __delay_us(120)
#define rowcnt 2
#define colcnt 16

int tflag = 0;          //Flag set when interrupt is triggered (4ms)

void main(void) {

    startUp();
    __delay_ms(120);
    fLCD_Start();

    /*fLCD_PrintASCII('R');
    fLCD_PrintASCII('y');
    fLCD_PrintASCII('a');
    fLCD_PrintASCII('n');
    fLCD_PrintASCII('.');
    fLCD_PrintASCII('L');
    fLCD_PrintASCII('.');
    fLCD_PrintASCII('M');
    fLCD_PrintNumber(10);*/

    for(;;){
        fLCD_PrintNumber(380);
        fLCD_PrintString("fps",3);
        __delay_ms(1000);
        fLCD_Clear();
        //fLCD_PrintNumber(250);
        fLCD_PrintString("oh yes!",7);
        __delay_ms(1000);
        fLCD_Clear();

        /*fLCD_Clear();
        __delay_ms(1000);
        fLCD_PrintString("Sweet, i",8);
        fLCD_Cursor(0, 1);
        fLCD_PrintString("t works!",8);
        __delay_ms(1000);
        fLCD_Clear();
        __delay_ms(1000);
        fLCD_PrintNumber(380);
        fLCD_PrintString("fps",3);
        */
        //fLCD_PrintNumber(300);

    }
}



void fLCD_Start(){
    PORTB = 0;
    PORTA = 0;
    __delay_ms(12);
    fLCD_RawSend(0x33, 0);
    __delay_ms(2);
    fLCD_RawSend(0x33, 0);
    __delay_ms(2);
    fLCD_RawSend(0x32, 0);
    __delay_ms(2);
    fLCD_RawSend(0x2c, 0);
    __delay_ms(2);
    fLCD_RawSend(0x06, 0);
    __delay_ms(2);
    fLCD_RawSend(0x0c, 0);
    __delay_ms(2);

    fLCD_Clear();
}

void fLCD_RawSend(char in, char mask){
    unsigned char pt;
    PORTB = 0;
    PORTA = 0;
    pt = ((in >> 4) & 0x0f);
    if(pt & 0x01)
        LCD_0 = 1;
    if(pt & 0x02)
        LCD_1 = 1;
    if(pt & 0x04)
        LCD_2 = 1;
    if(pt & 0x08)
        LCD_3 = 1;
    if(mask)
        LCD_RS = 1;
    delay;
    LCD_E = 1;
    delay;
    LCD_E = 0;
    pt = (in & 0x0f);
    delay;
    LCD_0 = 0;
    LCD_1 = 0;
    LCD_2 = 0;
    LCD_3 = 0;
    LCD_RS = 0;
    LCD_E = 0;
    if(pt & 0x01)
        LCD_0 = 1;
    if(pt & 0x02)
        LCD_1 = 1;
    if(pt & 0x04)
        LCD_2 = 1;
    if(pt & 0x08)
        LCD_3 = 1;
    if(mask)
        LCD_RS = 1;
    delay;
    LCD_E = 1;
    delay;
    LCD_E = 0;
    delay;
}

void fLCD_Clear(){
    //clear the display
    fLCD_RawSend(0x01, 0);//Clear data
    __delay_ms(2);
    fLCD_RawSend(0x02, 0);//Move to home position
    __delay_ms(2);
}

void fLCD_PrintASCII(char Character){
    fLCD_RawSend(Character, 0x10);
}

void fLCD_Command(char in){
    fLCD_RawSend(in, 0);
    __delay_ms(2);
}

void fLCD_Cursor(char x, char y){

#if (rowcnt == 1)
    y=0x80;
#endif

#if (rowcnt == 2)
    if (y==0)
        y=0x80;
    else
        y=0xc0;
#endif
#if (rowcnt == 4)
    if (y==0)
        y=0x80;
    else if (y==1)
        y=0xc0;

#if (colcnt == 16)
    else if (y==2)
        y=0x90;
    else
        y=0xd0;
#endif

#if (colcnt == 20)
    else if (y==2)
        y=0x94;
    else
        y=0xd4;
#endif
#endif

    fLCD_RawSend(y+x, 0);
    __delay_ms(2);
}

void fLCD_PrintNumber(short Number){
    short tmp_int;
    char tmp_byte;
    if(Number < 0){
        fLCD_RawSend('-', 0x10);
        Number = 0 - Number;
    }

    tmp_int = Number;
    if(Number >= 10000){
        tmp_byte = tmp_int/10000;
        fLCD_RawSend('0' + tmp_byte, 0x10);

        while(tmp_byte > 0){
            tmp_int = tmp_int - 10000;
            tmp_byte--;
        }
    }

    if(Number >= 1000){
        tmp_byte = tmp_int/1000;
        fLCD_RawSend('0' + tmp_byte, 0x10);

        while(tmp_byte > 0){
            tmp_int = tmp_int - 1000;
            tmp_byte--;
        }
    }

    if(Number >= 100){
        tmp_byte = tmp_int/100;
        fLCD_RawSend('0' + tmp_byte, 0x10);

        while(tmp_byte > 0){
            tmp_int = tmp_int - 100;
            tmp_byte--;
        }
    }

    if(Number >= 10){
        tmp_byte = tmp_int/10;
        fLCD_RawSend('0' + tmp_byte, 0x10);

        while(tmp_byte > 0){
            tmp_int = tmp_int - 10;
            tmp_byte--;
        }
    }
    fLCD_RawSend('0' + tmp_int, 0x10);
}

void fLCD_PrintString(char* String, char MSZ_String){
    char idx = 0;

    for(idx=0; idx<MSZ_String; idx++){
        if(String[idx] == 0)
            break;
        fLCD_RawSend(String[idx], 0x10);
        //String++;
    }
}

void fLCD_ScrollDisplay(char Direction, char Num_Positions){
    char cmd = 0;
    char count;

    switch(Direction){
        case 0:
        case 'l':
        case 'L':
            cmd = 0x18;
            break;
        case '1':
        case 'r':
        case 'R':
            cmd = 0x1c;
            break;
        default:
            break;
    }

    if(cmd)
        for(count=0; count<Num_Positions; count++)
            fLCD_Command(cmd);
}

void fLCD_ClearLine(char Line){
    char count;
    char rowcount;

#if (rowcnt == 1)
    rowcount=80;
#endif

#if (rowcnt == 2)
    rowcount=40;
#endif

#if (rowcnt == 4)
#if (colcnt == 16)
    rowcount=16;
#endif
#if (colcnt == 20)
    rowcount=20;
#endif
#endif
    fLCD_Cursor(0, Line);

    for(count=0; count<rowcount; count++)
        fLCD_RawSend(' ', 0x10);
    fLCD_Cursor(0,Line);
}
void startUp(){
    OSCCON = 0x70;//Set oscillator for 8MHz

    ANSEL = 0;
    CMCON = 0x07;

    OPTION_REG = 0xc0;

    TRISB = 0b00000000;//Set input (pin 3,4,5,7,8) and output (pin 1,2,6)
    TRISA = 0b00000000;
    PORTB = 0;//Set initial output to 0
    PORTA = 0;

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
        TMR1IF = 0;
    }
    //timer0 interrupt
    if(TMR0IF){
        TMR0IF = 0;
    }
}

//Interrupts required for the timer and input detection.
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

//This sets up approximately 4ms timer
void initializeTimer0(){
    OPTION_REGbits.T0CS = 0;	//Select internal instruction cycle
    OPTION_REGbits.PSA = 0;     //Prescaler on timer0
    OPTION_REGbits.PS = 0b100;	//set prescaler value (1:32)
}

//Time on = 1/(Fosc/4)*32*255*2
