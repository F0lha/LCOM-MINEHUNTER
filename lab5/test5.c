#include <minix/syslib.h>
#include <minix/drivers.h>
#include "vbe.h"
#include "keyboard.h"
#include "test5.h"
#include "timer.h"
#include "i8254.h"
#include "pixmap.h"
#include "sprite.h"



#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8


void *test_init(unsigned short mode, unsigned short delay) {
	/* To be completed */
	long video_mem = vg_init(mode);
	int counter = 0;
	int ipc_status, loops = 0;
	message msg;
	int irq_set = timer_subscribe_int();

	while (loops != delay) {
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
					counter++;

					if (counter == 60) {
						loops++;
						counter = 0;
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

	timer_unsubscribe_int();
	vg_exit();
	printf ("%0x%.8X \n",get_phys_mem());
}


int test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color) {
	/* To be completed */

	unsigned int *ptr = vg_init(0x105);
	/*
	if(x+size>H_RES||y+size>V_RES) return -1;
	ptr += H_RES*2*size;
	ptr += 2*size;
	ptr -= 1;
	int i,diferenca=H_RES-(x+size);
	for(i=0;i<=x;i++){
		*ptr = color;
		ptr += 2;
	}
	ptr -= 2;
	for(i=0;i<(y-2);i++){
		ptr+=diferenca*2;
		ptr+=size*2;
		*ptr = color;
		ptr += x*2;
		*ptr = color;
	}
	ptr+=diferenca*2 + size*2;
	for(i=0;i<=x;i++){
		*ptr = color;
		ptr += 2;
	}
*/
	vg_fill(x,y, size ,size , color);
	//////////

	int breaker = 1;
	int two_bytes = 0;
	int ipc_status, r;
	message msg;

	int irq_set = kbd_subscribe_int();

	while(breaker) {

		/* Get a request message. */
		if ( (r=driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { // received notification
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) {
					kbd_int_handler();
					if(scan_code==TWO_BYTE_CODE){two_bytes = 1;}
					else if(scan_code>>7){
						if(two_bytes) ;
						else ;
						if (scan_code==BREAK_CODE_ESC)
							breaker = 0;
					}
					else
					{
						if (two_bytes){
							two_bytes=0;
						}
						else ;
					}
				}
				break;
			default:
				break; // no other notifications expected: do nothing
			}
		}
	}
	kbd_unsubscribe_int();

	//////////////
	vg_exit();
	return 0;
}

int test_line(unsigned short xi, unsigned short yi, 
		unsigned short xf, unsigned short yf, unsigned long color) {

	//plotLine(x0,y0, x1,y1)

	unsigned int *ptr = vg_init(0x105);typedef struct
	{
	uint8_t VESASignature[4]; /* 'VESA' 4 byte signature */
	uint16_t VESAVersion; /* VBE version number */
	phys_bytes OEMStringPtr; /* Pointer to OEM string */
	uint8_t Capabilities[4]; /* Capabilities of video card */
	phys_bytes VideoModePtr; /* Pointer to supported modes */
	short TotalMemory; /* Number of 64kb memory blocks */
	phys_bytes OemSoftwareRev; //VBE implementation Software revision
	phys_bytes OemVendorNamePtr; //Pointer to Vendor Name String
	phys_bytes OemProductNamePtr; //Pointer to Product Name String
	phys_bytes OemProductRevPtr; //Pointer to Product Revision String
	char reserved[236]; /* Pad to 256 byte block size */
	char OemData[256]; // Data Area for OEM Strings
	} VbeInfoBlock;

	vg_set_line(xi, yi, xf, yf, color);

		//////////


		//////////

	  	int breaker = 1;
	  	int two_bytes = 0;
	  	int ipc_status, r;
	  	message msg;

	  	int irq_set = kbd_subscribe_int();

	  	while(breaker) {

	  		/* Get a request message. */
	  		if ( (r=driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
	  			printf("driver_receive failed with: %d", r);
	  			continue;
	  		}
	  		if (is_ipc_notify(ipc_status)) { // received notification
	  			switch (_ENDPOINT_P(msg.m_source)) {
	  			case HARDWARE: /* hardware interrupt notification */
	  				if (msg.NOTIFY_ARG & irq_set) {
	  					kbd_int_handler();
	  					if(scan_code==TWO_BYTE_CODE){two_bytes = 1;}
	  					else if(scan_code>>7){
	  						if(two_bytes) ;
	  						else ;
	  						if (scan_code==BREAK_CODE_ESC)
	  							breaker = 0;
	  					}
	  					else
	  					{
	  						if (two_bytes){
	  							two_bytes=0;
	  						}
	  						else ;
	  					}
	  				}
	  				break;
	  			default:
	  				break; // no other notifications expected: do nothing
	  			}
	  		}
	  	}
	  	kbd_unsubscribe_int();

	  	//////////////
	  	vg_exit();
	  	return 0;
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {
	unsigned int *ptr = vg_init(0x105);
	int two_bytes = 0;
	int ipc_status, r;
	message msg;
	int breaker = 1;

	int irq_set = kbd_subscribe_int();


	// copy it to graphics memory

	draw_sprite(create_sprite(xpm, xi,yi));


	while(breaker) {

		/* Get a request message. */
		if ( (r=driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { // received notification
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) {
					kbd_int_handler();
					if(scan_code==TWO_BYTE_CODE){two_bytes = 1;}
					else if(scan_code>>7){
						if(two_bytes) ;
						else ;
						if (scan_code==BREAK_CODE_ESC)
							breaker = 0;
					}
					else
					{
						if (two_bytes){
							two_bytes=0;
						}
						else ;
					}
				}
				break;
			default:
				break; // no other notifications expected: do nothing
			}
		}
	}
	kbd_unsubscribe_int();

	//////////////
	vg_exit();


}	

int test_move(unsigned short xi, unsigned short yi, char *xpm[], 
		unsigned short hor, short delta, unsigned short time) {

	unsigned int *ptr = vg_init(0x105);
	int two_bytes = 0,loops = 0,global_counter = 0;
	int ipc_status, r;
	message msg;
	int breaker = 1;
	int negative = 0;

	float pixels_per_sec = (float)delta /((float)time*60), x = 0, y = 0, x_se = 0,y_se = 0, andado = 0;
	if(delta < 0){
		negative = 1;
		delta = -delta;
		pixels_per_sec = -pixels_per_sec;
	}

	int irq_set_key = kbd_subscribe_int();
	int irq_set_timer =timer_subscribe_int();


	// copy it to graphics memory
	Sprite *sp = (Sprite *) malloc ( sizeof(Sprite));
	sp = create_sprite(xpm, xi,yi);
	draw_sprite(sp);


	while(breaker) {

		/* Get a request message. */
		if ( (r=driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { // received notification
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_timer) {
					global_counter++;
					if(hor)
					{
						x += pixels_per_sec;
						x_se += pixels_per_sec;
						if(x_se>1)
						{

							int x_se_int = x_se;
							x_se = x_se - (float)x_se_int;
							erase_sprite(sp);
							if(negative)
								sp->x -= x - x_se;
							else
								sp->x += x - x_se;
							andado += x - x_se;
							draw_sprite(sp);
							x = x_se;

						}
						if(negative){
							if((sp->x - pixels_per_sec) < 0){
								breaker = 0;

							}
							else
								if((sp->x + pixels_per_sec + sp->width) >= H_RES){
									breaker = 0;
								}
						}
					}
						else{
							y += pixels_per_sec;
							y_se += pixels_per_sec;
							if(y_se>1)
							{
								int y_se_int = y_se;
								y_se = y_se - (float)y_se_int;
								erase_sprite(sp);
								if(negative)
									sp->y -= y - y_se;
								else
									sp->y += y - y_se;
								andado +=y - y_se;
								draw_sprite(sp);
								y = y_se;
							}
							if(negative){
								if((sp->y - pixels_per_sec) < 0){
									breaker = 0;
								}
							}
							else{
								if((sp->y + pixels_per_sec + sp->height) >= V_RES){
									breaker = 0;
								}
							}

							if (global_counter == 60) {
								loops++;
								global_counter = 0;
							}
							if(andado > delta){
								breaker = 0;
							}
						}
				}

				else if (msg.NOTIFY_ARG & irq_set_key) {
					kbd_int_handler();
					if(scan_code==TWO_BYTE_CODE){two_bytes = 1;}
					else if(scan_code>>7){
						if(two_bytes) ;
						else ;
						if (scan_code==BREAK_CODE_ESC)
							breaker = 0;
					}
					else
					{
						if (two_bytes){
							two_bytes=0;
						}
						else ;
					}
				}
				break;
			default:
				break; // no other notifications expected: do nothing
			}
		}
	}

	/// para meter para tras basta fazer um if, e depois copy paste com - em vez dos +... funciona como positives se meteres em absoluto


	kbd_unsubscribe_int();
	timer_unsubscribe_int();
	//////////////
	vg_exit();

}					


int test_controller(){
	VbeInfoBlock estrutura;
	vbe_get_controller_info(&estrutura);
	printf("Capabilities : \n");
	if(estrutura.Capabilities[0] && BIT(0))
		printf("DAC is fixed width, with 6 bits per primary color\n");
	else printf("AC width is switchable to 8 bits per primary color\n");
	if(estrutura.Capabilities[0] && BIT(1))
			printf("Controller is not VGA compatible\n");
		else printf("Controller is VGA compatible\n");
	if(estrutura.Capabilities[0] && BIT(2))
			printf("When programming large blocks of information to the RAMDAC,use the blank bit in Function 09h.\n");
		else printf("Normal RAMDAC operation\n");
	printf("Size Of VRam Memory: %d KB\n" ,estrutura.TotalMemory*64);
	printf("VideoModePtr : 0x%08x",estrutura.VideoModePtr);
}




