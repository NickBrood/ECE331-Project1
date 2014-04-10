/*
   
   Description: LCD Kernel driver
   Author: Nicholas P. Grant
   Class: ECE 331

   Sources Cited:
   http://lxr.free-electrons.com/source/include/linux/fs.h
   https://github.com/wendlers/rpi-kmod-samples/blob/master/modules/kmod-gpio_output/gpiomod_output.c
   http://courses.eece.maine.edu/ece331/lcd-mod.c
   http://tuxthink.blogspot.com/2013/06/using-spinlock-in-linux-example.html
   http://sourceforge.net/p/linux-adm5120/code/HEAD/tree/OpenWrt/hd44780/src/hd44780.c#l155

 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/gpio.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/ioctl.h>
#include <linux/io.h>
#include <mach/platform.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <linux/kthread.h>
//#include "gpiolcd.h"

#define SYSTIMER_MOD_AUTH "Nicholas Grant"
#define SYSTIMER_MOD_DESCR "GPIO LCD Driver"
#define SYSTIMER_MOD_SDEV "GPIO LCD RPi"

// Minor device number (major device is 'misc').
#define LCD_DEVICE_MINOR 153

// Pin assignments for the board.  All necessary GPIO numbers listed here

//  Pin ID     Linux GPIO number      
#define RS      25		//
#define RW      4		//
#define E       24		//
#define DB4     23		//
#define DB5     17		//
#define DB6     27		//
#define DB7     22

#define PINS_NEEDED (sizeof(pins)/sizeof(tPinSet))

// Pin Macros
#define PULL_RS_LOW  gpio_set_value(RS, 0);
#define PULL_RS_HIGH gpio_set_value(RS, 1);
#define PULL_E_LOW   gpio_set_value(E, 0);
#define PULL_E_HIGH  gpio_set_value(E, 1);

//Function prototypes
static int lcd_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int lcd_open (struct inode *inode, struct file *filp);
static int lcd_release (struct inode *inode, struct file *filp);
static char *lcd_devnode (struct device *dev, umode_t * mode);
static void write_nibble (unsigned int val);
static void write_byte (char c);
static void display (char c);
static ssize_t lcd_write (struct file *file, const char *buf, size_t count,loff_t * ppos);

//File Operations
static struct file_operations lcd_fops = {
  .owner = THIS_MODULE,
  .open = lcd_open,
  .release = lcd_release,
  .unlocked_ioctl = lcd_ioctl,
  .write = lcd_write,
};

//Making use of a miscellaneous character driver
static struct miscdevice lcd_device = {
  LCD_DEVICE_MINOR,
  "lcd_driver",
  &lcd_fops,
};


// Structures to hold the pins
typedef struct pinstruct
{
  int pin;			// Linux GPIO pin number
  char *name;			// Name of the pin, supplied to gpio_request()
  int result;			// set to zero on successfully obtaining pin
} tPinSet;


//All GPIO pins to be used
static tPinSet pins[] = {
  {E, "Pin E", -1},
  {RW, "Pin RW", -1},
  {RS, "Pin RS", -1},
  {DB4, "Pin DB4", -1},
  {DB5, "Pin DB5", -1},
  {DB6, "Pin DB6", -1},
  {DB7, "Pin DB7", -1},
};

/*
//Setting up Spinlock ahead of time
static spinlock_t my_lock = SPIN_LOCK_UNLOCKED;
*/

// ioctl - I/O control
static int lcd_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
  int data;

  // Read the request data
  if (copy_from_user (&data, (int *) arg, sizeof (data)))
    {
      printk (KERN_INFO "copy_from_user error on ioctl.\n");
      return -EFAULT;
    }

  switch (cmd)
    {

    default:
      printk (KERN_INFO "Invalid ioctl\n");
      return -EINVAL;
    }

  // return the result
  if (copy_to_user ((int *) arg, &data, 4))
    {
      printk (KERN_INFO "copy_to_user error on ioctl\n");
      return -EFAULT;
    }

  return 0;
}

static int lcd_open (struct inode *inode, struct file *filp)
{
  /*

     //If file is open for writing, make sure other files can't open
     if(filp->fmode == FMODE_WRITE){
     spin_lock(&my_lock);

     }
     //If file is open for reading, make sure it can only be opened for more reading
     else if(filp->fmode == FMODE_READ){
     spin_lock(&my_lock);
     }
     //If file isn't open, no lock needed
     else{

     }
   */

  return 0;
}

static int lcd_release (struct inode *inode, struct file *filp)
{
  //Once released, let other processes in
  //spin_unlock(&my_lock);
  return 0;
}

