ipf
==========

*No one is idiotproof*.  When I was working with the ATMega328P, I kept on running into my own mistakes, and got sick and tired of crawling through register descriptions for hours only to find I had missed a register name, or accidentally switched it with another similarly named one.

There are two ways to go about fixing this problem.  
 1. Rewrite the avr/io.h library
 2. Create configurable functions that wrap common register setups

Instead of searching through the [datasheet](http://www.atmel.com/Images/doc8161.pdf), the goal of this library is to get you up and on your feet in a couple of lines, without bothering you to walk through 5 lines of code while alt-tabbing to the specs.

Next Steps:
 - A-D Converter - Enums for modes, prescalers, and setup
 - PWM Mode(s)   - Enums for prescalers.  Maybe enums for timer modes... if it's logical to do so
