
#include "peripheral/gpio.h"
#include "peripheral/watchdog.h"
#include "internal/alloc.h"
#include "peripheral/uart.h"
#include "peripheral/pwm.h"
#include "peripheral/spi.h"

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


void test_spi() {
    uint8_t instance = 2;
    spi_config_t config;
    config.clk_pin = 66; // alt mode 4
    config.mosi_pin = 28; // alt mode 3;
    config.miso_pin = 74; // alt mode 3;
    config.data_size = 8;
    config.mode = 0;
    config.baudrate_prescaler = 2;
    config.first_bit = 0;
    config.priority = 0;
    config.mutex_timeout =0;
    spi_init(instance, &config);
    asm("BKPT #0");

    spi_device_t device;
    device.instance = 2;
    device.gpio_pin = 0;

    struct spi_sync_transfer_t transfer;
    uint8_t* source;
    (*source) = 0b10101010;
    transfer.source = source;
    uint8_t* dest;
    transfer.dest = dest;
    size_t size = 1;
    transfer.size = size;
    uint32_t timeout = 10000000;
    transfer.timeout = timeout;
    bool read_inc = false;
    transfer.read_inc = read_inc;
    asm("BKPT #0");

    spi_transfer_sync(&transfer);
    asm("BKPT #0");
}


void test_uart(){
    uart_config_t config;
    uart_channel_t channel = UART1;
    uart_parity_t parity = UART_PARITY_DISABLED;
    uart_datalength_t data_length = UART_DATALENGTH_8;
    config.channel = channel;
    config.parity = parity;
    config.data_length = data_length;
    config.baud_rate = 9600;
    config.clk_freq = 4000000; // 60 MHz

    // UART 1-3, 6 is fine (maybe)
    // UART 4-5, probably 7/8 isn't working (maybe)

    
    

    // asm("BKPT #0");
    int n = uart_init(&config, (void*) ( 0), (void*) (0), (void*) (0));
    asm("BKPT #0");
    // int n = 1; 


    uint8_t nums[10];
    for (uint8_t i = 0; i < 10; i++) {
      nums[i] = i;
    }
    uart_write_blocking(channel, nums, 10);
    asm("BKPT #0");
    // testing reading one byte
    // uint8_t buff[10];
    // int reading = uart_read_blocking(channel, buff, 10);
    // asm("BKPT #0");

    // // multiple byte
    // uint8_t buff[10];
    // uart_read_blocking(channel, buff, 10);
    // asm("BKPT #0");
}

void test_pwm(){
    tal_pwm_pin_init(TIM2_CH1_1, 2000, 30000, (void*)0);

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

    // tal_set_pin(YELLOW_LED, 1);
    

    // test_pwm();
    // test_uart();
    test_spi();
}

