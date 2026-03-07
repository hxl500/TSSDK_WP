#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
/* needed for __init,__exit directives */
#include <linux/init.h>
/* needed for remap_page_range
 * SetPageReserved
 * ClearPageReserved
 */
#include <linux/mm.h>
/* obviously, for kmalloc */
#include <linux/slab.h>
/* for struct file_operations, register_chrdev() */
#include <linux/fs.h>
/* standard error codes */
#include <linux/errno.h>


#include <linux/ide.h>

#include <linux/ioport.h>
#include <asm/io.h>

#include <linux/version.h>


#include <linux/cdev.h>
#include <linux/device.h>

#include <linux/semaphore.h>
#include <linux/spinlock.h>


#include <linux/timer.h>
#include <linux/delay.h>

#include <linux/gpio.h>

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>

#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <linux/major.h>
//#include <asm/page.h>

/* spi struct */
#include <linux/spi/spi.h>
#include "gc9108reg.h"



/***************************************************************
Copyright Tsingmicro. All rights reserved.
file		: gc9108.c

***************************************************************/
#define gc9108_CNT	1
#define gc9108_NAME	"gc9108"


#define LCD_W 128

#define LCD_H 128

#define RGB565_BYTE 2

#define DRIVER_TEST 0

u16 color_buf[14] = {

  GREEN, BLUE,WHITE, BLACK,	YELLOW,GRAY ,BRRED ,CYAN ,BRED,GRED,GBLUE,MAGENTA,BROWN,RED

};

//static DEFINE_SPINLOCK(gc9108_lock);

static spinlock_t gc9108_lock = __SPIN_LOCK_UNLOCKED(gc9108_lock);



uint16_t* gc9108_gram = NULL;

struct gc9108_dev {
	dev_t devid;				/* num 	 */
	struct cdev cdev;			/* cdev 	*/
	struct class *class;		/* class 		*/
	struct device *device;		/* dev 	 */
	struct device_node	*nd; 	/* dev_ep */
	int major;					/* mojar */
	void *private_data;			/* private	*/
          
    int                  res_gpio;   //reset          
    int                  dc_gpio;   //rs        
    int                  cs_gpio;  //blk_gpio--cs
    int                  bk_gpio;  //blk_gpio--bk
    unsigned long buffer_virbase;
	unsigned int buffer_size;
	unsigned int buffer_physaddr;
	lcd_buffer_t gc9108_lcd_buffer;

};

static struct gc9108_dev gc9108dev;



lcd_init_cmd_t init_cmds[] = {
	{0xfe, {0x00}, 0x00,0}, //


	{0xfe, {0x00}, 0x00,0}, // 
	{0xef, {0x00}, 0x00,0},	  // 


	{0xb3, {0x03}, 0x01,0},	  // 


	{0xb6, {0x01}, 0x01,0},		// 


	{0xA3, {0x11}, 0x01,0},	// Frame Rate Control
	{0x21, {0x00}, 0x00,0}, //Display Inversion ON
	{0x36, {0xd0}, 0x01,0}, //Memory Access Ctrl
	{0x3A, {0x05}, 0x01,0},	//COLMOD: Pixel Format Set:rgb565
	{0xB4, {0x21}, 0x01,0},  //Display Inversion Control
	{0xB1, {0xc0}, 0x01,0},	 //To have access to E6, bit [6] of B1 need be set to 1
	{0xE6, {0x50,0x43}, 0x02,0},	  //VREG1_CTL
	{0xE7, {0x56,0x43}, 0x02,0},	//To have access to E7, bit [7] of B1h need be set to 1
	{0xF0, {0x1A,0x60,0x33,0x6F,0xFD,0x3D,0x30,0x0,0x0,0x1F,0x1F,0x18,0x1A,0xF}, 0xe,0},  //SET_GAMMA1 (F0h) 
	{0xF1, {0x2,0x27,0x1B,0x37,0x59,0x03,0x30,0x00,0x00,0x1F,0xF,0x17,0x1A,0xF}, 0xe,0}, // 
	{0xfe, {0x00}, 0x00,0},
	{0xff, {0x00}, 0x00,0},				 //
	{0x35, {0x00}, 0x01,0}, //
	{0x44, {0x00}, 0x01,0},	  // 
	{0x11, {0x00}, 0x00,120},	  // 
	{0x29, {0x00}, 0x00,0},				 //
	//{0x2c, {0x00}, 0x00,0},

	{0x0, {0x0}, 0xff}  //0xff-endflag
};


