
#include "peripheral/gpio.h"
#include "peripheral/watchdog.h"

void _start() {
    // configure clock

    tal_enable_clock(49);
    tal_enable_clock(9);

    tal_set_mode(49, 1);
    tal_set_mode(9, 0);

    ti_IWDG_enable();
    for(int i = 0; i < 1000000; i++){
        asm("nop");
    }

    while(true){
        bool read = tal_read_pin(9); // press blue button to exit loop, triggering watchdog reset ideally
        if(read){
            break;
        }
        tal_set_pin(49, 1);
        for(int i = 0; i < 1000000; i++){
            asm("nop");
        }
        tal_set_pin(49, 0);
        for(int i = 0; i < 1000000; i++){
            asm("nop");
        }

        // reset watchdog timer
        ti_IWDG_reset_timer();
    }

    while(true){
        tal_set_pin(49, 1);
        for(int i = 0; i < 100000; i++){
            asm("nop");
        }
        tal_set_pin(49, 0);
        for(int i = 0; i < 100000; i++){
            asm("nop");
        }
    }
}
