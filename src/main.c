
#include "peripheral/gpio.h"
#include "peripheral/watchdog.h"
#include "internal/alloc.h"


#define USR_BUTTON 9
#define GREEN_LED 49
#define YELLOW_LED 139
#define RED_LED 74



static bool on = false;
void press_button(){
    on = !on;
    tal_set_pin(GREEN_LED, on);
}

void delay(){
    for(int i = 0; i < 1000000; i++){
        asm("nop");
    }
}



void _start() {

    tal_enable_clock(GREEN_LED);
    tal_enable_clock(RED_LED);
    tal_enable_clock(YELLOW_LED);
    tal_enable_clock(USR_BUTTON);
    

    tal_set_mode(GREEN_LED, 1);
    tal_set_mode(RED_LED, 1);
    tal_set_mode(YELLOW_LED, 1);
    tal_set_mode(USR_BUTTON, 0);

    // ti_IWDG_enable();

    extern uint32_t __heap_start;
    uint32_t* heap_start_addr = &__heap_start;
    HEAP_START = heap_start_addr;

    int init = init_heap(); // initialize allocator
    if(init == -1){
        for(int i = 0; i < 3; i++){
            tal_set_pin(RED_LED, 1);
            delay();
            tal_set_pin(RED_LED, 0);
            delay();
        }
    }else{
        for(int i = 0; i < 5; i++){
            tal_set_pin(GREEN_LED, 1);
            delay();
            tal_set_pin(GREEN_LED, 0);
            delay();
        }
    }


    uint8_t* my_mem = alloc(5);





    for(int i = 0; i < 1000000; i++){
        asm("nop");
    }

    uint32_t button_cooldown = 0;
    while(true){
        bool read = tal_read_pin(USR_BUTTON); // press blue button to exit loop, triggering watchdog reset ideally
        if(read && button_cooldown == 0){
            button_cooldown = 100000;
            press_button();
        }

        // reset watchdog timer
        // ti_IWDG_reset_timer();

        button_cooldown = (button_cooldown > 0 ? button_cooldown - 1 : 0);
    }
}


