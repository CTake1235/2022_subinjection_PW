#include "mbed.h"
#include "sonMD.h"

#define wingPWadd 0x58

UnbufferedSerial raspi(PA_0,PA_1,9600);

I2C              i2c(PB_9,PB_8);

sonMD            inside(PA_11,PB_2,0.00015);
sonMD            outside(PA_10,PB_5,0.00015);
sonMD            inside_reload(PA_9,PA_8,0.00015);
sonMD            outside_reload(PB_6,PA_6,0.00015);    

DigitalIn        sitalimitswitch(PC_0);

void send(char add, char dat);

int main(){
    raspi.format(8,BufferedSerial::None,1);
    int res,time;
    static char data; 
    float shotpower = 0.6;
    float reloadpower = 0.7;
    bool state = 0;
    while (true) {
        res = raspi.read(&data,4);
        if(res == 1){
            switch (int(data)){
                case 7://L1,open wing
                    send(wingPWadd,0xff);
                    break;
                case 8://L2,close wing
                    send(wingPWadd,0x00);
                    break;
                case 9://maru,inject
                    state = 1;
                    break;
                case 10://sikaku,reload back
                    inside_reload.move_p1(0.02,reloadpower);
                    outside_reload.move_p1(0.02,reloadpower);
                    break;
                case 11://sankaku,reload go
                    inside_reload.move_p2(0.02,reloadpower);
                    outside_reload.move_p2(0.02,reloadpower);
                    break;
                case 12://batu,stop
                    state = 0;
                    break;
                default:
                    inside_reload.stop();
                    outside_reload.stop();
                    send(wingPWadd,0x80);
                    break;
            }
            if(state == 1){
                inside.move_p1(0.02, shotpower);
                outside.move_p1(0.02, shotpower);
            }
            else{
                inside.stop();
                outside.stop();
            }
        }
        else{
            inside.stop();
            outside.stop();
            inside_reload.stop();
            outside_reload.stop();
            send(wingPWadd,0x80); 
        }
    }
}

void send(char add, char dat){
    i2c.start();
    i2c.write(add);
    i2c.write(dat);
    i2c.stop();
    wait_us(25000);
}