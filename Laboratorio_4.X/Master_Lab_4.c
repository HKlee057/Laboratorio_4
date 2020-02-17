/*
 * File:   Master_Lab_4.c
 * Author: Hector
 *
 * Created on 16 de febrero de 2020, 08:11 AM
 */
//******************************************************************************
// Palabra de Configuraci�n
//******************************************************************************
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

//******************************************************************************
// Definici�n de librer�as
//******************************************************************************
#define _XTAL_FREQ 8000000 //Se define la frecuencia del oscilador para el delay
#include <xc.h>
#include <stdint.h>
#include <stdio.h>

#include "Oscilador.h"
#include "UART.h"
#include "SPI.h"

//******************************************************************************
// Prototipos de Funciones y Declaraci�n de Variables
//******************************************************************************
void init(void);
//******************************************************************************
// Variables
//******************************************************************************
uint8_t RecPOT1;
uint8_t RecPOT2;
//******************************************************************************
//Void Principal
//******************************************************************************
void main(void) {
    initOsc(7);                 // Se usa un reloj interno de 8 MHz
    init();                     //Se inicializan los puertos
    UART_Init(9600);            //Se inicializa la comunicaci�n UART
    
    PORTA = 0;                  //Inicializaci�n de puertos
    PORTB = 0;
    PORTC = 0;
    PORTD = 0; 
    
    while (1){
        PORTB = UART_Read();     //Valor transmitido por la computadora es le�do y colocado en puerto B
        
        PORTCbits.RC2 = 0;          //Seleccionar al SLAVE
        __delay_ms (1);             //Delay de 1 milisegundo
        spiWrite(1);
        RecPOT1 = spiRead();        //Leer dato preveniente de SLAVE PIC
        __delay_ms(1);
        PORTCbits.RC2 = 1;          //Slave Deselect 
        
        PORTCbits.RC2 = 0;          //Seleccionar al SLAVE
        __delay_ms (1);             //Delay de 1 milisegundo
        spiWrite(2);
        RecPOT2 = spiRead();        //Leer dato preveniente de SLAVE PIC
        __delay_ms(1);
        PORTCbits.RC2 = 1;          //Slave Deselect
        
        UART_Write(RecPOT1);        //Escribir el el registro de UART para transmitir dato 1
        __delay_ms(5);              //Delay de 5 milisegundos
        UART_Write(RecPOT2);        //Escribir el el registro de UART para transmitir dato 1
    }
    return;
}
//******************************************************************************
//Funci�n de Inicializaci�n de Puertos
//******************************************************************************
void init(void){
    TRISA = 0;                      // PORTA configurado como salida
    TRISB = 0;                      // PORTB configurado como salida
    TRISC = 0;                      // PORTC configurado como salida
    TRISCbits.TRISC7 = 1;           //RC7 configurado como entrada
    TRISD = 0;                      // PORTD configurado como salida
    ANSEL = 0;                      // Pines connfigurados como entradas digitales
    ANSELH = 0b00000000;           //Pines connfigurados como entradas digitales  
    INTCON = 0b11100000;           //Habilita GIE, PIE y T0IE 
    //Inicializaci�n de SPI
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}