#if 1
/*
 * @description	: send
 * @param - dev:  
 * @param - buf:  data_buf
 * @param - len:  len
 * @return 	  :   
 */
static s32 gc9108_write_regs(struct gc9108_dev *dev, u8 *buf, int len)
{
	int ret = -1;
	//unsigned char *txdata;
	struct spi_message m;
	struct spi_transfer *t;
	struct spi_device *spi = (struct spi_device *)dev->private_data;
	
	t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);	/* 申请内存 */
	//memset(t,0x0,sizeof(struct spi_transfer));
	if(!t) {
		return -ENOMEM;
	}
#if 0
	txdata = kzalloc(sizeof(char)+len, GFP_KERNEL);
	if(!txdata) {
		goto out1;
	}
#endif
	t->tx_buf = buf;			/* send buffer */


	t->len = len;

	//t->delay_usecs = 0;
	t->speed_hz = 8000000;
	//t->bits_per_word = 8;


	
	spi_message_init(&m);		/*  */
	spi_message_add_tail(t, &m);/*  */
	ret = spi_sync(spi, &m);	/* send */


    if(ret) {
        printk("###spi_sync return fail###\r\n");
    }
#if 0	
out2:
	kfree(txdata);				/*  */
#endif

	kfree(t);					/*  */
	return ret;
}

/*
 * @description	: gc9108_write_onereg
 * @param - dev:  
 * @param - buf: 
 * @return   : 
 */	
static void gc9108_write_onereg(struct gc9108_dev *dev, u8 buf)
{
	gc9108_write_regs(dev, &buf, 1);

}


static int gc9108_read_regs(struct gc9108_dev *dev, void *buf, int len)
{

	int ret = -1;
	//unsigned char txdata[1];
	unsigned char * rxdata;
	struct spi_message m;
	struct spi_transfer *t;
	struct spi_device *spi = (struct spi_device *)dev->private_data;
    
	t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);	/*  */
	if(!t) {
		return -ENOMEM;
	}

	rxdata = kzalloc(sizeof(char) * len, GFP_KERNEL);	/*  */
	if(!rxdata) {
		goto out1;
	}

    t->rx_buf = rxdata;			/* read_date */
	t->len = len;	/* len */

	//t->delay_usecs = 0;
	t->speed_hz = 8000000;
	//t->bits_per_word = 8;
	
	spi_message_init(&m);		/*  */
	spi_message_add_tail(t, &m);/*  */
	ret = spi_sync(spi, &m);	/*  */
	if(ret) {
		goto out2;
	}
	
    memcpy(buf , rxdata, len);  /* copytouser */

out2:
	kfree(rxdata);					/*  */
out1:	
	kfree(t);						/*  */
	
	return ret;
}

/*
 * @description	: gc9108_read_lcd_data
 * @param - dev:  
 * @param - buf:  
 * @return 	  :  
 */
static unsigned char gc9108_read_lcd_data(struct gc9108_dev *dev, u8* buf,int len)
{

		gpio_set_value(dev->dc_gpio, 1);
		//gpio_direction_output(gc9108dev.cs_gpio, 0);
		
		gc9108_read_regs(dev,  buf, len);
		
		//gpio_direction_output(gc9108dev.cs_gpio, 1);

	return 0;
}