static char * lcd_devnode (struct device *dev, umode_t * mode)
{
  if (mode)
    *mode = 0666;
  return NULL;
}


/*
   Writes a nibble out to the associated GPIO registers
*/
static void writenibble (unsigned int val)
{
  gpio_set_value (DB4, (val & 0x1));
  gpio_set_value (DB5, (val & 0x2) >> 1);
  gpio_set_value (DB6, (val & 0x4) >> 2);
  gpio_set_value (DB7, (val & 0x8) >> 3);
  udelay (1);
  PULL_E_LOW;
  udelay (1);			// data setup time
  PULL_E_HIGH;
  udelay (1);			// data hold time
}

/*
   Makes use of two writenibble() calls to 
   write a full byte out to associated GPIO registers

   This speeds up the initialization process by cutting
   the calls in half when pins needs to be written to in bulk

*/
static void writebyte (char c)
{
  udelay (1);
  PULL_RS_LOW;
  udelay (1);
  writenibble ((c >> 4) & 0xf);
  writenibble (c & 0xf);
  udelay (50);
}


/*
   Used when actually writing out characters to the display
*/
static void display (char c)
{
  udelay (1);
  PULL_RS_HIGH;
  udelay (1);
  writenibble ((c >> 4) & 0xf);
  writenibble (c & 0xf);
  udelay (50);
}

/*
   Writes out to the device file 
   (message displayed to the screen)
*/
static ssize_t lcd_write (struct file *file, const char *buf, size_t count, loff_t * ppos)
{
  int i;
  int err;
  char c;
  const char *ptr = buf;

  for (i = 0; i < count; i++)
    {
      err = copy_from_user (&c, ptr++, 1);
      if (err != 0)
	return -EFAULT;
      // Write the byte to the display.
      display (c);
    }
  return count;
}


/*
   Release all GPIO pins when not needed anymore
*/
static void pin_release (void)
{
  int i;
  for (i = 0; i < PINS_NEEDED; i++)
    {
      if (pins[i].result == 0)
	{
	  gpio_free (pins[i].pin);
	  pins[i].result = -1;	// defensive programming - avoid multiple free.
	}
    }
}


static int __init gpio_lcd_init (void)
{
  int i;
  int pin_not_missed = 1;	//Will only become 0 if pin data missed
  int ret = 0;

  // Register misc device
  if (misc_register (&lcd_device))
    {
      printk (KERN_WARNING "Couldn't register device %d\n", LCD_DEVICE_MINOR);
      return -EBUSY;
    }

/*
  struct device *dev=NULL;
  //uint32_t * base_address;

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
*/


  // Request GPIO  pins
  for (i = 0; i < PINS_NEEDED; i++)
    {
      pins[i].result = gpio_request (pins[i].pin, pins[i].name);
      if (pins[i].result != 0)
	pin_not_missed = 0;
    }

  // If failure on any of pin requests, free the rest
  if (!pin_not_missed)
    {
      pin_release ();
      return -EBUSY;
    }

  //Set port direction
  for (i = 0; i < PINS_NEEDED; i++)
    {
      gpio_direction_output (pins[i].pin, 0);
    }

  // Initialize LCD Display
  writenibble (0x03);
  msleep (1);
  writenibble (0x03);
  msleep (1);
  writenibble (0x03);
  msleep (1);
  writenibble (0x02);
  msleep (1);

  writebyte (0x28);
  udelay (50);
  writebyte (0x0c);
  udelay (50);
  writebyte (0x01);
  udelay (50);
  writebyte (0x06);
  udelay (50);

  printk (KERN_INFO "kernel driver loaded, LCD initialized.\n");
  return ret;
}

static void __exit gpio_lcd_exit (void)
{
  pin_release ();
  misc_deregister (&lcd_device);
  printk (KERN_INFO "kernel driver totally unloaded.\n");
  /*
     device_destroy(lcd.lcd_class,MKDEV(lcd.lcd_mjr,0));
     class_destroy(lcd.lcd_class);
     unregister_chrdev(lcd.lcd_mjr, "lcd_gpio");
     printk(KERN_INFO "kernel driver unloaded.\n");
     return;
   */
}

module_init (gpio_lcd_init);
module_exit (gpio_lcd_exit);

//Basic module information
MODULE_LICENSE ("GPL");
MODULE_AUTHOR (SYSTIMER_MOD_AUTH);
MODULE_DESCRIPTION (SYSTIMER_MOD_DESCR);
MODULE_SUPPORTED_DEVICE (SYSTIMER_MOD_SDEV);
