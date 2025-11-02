
#include "peripheral/gpio.h"
#include "peripheral/watchdog.h"
// #include "internal/alloc.h"
#include "peripheral/uart.h"
#include <stdio.h>

#define USR_BUTTON 9
#define GREEN_LED 49
#define YELLOW_LED 139
#define RED_LED 74

static bool on = false;
void press_button() {
  on = !on;
  tal_set_pin(GREEN_LED, on);
}

void delay() {
  for (int i = 0; i < 100000; i++) {
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
  uart_config_t config;
  uart_channel_t channel = UART1;
  uart_parity_t parity = UART_PARITY_DISABLED;
  uart_datalength_t data_length = UART_DATALENGTH_8;
  config.channel = channel;
  config.parity = parity;
  config.data_length = data_length;
  config.timeout = 0;
  

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

  // // ti_IWDG_enable();

  // HEAP_START = &__heap_start;

  // int init = init_heap(); // initialize allocator
  // if(init == -1){
  //     for(int i = 0; i < 3; i++){
  //         tal_set_pin(RED_LED, 1);
  //         delay();
  //         tal_set_pin(RED_LED, 0);
  //         delay();
  //     }
  // }else{
  //     for(int i = 0; i < 5; i++){
  //         tal_set_pin(GREEN_LED, 1);
  //         delay();
  //         tal_set_pin(GREEN_LED, 0);
  //         delay();
  //     }
  // }

  // tal_set_pin(YELLOW_LED, 1);

  // uint8_t* pts16[10];
  // for(int i = 0; i < 10; i++){
  //     pts16[i] = alloc(16);
  // }

  // uint8_t* pts32[10];
  // for(int i = 0; i < 10; i++){
  //     pts32[i] = alloc(17);
  // }

  // bool free[20];
  // for(int i = 0; i < 20; i++){
  //     int j = (i >= 10 ? i - 10 : i);
  //     free[i] = isFree((i >= 10 ? pts32[j] : pts16[j]));
  // }

  // bool a = isFree(pts32[9] + 31);
  // bool b = isFree(pts32[9] + 32);
  // bool c = isFree(pts32[9] + 33);

  // tal_set_pin(YELLOW_LED, 0);
  asm("BKPT #0");

  // uint32_t button_cooldown = 0;
  // while(true){
  //     bool read = tal_read_pin(USR_BUTTON); // press blue button to exit
  //     loop, triggering watchdog reset ideally if(read && button_cooldown ==
  //     0){
  //         button_cooldown = 100000;
  //         press_button();
  //     }

  //     // reset watchdog timer
  //     // ti_IWDG_reset_timer();

  //     button_cooldown = (button_cooldown > 0 ? button_cooldown - 1 : 0);
  // }
}

/**
 * This file is part of the Titan Project.
 * Copyright (c) 2025 UW SARP
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * @file modules/apps/src/launch-sequence.c
 * @authors Charles Faisandier
 * @brief Implementation of Launch Sequence
 */

/**
 * @brief launch sequence. Responds to commands from umbilical.
 */
//  void ti_launch_sequence() {
//    while (1) {
//      if (uart_data_available()) {
//        read_message();
//        if (command == LAUNCH)
//          do_launch();
//        else if (command == ABORT)
//          do_abort();
//      }
//    }
//  }
