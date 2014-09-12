/* Author: CptSpaceToaster */
/* YOU STILL NEED TO INVOKE SEI() YOURSELF! DON'T FORGET */

#ifndef _IPF_ADC_SETUP_
#define _IPF_ADC_SETUP_ 1

#include <stdio.h>
#include <avr/io.h>
#include <stdbool.h>

void ADC_init(uint8_t channel, uint8_t adc_prescaler, uint8_t trigger_source, bool left_aligned, bool adc_interrupt_enable)
{
	// Error checking
	if( channel > 8 || channel < 0 )
	{
		printf("Error: The selected channel is outside the bounds of possible ADC channels 0 to 8.\n");
		return;
	} 

	if( adc_prescaler != 2 && adc_prescaler != 4 && adc_prescaler != 8 && adc_prescaler != 16 &&
		adc_prescaler != 32 && adc_prescaler != 64 && adc_prescaler != 128 )
	{
		printf("Error: The selected prescaler is not an acceptable value {2, 4, 8, 16, 32, 64, 128}\n");
		return;
	}

	if( trigger_source > 7 || trigger_source < 0 )
	{
		printf("Error: The selected trigger source is invalid. Value must be between 0 and 7 (0b000to 0b111)\n");
		return;
	}

	// Set up ADC
	ADMUX = _BV(REFS0) | (_BV(ADLAR) * left_aligned) | channel; // AREF set, so no external ref needed
	ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADIE) * adc_interrupt_enable; // ADC enable, Autotrigger, Interrupt flag

	// Checks the prescaler setting and sets the proper bits in ADCSRA
	switch(adc_prescaler)
	{
		case 4:
			ADCSRA |= _BV(ADPS1); // _BV(1)
			break;
		case 8:
			ADCSRA |= _BV(ADPS1) | _BV(ADPS0); //_BV(1)  | _BV(0) 
			break;
		case 16:
			ADCSRA |= _BV(ADPS2); // _BV(2)
			break;
		case 32:
			ADCSRA |= _BV(ADPS2) | _BV(ADPS0); // _BV(2) | _BV(0)
			break;
		case 64:
			ADCSRA |= _BV(ADPS2) | _BV(ADPS1); // _BV(2) | _BV(1)
			break;
		case 128:
			ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // _BV(2) | _BV(1) | _BV(0)
			break;
	}

	// Checks the trigger source setting and sets the proper bits in ADCSRA
	switch(trigger_source)
	{
		case 0:
			ADCSRA |= _BV(ADSC); // free running mode, start conversions
			break;
		case 1:
			ADCSRB |= _BV(ADTS0); // analog comparator
			break;
		case 2:
			ADCSRB |= _BV(ADTS1); // external interrupt request 0
			break;
		case 3:
			ADCSRB |= _BV(ADTS1) | _BV(ADTS0); // timer/counter0 compare match A
			break;
		case 4:
			ADCSRB |= _BV(ADTS2); // timer/counter0 overflow
			break;
		case 5:
			ADCSRB |= _BV(ADTS2) | _BV(ADTS0); // timer/counter1 compare match B
			break;
		case 6:
			ADCSRB |= _BV(ADTS1) | _BV(ADTS2); // timer/counter overflow
			break;
		case 7:
			ADCSRB |= _BV(ADTS2) | _BV(ADTS1) | _BV(ADTS0); // timer/counter 1 capture event
			break;
	}
}

#endif //_IPF_ADC_SETUP_