/*
 * @description	: write_command
 * @param - dev:  
 * @param - cmd:  
 * @return 		: 
 */
void write_command(struct gc9108_dev *dev, u8 cmd)

{

    // dc , command:0



	gpio_set_value(dev->dc_gpio, 0);

	//gpio_direction_output(gc9108dev.cs_gpio, 0);

 

    gc9108_write_onereg(dev,cmd);


	//gpio_direction_output(gc9108dev.cs_gpio, 1);

}


#endif



/*
 * @description	:
 * 				: 
 * @param - dev	: 
 * @return 		: 
 */
void read_datas(struct gc9108_dev *dev,u8 cmd,u8* buf,int len)
{



	write_command(dev, cmd); //send_command

	gc9108_read_lcd_data(dev,buf,len);//recv_buf_data



}

/*

    funciton: 

*/
void write_data(struct gc9108_dev *dev, u8 data)

{

    gpio_set_value(dev->dc_gpio, 1);

	//gpio_direction_output(gc9108dev.cs_gpio, 0);


    gc9108_write_onereg(dev,data);
	//gpio_direction_output(gc9108dev.cs_gpio, 1);

}

/*

    funciton: 

*/
void write_datas(struct gc9108_dev *dev, u8* data,int len)

{

    

	gpio_set_value(dev->dc_gpio, 1);
	//gpio_direction_output(gc9108dev.cs_gpio, 0);

    gc9108_write_regs(dev,data,len);
	//gpio_direction_output(gc9108dev.cs_gpio, 1);

}


inline void gc9108_gram_draw_point(int x, int y,uint16_t color)
{
	uint16_t c = 0;	  //
	c = color << 8;
	c |= (color>>8 & 0x00ff);
#if 1
	gc9108_gram[y * 128 + x] = c; 
#endif
 
}
 
void gc9108_full_color(uint16_t color)
{
  unsigned int x,y;	
 
  for(y = 0;y < 128; y++){           
	for(x = 0;x < 128 ; x++){		
		gc9108_gram_draw_point(x,y,color);
    }

}
}
void gc9108_full_color_test(void)
{
	  unsigned int x,y;	
	  uint16_t c = 0;	
	  for(y = 0;y < 128; y++){           
		for(x = 0;x < 128 ; x++){
			c = (lcd_screen_rgb565[y * 128 *2  + x*2] << 8) | (lcd_screen_rgb565[y * 128 *2 + x*2+1]);
			gc9108_gram[y * 128 + x] = c; 
	    }
	 }
}

//send_n_rows
int gc9108_send_lines(uint16_t* color, int n)
{
	int ret = 0;
    //int base =0;
	struct spi_message m;
	struct spi_transfer *t;
	struct spi_device *spi = (struct spi_device *)gc9108dev.private_data;
     
	t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);
	if(t == NULL){
		 printk("gc9108_send_lines malloc fails!\n");
		return -ENOMEM;
	}

 	printk("#######   %s buf_addr =0x%p #####\r\n",__func__,color);
 	//base = (int)color | 0xc0000000;
 	//printk("#######   %s buf_addr =0x%08x #####\r\n",__func__,base);

	//gpio_set_value(gc9108dev.cs_gpio, 0); 


	t->tx_buf = color ;       //
	//t->tx_buf = (void *)base;       //
	//printk("t->tx_buf =%p!\n",t->tx_buf);
	t->len = n * 2* 128; 

	//t->delay_usecs = 0;
	t->speed_hz = 8000000;
	//t->bits_per_word = 8;



	
	spi_message_init(&m);        //
	spi_message_add_tail(t,&m);   //
	ret = spi_sync(spi,&m);       //
	kzfree(t); 

	return ret;
	//gpio_set_value(gc9108dev.cs_gpio, 1);

}
int gc9108_refresh(struct gc9108_dev *dev)
{ 
  //int n = 120, x, y;
#if 0
  write_command(dev,0x2A); //Set Column Address y
  write_data(dev,0x00); 
  write_data(dev,0x00+2); 
  write_data(dev,0x00) ; 
  write_data(dev,0x7F+2); 
  write_command(dev,0x2B); //Set Page Address x
  write_data(dev,0x00);
  write_data(dev,0x00+1); 
  write_data(dev,0x00); 
  write_data(dev,0x7f+1);
  write_command(dev,0x2c); 
#endif
 // printk("####### driver  %s  #####\r\n",__func__);

	//printk("####### phy_addr =0x%08x  ,size =%d#####\r\n",gc9108dev.gc9108_lcd_buffer.phys_addr,gc9108dev.gc9108_lcd_buffer.size);

	gpio_set_value(gc9108dev.dc_gpio, 1);
#if DRIVER_TEST
	gc9108_send_lines((uint16_t*)(&gc9108_gram[0]), 128); 
#else
	///gc9108_send_lines((uint16_t*)(gc9108dev.gc9108_lcd_buffer.phys_addr), 128); 
	
	gc9108_send_lines((uint16_t*)(dev->gc9108_lcd_buffer.virt_addr), 128); 

   return  0;

#endif
}
void Address_set(struct gc9108_dev *dev,unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)

