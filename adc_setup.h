/* Author: CptSpaceToaster and AetheralLabyrinth */
/* YOU STILL NEED TO INVOKE SEI() YOURSELF! DON'T FORGET */

#ifndef _IPF_ADC_SETUP_
#define _IPF_ADC_SETUP_ 1

#ifndef F_CPU
#  error "define F_CPU before ipf/adc_setup.h"
#endif

#include <stdio.h>
#include <avr/io.h>
#include <stdbool.h>
//////////////////////////////////////////////////////////////////////////
// Argument 1
// ADC Source Pins
#define	ADC_PC0                             0x00
#define	ADC_PC1                             0x01
#define	ADC_PC2                             0x02
#define	ADC_PC3                             0x03
#define	ADC_PC4                             0x04
#define	ADC_PC5                             0x05
#define	ADC_PC6                             0x06
#define	ADC_PC7                             0x07
#define	ADC_INTERNAL_TEMPERATURE_SENSOR     0x08 // Yes, there is no PC8...that's Pin 7 you're thinking about

// Argument 2
// Trigger Sources (or operation mode)
#define ADC_FREE_RUNNING_MODE               0x00 // This is the only one I used
#define ADC_ANALOG_COMPARATOR               0x01 // Not going to lie here... I have no idea what this even is...
#define ADC_EXTERNAL_INTERRUPT_REQUEST_0    0x02 // Or this..
#define ADC_TIMER0_COMPARE_MATCH_A          0x03 // Or any of these......
#define ADC_TIMER0_OVERFLOW                 0x04
#define ADC_TIMER1_COMPARE_MATCH_B          0x05
#define ADC_TIMER1_OVERFLOW                 0x06
#define ADC_TIMER1_CAPTURE_EVENT            0x07
#define ADC_NO_TRIGGER						0x08

// Argument 3
// ADC Reference Sources
#define	ADC_INTERNAL_REFERENCE_OFF          0x00
#define	ADC_REFERENCE_VCC                   0x40 // This is usually 5v, unless you're running the device at 3.3v...... then it's 3.3v
#define	ADC_REFERENCE_1V1                   0xC0 // This is the 1.1v reference for anyone who's looking
// Arguments are done (The next two are booleans)
//////////////////////////////////////////////////////////////////////////

// TODO: ADCSRA Register Stuffs are all hard set atm

uint8_t ADC_channel_switch(uint8_t ADC_channel) {
	if(ADC_channel > 0x08) {
		// Error: The selected channel is outside the bounds of possible ADC channels 0 to 8.
		return 1;
	}

	// select channel
	DDRC &= ADC_channel; //Set the given bits to inputs, Leave the pull-up resistors alone
	// Clear the current channel setting
	ADMUX &= ~(0x0F);
	// Turn on the bits that matter
	ADMUX |= ADC_channel;
	return 0;
}

uint8_t ADC_set_trigger(uint8_t ADC_trigger) {
	if (ADC_trigger == ADC_NO_TRIGGER) {
		return 0;
	}
	if(ADC_trigger > 0x07) {
		// Error: The selected trigger is outside the bounds of possible ADC channels 0 to 7.
		return 2;
	}

	ADCSRB &= ~(0x07); // Clear the trigger setting
	// Turn on the bits that matter
	ADCSRB |= ADC_trigger;
	return 0;
}

uint8_t ADC_reference_switch(uint8_t ADC_reference_source) {
	if( ADC_reference_source != ADC_INTERNAL_REFERENCE_OFF &&
	ADC_reference_source != ADC_REFERENCE_VCC &&
	ADC_reference_source != ADC_REFERENCE_1V1)	{
		// Error: The selected reference voltage is invalid. Value must be either ADC_INTERNAL_REFERENCE_OFF, ADC_REFERENCE_VCC, or ADC_REFERENCE_1V1
		return 3;
	}
	// Clear the current reference setting
	ADMUX &= ~(0xC0);
	// Turn on the bits that matter
	ADMUX |= ADC_reference_source;
	return 0;
}

void ADC_set_alignment(bool is_left_aligned) {
	// Clear the Left Aligned Bit
	ADMUX &= ~(0x20);
	// Set it if it's wanted
	ADMUX |= 0x20 * is_left_aligned;
}

#define ADC_start_conversion()     ADCSRA |= 0x40
#define	ADC_get_value()            ADCH // Only works if the value is left adjusted.  Also disregards the bottom low bits... they are unreliable according to the spec sheet.
#define	ADC_get_low_register()     ADCL // You shouldn't need this...

uint8_t ADC_init(uint8_t ADC_channel, uint8_t trigger_source, uint8_t ADC_reference_source, bool is_left_aligned, bool adc_interrupt_enable) {
	uint8_t err;
	
	err = ADC_channel_switch(ADC_channel);
	if (err) {
		return err;
	} 
	err = ADC_set_trigger(trigger_source);
	if (err) {
		return err;
	}
	err = ADC_reference_switch(ADC_reference_source);
	if (err) {
		return err;
	}
	
	// You usually want left (so true)
	ADC_set_alignment(is_left_aligned);

	// determine appropriate prescaler, F_CPU must be defined before hand!
	uint8_t adc_prescaler;
	
	if((F_CPU / 2) > 50000 && (F_CPU / 2) < 200000) {
		adc_prescaler = 0x01;
	} else if((F_CPU / 4) > 50000 && (F_CPU / 4) < 200000) {
		adc_prescaler = 0x02;
	} else if((F_CPU / 8) > 50000 && (F_CPU / 8) < 200000) {
		adc_prescaler = 0x03;
	} else if((F_CPU / 16) > 50000 && (F_CPU / 16) < 200000) {
		adc_prescaler = 0x04;
	} else if((F_CPU / 32) > 50000 && (F_CPU / 32) < 200000) {
		adc_prescaler = 0x05;
	} else if((F_CPU / 64) > 50000 && (F_CPU / 64) < 200000) {
		adc_prescaler = 0x06;
	} else if((F_CPU / 128) > 50000 && (F_CPU / 128) < 200000) {
		adc_prescaler = 0x07;
	} else {
		// Error: Your FCPU be like... WAY TO BIG for a pre-scaler of 128 to handle... this library isn't for you!
		return 4;
	}
	
	ADCSRA = _BV(ADEN) | _BV(ADATE) * (trigger_source != ADC_NO_TRIGGER) | _BV(ADIE) * adc_interrupt_enable | adc_prescaler; // ADC enable, Autotrigger, Interrupt flag, prescaler
	
	ADC_start_conversion(); // if free running mode, start conversions and they'll keep going, otherwise, this will read once.
	return 0;
}

#endif //_IPF_ADC_SETUP_