
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


extern uint32_t __heap_start;

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


    HEAP_START = &__heap_start;
    
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


    tal_set_pin(YELLOW_LED, 1);

    uint8_t* pts16[10];
    for(int i = 0; i < 10; i++){
        pts16[i] = alloc(16);
    }

    uint8_t* pts32[10];
    for(int i = 0; i < 10; i++){
        pts32[i] = alloc(17);
    }

    bool free[20];
    for(int i = 0; i < 20; i++){
        int j = (i >= 10 ? i - 10 : i);
        free[i] = isFree((i >= 10 ? pts32[j] : pts16[j]));
    }

    bool a = isFree(pts32[9] + 31);
    bool b = isFree(pts32[9] + 32);
    bool c = isFree(pts32[9] + 33);


    tal_set_pin(YELLOW_LED, 0);
    asm("BKPT #0");

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