{ 
	x1 +=1;
	x2 +=1;
	y1 +=2;
	y2 +=2;


    write_command(dev,0x2a);
	
    write_data(dev,y1>>8);

    write_data(dev,y1);

    write_data(dev,y2>>8);

    write_data(dev,y2);



  

    write_command(dev,0x2b);

    write_data(dev,x1>>8);

    write_data(dev,x1);

    write_data(dev,x2>>8);

    write_data(dev,x2);



    write_command(dev,0x2C);					 						 

}
//display
void LCD_DisplayOn(void)
{
		struct gc9108_dev *dev = &gc9108dev;
		write_command(dev,0x2c);
  	
}
 

/*



*/
void LCD_Clear(struct gc9108_dev *dev,u16 Color)

{

	u16 i,j;

	//u16 data_buf = WHITE;

//	Address_set(dev,0,0,LCD_W-1,LCD_H -1);

    //write_command(dev,0x2C);
     //   write_command(dev,0x29);
	//LCD_DisplayOn();
   gpio_set_value(dev->dc_gpio, 1);

	//gpio_direction_output(gc9108dev.cs_gpio, 0);

    for(i=0;i<LCD_W -1;i++)

	 {

	  for (j=0;j<LCD_H -1;j++)
	   	{

        	//write_datas(dev,(u8 *)&data_buf,2);	   
			gc9108_write_onereg(dev,Color>>8);

          // write_data(dev,Color>>8);

            //write_data(dev,Color);
			gc9108_write_onereg(dev,Color);

	    }



	 }


	//gpio_direction_output(gc9108dev.cs_gpio, 1);

}

/*
 * @description		: gc9108_open
 * @param - inode 	: 
 * @param - filp 	: 
 * 					  
 * @return 			: 
 */
static int gc9108_open(struct inode *inode, struct file *filp)
{
	filp->private_data = &gc9108dev; /*  */

	printk("############gc9108_open#################!\n");

	return 0;
}


static int gc9108_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *off)
{
	struct gc9108_dev *dev = (struct gc9108_dev *)filp->private_data;

#if 0
	//long err = 0;

	//err = copy_to_user(buf, data, sizeof(data));

	//err = copy_from_user(write_data, buf, cnt);
	//if(err == 0){
	//	printk("gc9108_write,len =%d\r\n", cnt);
	//}else{
	//	printk("gc9108_write failed!\r\n"); 
	//}
#endif
	//Address_set(dev,0,0,LCD_W-1,LCD_H -1);
#if DRIVER_TEST
		write_datas(dev,(u8 *)&gc9108_gram[0], RGB565_BYTE*LCD_H*LCD_W);
#else
		//write_datas(dev,(u8*)(gc9108dev.gc9108_lcd_buffer.phys_addr), RGB565_BYTE*LCD_H*LCD_W);
		write_datas(dev,(u8*)(gc9108dev.gc9108_lcd_buffer.virt_addr), RGB565_BYTE*LCD_H*LCD_W);		
#endif

	//gc9108_refresh(dev);



	return 0;
}


