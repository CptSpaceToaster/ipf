/*
 * watchdog.h - An idiomatic approach to manage the watchdog timer
 *
 * Created: 10/30/2014 1:23:49 PM
 *  Author: CaptainSpaceToaster
 *
 * References:
 *  http://electronics.stackexchange.com/questions/74840/use-avr-watchdog-like-normal-isr
 *  http://www.leonardomiliani.com/en/2013/impariamo-ad-usare-il-watchdog-2/
 * Datasheets:
 *  http://www.atmel.com/Images/doc8161.pdf - ATMega328P Specsheet
 *  http://www.atmel.com/Images/doc2551.pdf - Using the watchdog timer
 */ 

#include <avr/wdt.h>

typedef enum WATCHDOG_MODE {
	WD_STOPPED,						//00
	WD_INTERRUPT,					//01
	WD_SYSTEM_RESET,				//10
	WD_INTERRUPT_AND_SYSTEM_RESET,	//11
} WATCHDOG_MODE_t;

typedef enum WATCHDOG_TIMEOUT {
	WD_TIMEOUT_16_MS,
	WD_TIMEOUT_32_MS,
	WD_TIMEOUT_64_MS,
	WD_TIMEOUT_125_MS,
	WD_TIMEOUT_250_MS,
	WD_TIMEOUT_500_MS,
	WD_TIMEOUT_1_S,
	WD_TIMEOUT_2_S,
	WD_TIMEOUT_4_S,
	WD_TIMEOUT_8_S,
} WATCHDOG_TIMEOUT_t;


/* Please remember to invoke sei() yourself */
void watchdog_set(WATCHDOG_MODE_t mode, WATCHDOG_TIMEOUT_t timeout) {
	wdt_reset();
	// Announce that we are going to change the watchdog timer
	WDTCSR = _BV(WDCE) | _BV(WDE);
	// set-clear system reset mode, set-clear interrupt mode, and set the timeout
	WDTCSR = ((mode & 0x1) * _BV(WDIE)) | (((mode>>1) & 0x1) * _BV(WDE)) | timeout;
}


/* I mean... why not? 
 *   0000 0001 - Watchdog System Reset Flag
 *   0000 0010 - Brown out Reset Flag
 *   0000 0100 - External Reset Flag
 *   0000 1000 - Power-On Reset Flag */
uint8_t watchdog_get_status() {
	return MCUSR;
}


/* Clears all of the flags!
 *   Watchdog System Reset Flag
 *   Brown out Reset Flag
 *   External Reset Flag
 *   Power-On Reset Flag 
 *
 * Page 52 - http://www.atmel.com/Images/doc8161.pdf
 * Note: If the Watchdog is accidentally enabled, for example by a runaway pointer or brown-out
 *   condition, the device will be reset and the Watchdog Timer will stay enabled. If the code is not
 *   set up to handle the Watchdog, this might lead to an eternal loop of time-out resets. To avoid this
 *   situation, the application software should always clear the Watchdog System Reset Flag
 *   (WDRF) and the WDE control bit in the initialisation routine */
void watchdog_clear_status() {
	MCUSR = 0; //reset all statuses in the control register of the MCU
}


/* Ya... I know... you want to fight about it? */
void watchdog_feed() {
	wdt_reset();
}