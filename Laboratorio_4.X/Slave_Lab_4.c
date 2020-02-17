/*
 * File:   Master_Lab_4.c
 * Author: Hector
 *
 * Created on 16 de febrero de 2020, 08:11 AM
 */
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#define _XTAL_FREQ 8000000 //Se define la frecuencia del oscilador para el delay
#include <xc.h>
#include <stdint.h>
#include <stdio.h>

#include "Oscilador.h"
#include "UART.h"
#include "SPI.h"
#include "ADC.h"

//********************************************************************************************************
// Funciones prototipo
//********************************************************************************************************
void init(void);
void initTMR0(void);

//********************************************************************************************************
// Variables
//********************************************************************************************************
uint8_t ADC_CH1_BIN = 0;
uint8_t ADC_CH2_BIN = 0;
uint8_t CONT = 0;

//********************************************************************************************************
// Interrupciones
//********************************************************************************************************

void __interrupt() isr(void){
    //Interrupción del ADC
    if (ADIF){ //Si la bandera del ADC se encendió
        ADIF = 0; //Apgaue la bandera
        if (CONT == 0){ //Si contador está en 0
            channel(0); //Asigna AN0
            ADC_CH1_BIN = ADRESH; //Guarde el valor del ADRESH en la variable

        }
        if (CONT == 1){ //Si contador está en 1
            channel(3); //Asigna AN3
            ADC_CH2_BIN = ADRESH; //Guarde el valor del ADRESH en la variable

        }   
    }
    
    //Interrupción del TMR0
    if (TMR0IF){ //Si la bandera del Timer 0 se prendió
        TMR0IF = 0; //Apague la bandera
        TMR0 = 68; //Ponga en TMR0 el valor para un desborde de 3 ms
        CONT++; //Se incrementa la variable y esta se incrementará cada 3 ms
        
        if (CONT > 2){ //Si el valor de la variable llega a 1, reinice a cero el valor de la variable
            CONT = 0; 
        }        
    }
}
//********************************************************************************************************
//Void Principal
//********************************************************************************************************
void main(void) {
    initOsc(7); // Se usa un reloj interno de 8 MHz
    init(); //Se inicializan los puertos
    initADC(); //Se inicializa el ADC
    initTMR0(); //Se configura el timer 0 para un desborde cada 3 ms
    
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0; //Se inicializan todas los puertos en 0
    
    while (1){
        ADCON0bits.GO_nDONE = 1; //Inicia la conversión del ADC   
        //********************************************************************************************************
        // Manda el valor de los pots al PIC que funciona como MASTER
        //********************************************************************************************************         
        spiWrite(ADC_CH1_BIN);
        __delay_ms(5);
        spiWrite(ADC_CH2_BIN);
    }
    
    
    return;
}

//********************************************************************************************************
//Función de Inicialización de Puertos
//********************************************************************************************************

void init(void){
    TRISA = 0b00001001; // PORTA configurado como entrada
    TRISB = 0; // PORTB configurado como salida
    TRISC = 0; // PORTC configurado como salida
    TRISD = 0; // PORTD configurado como salida
    ANSEL = 0b00001001; // Pines connfigurados A0 y A3 como entradas analógicas
    ANSELH = 0; //Pines configurados como digitales 
    INTCON = 0b11100000; //GIE, PIE Y T0IE Activadas
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}

//********************************************************************************************************
//Función de Inicialización de TMR0
//********************************************************************************************************

void initTMR0(void){
    OPTION_REG	 = 0x84; //Prescaler de 1:32, Pull-ups en PORTB están desabilitadas
    TMR0		 = 68; //Valor para obtener desborde cada 3 ms
    
}