/*
 * @description		: gc9108_read 
 * @param - filp 	:
 * @param - buf 	: 
 * @param - cnt 	: 
 * @param - offt 	: 
 * @return 			: 
 */
static int gc9108_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{

#if 0
	//long err = 0;
	struct gc9108_dev *dev = (struct gc9108_dev *)filp->private_data;

	//err = copy_to_user(buf, data, sizeof(data));
#endif


	return 0;
}



static long gc9108_ioctl(struct file *filp, unsigned int cmd,unsigned long arg)
{
	int err = 0;
	//unsigned long flags;

	struct gc9108_dev *dev = (struct gc9108_dev *)filp->private_data;
	lcd_buffer_t lcd_buffer;
		/*
		 * extract the type and number bitfields, and don't encode
		 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
		 */
#if 1 
		if (_IOC_TYPE(cmd) != GC9108_IOC_MAGIC)
			return -ENOTTY;
		if (_IOC_NR(cmd) > GC9108_IOC_MAXNR)
			return -ENOTTY;
	
		/*
		 * the direction is a bitmask, and VERIFY_WRITE catches R/W
		 * transfers. `Type' is user-oriented, while
		 * access_ok is kernel-oriented, so the concept of "read" and
		 * "write" is reversed
		 */
#if (KERNEL_VERSION(5, 0, 0) > LINUX_VERSION_CODE)
		if (_IOC_DIR(cmd) & _IOC_READ)
			err = !access_ok(VERIFY_WRITE, (void __user *)arg,
							 _IOC_SIZE(cmd));
		else if (_IOC_DIR(cmd) & _IOC_WRITE)
			err = !access_ok(VERIFY_READ, (void __user *)arg,
							 _IOC_SIZE(cmd));
#else
		if (_IOC_DIR(cmd) & _IOC_READ)
			err = !access_ok((void __user *)arg,
							 _IOC_SIZE(cmd));
		else if (_IOC_DIR(cmd) & _IOC_WRITE)
			err = !access_ok((void __user *)arg,
							 _IOC_SIZE(cmd));
#endif
		if (err)
			return -EFAULT;
#endif
	switch (cmd) {
		case GC9108_IOC_BUFFERBASE:
		{
			spin_lock(&gc9108_lock);
			__put_user(dev->buffer_virbase,(unsigned long __user *)arg);
			spin_unlock(&gc9108_lock);
			break;
		}
		case GC9108_IOC_BUFFERSIZE:
		{
			spin_lock(&gc9108_lock);
			__put_user(dev->buffer_size, (unsigned int __user *)arg);
			spin_unlock(&gc9108_lock);
			break;
		}
		case GC9108_IOC_ALLOCATE_PHYSICAL_MEMORY:
		{
			spin_lock(&gc9108_lock);


			err = copy_from_user(&lcd_buffer, (lcd_buffer_t *)arg, sizeof(lcd_buffer_t));
			if (err) {
					return -EFAULT;
			}
			//printk("gc9108_ioctl phy_addr = 0x%08x,size = %d\r\n", lcd_buffer.phys_addr,lcd_buffer.size);

			//printk("#######   %s  #####\n",__func__);
			
			dev->gc9108_lcd_buffer.size = lcd_buffer.size;
			dev->gc9108_lcd_buffer.phys_addr = lcd_buffer.phys_addr;
			dev->gc9108_lcd_buffer.virt_addr = (unsigned long)phys_to_virt(dev->gc9108_lcd_buffer.phys_addr);
			//printk("gc9108_ioctl virt_addr = 0x%lx\r\n", dev->gc9108_lcd_buffer.virt_addr);

			spin_unlock(&gc9108_lock);
			break;	
		}
		case GC9108_IOC_REFRESH:
		{
		    int ret = 0;
			ret = gc9108_refresh(dev);
			return ret;
		}		
		default:
		{
			printk(KERN_ERR "[VPUDRV] No such IOCTL, cmd is %d\n", cmd);
		}
		break;
	}

	return 0;
}
int gc9108_mmap (struct file *filp, struct vm_area_struct *vma)
{
	unsigned int gc9108_phy_addr = 0;
	struct gc9108_dev *dev = (struct gc9108_dev *)filp->private_data;
	gc9108_phy_addr = virt_to_phys(gc9108_gram);
	dev->buffer_physaddr = gc9108_phy_addr;
	 printk("gc9108_phy =%p\r\n",(void *)gc9108_phy_addr);
	 printk("dev->buffer_physbase =0x%08x\r\n",dev->buffer_physaddr);	 
	 #if 1
     if(remap_pfn_range(vma,vma->vm_start,gc9108_phy_addr >> PAGE_SHIFT,vma->vm_end-vma->vm_start,vma->vm_page_prot)){
	 	printk("remap_pfn_range mmap failed \r\n");
        return -ENOBUFS;

	 }
	 return 0;
	 #else
	//remap_pfn_range(vma,vma->vm_start,virt_to_phys(gc9108_gram)>>12,vma->vm_end-vma->vm_start,vma->vm_page_prot);
	return remap_pfn_range(vma,vma->vm_start,gc9108_phy_addr >> PAGE_SHIFT,vma->vm_end-vma->vm_start,vma->vm_page_prot);
	 #endif
	
}

