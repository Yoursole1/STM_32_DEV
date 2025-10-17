
#include "peripheral/gpio.h"
#include "peripheral/watchdog.h"
#include "internal/alloc.h"


static bool on = false;
void press_button(){
    on = !on;
    tal_set_pin(49, on);
}



void _start() {
    // configure clock
    extern uint32_t __heap_start;
    uint32_t* heap_start_addr = &__heap_start;
    HEAP_START = heap_start_addr;
    

    tal_enable_clock(49);
    tal_enable_clock(9);

    tal_set_mode(49, 1);
    tal_set_mode(9, 0);

    ti_IWDG_enable();


    for(int i = 0; i < 1000000; i++){
        asm("nop");
    }

    uint32_t button_cooldown = 0;
    while(true){
        bool read = tal_read_pin(9); // press blue button to exit loop, triggering watchdog reset ideally
        if(read && button_cooldown == 0){
            button_cooldown = 100000;
            press_button();
        }

        // reset watchdog timer
        ti_IWDG_reset_timer();

        button_cooldown = (button_cooldown > 0 ? button_cooldown - 1 : 0);
    }
}


