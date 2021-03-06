#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"
#include "timer.h"

int global_counter, hook_id;

int timer_set_square(unsigned long timer, unsigned long freq) {

	unsigned long div;

	printf("%lu \n", freq);

	unsigned long timer_selector = TIMER_0 + timer;
	unsigned long arg;
	arg = TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_; //forms read-back command
	sys_outb(TIMER_CTRL, arg); //writes in TIMER_CTRL the read-back command formed in arg

	div = TIMER_FREQ / freq;
	unsigned long freq_lsb = div;
	unsigned long freq_msb = (div >> 8);

	if (timer == 0)
		arg |= TIMER_SEL0;
	else if (timer == 1)
		arg |= TIMER_SEL1;
	else if (timer == 2)
		arg |= TIMER_SEL2;
	else
		return 1;

	arg |= TIMER_LSB_MSB;
	arg |= TIMER_SQR_WAVE;
	printf("%lu   %lu\n", freq_lsb, freq_msb);

	sys_outb(TIMER_CTRL, arg);
	sys_outb(TIMER_0 + timer, freq_lsb);
	sys_outb(TIMER_0 + timer, freq_msb);

	return 0;
}


int timer_subscribe_int(void) {
	hook_id = HOOK_ID;
	sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id); //used to subscribe a notification on every interrupt in the input TIMER0_IRQ
	sys_irqenable(&hook_id); //enables interrupts on the IRQ line associated with the specified hook_id
	return HOOK_ID;
}

int timer_unsubscribe_int() {
	return (sys_irqrmpolicy(&hook_id)||sys_irqdisable(&hook_id)); //unsubscribes a previous subscription of the interrupt notification associated with the specified hook_id
}

void timer_int_handler() { //increments a counter on every interrupt
	global_counter++;
}

int timer_get_conf(unsigned long timer, unsigned char *st) {
	unsigned long timer_selector = TIMER_0 + timer;
	unsigned long temp;
	unsigned long arg;
	arg = TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_;
	sys_outb(TIMER_CTRL, arg);
	sys_inb(timer_selector, &temp);
	*st = temp;

	return 0;
}

int timer_display_conf(unsigned char conf) {

	if ((conf & BIT(0))) {
		printf("BCD\n");
	} else {
		printf("binary\n");
	}

	if ((conf & BIT(6)) == BIT(6)) {
		printf("null_counter = 1\n");
	} else if ((conf & BIT(6)) == 0) {
		printf("null_counter = 0\n");
	}
	if ((conf & (BIT(1) | BIT(2) | BIT(3))) == 0)
		printf("INTERRUPT ON TERMINAL COUNT\n");
	else if ((conf & (BIT(1) | BIT(2) | BIT(3))) == BIT(1))
		printf("HARDWARE RETRIGGERABLE ONE-SHOT\n");
	else if ((conf & (BIT(1) | BIT(2) | BIT(3))) == BIT(2))
		printf("RATE GENERATOR\n");
	else if ((conf & (BIT(1) | BIT(2) | BIT(3))) == (BIT(1) | BIT(2)))
		printf("SQUARE WAVE MODE\n");
	else if (conf & (BIT(1) | BIT(2) | BIT(3)) == BIT(3))
		printf("SOFTWARE TRIGGERED STROBE\n");
	else if (((conf & (BIT(1) | BIT(2) | BIT(3)))) == (BIT(3) | BIT(1)))
		printf("HARDWARE TRIGGERED STROBE (RETRIGGERABLE)\n");
	else
		printf("ERRO\n");

	if ((conf & (BIT(7))) == 0)
		printf("OUTPUT = 0\n");
	else
		printf("OUTPUT = 1\n");

	if ((conf & (BIT(4) | BIT(5))) == BIT(4))
		printf("LSB\n");
	else if ((conf & (BIT(4) | BIT(5))) == BIT(5))
		printf("MSB\n");
	else if ((conf & (BIT(4) | BIT(5))) == BIT(4) | BIT(5))
		printf("LSB FOLLOWED BY MSB\n");

	return 0;
}

int timer_test_square(unsigned long freq) { //configures a timer to generate a square wave with a given frequency
	if (freq == 0) {
		return 1;
	}
	else {
		int res = timer_set_square(0, freq);
		return res;
	}
}

int timer_test_int(unsigned long time) { //tests if it handles interrupts generated by time

	global_counter = 0;
	int ipc_status, loops = 0;
	message msg;
	int irq_set = BIT(timer_subscribe_int());

	while (loops != time) {
		/* Get a request message. */
		if (driver_receive(ANY, &msg, &ipc_status) != 0) {
			printf("driver_receive failed with: %d");
			continue;
		}
		if (is_ipc_notify(ipc_status)) {

			/* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) {
					timer_int_handler();

					if (global_counter == 60) {
						loops++;
						global_counter = 0;
						printf("message %d\n", loops);
					}
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	return timer_unsubscribe_int();
}

int timer_test_config(unsigned long timer) {
	if (timer > 2 || timer < 0)
	{
		printf("Timer invalid\n");
		return 1;
	}
	unsigned char test;
	timer_get_conf(timer, &test);
	timer_display_conf(test);
	return 0;
}
