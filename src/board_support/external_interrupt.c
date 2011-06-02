#include "external_interrupt.h"

void enable_external_interrupt_input(uint8_t id, interrupt_edge edge) {
	switch (id) {
		case 0:
			EIMSK |= (1<<INT0);
        		// Signal change triggers interrupt
        		switch (edge) {
				case lowlevel:
					EICRA |= (0<<ISC00);
					EICRA |= (0<<ISC01);
				break;
				case any:
					EICRA |= (1<<ISC00);
					EICRA |= (0<<ISC01);
				break;
				case falling:
					EICRA |= (0<<ISC00);
					EICRA |= (1<<ISC01);
				break;
				case rising:
					EICRA |= (1<<ISC00);
					EICRA |= (1<<ISC01);
				break;
			}
		break;
		case 1:
			EIMSK |= (1<<INT1);
        		// Signal change triggers interrupt
        		switch (edge) {
				case lowlevel:
					EICRA |= (0<<ISC10);
					EICRA |= (0<<ISC11);
				break;
				case any:
					EICRA |= (1<<ISC10);
					EICRA |= (0<<ISC11);
				break;
				case falling:
					EICRA |= (0<<ISC10);
					EICRA |= (1<<ISC11);
				break;
				case rising:
					EICRA |= (1<<ISC10);
					EICRA |= (1<<ISC11);
				break;
			}
		break;
		case 2:
			EIMSK |= (1<<INT2);
        		// Signal change triggers interrupt
        		switch (edge) {
				case lowlevel:
					EICRA |= (0<<ISC20);
					EICRA |= (0<<ISC21);
				break;
				case any:
					EICRA |= (1<<ISC20);
					EICRA |= (0<<ISC21);
				break;
				case falling:
					EICRA |= (0<<ISC20);
					EICRA |= (1<<ISC21);
				break;
				case rising:
					EICRA |= (1<<ISC20);
					EICRA |= (1<<ISC21);
				break;
			}
		break;
		case 3:
			EIMSK |= (1<<INT3);
        		// Signal change triggers interrupt
        		switch (edge) {
				case lowlevel:
					EICRA |= (0<<ISC30);
					EICRA |= (0<<ISC31);
				break;
				case any:
					EICRA |= (1<<ISC30);
					EICRA |= (0<<ISC31);
				break;
				case falling:
					EICRA |= (0<<ISC30);
					EICRA |= (1<<ISC31);
				break;
				case rising:
					EICRA |= (1<<ISC30);
					EICRA |= (1<<ISC31);
				break;
			}
		break;
		case 4:
			EIMSK |= (1<<INT4);
        		// Signal change triggers interrupt
        		switch (edge) {
				case lowlevel:
					EICRB |= (0<<ISC40);
					EICRB |= (0<<ISC41);
				break;
				case any:
					EICRB |= (1<<ISC40);
					EICRB |= (0<<ISC41);
				break;
				case falling:
					EICRB |= (0<<ISC40);
					EICRB |= (1<<ISC41);
				break;
				case rising:
					EICRB |= (1<<ISC40);
					EICRB |= (1<<ISC41);
				break;
			}
		break;
		case 5:
			EIMSK |= (1<<INT5);
        		// Signal change triggers interrupt
        		switch (edge) {
				case lowlevel:
					EICRB |= (0<<ISC50);
					EICRB |= (0<<ISC51);
				break;
				case any:
					EICRB |= (1<<ISC50);
					EICRB |= (0<<ISC51);
				break;
				case falling:
					EICRB |= (0<<ISC50);
					EICRB |= (1<<ISC51);
				break;
				case rising:
					EICRB |= (1<<ISC50);
					EICRB |= (1<<ISC51);
				break;
			}
		break;
		case 6:
			EIMSK |= (1<<INT6);
        		// Signal change triggers interrupt
        		switch (edge) {
				case lowlevel:
					EICRB |= (0<<ISC60);
					EICRB |= (0<<ISC61);
				break;
				case any:
					EICRB |= (1<<ISC60);
					EICRB |= (0<<ISC61);
				break;
				case falling:
					EICRB |= (0<<ISC60);
					EICRB |= (1<<ISC61);
				break;
				case rising:
					EICRB |= (1<<ISC60);
					EICRB |= (1<<ISC61);
				break;
			}
		break;
		case 7:
			EIMSK |= (1<<INT7);
        		// Signal change triggers interrupt
        		switch (edge) {
				case lowlevel:
					EICRB |= (0<<ISC70);
					EICRB |= (0<<ISC71);
				break;
				case any:
					EICRB |= (1<<ISC70);
					EICRB |= (0<<ISC71);
				break;
				case falling:
					EICRB |= (0<<ISC70);
					EICRB |= (1<<ISC71);
				break;
				case rising:
					EICRB |= (1<<ISC70);
					EICRB |= (1<<ISC71);
				break;
			}
		break;
	}
        // Global interrupt enable
        sei();
}

void disable_external_interrupt_input(uint8_t id) {
	switch (id) {
		case 1:
			EIMSK |= (0<<INT1);
		break;
		case 2:
			EIMSK |= (0<<INT2);
		break;
		case 3:
			EIMSK |= (0<<INT3);
		break;
		case 4:
			EIMSK |= (0<<INT4);
		break;
		case 5:
			EIMSK |= (0<<INT5);
		break;
		case 6:
			EIMSK |= (0<<INT6);
		break;
		case 7:
			EIMSK |= (0<<INT7);
		break;
	}
}
