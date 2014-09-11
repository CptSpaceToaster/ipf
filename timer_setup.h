/* Author: CptSpaceToaster */
/* YOU STILL NEED TO INVOKE SEI() YOURSELF! DON'T FORGET */

#ifndef _IPF_TIMER_SETUP_
#define _IPF_TIMER_SETUP_ 1

#include <stdio.h>
#include <avr/io.h>
#include <stdbool.h>

#ifndef F_CPU
#  error "define F_CPU before ipf/timer_setup.h"
#endif

/* Takes in a period in seconds, and sets timer0 to the closest possible value always rounded down */
int timer0_ctc(double period, bool enable_compare_A) {
	if (period < 0) {
		printf("Error: Time was set to a value below 0\n");
		return 1;
	}
	uint32_t cycles = (period*(F_CPU));
	if (cycles > (uint32_t)1024 * 255) {
		printf("Error: Value exceeded maximum timer0 threshold\n");
		return 1;
	}
	// Enable Interrupts
	TIMSK0 = _BV(OCIE0A)*enable_compare_A;
	// Set the Timer Mode to "Clear Timer on Compare Match (CTC)"
	TCCR0A = _BV(WGM01);
	// Calculate prescaler
	uint8_t prescaler=1;
	while(cycles>255) {
		cycles/=prescaler>2?4:8;
		prescaler++;
	}
	// Set cycle limit
	OCR0A = cycles;
	// Set prescaler
	TCCR0B = prescaler;
	// reset the counter
	TCNT0 = 0;
	return 0;
}

/* Takes in a period in seconds, and sets timer1 to the closest possible value always rounded down */
int timer1_ctc(double period, bool enable_compare_A) {
	if (period < 0) {
		printf("Error: Time was set to a value below 0\n");
		return 1;
	}
	uint32_t cycles = (period*(F_CPU));
	if (cycles > (uint32_t)1024 * 65535) {
		printf("Error: Value exceeded maximum timer1 threshold\n");
		return 1;
	}
	// Enable Interrupts
	TIMSK1 = _BV(OCIE1A)*enable_compare_A;
	// Calculate prescaler
	uint8_t prescaler=1;
	while(cycles>65535) {
		cycles/=prescaler>2?4:8;
		prescaler++;
	}
	// Set cycle limit
	OCR1A = cycles;
	// Set the Timer Mode to "Clear Timer on Compare Match (CTC)" (and prescaler)
	TCCR1B = _BV(WGM12) | prescaler;
	// reset the counter
	TCNT1 = 0;
	return 0;
}

/* Takes in a period in seconds, and sets timer2 to the closest possible value always rounded down */
int timer2_ctc(double period, bool enable_compare_A) {
	if (period < 0) {
		printf("Error: Time was set to a value below 0\n");
		return 1;
	}
	uint32_t cycles = (period*(F_CPU));
	if (cycles > (uint32_t)1024 * 255) {
		printf("Error: Value exceeded maximum timer2 threshold\n");
		return 1;
	}
	// Enable Interrupts
	TIMSK2 = _BV(OCIE2A)*enable_compare_A;
	// Set the Timer Mode to "Clear Timer on Compare Match (CTC)"
	TCCR2A = _BV(WGM21);
	// Calculate prescaler
	uint8_t prescaler=1;
	uint8_t prescales[] = {8,4,2,2,2,4};
	while(cycles>255) {
		cycles/=prescales[prescaler-1];
		prescaler++;
	}
	// Set cycle limit
	OCR2A = cycles;
	// Set prescaler
	TCCR2B = prescaler;
	// reset the counter
	TCNT2 = 0;
	return 0;
}

/* Takes in cycles on, cycles in a period,  which pin is the output (in PortB), and whether or not you want to enable output compare's*/
int timer1_pwm_prescaler_compare(uint16_t Ton, uint16_t period, uint8_t prescaler, int8_t output_pin, bool enable_compare_A, bool enable_compare_B) {
	if (output_pin == 0 || output_pin > 2) {
		printf("Error: Output pin must be 1, 2, or below 0\n");
		return 1;
		} else if (output_pin>0) {
		DDRB |= _BV(output_pin); // Force output_pin to actually be an OUTPUT
	}
	TIMSK1 = _BV(OCIE1A)*enable_compare_A | _BV(OCIE1B)*enable_compare_B;
	OCR1A = period >> 1; // We are counting up AND down, so we need to half the incoming cycle count to maintain accuracy
	OCR1B = Ton >> 1;
	// Allow Timer 1 to control PB1 or PB2, set timer to PWM mode with ceiling at OCR1A
	TCCR1A = _BV(COM1A1)*(output_pin==1) | _BV(COM1B1)*(output_pin==2) | _BV(WGM11) | _BV(WGM10);
	// Start the timer with the given prescaler, finish setting timer to PWM mode with ceiling at OCR1A
	TCCR1B	= _BV(WGM13) | prescaler;
	return 0;
}

/* Takes in Time-on, and period in seconds, and whether or not you want to enable output compare's*/
int timer1_pwm_sec_compare(double Ton, double period, int8_t output_pin, bool enable_compare_A, bool enable_compare_B) {
	if (Ton < 0 || period < 0) {
		printf("Error: Time was set to a value below 0\n");
		return 1;
	}
	// Get cycle counts from input times
	uint32_t cycles_on = Ton*(F_CPU);
	uint32_t cycles_period = period*(F_CPU);
	if (cycles_period > (uint32_t)1024 * 65535) {
		printf("Error: Value exceeded maximum timer1 threshold\n");
		return 1;
	}
	if (Ton > period) {
		printf("Error: Ton exceeded period\n");
		return 1;
	}
	// Calculate prescaler
	uint8_t prescaler=1;
	while(cycles_period>65535) {
		cycles_on/=prescaler>2?4:8;
		cycles_period/=prescaler>2?4:8;
		prescaler++;
	}
	return timer1_pwm_prescaler_compare((uint16_t)cycles_on, (uint16_t)cycles_period, prescaler, output_pin, enable_compare_A, enable_compare_B);
}

/* A nice shortcut if you don't want to mess with interrupts */
int timer1_pwm_sec(double Ton, double period, int8_t output_pin) {
	return timer1_pwm_sec(Ton, period, output_pin, false, false);
}

/* A nice shortcut if you don't want to mess with the prescaler */
int timer1_pwm_compare(uint16_t Ton, uint16_t period, int8_t output_pin, bool enable_compare_A, bool enable_compare_B) {
	return timer1_pwm_prescaler_compare(Ton, period, 1, output_pin, enable_compare_A, enable_compare_B);
}

/* Another nice shortcut if you don't want to mess with interrupts and prescaler */
int timer1_pwm(uint16_t Ton, uint16_t period, int8_t output_pin) {
	return timer1_pwm_prescaler_compare(Ton, period, 1, output_pin, false, false);
}

#endif //_IPF_TIMER_SETUP_
