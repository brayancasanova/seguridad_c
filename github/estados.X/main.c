/*!
\file   seguridad.c
\date   2022-06-28
\author Fulvio Vivas <fyvivas@unicauca.edu.co>
\brief  Security House.

\par Copyright
Information contained herein is proprietary to and constitutes valuable
confidential trade secrets of unicauca, and
is subject to restrictions on use and disclosure.

\par
Copyright (c) unicauca 2022. All rights reserved.

\par
The copyright notices above do not evidence any actual or
intended publication of this material.
******************************************************************************
*/
#include <xc.h>
#include <pic16f887.h>  /*Header file PIC16f887 definitions*/
#include <stdio.h>
#include <string.h>
#include "CONFIG.h"
#include "LCD.h"
#include "KEYPAD.h"
#include "adc.h"
#include "timer.h"


enum estado{
    INIT,
    MONITOREO,
    ALARMA,
    ALERTA,
    EVENTOS,
    BLOQUEADO
};

enum estado estadoactual = INIT;
// Definición de los pines de los LEDs
#define LED_VERDE PORTEbits.RE0
#define LED_AZUL PORTEbits.RE1
#define LED_ROJO PORTEbits.RE2





const char password[6] ={'2','0','2','1','1',0};
char pass_user[6];
unsigned char idx = 0;
int intentos = 0;

int count_5sec = 0;
unsigned char flag_count_5sec = 0;
#define TIMEOUT_5000 5000

int count_2sec = 0;
unsigned char flag_count_2sec = 0;
#define TIMEOUT_2000 2000

int count_10sec = 0;
unsigned char flag_count_10sec = 0;
#define TIMEOUT_10000 10000

int count_800ms = 0;
unsigned char flag_count_800ms = 0;
#define TIMEOUT_800 800

int count_500ms = 0;
unsigned char flag_count_500ms = 0;
#define TIMEOUT_500 500


void delay_ms_variable(unsigned int ms) {
    while (ms--) {
        __delay_ms(1);  // Llamada a __delay_ms con un valor constante
    }
}

void secuencia5(void);
void secuencia6(void);
void secuencia7(void);
void monitoreo(void);
void letrero(void);
void seguridad(void);
void eventos(void);
void alarma(void);
void bloqueado(void);

//void interrupt Timer1_ISR();


unsigned char flag_correcto;

void main() {
 
    //Configura Fosc = 8Mhz interno, Fuente de Fosc del sistema = interno
    OSCCON = 0x71; //Configura oscilador interno (FOSC = 8Mhz)
     
    LCD_Init(); //Inicializa el LCD

    keypad_init(); //Inicializa el keypad
    TRISE = 0b00000000;
    ANSELH = 0x00;

    TRISD = 0b00000000;  
    
    
    TRISAbits.TRISA0 = 1; //sensor hold digital
    TRISAbits.TRISA1 = 0; //efecto movimiento digital
    TRISAbits.TRISA2 = 1; //sensor magnetismo digital
    TRISAbits.TRISA3 = 1; //sensor temperatura analogico
    //TRISAbits.TRISA4 = 0; 
    TRISAbits.TRISA5 = 1; //sensor luz analogico
    TRISAbits.TRISA7 = 1; //pulsador reset
    ANSEL = 0b00011000;
    adc_init();
    Timer1_start();

    estadoactual = INIT;
    PORTD = 0;
                    
    while (1) {
        
        switch(estadoactual) {
            case INIT:
                LCD_Clear();
                LCD_String_xy(0,0,"INIT");
                seguridad();
                if(flag_correcto==1){
                    estadoactual=MONITOREO;
                    count_5sec=0;
                }
                else if(flag_correcto==0){
                    estadoactual=INIT;
                }
                else if(flag_correcto==2){
                    estadoactual=BLOQUEADO;
                    count_500ms = 0;
                    count_10sec = 0;
                }
                flag_correcto=0;
                break;
            case MONITOREO:
                monitoreo();
                if(flag_correcto==4){
                    estadoactual=ALARMA;
                    count_800ms = 0;
                    count_5sec = 0;
                    flag_correcto=0;
                    flag_count_5sec=0;
                }
                else if(flag_count_5sec){
                    estadoactual=EVENTOS;
                    count_2sec = 0;
                    flag_count_5sec=0;
                }
                __delay_ms(500);
                break;
                
            case ALARMA:
                PORTAbits.RA1 = 1;
                alarma();
                if(flag_count_5sec){
                    estadoactual=MONITOREO;
                    flag_count_5sec=0;
                }
                else if(PORTAbits.RA7==1){
                    estadoactual=INIT;
                }
                break;
            case ALERTA:
                secuencia5();
                if(flag_count_5sec){
                    estadoactual=BLOQUEADO;
                    flag_count_5sec=0;
                } 
                break;
            case EVENTOS:
                eventos();
                if(flag_correcto==1){
                    estadoactual=ALERTA;
                }
                else if(flag_count_2sec){
                    estadoactual=MONITOREO;
                    count_5sec = 0;
                    flag_count_2sec=0;
                }
                flag_correcto = 0;
                break;
            case BLOQUEADO:
                LCD_Clear();
                LCD_String_xy(0,0,"BLOQUEADO");
                secuencia6();
                if(flag_count_10sec){
                    estadoactual=INIT;
                    count_10sec = 0;
                    flag_count_10sec=0;
                }
                
                break;
        }
    }
}
void secuencia5(void){
volatile unsigned int var1=0xA0,var2=0x01,var12 = 0;
    LCD_Clear();
    LCD_String_xy(0,0,"ALERTA");

    for (int i = 0; i < 2; i++) {
        var12 = var1 | var2;
        PORTD = (unsigned char)(var12);
        __delay_ms(500);
        var1 = var1 >> 1;
        var2 = var2 << 1;
        var12 = var1 | var2;
        PORTD = (unsigned char)(var12);
        __delay_ms(500);
        var12 = var1 | var2;
        var1 = var1 << 1;
        var2 = var2 << 1;

    }

volatile unsigned int var3=0xA0,var4=0x08,var34 = 0;
    for (int i = 0; i < 2; i++){
        var34 = var3 | var4;
        PORTD = (unsigned char)(var34);
        __delay_ms(500);
        var3 = var3 >> 1;
        var4 = var4 >> 1;
        var34 = var3 | var4;
        PORTD = (unsigned char)(var34);
        __delay_ms(500);
        var34 = var3 | var3;
        var3 = var3 << 1;
        var4 = var4 >> 1;
        
    }
 }


