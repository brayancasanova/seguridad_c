/* 
 * File:   adc.h
 * Author: estudiante
 *
 * Created on 17 de octubre de 2024, 07:22 AM
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

void adc_init(void);
unsigned int adc_read(int channel);


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */


