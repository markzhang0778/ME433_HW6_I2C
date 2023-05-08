#include "nu32dip.h"
#include "i2c_master_noint.h"
#include <stdio.h>
#include <math.h>

//#define ADDRW 64
//#define ADDRR 65
//#define IODIR 0
//#define GPIO  9
//#define OLAT  10

#define ADDRW 0b01000000
#define ADDRR 0b01000001
#define IODIR 0x0
#define GPIO  0x9
#define OLAT  0xA

void i2c_write_reg(unsigned char addr, unsigned char reg_addr, 
        unsigned char val);

unsigned char i2c_read_GPIO(unsigned char pin);

int main(void) {
  
  NU32DIP_Startup();
  i2c_master_setup();
  //i2c_master_restart();
  unsigned char LED_stat = 1;
  //IODIR: GP7 output, GP0 input, GP1-6 do not matter
  i2c_write_reg(ADDRW, IODIR, 0b01);
  
  //24 MHz core timer
  _CP0_SET_COUNT(0);
  
  while (1) {
    //blink yellow for "heartbeat" to indicate code is running
    NU32DIP_YELLOW = ! PORTBbits.RB5;
    //READ GP0
    //pull down GP0 so that it is low while button is not pressed
    //and high when button is pressed
    unsigned char bp = i2c_read_GPIO(0);
    
    while(_CP0_GET_COUNT() < 480){
        ;
    }
    _CP0_SET_COUNT(0);
    i2c_write_reg(ADDRW, OLAT, bp << 7);
    
  }
}

//function writes to a register on the IO expander
void i2c_write_reg(unsigned char addr, unsigned char reg_addr, 
        unsigned char val){
    i2c_master_start();
    i2c_master_send(addr);
    i2c_master_send(reg_addr);
    i2c_master_send(val);
    i2c_master_stop();
}

//function returns the state of a pin using the GPIO register
unsigned char i2c_read_GPIO(unsigned char pin){
    //write GPIO address to the device
    i2c_master_start();
    i2c_master_send(ADDRW);
    i2c_master_send(GPIO);
    i2c_master_restart();
    
    //get the byte
    i2c_master_send(ADDRR);
    unsigned char output = i2c_master_recv();
    
    //acknowledge and stop
    i2c_master_ack(1);
    i2c_master_stop();
    return output;
}