void monitoreo(void){

    int celsius;
    char StringTemperature[20];

    LCD_Clear();
    LCD_String_xy(0,0,"MONITOREO");
    LCD_Command(0xC0); 
    
    unsigned int temperatura = adc_read(3);
    unsigned int luz = adc_read(4);
        
        //celsius = int(temperatura*4.88);
        //celsius = int(celsius/10.00);
        //sprintf(StringTemperature,"TEMP %.2f %cC  ", celsius,0xdf); 
                
    int value_adc = 1023 - (int)temperatura; 
    celsius = (int)(value_adc * 0.04058); 
    sprintf(StringTemperature, "T: %d L: %d", celsius, luz);  
        //LCD_String_xy(1,0,StringTemperature);
    LCD_String(StringTemperature);
    if(luz > 900){
        flag_correcto =4;     
    }
    else{
        flag_correcto = 0;
    }
}
   

void secuencia7(void){
volatile unsigned int var1=0x01,var2=0x80,var12 = 0;
    


    for (int i = 0; i < 4; i++) {
        var12 = var1 | var2;

        PORTD = (unsigned char)(var12);
         __delay_ms(500);
        STATUSbits.CARRY = 0;
        var1 = var1 << 1;
        STATUSbits.CARRY = 0;
        var2 = var2 >> 1;
        STATUSbits.CARRY = 0;
        var2 |= (1 << 7);
        STATUSbits.CARRY = 0;
 
    }

volatile unsigned int var3=0x10,var4=0x08,var34=0 ;
    for (int i = 0; i < 4; i++) {
        var34 = var3 | var4;
        PORTD = (unsigned char)(var34);
         __delay_ms(500);
        STATUSbits.CARRY = 0;
        var3 = var3 << 1;
        STATUSbits.CARRY = 0;
        var4 = var4 >> 1;
        STATUSbits.CARRY = 0;
        var4 |= (1 << 3);
        STATUSbits.CARRY = 0;
 
    }

}


void blink_led(unsigned char led, unsigned int on_time, unsigned int off_time, unsigned int duration) {
    unsigned int elapsed_time = 0;

    while (elapsed_time < duration) {
        if (led == 'V') {
            LED_VERDE = 1;  // Enciende LED verde
        } else if (led == 'A') {
            LED_AZUL = 1;   // Enciende LED azul
        } else if (led == 'R') {
            LED_ROJO = 1;   // Enciende LED rojo
        }
        delay_ms_variable(on_time);

        // Apagar LED
        if (led == 'V') {
            LED_VERDE = 0;
        } else if (led == 'A') {
            LED_AZUL = 0;
        } else if (led == 'R') {
            LED_ROJO = 0;
        }
        delay_ms_variable(off_time);

        elapsed_time += on_time + off_time;
    }
}

