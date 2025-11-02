
#include "peripheral/gpio.h"
#include "peripheral/watchdog.h"
#include "internal/alloc.h"
#include "peripheral/uart.h"
#include "peripheral/pwm.h"

#define USR_BUTTON 9
#define GREEN_LED 49
#define YELLOW_LED 139
#define RED_LED 74

void delay() {
  for (int i = 0; i < 100000; i++) {
    asm("nop");
  }
}

extern uint32_t __heap_start;


void test_uart(){
    uart_config_t config;
    uart_channel_t channel = UART1;
    uart_parity_t parity = UART_PARITY_DISABLED;
    uart_datalength_t data_length = UART_DATALENGTH_8;
    config.channel = channel;
    config.tx_pin = 98;
    config.rx_pin = 99;
    config.ck_pin = 121;
    config.baud_rate = 9600;
    config.parity = parity;
    config.data_length = data_length;
    config.timeout = 0;
    tal_enable_clock(config.tx_pin);
    tal_enable_clock(config.rx_pin);
    tal_enable_clock(config.ck_pin);
    
    

    // asm("BKPT #0");
    int n = uart_init(&config, (void*) ( 0), (void*) (0), (void*) (0));
    asm("BKPT #0");
    // int n = 1; 
    if (n == 1) {
        for (int i = 0; i < 5; i++) {
        tal_set_pin(GREEN_LED, 1);
        delay();
        tal_set_pin(GREEN_LED, 0);
        delay();
        }
    } else {
        for (int i = 0; i < 5; i++) {
        tal_set_pin(RED_LED, 1);
        delay();
        tal_set_pin(RED_LED, 0);
        delay();
        }
    }


    uint8_t num = 0b10101010;
    uart_write_blocking(UART1, &num, 1);
    asm("BKPT #0");

}

void test_pwm(){
    tal_pwm_pin_init(TIM2_CH1_1, 200, 30000, (void*)0);

    //----------
    // should be done inside the PWM driver.  Here for testing
    tal_enable_clock(TIM2_CH1_1);
    SET_FIELD(RCC_APB1LENR, RCC_APB1LENR_TIMxEN[2]); // enable clock for TIM2

    tal_set_mode(TIM2_CH1_1, 2);
    tal_alternate_mode(TIM2_CH1_1, 0);
    //----------
    

    tal_pwm_pin_enable(TIM2_CH1_1, (void*)0);
    asm("BKPT #0");
    tal_pwm_pin_disable(TIM2_CH1_1, (void*)0);
    asm("BKPT #0");
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
    
    HEAP_START = &__heap_start;
    int heap_status = init_heap();

    if(heap_status != 1){
        asm("BKPT #0"); // heap init failure 
    }

    test_pwm();
    
}