/*
 * @description		: gc9108_release
 * @param - filp 	:
 * @return 			:
 */
static int gc9108_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/* gc9108操作函数 */
static const struct file_operations gc9108_ops = {
	.owner = THIS_MODULE,
	.open = gc9108_open,
	.read = gc9108_read,
	.write = gc9108_write,
	.unlocked_ioctl = gc9108_ioctl,
	.mmap = gc9108_mmap,
	.release = gc9108_release,
};







/*
 * gc9108_reginit 
 * @param  	: 
 * @return 	: 
 */
void gc9108_reginit(struct gc9108_dev *dev)
{

		int i = 0;
		int j = 0;
		// u8 data_buf[3] = {0};	
	
		//u16 color_test = 0x00f8;


		 gpio_set_value(dev->res_gpio, 1);
		
		 mdelay(50);
		
		 gpio_set_value(dev->res_gpio, 0);
		
		 mdelay(50);
		 gpio_set_value(dev->res_gpio, 1);
		
		 mdelay(120);
#if 1	
	for (i =0; init_cmds[i].databytes != 0xff; i++){

		//gpio_direction_output(gc9108dev.cs_gpio, 0);
		//gpio_set_value(dev->dc_gpio, 0); 

        write_command(dev, init_cmds[i].cmd);
	    //udelay(100);
        if (init_cmds[i].databytes & 0x1F){
            // write_datas(dev,init_cmds[i].data, init_cmds[i].databytes & 0x1F);
             for(j=0;j < init_cmds[i].databytes;j++)
			    write_data(dev,init_cmds[i].data[j]);
		}


		if (init_cmds[i].delay_ms) {//
			mdelay(init_cmds[i].delay_ms);
		}
		//gpio_set_value(dev->dc_gpio, 1); 
		//gpio_direction_output(gc9108dev.cs_gpio, 1);
    }

#endif
#if 0
	write_command(dev,0x2A); //Set Column Address y
	write_data(dev,0x00); 
	write_data(dev,0x00+2); 
	write_data(dev,0x00) ; 
	write_data(dev,0x7F+2); 
	write_command(dev,0x2B); //Set Page Address x
	write_data(dev,0x00);
	write_data(dev,0x00+1); 
	write_data(dev,0x00); 
	write_data(dev,0x7f+1);

	write_command(dev,0x2c); 
#endif



	//probe函数中分配buffer内存	
    gc9108_gram = kmalloc(RGB565_BYTE * LCD_W*LCD_H, GFP_KERNEL); 
   memset(&gc9108_gram[0],0x00,RGB565_BYTE * LCD_W*LCD_H);
   dev->buffer_virbase = (unsigned long)&gc9108_gram[0];
   dev->buffer_size = RGB565_BYTE * LCD_W * LCD_H;
   printk("gc9108_gram  = %p!\n",gc9108_gram);




   Address_set(dev,0,0,LCD_W-1,LCD_H -1);
   write_datas(dev,(u8 *)gc9108_gram, 128*128*2);  

   j = 0;

#if DRIVER_TEST

while(j < 1 ){

		printk("full_color begin = %d!\n",j);
		//gc9108_full_color(color_buf[j % 14]);
		//gc9108_full_color(color_buf[2]);

		//printk("color   =0x%x!\n",color_buf[j]);
		//gc9108_full_color_test();
        memcpy(&gc9108_gram[0],&lcd_screen_rgb565,128*128*2);
		printk("value =0x%x,0x%x 0x%x!\n",gc9108_gram[0],gc9108_gram[1],gc9108_gram[128*127+1]);
		write_datas(dev,(u8 *)gc9108_gram, 128*128*2);	 

		//gc9108_refresh(dev);
		printk("full_color end = %d\n",j);
		mdelay(1000);
		j++;

}
#endif
	while(0){
			printk("111 begin = %d!\n",j);
		LCD_Clear(dev,YELLOW);
		printk("222 begin = %d!\n",j);
		LCD_Clear(dev,BLACK);
			printk("333 begin = %d!\n",j);

		LCD_Clear(dev,RED);
			printk("444 begin = %d!\n",j);

		LCD_Clear(dev,WHITE);
			printk("555 begin = %d!\n",j);

		LCD_Clear(dev,CYAN);
			printk("666 begin = %d!\n",j);
		j++;
		if(j >=10){

        break;
		}
	}

	printk("spi_lcd init finish!\n");

}

 /*
  * @description     : gc9108_probe
  *                    
  * @param - client  : 
  * @param - id      : 
  * 
  */	
