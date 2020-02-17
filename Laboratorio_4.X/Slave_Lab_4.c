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
#include "ADC.h"

//******************************************************************************
// Funciones prototipo
//******************************************************************************
void init(void);
void initTMR0(void);

//******************************************************************************
// Variables
//******************************************************************************
uint8_t ADC_CH1_BIN = 0;
uint8_t ADC_CH2_BIN = 0;
uint8_t CONT = 0;

//******************************************************************************
// Interrupciones
//******************************************************************************

void __interrupt() isr(void){
    //Interrupci�n del ADC
    if (ADIF){                        //Si la bandera del ADC se encendi� se realizar�
        ADIF = 0;                     //Apagar la bandera
        if (CONT == 0){               //Dependiendo si valor de contador est� en 0
            channel(0);               //Asigna AN0
            ADC_CH1_BIN = ADRESH;    //Guarde el valor del ADRESH en la variable

        }
        if (CONT == 1){             //Dependiendo si valor de contador est� en 0
            channel(3);             //Asigna AN3
            ADC_CH2_BIN = ADRESH;   //Guarde el valor del ADRESH en la variable

        }   
    }
    
    //Interrupci�n del TMR0
    if (TMR0IF){                //Si la bandera del Timer 0 se encendi�
        TMR0IF = 0;             //Apagar la bandera
        TMR0 = 68;              //TMR0 con desborde de cada 3 ms
        CONT++;                 //Incrementar variable
        
        if (CONT > 2){          //Si el valor de la variable llega a 1, reinice conteo
            CONT = 0; 
        }        
    }
    //Interrupci�n del SSP
       if(SSPIF == 1){
           PORTD = spiRead();
           if (PORTD == 1){
               spiWrite(ADC_CH1_BIN);
           }
           if (PORTD == 2){
               spiWrite(ADC_CH2_BIN);
           }
        SSPIF = 0;
    }
}
//******************************************************************************
//Void Principal
//******************************************************************************
void main(void) {
    initOsc(7);             //Se usa un reloj interno de 8 MHz
    init();                 //Se inicializan los puertos
    initADC();              //Se inicializa el ADC
    initTMR0();             //Se configura el timer 0 para un desborde cada 3 ms
    
    PORTA = 0;              //Inicializaci�n de puertos
    PORTB = 0;
    PORTC = 0;
    PORTD = 0; 
    
    while (1){              //Loop infinito
        ADCON0bits.GO_nDONE = 1;                  //Inicia la conversi�n del ADC   
    }
    return;
}
//******************************************************************************
//Funci�n de Inicializaci�n de Puertos
//******************************************************************************

void init(void){
    TRISA = 0b00001001;                 // PORTA configurado como entrada
    TRISB = 0;                          // PORTB configurado como salida
    TRISC = 0;                          // PORTC configurado como salida
    TRISD = 0;                          // PORTD configurado como salida
    ANSEL = 0b00001001;                 // Pines connfigurados A0 y A3 como entradas anal�gicas
    ANSELH = 0;                         //Pines configurados como digitales 
    INTCON = 0b11100000;                //Habilita GIE, PEIE y T0IE 
    PIR1bits.SSPIF = 0;                 // Borramos bandera interrupci�n MSSP
    PIE1bits.SSPIE = 1;                 // Habilitamos interrupci�n MSSP
    TRISAbits.TRISA5 = 1;               // Slave Select
    //Inicializaci�n de SPI
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}
//******************************************************************************
//Funci�n de Inicializaci�n de TMR0
//******************************************************************************
void initTMR0(void){
    OPTION_REG	 = 0x84;                //Prescaler de 1:32, Pull-ups en PORTB est�n desabilitadas
    TMR0		 = 68;                  //Valor para obtener desborde cada 3 ms
    
}