void secuencia6(void){
    blink_led('R', 300, 500, 8000);  // Parpadear LED rojo por 8 segundos
volatile unsigned int var1=0x01,var2=0x80,var12 = 0;
    for (int i = 1; i < 5; i++) {
        
            var12 = var2 | var1;
            PORTD = (unsigned char)(var12);
             __delay_ms(500);
            var1 = var1 + 2;
            var2= var2 >> 1;
           }
volatile unsigned int var3=0x09,var4=0x10,var34 = 0;
    var34 = var4 | var3;
    PORTD = (unsigned char)(var34);
     __delay_ms(500);
    for (int i = 0; i < 3; i++){
        var3= var3 + 2;
        var4= var4 << 1;
        var34 = var4 | var3;
        PORTD = (unsigned char)(var34);
         __delay_ms(500);
    }
     
     
}



void seguridad(void){
   char key = '0';
                LCD_String_xy(0,0,"Press a Key");
                LCD_Char(2);
                LCD_Command(0xC0); 
                

                do{
                    key = keypad_getkey();
                    if(key != 0){
                        LCD_Char('*');/* display pressed key on LCD16x2 */
                        //LCD_Char(key);/* display pressed key on LCD16x2 */
                        pass_user[idx++] = key;
                    }
                    //__delay_ms(100);
                }while(idx < 6);

                if(strncmp(pass_user,password,4)==0){
                    LCD_Clear();
                    LCD_String_xy(0,0,"Clave Correcta");
                    blink_led('V', 500, 500, 3000);  // Parpadear LED verde por 3 segundos
                    intentos = 0;
                    flag_correcto = 1;           
                }
                else{
                    LCD_Clear();
                    LCD_String_xy(0,0,"Clave Incorrecta"); 
                    blink_led('A', 300, 700, 2000);  // Parpadear LED azul por 2 segundos
                    intentos = intentos + 1;
                    flag_correcto = 0;
                    if(intentos ==3){
                        LCD_Clear();
                        LCD_String_xy(0,0,"sistema bloqueado");
                        //blink_led('R', 300, 500, 8000);  // Parpadear LED rojo por 8 segundos
                        flag_correcto = 2;
                        intentos = 0;  // Reiniciar intentos después del bloqueo
                        
                    }
                }
                //__delay_ms(2000);
                
                LCD_Clear();
                idx = 0;
            }
                
void eventos(void){
    LCD_Clear();
    LCD_String_xy(0,0,"EVENTOS");
    __delay_ms(500);
    if (PORTAbits.RA0==1) {
        flag_correcto = 1;    
    }
    //else if (PORTAbits.RA1){
        //flag_correcto = 2;
    //}
    //else if (PORTAbits.RA2){
      //  flag_correcto = 2;
    //}
    else{
        flag_correcto = 0;
    }
}

void __interrupt() Timer1_ISR()
{
    TMR1=0xFC16;
    count_2sec++;
    if(count_2sec > TIMEOUT_2000){
        count_2sec = 0;
        flag_count_2sec = 1;
    }
    
    count_5sec++;        
    if(count_5sec > TIMEOUT_5000){
        count_5sec = 0;
        flag_count_5sec = 1;
    }
    
    count_10sec++;        
    if(count_10sec > TIMEOUT_5000){
        count_10sec = 0;
        flag_count_10sec = 1;
    }
    
    count_800ms++;        
    if(count_800ms > TIMEOUT_800){
        count_800ms = 0;
        flag_count_800ms = 1;
    }
    
    count_500ms++;        
    if(count_500ms > TIMEOUT_500){
        count_500ms = 0;
        flag_count_500ms = 1;
    }
    //Pulse = ~Pulse;    /* Toggle Value at PortB to generate waveform of 500 Hz */   
    PIR1bits.TMR1IF=0; /* Make Timer1 Overflow Flag to '0' */
}

            

               
   void alarma(void){ 
                LCD_Clear();
                LCD_String_xy(0,0,"ALARMA");
                __delay_ms(500);
                if(flag_count_800ms){
                    flag_count_800ms=0;
                    PORTAbits.RA1 = 0;                   
                }    
                if(PORTAbits.RA7){
                    estadoactual=INIT;
                }
                else if(flag_count_5sec){
                    estadoactual=MONITOREO;
                    flag_count_5sec=0;
                    count_5sec = 0;
                }    
                
    

   }
   
  /* void bloqueado(void){ 
                PORTDbits.RD4 = 1;
                PORTDbits.RD5 = 1;
                if(flag_count_500ms){
                    flag_count_500ms=0;
                    PORTDbits.RD4 = 0;
                    PORTDbits.RD5 = 0;
                }
               
                if(flag_count_10sec){
                    estadoactual=INIT;
                    flag_count_10sec=0;
                } 
   }*/