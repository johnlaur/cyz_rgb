#include <avr/interrupt.h>
#include "cyz/cyz_rgb.h"
#include "cyz/cyz_cmd.h"

#define TWI_GEN_CALL         0x00
#include "usiTwi/usiTwiMaster.h"

Cyz_rgb* cyz_rgb;

script_line line1;
script_line line2;

int main(void) {
	cyz_rgb = CYZ_RGB_GET_INSTANCE();
	cyz_rgb->init();
	cyz_rgb->set_color(cyz_rgb, 255,125,50);

	USI_TWI_Master_Initialise();

	TIFR   = (1 << TOV0);  /* clear interrupt flag */
  	TIMSK  = (1 << TOIE0); /* enable overflow interrupt */
	TCCR0B = (1 << CS00);  /* start timer, no prescale */

	sei(); // enable interrupts
	line1.dur = 255;
	line1.cmd[0] = 'c';
	line1.cmd[1] = 255;
	line1.cmd[2] = 0;
	line1.cmd[3] = 0;

	line2.dur = 255;
	line2.cmd[0] = 'c';
	line2.cmd[1] = 0;
	line2.cmd[2] = 0;
	line2.cmd[3] = 255;
	for(;;)
	{}

	return 1;
}

void cyz_master_send_color() {
	unsigned char messageBuf[5];
	// send new color to slave
	messageBuf[0] = TWI_GEN_CALL; // The first byte must always consist of General Call code or the TWI slave address.
    messageBuf[1] = 'c';          // The command or data to be included in the general call.
    messageBuf[2] = cyz_rgb->fade_color.r;
    messageBuf[3] = cyz_rgb->fade_color.g;
    messageBuf[4] = cyz_rgb->fade_color.b;
    unsigned char success = USI_TWI_Start_Transceiver_With_Data( messageBuf, 5 );
    if (!success) {
		USI_TWI_Master_Initialise();

		switch(USI_TWI_Get_State_Info( )) {

			case USI_TWI_NO_DATA:             // Transmission buffer is empty
				//CYZ_RGB_set_color(255, 0 , 0);
			break;
			case USI_TWI_DATA_OUT_OF_BOUND:   // Transmission buffer is outside SRAM space
				//CYZ_RGB_set_color(0, 255 , 0);
			break;
			case USI_TWI_UE_START_CON:        // Unexpected Start Condition
				//CYZ_RGB_set_color(255, 0 , 0);
			break;
			case USI_TWI_UE_STOP_CON:        // Unexpected Stop Condition
				//CYZ_RGB_set_color(255, 255 , 0);
			break;
			case USI_TWI_UE_DATA_COL:         // Unexpected Data Collision (arbitration)
				//CYZ_RGB_set_color(0, 255 , 255);
			break;
			case USI_TWI_NO_ACK_ON_DATA:      // The slave did not acknowledge  all data
				//CYZ_RGB_set_color(255, 0 , 255);
			break;
			case USI_TWI_NO_ACK_ON_ADDRESS:   // The slave did not acknowledge  the address
				//CYZ_RGB_set_color(50, 0 , 0);
			break;
			case USI_TWI_MISSING_START_CON:   // Generated Start Condition not detected on bus
				//CYZ_RGB_set_color(0, 50 , 0);
			break;
			case USI_TWI_MISSING_STOP_CON:    // Generated Stop Condition not detected on bus
				//CYZ_RGB_set_color(0, 0 , 50);
			break;
		}
	}
}

/*	Triggered when timer overflows. */

ISR(SIG_OVERFLOW0)
{
	static unsigned int outercount = 0;
	static unsigned int sigcount = -1;
	if (++sigcount == 0) { //TODO: better to use another clock, prescaled



		// TODO: learn to predict how long beteen each overflow
		cyz_master_send_color();

		CYZ_CMD_play_script_line(cyz_rgb, (++outercount%2==0) ? &line1 : &line2);
	}

	cyz_rgb->pulse(cyz_rgb);
}