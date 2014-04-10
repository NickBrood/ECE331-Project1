#include <stdio.h>
#include <bcm2835.h>

//map lcd pin numbers to corresponding RPi gpio ports
#define LCD4 25
#define LCD5 4
#define LCD6 24
#define LCD11 23
#define LCD12 17
#define LCD13 27
#define LCD14 22

//map lcd pin names to lcd pin numbers
#define RS      LCD4
#define RW      LCD5
#define E       LCD6
#define DB4     LCD11
#define DB5     LCD12
#define DB6     LCD13
#define DB7     LCD14

void nibble();

int main(int argc, char **argv)
{
//guarantee bcm2835 functionality
    if (!bcm2835_init())
return -1;

    // Set the appropriate GPIO pins to be outputs
    bcm2835_gpio_fsel(E, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RS, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RW, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(DB7, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(DB6, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(DB5, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(DB4, BCM2835_GPIO_FSEL_OUTP);

    //wait at least 15ms after Vcc = 4.5V
    bcm2835_delay(20);

    //Function Set: set interface to four bits
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, LOW);
    bcm2835_gpio_write(DB5, HIGH);
    bcm2835_gpio_write(DB4, LOW);
    nibble();
    bcm2835_delay(1);

    //function set
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, LOW);
    bcm2835_gpio_write(DB5, HIGH);
    bcm2835_gpio_write(DB4, LOW);
    nibble();
    bcm2835_delay(1);
    bcm2835_gpio_write(DB7, HIGH);
    bcm2835_gpio_write(DB6, LOW);
    bcm2835_gpio_write(DB5, LOW);
    bcm2835_gpio_write(DB4, LOW);
    nibble();
    bcm2835_delay(1);

 //display on
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, LOW);
    bcm2835_gpio_write(DB5, LOW);
    bcm2835_gpio_write(DB4, LOW);
    nibble();
    bcm2835_delay(1);
    bcm2835_gpio_write(DB7, HIGH);
    bcm2835_gpio_write(DB6, HIGH);
    bcm2835_gpio_write(DB5, HIGH);
    bcm2835_gpio_write(DB4, LOW);
    nibble();
    bcm2835_delay(1);



    //Entry Mode Set
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, LOW);
    bcm2835_gpio_write(DB5, LOW);
    bcm2835_gpio_write(DB4, LOW);
    nibble();
    bcm2835_delay(1);
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, HIGH);
    bcm2835_gpio_write(DB5, HIGH);
    bcm2835_gpio_write(DB4, LOW); //was high
    nibble();
    bcm2835_delay(1);


// write E
    bcm2835_gpio_write(RS, HIGH);
    bcm2835_gpio_write(RW, HIGH);
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, HIGH);
    bcm2835_gpio_write(DB5, LOW);
    bcm2835_gpio_write(DB4, LOW);
    nibble();
    bcm2835_delay(1);
    bcm2835_gpio_write(RS, HIGH);
    bcm2835_gpio_write(RW, LOW);
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, HIGH);
    bcm2835_gpio_write(DB5, LOW);
    bcm2835_gpio_write(DB4, HIGH);
    nibble();
    bcm2835_delay(1);
// write c
    bcm2835_gpio_write(RS, HIGH);
    bcm2835_gpio_write(RW, HIGH);
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, HIGH);
    bcm2835_gpio_write(DB5, LOW);
    bcm2835_gpio_write(DB4, LOW);
    nibble();
    bcm2835_delay(1);
    bcm2835_gpio_write(RS, HIGH);
    bcm2835_gpio_write(RW, LOW);
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, LOW);
    bcm2835_gpio_write(DB5, HIGH);
    bcm2835_gpio_write(DB4, HIGH);
    nibble();
    bcm2835_delay(1);

// write e
    bcm2835_gpio_write(RS, HIGH);
    bcm2835_gpio_write(RW, HIGH);
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, HIGH);
    bcm2835_gpio_write(DB5, LOW);
    bcm2835_gpio_write(DB4, LOW);
    nibble();
    bcm2835_delay(1);
    bcm2835_gpio_write(RS, HIGH);
    bcm2835_gpio_write(RW, LOW);
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, HIGH);
    bcm2835_gpio_write(DB5, LOW);
    bcm2835_gpio_write(DB4, HIGH);
    nibble();
    bcm2835_delay(1);
/*
// write a blank
  bcm2835_gpio_write(RS, HIGH);
    bcm2835_gpio_write(RW, HIGH);
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, HIGH);
    bcm2835_gpio_write(DB5, LOW);
    bcm2835_gpio_write(DB4, LOW);
    nibble();
    bcm2835_delay(1);
    bcm2835_gpio_write(RS, LOW);
    bcm2835_gpio_write(RW, HIGH);
    bcm2835_gpio_write(DB7, HIGH);
    bcm2835_gpio_write(DB6, LOW);
    bcm2835_gpio_write(DB5, LOW);
    bcm2835_gpio_write(DB4, HIGH);
    nibble();
    bcm2835_delay(1);
*/
// write 3
    bcm2835_gpio_write(RS, HIGH);
    bcm2835_gpio_write(RW, HIGH);
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, LOW);
    bcm2835_gpio_write(DB5, HIGH);
    bcm2835_gpio_write(DB4, HIGH);
    nibble();
    bcm2835_delay(1);
    bcm2835_gpio_write(RS, HIGH);
    bcm2835_gpio_write(RW, LOW);
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, LOW);
    bcm2835_gpio_write(DB5, HIGH);
    bcm2835_gpio_write(DB4, HIGH);
    nibble();
    bcm2835_delay(1);

// write 3
    bcm2835_gpio_write(RS, HIGH);
    bcm2835_gpio_write(RW, HIGH);
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, LOW);
    bcm2835_gpio_write(DB5, HIGH);
    bcm2835_gpio_write(DB4, HIGH);
    nibble();
    bcm2835_delay(1);
    bcm2835_gpio_write(RS, HIGH);
    bcm2835_gpio_write(RW, LOW);
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, LOW);
    bcm2835_gpio_write(DB5, HIGH);
    bcm2835_gpio_write(DB4, HIGH);
    nibble();
    bcm2835_delay(1);

// write 1
    bcm2835_gpio_write(RS, HIGH);
    bcm2835_gpio_write(RW, HIGH);
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, LOW);
    bcm2835_gpio_write(DB5, HIGH);
    bcm2835_gpio_write(DB4, HIGH);
    nibble();
    bcm2835_delay(1);
    bcm2835_gpio_write(RS, HIGH);
    bcm2835_gpio_write(RW, LOW);
    bcm2835_gpio_write(DB7, LOW);
    bcm2835_gpio_write(DB6, LOW);
    bcm2835_gpio_write(DB5, LOW);
    bcm2835_gpio_write(DB4, HIGH);
    nibble();
    bcm2835_delay(1);


    bcm2835_close();
    return 0;
}

void nibble()
{
bcm2835_gpio_write(E, HIGH);
bcm2835_delay(5);
bcm2835_gpio_write(E, LOW);
}
