#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

static char *phys_mem;

char *get_phys_mem(){
	return phys_mem;
}


int vg_exit() {
  struct reg86u reg86;

  reg86.u.b.intno = 0x10; /* BIOS video services */

  reg86.u.b.ah = 0x00;    /* Set Video Mode function */
  reg86.u.b.al = 0x03;    /* 80x25 text mode*/

  if( sys_int86(&reg86) != OK ) {
      printf("\tvg_exit(): sys_int86() failed \n");
      return 1;
  } else
      return 0;
}


void *vg_init(unsigned short mode){
	struct reg86u r;
	r.u.w.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
	r.u.w.bx = 1<<14|mode; // set bit 14: linear framebuffer
	r.u.b.intno = 0x10;
	if( sys_int86(&r) != OK ) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return;
	}
	int s;
	struct mem_range mr;


	/* Allow memory mapping */
	mr.mr_base = (phys_bytes)(VRAM_PHYS_ADDR);
	mr.mr_limit = mr.mr_base + (H_RES*V_RES*BITS_PER_PIXEL/8);

	if( OK != (s = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", s);

	/* Map memory */
	video_mem = vm_map_phys(SELF, (void *)mr.mr_base, H_RES*V_RES*BITS_PER_PIXEL/8);

	if(video_mem == MAP_FAILED)
		panic("video_txt: couldn't map video memory");
	return video_mem;
}


void vg_set_pixel(unsigned int x,unsigned int y, unsigned long color){
	char *ptr = video_mem;
	ptr += x;
	ptr += H_RES*y;
	*ptr = color;
}

void vg_fill(unsigned int x,unsigned int y, unsigned int width, unsigned int height,unsigned long color){
	int i,j;
	for(j = 0; j < height;j++){
		for(i = 0; i < width;i++)
		{
			vg_set_pixel(x+i,y+j,color);
		}
	}
}

void vg_set_line(unsigned short xi, unsigned short yi,
		unsigned short xf, unsigned short yf, unsigned long color){
	float dx=xf-xi;
	float dy=yf-yi;
	float d = 2*dy - dx;
	float y = yi;
	float x = xi;






	if(xi==xf){
		if(yi > yf){
			for (yf; yf < yi; yf++)
				vg_set_pixel(xi, yf,color);
		}else{
			for (yi; yi < yf; yi++)
				vg_set_pixel(xi, yi,color);
		}

	}
	else if(yi==yf){
		if(xi > xf){
					for (xf; xf < xi; xf++)
						vg_set_pixel(xf, yi,color);
				}else{
					for (xi; xi < xf; xi++)
						vg_set_pixel(xi, yi,color);
				}
	}
	else{
		if(xf < xi && yf > yi){
			x = xf;
			xf = xi;
			xi = x;
		}
		else if(xf < xi && yf < yi)
			{
				x = xf;
				xf = xi;
				xi = x;
				y = yf;
				yf = yi;
				yi = y;
			}



			dx=xf-xi;
			dy=yf-yi;



		float erro;

		if(dy > 0)
			erro= 2*dy - dx;
		else erro= 2*dx + dy;
		vg_set_pixel(x,y,color);


		//


		vg_set_pixel(x, y, color);
		if(dy > 0){
			while((x <= xf) && (y <= yf)){

				vg_set_pixel(x,y,color);
				if(erro > 0)
				{
					y = y + 1;
					erro = erro - 2 * dx;
				}
				else
				{
					x = x + 1;
					erro = erro + 2 * dy;
				}
			}
		}
		else{
			while((x <= xf) && (y >= xi)){
				vg_set_pixel(x,y,color);
				if(erro > 0)
				{
					y = y - 1;
					erro = erro - 2 * dx;
				}
				else
				{
					x = x + 1;
					erro = erro - 2 * dy;
				}
			}
		}



	}
}