static int gc9108_probe(struct spi_device *spi)
{
	int ret = 0;
    memset(&gc9108dev,0x0,sizeof(gc9108dev));

	/*  */
#if defined(TS_LCD_MAJOR)
	gc9108dev.major = TS_LCD_MAJOR;
#endif
	if (gc9108dev.major) {
		gc9108dev.devid = MKDEV(gc9108dev.major, 0);
		register_chrdev_region(gc9108dev.devid, gc9108_CNT, gc9108_NAME);
	} else {
		alloc_chrdev_region(&gc9108dev.devid, 0, gc9108_CNT, gc9108_NAME);
		gc9108dev.major = MAJOR(gc9108dev.devid);
	}

	/*  */
	cdev_init(&gc9108dev.cdev, &gc9108_ops);
	cdev_add(&gc9108dev.cdev, gc9108dev.devid, gc9108_CNT);

	/*  */
	gc9108dev.class = class_create(THIS_MODULE, gc9108_NAME);
	if (IS_ERR(gc9108dev.class)) {
		return PTR_ERR(gc9108dev.class);
	}

	/*  */
	gc9108dev.device = device_create(gc9108dev.class, NULL, gc9108dev.devid, NULL, gc9108_NAME);
	if (IS_ERR(gc9108dev.device)) {
		return PTR_ERR(gc9108dev.device);
	}


	gc9108dev.nd = of_find_node_by_path("/soc/spi@f244d000/gc9108@0");

	if(gc9108dev.nd == NULL) {

		printk("gc9108dev node not find!\r\n");

		return -EINVAL;

	}
#if 1
	/* bk */

	gc9108dev.bk_gpio = of_get_named_gpio(gc9108dev.nd, "bk-gpios", 0);

	if(gc9108dev.bk_gpio <= 0) {

		printk("can't get bk-gpio");

		return -EINVAL;

	}

		ret = gpio_direction_output(gc9108dev.bk_gpio, 1);
	
		if(ret < 0) {
	
			printk("can't set bk gpio!\r\n");
	
		}
#endif

#if 1
    gc9108dev.res_gpio = of_get_named_gpio(gc9108dev.nd, "rst-gpios", 0);

    if(gc9108dev.res_gpio <= 0) {

		printk("can't get rst-gpio");

		return -EINVAL;

	}

    ret = gpio_direction_output(gc9108dev.res_gpio, 0);

	if(ret < 0) {

		printk("can't set res gpio!\r\n");

	}
#endif

#if 1

    gc9108dev.dc_gpio = of_get_named_gpio(gc9108dev.nd, "rs-gpios", 0);

    if(gc9108dev.dc_gpio <= 0) {

		printk("can't get Dc-gpio");

		return -EINVAL;

	}

	//printk("gc9108dev.cs_gpio = %d,gc9108dev.dc_gpio =%d,gc9108dev.res_gpio =%d\r\n",gc9108dev.cs_gpio,gc9108dev.dc_gpio,gc9108dev.res_gpio);




    ret = gpio_direction_output(gc9108dev.dc_gpio, 1);

	if(ret < 0) {

		printk("can't set dc gpio!\r\n");

	}
#endif
	/* */
	spi->mode = SPI_MODE_0;	/*MODE0，CPOL=0，CPHA=0*/
	//spi->delay_usecs = 0;


	//spi->bits_per_word = 8;
	printk("spi_setup!\r\n");
	spi_setup(spi);
	printk("spi_setup end!\r\n");
	gc9108dev.private_data = spi; /*  */

	/*  */
	gc9108_reginit(&gc9108dev);		
	return 0;
}

