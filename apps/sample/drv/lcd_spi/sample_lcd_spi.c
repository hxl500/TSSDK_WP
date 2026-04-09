#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>

#include <stdint.h>
#include <sys/mman.h>

#include <pthread.h>


//#include "gc9108reg.h"
/* Use 'j' as magic number */
#define GC9108_IOC_MAGIC  'j'
#define GC9108_IOC_BUFFERBASE     _IOR(GC9108_IOC_MAGIC,  3, unsigned long)
#define GC9108_IOC_BUFFERSIZE     _IOR(GC9108_IOC_MAGIC,  4, unsigned int)
#define GC9108_IOC_REFRESH        _IO(GC9108_IOC_MAGIC,  5)
#define GC9108_IOC_ALLOCATE_PHYSICAL_MEMORY   _IOR(GC9108_IOC_MAGIC,  6, unsigned int)

#ifndef GC9108_MODULE_PATH
#define GC9108_MODULE_PATH "/dev/gc9108"
#endif
#define LCD_WIDTH 128
#define LCD_HEIGHT 128

pthread_mutex_t gc9108_mutex;
#define DRIVER_TEST 1


#if 1
#define WHITE            0xFFFF
#define BLACK            0x0000   
#define BLUE             0x001F  
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40 //
#define BRRED            0XFC07 //
#define GRAY             0X8430 //
#endif
uint16_t test_color_format[14] = 
{
  GREEN, BLUE,  WHITE, BLACK, YELLOW,
  GRAY,  BRRED, CYAN,  BRED,  GRED,
  GBLUE, MAGENTA, BROWN, RED

};

uint16_t* lcd_data_buf =NULL;

typedef struct lcd_buffer{
    unsigned int size;
    unsigned int phys_addr;
    unsigned long   base;
    unsigned long   virt_addr;
} lcd_buffer_t;



void gc9108_draw_point(int x, int y,uint16_t color)
{
	uint16_t c = 0;	  //set_color_data to spi_screen
	c = color << 8;
	c |= (color>>8 & 0x00ff);
	lcd_data_buf[y * 128 + x] = c; 
 
}
 
void set_gc9108_color(uint16_t color)
{
  unsigned int x,y;	
 
  for(y = 0;y < LCD_WIDTH; y++){           
	for(x = 0;x < LCD_WIDTH ; x++){		
		gc9108_draw_point(x,y,color);
    }

}
}

/*
 * @description		: main
 * @param - argc 	: argv
 * @param - argv 	: 
 * @return 			: 0
 */
//int main(int argc, char *argv[])
int main(void)
{
	int gc9108_fd = -1;
	int ret = 0;



	gc9108_fd = open(GC9108_MODULE_PATH, O_RDWR);
	if(gc9108_fd < 0) {
		printf("can't open file %s\r\n", GC9108_MODULE_PATH);
		return -1;
	}
#if DRIVER_TEST	
	//mmap
	lcd_data_buf =  (uint16_t*)mmap(NULL,(2*LCD_WIDTH *LCD_HEIGHT),PROT_READ|PROT_WRITE,MAP_SHARED, gc9108_fd, 0);
    if(lcd_data_buf == MAP_FAILED)
    {
        printf("lcd_data_buf: Failed to mmap driver kmalloc buf\n");
        return -1;
    }
	printf("lcd_data_buf =%p !!!!!\r\n", lcd_data_buf);

	
#endif
	pthread_mutex_init(&gc9108_mutex,NULL);

	
	int   j =10;
	int tmp =0;



	while (j > 0) {
		pthread_mutex_lock(&gc9108_mutex); 
		printf("TIMER A begin,j =%d\r\n",j);
#if !DRIVER_TEST	
		unsigned long base;
		unsigned int size;
		lcd_buffer_t lcd_buffer;

		//ioctl(gc9108_fd, GC9108_IOC_BUFFERBASE, &base);
		//printf("base  =0x%08x\r\n",(unsigned long)base);
		//ioctl(gc9108_fd, GC9108_IOC_BUFFERSIZE, &size);
		//printf("size  =0x%08x\r\n",size);
        lcd_buffer.virt_addr = 0xa0000000;
		lcd_buffer.phys_addr = 0x32768; 
        lcd_buffer.size = (2*LCD_WIDTH *LCD_HEIGHT);//default:rgb565
        
		ioctl(gc9108_fd, GC9108_IOC_ALLOCATE_PHYSICAL_MEMORY, &lcd_buffer);
		/***test_full_lcd_data***/
#endif
		set_gc9108_color(test_color_format[j % 14]);
		//ret = write(fd, lcd_data_buf, sizeof(uint16_t)*LCD_WIDTH *LCD_HEIGHT);
		ioctl(gc9108_fd, GC9108_IOC_REFRESH, &tmp);
		printf("TIMER B end =%d\n",j);

		if(ret != 0) { 			/*  */

			printf("write lcd screen failed\r\n");
            break;
		}
		pthread_mutex_unlock(&gc9108_mutex);	
		j--;
		//usleep(500000); /*100ms */
	}
	j=0;



	pthread_mutex_destroy(&gc9108_mutex);		
	close(gc9108_fd);	/*  */	
	return 0;
}

