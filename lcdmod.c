#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/ioctl.h>
#include <asm/uaccess.h>
#include <linux/io.h>
#include <mach/platform.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <asm/gpio.h>
#include <asm/io.h>
//#include "gpiolcd.h"


#define SYSTIMER_MOD_AUTH "Sheaff"
#define SYSTIMER_MOD_DESCR "GPIO LCD Driver"
#define SYSTIMER_MOD_SDEV "GPIO LCD RPi"

static long lcd_ioctl(struct file * flip, unsigned int cmd, unsigned long arg);
static int lcd_open(struct inode *inode, struct file *filp);
static int lcd_release(struct inode *inode, struct file *filp);
static char *lcd_devnode(struct device *dev, umode_t *mode);
void lcd_init(uint32_t *base_address);
void clock_enable(uint32_t *base_address);


//#define BCM2708_PERI_BASE 0x20000000
//#define GPIO_BASE (BCM2708_PERI_BASE + 0x200000) /*GPIO controller*/
#define GPCLR0 0x28
#define GPSET0 0x1C
#define RS 25
#define RW 4
#define E 24
#define DB4 23
#define DB5 17
#define DB6 27
#define DB7 22

static const struct file_operations lcd_fops = {
	.owner=THIS_MODULE,
	.open=lcd_open,
	.release=lcd_release,
	.unlocked_ioctl=lcd_ioctl,
};

struct lcd_data {
	int lcd_mjr;
	struct class *lcd_class;
};

static struct lcd_data lcd = {
	.lcd_mjr=0,
	.lcd_class=NULL,
	
};

/*
static struct gpio pins[] = {
	{ 4, GPIOF_OUT_INIT_HIGH, "LED 1" },
	{ 23, GPIOF_OUT_INIT_HIGH, "LED 2" }, 
	{ 17, GPIOF_OUT_INIT_HIGH, "LED 3" },  
};
*/

static long lcd_ioctl(struct file * flip, unsigned int cmd, unsigned long arg)
{
	return -EINVAL;
}

static int lcd_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int lcd_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static char *lcd_devnode(struct device *dev, umode_t *mode)
{
	if (mode) *mode = 0666;
	return NULL;
}

// Module init
static int __init rpigpio_lcd_minit(void)
{
	// So we're going to need to register a platform device
	// struct platform_device
	//bcm_register_device(&bcm2708_gpio_lcd_device);
	// request GPIO pins connected to LCD
	// Initialize the LCD
	// If we use busy flag then we need to have a timeout
	// Cannot hold processor indef.
	// Create a character device that is read and writable
	// Read will read the LCD data - not sure where it starts
	// Write will add characters to screen
	// ioctl?
	// open/close
	struct device *dev=NULL;
	int ret=0;
	uint32_t * base_address;
	
	printk(KERN_INFO "%s\n",SYSTIMER_MOD_DESCR);
    	printk(KERN_INFO "By: %s\n",SYSTIMER_MOD_AUTH);

	lcd.lcd_mjr=register_chrdev(0,"gpio_lcd",&lcd_fops);
	if (lcd.lcd_mjr<0) {
		printk(KERN_NOTICE "Cannot register char device\n");
		return lcd.lcd_mjr;
	}
	lcd.lcd_class=class_create(THIS_MODULE, "lcd_class");
	if (IS_ERR(lcd.lcd_class)) {
		unregister_chrdev(lcd.lcd_mjr,"lcd_gpio");
		return PTR_ERR(lcd.lcd_class);
	}
	lcd.lcd_class->devnode=lcd_devnode;
	dev=device_create(lcd.lcd_class,NULL,MKDEV(lcd.lcd_mjr,0),(void *)&lcd,"lcd");
	if (IS_ERR(dev)) {
		class_destroy(lcd.lcd_class);
		unregister_chrdev(lcd.lcd_mjr,"lcd_gpio");
		return PTR_ERR(dev);
	}
	
	/*

	//Use ioremap to make I/O memory accessible to the kernel
	void *ioremap(unsigned long phys_addr, unsigned long size);
	
	//addr should be an address obtained from ioremap...//
	
	//Read from I/O memory
	unsigned int ioread32(void *addr);
	//Write to I/O memory
	void iowrite32(u32 value, void *addr);
	//Later need to iounmap when done
        void iounmap(void * addr);

	//Repeating versions of these functions, if you want to write a series of values
	void ioread32_rep(void *addr, void *buf, unsigned long count);
B
	void iowrite32_rep(void *addr, const void *buf, unsigned long count);
	*/

	//So device driver can access any I/O memory address
	base_address = (uint32_t *)ioremap(GPIO_BASE, 4096);
	printk("Ioremap returned: %d\n", *base_address);
	lcd_init(base_address);
	
	return ret;
}