/*
 * @description     : gc9108_remove
 * @param - client 	: 
 * @return          : 
 */
static int gc9108_remove(struct spi_device *spi)
{

    gpio_set_value(gc9108dev.res_gpio, 0);
   // gpio_set_value(gc9108dev.cs_gpio, 1);
	/*  */
	if(gc9108_gram){
	
	 kfree(gc9108_gram);
	}

	cdev_del(&gc9108dev.cdev);
	unregister_chrdev_region(gc9108dev.devid, gc9108_CNT);

	/*  */
	device_destroy(gc9108dev.class, gc9108dev.devid);
	class_destroy(gc9108dev.class);

	
	return 0;
}

/*  */
static const struct spi_device_id gc9108_id[] = {
	{"gc9108,spi-lcd", 0},  
	{}
};

/*  */
static const struct of_device_id gc9108_of_match[] = {
	{ .compatible = "gc9108,spi-lcd" },
	{ /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, gc9108_of_match);
/*  */	
static struct spi_driver gc9108_driver = {
	.probe = gc9108_probe,
	.remove = gc9108_remove,
	.driver = {
			.owner = THIS_MODULE,
		   	.name = "gc9108",
		   	.of_match_table = gc9108_of_match, 
		   },
	.id_table = gc9108_id,
};
		   
/*
 * @description	: gc9108_init
 * @param 		: 
 * @return 		: 
 */
static int __init gc9108_init(void)
{
	return spi_register_driver(&gc9108_driver);
}

/*
 * @description	: gc9108_exit
 * @param 		: 
 * @return 		: 
 */
static void __exit gc9108_exit(void)
{
	spi_unregister_driver(&gc9108_driver);
}

module_init(gc9108_init);
module_exit(gc9108_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("TSM");









