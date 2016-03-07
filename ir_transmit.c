#include <stdio.h>
#include <stdlib.h>
#include <mraa/pwm.h>
#include <signal.h>

static volatile int run_flag = 1;

#define SCALING_FACTOR 5 
#define DUTY 0.5
#define PREAMBLE_DELAY 750000/SCALING_FACTOR // 10 miliseconds
#define SHORT_DELAY 350000/SCALING_FACTOR // 5 miliseconds
#define LONG_DELAY 1400000/SCALING_FACTOR // 20 miliseconds

mraa_pwm_context pwm1;

void do_when_ctrl_c(int sig) {
	if (sig == SIGINT) {
		run_flag = 0;
	}
}

void send_preamble_sequence(int preamble_length) {
	int i = 0, j = 0;
	
	for(i = preamble_length; i > 0; i--){
		
		//equal durations
		mraa_pwm_write(pwm1,DUTY);
		for(j = PREAMBLE_DELAY; j > 0; j--);

		mraa_pwm_write(pwm1,0);
		for(j = PREAMBLE_DELAY; j > 0; j--);
	}
}

void send_low_bit() {
	int i = 0;
	printf("0");

	mraa_pwm_write(pwm1,DUTY);
	for(i = SHORT_DELAY; i > 0; i--); // 5 ms
	
	mraa_pwm_write(pwm1,0);
	for(i = LONG_DELAY; i > 0; i--); // 20 ms
}

void send_high_bit() {
	int i = 0;
	printf("1");
	
	mraa_pwm_write(pwm1,DUTY);
	for(i = LONG_DELAY; i > 0; i--); //20 ms

	mraa_pwm_write(pwm1,0);
	for(i = SHORT_DELAY; i > 0; i--); // 5 ms
}

int main(int argc, char *argv[]) {
	
	printf("\n-----ir_transmit.c-----n");
	
	// GPIO Initialization	
	pwm1 = mraa_pwm_init(0);
	mraa_pwm_period_us(pwm1, 26);
	mraa_pwm_enable(pwm1, 1);
	int DATA_VALUE = 0, preamble_length = 5;

	if (argc == 2) {
		DATA_VALUE = atoi(argv[1]);
		printf("Arg(2) Passed In! DATA_VALUE set to: %d\n", DATA_VALUE);
	}
	
	while(1) {
		printf("\n---------------------Transmission---------------------\n");
		
		// Preamble - Signals the Receiver Message Incoming
		send_preamble_sequence(preamble_length);
		
		// Sending Edison Board ID # - 2 bits, MSB then LSB
		printf("DATA_VALUE: %d - ", DATA_VALUE);
		switch (DATA_VALUE) {
			case 0:
				send_low_bit();		// Send lsb bit 0 = LOW
				send_low_bit();		// Send msb bit 0 = LOW
				break;
			case 1:
				send_low_bit();		// Send msb bit 0 = LOW
				send_high_bit();	// Send lsb bit 1 = HIGH
				break;
			case 2:
				send_high_bit();	// Send msb bit 1 = HIGH
				send_low_bit();		// Send lsb bit 0 = LOW
				break;
			case 3:
				send_high_bit();	// Send lsb bit 1 = HIGH
				send_high_bit();	// Send msb bit 1 = HIGH
				break;
			default: // same as case 0
				send_low_bit();		// Send lsb bit 0 = LOW
				send_low_bit();		// Send msb bit 0 = LOW				
		}		
	}// end while loop

	mraa_pwm_write (pwm1, 0);
	mraa_pwm_enable(pwm1, 0);

	return 0;
}//end main
