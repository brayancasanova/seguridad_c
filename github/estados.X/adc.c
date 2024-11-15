#include "adc.h"
#include <xc.h>
#include <pic16f887.h>
#include "CONFIG.h"

void adc_init(void){
    
    ADCON1bits.ADFM = 1; //  Justificaci�n Derecha (modo-8bits).
    ADCON1bits.VCFG0 = 0; //  Selecciona Voltajes de Referencia (5v-0v).
    ADCON1bits.VCFG1 = 0;
    
    ADCON0bits.ADCS = 0b01; //  Tiempo de Conversi�n Fosc/8.
    ADRESH=0;		/* Flush ADC output Register */
    ADRESL=0;   
}


unsigned int adc_read(int channel){
    
    ADCON0bits.CHS = (0x0F & channel); //  Selecciona el Canal Anal�gico AN0.
    
    ADCON0bits.ADON = 1; //  Habilita el M�dulo AD.
    __delay_us(30);
    ADCON0bits.GO_DONE = 1; //  Inicia la COnversi� AD.
    while (ADCON0bits.GO_DONE); //  Espera a que termine la conversi�n AD.
    unsigned int value_adc = ADRESH; //  Lectura de valor AD.
    value_adc = (value_adc << 8) + ADRESL;
    return value_adc;
}
