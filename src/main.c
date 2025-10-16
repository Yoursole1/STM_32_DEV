
#include "peripheral/gpio.c"

void _start() {
    // configure clock

    tal_enable_clock(49);
    tal_set_mode(49, 1);

    while(true){
        tal_set_pin(49, 1);
        for(int i = 0; i < 1000000; i++){
            asm("nop");
        }
        tal_set_pin(49, 0);
        for(int i = 0; i < 1000000; i++){
            asm("nop");
        }
    }
}
