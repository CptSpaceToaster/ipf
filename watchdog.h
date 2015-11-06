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

// STATUS CHECK
#define WD_STATUS_INDICATES_WD_RESET       ((MCUSR & 0x01) > 0)
#define WD_STATUS_INDICATES_BROWNOUT_RESET ((MCUSR & 0x02) > 0)
#define WD_STATUS_INDICATES_EXTERNAL_RESET ((MCUSR & 0x04) > 0)
#define WD_STATUS_INDICATES_POWERON_RESET  ((MCUSR & 0x08) > 0)

// WATCHDOG_MODES
#define	WD_STOPPED                     0x00
#define	WD_INTERRUPT                   0x01
#define	WD_SYSTEM_RESET                0x02
#define	WD_INTERRUPT_AND_SYSTEM_RESET  0x03

// WATCHDOG TIMEOUT INTERVALS
#define	WD_TIMEOUT_16_MS               0x00
#define	WD_TIMEOUT_32_MS               0x01
#define	WD_TIMEOUT_64_MS               0x02
#define	WD_TIMEOUT_125_MS              0x03
#define	WD_TIMEOUT_250_MS              0x04
#define	WD_TIMEOUT_500_MS              0x05
#define	WD_TIMEOUT_1_S                 0x06
#define	WD_TIMEOUT_2_S                 0x07
#define	WD_TIMEOUT_4_S                 0x20
#define	WD_TIMEOUT_8_S                 0x21

/* Please remember to invoke sei() yourself */
void watchdog_set(uint8_t mode, uint8_t timeout_interval) {
	if (mode > 3) {
		//Error, mode is to high (0 to 3)
		return;
	}
	wdt_reset();
	// Announce that we are going to change the watchdog timer
	WDTCSR = _BV(WDCE) | _BV(WDE);

	// Check for a misplaced WDP3 bit: 0b00001111 versus 0b00100111
	if (timeout_interval & 0x08)
		timeout_interval &= ~0x08;
		timeout_interval |= 0x20;

	// set-clear system reset mode, set-clear interrupt mode, and set the timeout
	WDTCSR = ((mode & 0x1) * _BV(WDIE)) | (((mode>>1) & 0x1) * _BV(WDE)) | timeout_interval;
}


/* I mean... why not?
 *   0000 0001 - Watchdog System Reset Flag
 *   0000 0010 - Brown out Reset Flag
 *   0000 0100 - External Reset Flag
 *   0000 1000 - Power-On Reset Flag */
#define watchdog_get_status()      MCUSR

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
 *   (WDRF) and the WDE control bit in the initialization routine */
#define watchdog_clear_status()    MCUSR = 0 //reset all statuses in the control register of the MCU

/* Somewhere, a C-professor is rolling over in his/her grave right now... */
#define watchdog_feed()            wdt_reset()
