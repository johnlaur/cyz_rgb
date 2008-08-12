/*****************************************************************************
 *	Codalyze PWM RGB: A very basic implementation of pulse-width modulation (PWM) on an AVR controller with
 *  three leds of different color(R,G,B): display any 24-bit color using three leds.
 *
 *	PWM in Plain English:
 *	 PROBLEM:	We have three leds, each led has two positions (on/off): we can only get 8 colors.
 *				We fancy displaying 24-bit rgb colors.
 *
 *	 SOLUTION:	We turn leds on and off very fast and trick the eye. ex: we want to display
 *				RGB{255, 165, 0} (orange): overy 255 ticks
 *				- red led is always on
 *				- green led is on for the first 165 ticks and then is off
 *				- blue led is always off
 *	 Much more detail on wikipedia.
 *
 *	This code has been implemented for and tested on ATtiny45 with 3 leds connected on PB3, PB4, PB1.
 *	It should work on all avr chips, just remember to update definitions of
 *	PWM_PORT,PWM_DDR, PINRED, PINGRN, PINBLU according to your setup.
 *
 *	Compiler: AVR-GCC
 *
 *	Usage:
 *		1. invoke CYZ_RGB_setup(); in global space
 *		2. invoke CYZ_RGB_init(); in main
 *		3. invoke CYZ_RGB_pulse(); at fixed intervals;
 *		   putting it in SIGNAL(SIG_OVERFLOW0) {} works good (initialize timers and signals, if you do)
 *		4. every time CYZ_RGB_set_color(red, green, blue) is called, the perceived color will change
 *		   if CYZ_RGB_set_color(red, green, blue) is called, the color fades from one color to the other
 *
 *	Links:
 *	ATtiny45 - http://www.atmel.com/dyn/products/Product_card.asp?part_id=3618
 *
 *	Author: Matteo Caprari <matteo.caprari@gmail.com>
 *	Thanks to: Lorenzo Grespan, Todd E. Kurt, AVR Application note 136: Low-jitter Multi-channel Software PWM
 *
 *	License: new BSD license
 *******************************************************************************/

/* select port and data direction register on which pin leds are */
#include <stdlib.h>
#define PWM_PORT PORTB
#define PWM_DDR DDRB

/* map leds to actual pins */
#define PINRED PB3
#define PINGRN PB4
#define PINBLU PB1


/******************************/
/* turn off single leds */
#define RED_LED_OFF PWM_PORT &= ~(1<<PINRED)
#define GRN_LED_OFF PWM_PORT &= ~(1<<PINGRN)
#define BLU_LED_OFF PWM_PORT &= ~(1<<PINBLU)

/* turn on single leds */
#define RED_LED_ON PWM_PORT |= 1<<PINRED
#define GRN_LED_ON PWM_PORT |= 1<<PINGRN
#define BLU_LED_ON PWM_PORT |= 1<<PINBLU

typedef struct _color {
    unsigned char r;
	unsigned char g;
	unsigned char b;
} Color;

//TODO: i can't declare pointer to functions with Cyz_rgb* in the signature, have to fallback to void* and cas on assignment.
// it seems that Cyz_rgb does not exist yet.
typedef struct CYZ_RGB {
	void (*init)();
	void (*set_color)(void* cyz_rgb, unsigned char r, unsigned char g, unsigned char b);
	void (*set_fade_color)(void* cyz_rgb, unsigned char r, unsigned char g, unsigned char b);
	void (*pulse)(void* cyz_rgb);
	unsigned char pulse_count;
	unsigned char fade;
	Color color;
	Color fade_color;
} Cyz_rgb;

void _CYZ_RGB_init() {
	PWM_DDR |= 1<<PINRED;
	PWM_DDR |= 1<<PINGRN;
	PWM_DDR |= 1<<PINBLU;
}

void _CYZ_RGB_set_color(Cyz_rgb* this, unsigned char r, unsigned char g, unsigned char b) {
	this->color.r = r;
	this->color.g = g;
	this->color.b = b;
}

void _CYZ_RGB_set_fade_color(Cyz_rgb* this, unsigned char r, unsigned char g, unsigned char b) {
	this->fade = 1;
	this->fade_color.r = r;
	this->fade_color.g = g;
	this->fade_color.b = b;
}

void __CYZ_RGB_fade_step(Cyz_rgb* this) {
	if (this->color.r!=this->fade_color.r) { this->color.r += ((this->color.r>this->fade_color.r) ? -1 : +1); }
	if (this->color.r!=this->fade_color.g) { this->color.g += ((this->color.g>this->fade_color.g) ? -1 : +1); }
	if (this->color.r!=this->fade_color.b) { this->color.b += ((this->color.b>this->fade_color.b) ? -1 : +1); }
}

void _CYZ_RGB_pulse(Cyz_rgb* this) {
	if (++this->pulse_count == 0) { RED_LED_ON; GRN_LED_ON; BLU_LED_ON; }
	if (this->pulse_count == this->color.r) RED_LED_OFF;
	if (this->pulse_count == this->color.g) GRN_LED_OFF;
	if (this->pulse_count == this->color.b) BLU_LED_OFF;
	if (this->pulse_count == 0 && this->fade) {
		__CYZ_RGB_fade_step(this);
	}
}

Cyz_rgb* CYZ_RGB_GET_INSTANCE() {
	 Cyz_rgb* instance = (Cyz_rgb*) malloc(sizeof(struct CYZ_RGB));
	 instance->init = _CYZ_RGB_init;
	 instance->set_color = (void*)_CYZ_RGB_set_color;
	 instance->set_fade_color = (void*)_CYZ_RGB_set_fade_color;
	 instance->pulse_count = 0xFF;
	 instance->fade = 0;
	 instance->pulse = (void*)_CYZ_RGB_pulse;
	 return instance;
}