// Module removal
static void __exit rpigpio_lcd_mcleanup(void)
{
	device_destroy(lcd.lcd_class,MKDEV(lcd.lcd_mjr,0));
	class_destroy(lcd.lcd_class);
	unregister_chrdev(lcd.lcd_mjr,"lcd_gpio");
	printk(KERN_INFO "Goodbye\n");
	return;
}

//Initialize LCD Display
void lcd_init(uint32_t *base_address){
	printk("Ioremap(init): %d\n", *(base_address));
	//Initialize necessary GPIP pins to outputs
	iowrite32((ioread32(base_address + 0x8) | 1<<12), base_address + 0x8);//Pin 24
        iowrite32((ioread32(base_address + 0x8) | 1<<15), base_address + 0x8);//Pin 25
        iowrite32((ioread32(base_address + 0x0) | 1<<12), base_address + 0x0);//Pin 4
        iowrite32((ioread32(base_address + 0x8) | 1<<6), base_address + 0x8);//Pin 22
        iowrite32((ioread32(base_address + 0x8) | 1<<21), base_address + 0x8);//Pin 27
        iowrite32((ioread32(base_address + 0x4) | 1<<21), base_address + 0x4);//Pin 17
        iowrite32((ioread32(base_address + 0x8) | 1<<9), base_address + 0x8);//Pin 23

        //Wait at least 15ms after Vcc = 4.5V
        mdelay(20);

        //Function set: set interface to four bits
        iowrite32(0<<DB7, base_address + GPCLR0);
        iowrite32(0<<DB6, base_address + GPCLR0);
        iowrite32(1<<DB5, base_address + GPSET0);
        iowrite32(0<<DB4, base_address + GPCLR0);
        clock_enable(base_address);
        mdelay(1);

        //Function set
        iowrite32(0<<DB7, base_address + GPCLR0);
        iowrite32(0<<DB6, base_address + GPCLR0);
        iowrite32(1<<DB5, base_address + GPSET0);
        iowrite32(0<<DB4, base_address + GPCLR0);
        clock_enable(base_address);
        mdelay(1);

        iowrite32(1<<DB7, base_address + GPSET0);
        iowrite32(0<<DB6, base_address + GPCLR0);
        iowrite32(0<<DB5, base_address + GPCLR0);
        iowrite32(0<<DB4, base_address + GPCLR0);
        clock_enable(base_address);
        mdelay(1);

        //Display on
        iowrite32(0<<DB7, base_address + GPCLR0);
        iowrite32(0<<DB6, base_address + GPCLR0);
        iowrite32(0<<DB5, base_address + GPCLR0);
        iowrite32(0<<DB4, base_address + GPCLR0);
        clock_enable(base_address);
        mdelay(1);

        iowrite32(1<<DB7, base_address + GPSET0);
        iowrite32(1<<DB6, base_address + GPSET0);
        iowrite32(1<<DB5, base_address + GPSET0);
        iowrite32(0<<DB4, base_address + GPCLR0);
        clock_enable(base_address);
        mdelay(1);

        //Entry Mode Set
        iowrite32(0<<DB7, base_address + GPCLR0);
        iowrite32(0<<DB6, base_address + GPCLR0);
        iowrite32(0<<DB5, base_address + GPCLR0);
        iowrite32(0<<DB4, base_address + GPCLR0);
        clock_enable(base_address);
        mdelay(1);

        iowrite32(0<<DB7, base_address + GPCLR0);
        iowrite32(1<<DB6, base_address + GPSET0);
        iowrite32(1<<DB5, base_address + GPSET0);
        iowrite32(0<<DB4, base_address + GPCLR0);
        clock_enable(base_address);
        mdelay(1);
}

//Pull Enable line high then loq quickly to enable clock
void clock_enable(uint32_t *base_address){
	printk("Ioremap(clock): %d\n", *(base_address));
	iowrite32(1<<E, base_address + 0);
	mdelay(5);
	iowrite32(1<<E, base_address + 0x1C);
}


module_init(rpigpio_lcd_minit);
module_exit(rpigpio_lcd_mcleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(SYSTIMER_MOD_AUTH);
MODULE_DESCRIPTION(SYSTIMER_MOD_DESCR);
MODULE_SUPPORTED_DEVICE(SYSTIMER_MOD_SDEV);
