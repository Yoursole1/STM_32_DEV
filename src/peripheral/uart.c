/**
 * This file is part of the Titan Flight Computer Project
 * Copyright (c) 2024 UW SARP
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
 * @file common/platform/usart.h /// @todo UPDATE THIS
 * @authors Charles Faisandier, Joanna Zhou
 * @brief USART Driver Implementation
 */

#include "uart.h"
#include "../internal/mmio.h"
#include "clk.h"
#include "gpio.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define CR_REG_COUNT 3
#define NUM_REQUESTS_PER_UART 2

#define IS_USART_CHANNEL(channel)                                              \
  ((channel) == UART1 || (channel) == UART2 || (channel) == UART3 ||           \
   (channel) == UART6)

/**************************************************************************************************
 * @section  Data Structures
 **************************************************************************************************/

// Stores dmamux request numbers. Index 0 is for RX stream, 1 for TX stream.
const static uint8_t uart_dmamux_req[UART_CHANNEL_COUNT]
                                    [NUM_REQUESTS_PER_UART] = {
                                        [UART1] =
                                            {
                                                [0] = 41,
                                                [1] = 42,
                                            },
                                        [UART2] =
                                            {
                                                [0] = 43,
                                                [1] = 44,
                                            },
                                        [UART3] =
                                            {
                                                [0] = 45,
                                                [1] = 46,
                                            },
                                        [UART4] =
                                            {
                                                [0] = 63,
                                                [1] = 64,
                                            },
                                        [UART5] =
                                            {
                                                [0] = 65,
                                                [1] = 66,
                                            },
                                        [UART6] =
                                            {
                                                [0] = 71,
                                                [1] = 72,
                                            },
                                        [UART7] =
                                            {
                                                [0] = 79,
                                                [1] = 80,
                                            },
                                        [UART8] =
                                            {
                                                [0] = 81,
                                                [1] = 82,
                                            },
};

volatile dma_periph_streaminfo_t uart_to_dma[UART_CHANNEL_COUNT] = {0};

bool uart_busy[UART_CHANNEL_COUNT] = {0};

uart_context_t uart_contexts[UART_CHANNEL_COUNT] = {0};

uint32_t timeout;

/**************************************************************************************************
 * @section Private Function Implementations
 **************************************************************************************************/
bool set_alternate_function(uart_channel_t channel, uint8_t tx_pin,
                            uint8_t rx_pin) {
  switch (channel) {
  case UART1:
    if (tx_pin == 98 || tx_pin == 133) {
      tal_alternate_mode(tx_pin, 7);
    } else if (tx_pin == 74) {
      tal_alternate_mode(tx_pin, 4);
    } else {
      // // tal_raise(flag, "Invalid TX Pin for channel");
      return false;
    }
    if (rx_pin == 99 || rx_pin == 134) {
      tal_alternate_mode(rx_pin, 7);
    } else if (tx_pin == 75) {
      tal_alternate_mode(rx_pin, 4);
    } else {
      // // tal_raise(flag, "Invalid RX Pin for channel");
      return false;
    }
    break;
  case UART2:
    if (tx_pin == 39 || tx_pin == 117) {
      tal_alternate_mode(tx_pin, 7);
    } else {
      // tal_raise(flag, "Invalid TX Pin for channel");
      return false;
    }
    if (rx_pin == 40 || rx_pin == 120) {
      tal_alternate_mode(tx_pin, 7);
    } else {
      // tal_raise(flag, "Invalid RX Pin for channel");
      return false;
    }
    break;
  case UART3:
    if (tx_pin == 66 || tx_pin == 109 || tx_pin == 76) {
      tal_alternate_mode(tx_pin, 7);
    } else {
      // tal_raise(flag, "Invalid TX Pin for channel");
      return false;
    }
    if (rx_pin == 67 || tx_pin == 110 || tx_pin == 77) {
      tal_alternate_mode(rx_pin, 7);
    } else {
      // tal_raise(flag, "Invalid RX Pin for channel");
      return false;
    }
    break;
  case UART4:
    if (tx_pin == 37 || tx_pin == 137 || tx_pin == 109 || tx_pin == 113) {
      tal_alternate_mode(tx_pin, 8);
    } else if (tx_pin == 100) {
      tal_alternate_mode(tx_pin, 6);
    } else {
      // tal_raise(flag, "Invalid TX Pin for channel");
      return false;
    }
    if (rx_pin == 38 || rx_pin == 136 || rx_pin == 110 || rx_pin == 112) {
      tal_alternate_mode(rx_pin, 8);
    } else if (rx_pin == 101) {
      tal_alternate_mode(rx_pin, 6);
    } else {
      // tal_raise(flag, "Invalid RX Pin for channel");
      return false;
    }
    break;
  case UART5:
    if (tx_pin == 133 || rx_pin == 73) {
      tal_alternate_mode(tx_pin, 14);
    } else if (tx_pin == 111) {
      tal_alternate_mode(tx_pin, 8);
    } else {
      // tal_raise(flag, "Invalid TX Pin for channel");
      return false;
    }
    if (rx_pin == 132 || rx_pin == 72) {
      tal_alternate_mode(rx_pin, 14);
    } else if (rx_pin == 114) {
      tal_alternate_mode(rx_pin, 8);
    } else {
      // tal_raise(flag, "Invalid RX Pin for channel");
      return false;
    }
    break;
  case UART6:
    if (tx_pin == 93 || tx_pin == 122) {
      tal_alternate_mode(tx_pin, 7);
    } else {
      // tal_raise(flag, "Invalid TX Pin for channel");
      return false;
    }
    if (rx_pin == 94) {
      tal_alternate_mode(rx_pin, 7);
    } else {
      // tal_raise(flag, "Invalid RX Pin for channel");
      return false;
    }
    break;
  case UART7:
    if (tx_pin == 108 || tx_pin == 131) {
      tal_alternate_mode(tx_pin, 11);
    } else if (tx_pin == 58 || tx_pin == 21) {
      tal_alternate_mode(tx_pin, 7);
    } else {
      // tal_raise(flag, "Invalid TX Pin for channel");
      return false;
    }
    if (rx_pin == 97 || rx_pin == 130) {
      tal_alternate_mode(rx_pin, 11);
    } else if (rx_pin == 57 || rx_pin == 20) {
      tal_alternate_mode(rx_pin, 7);
    } else {
      // tal_raise(flag, "Invalid RX Pin for channel");
      return false;
    }
    break;
  case UART8:
    if (tx_pin == 139) {
      tal_alternate_mode(tx_pin, 8);
    } else {
      // tal_raise(flag, "Invalid TX Pin for channel");
      return false;
    }
    if (rx_pin == 138) {
      tal_alternate_mode(rx_pin, 8);
    } else {
      // tal_raise(flag, "Invalid RX Pin for channel");
      return false;
    }
    break;
  case UART_CHANNEL_COUNT:
    break;
  }
  return true;
}

bool uart_write_byte(uart_channel_t channel, uint8_t data) {
  uint32_t count = 0;
  // Get the correct registers/fields
  ro_reg32_t isr_reg;
  field32_t isr_field;
  rw_reg32_t tdr_reg;
  field32_t tdr_field;
  if (IS_USART_CHANNEL(channel)) {
    isr_reg = USARTx_ISR[channel];
    isr_field = USARTx_ISR_TXE;
    tdr_reg = USARTx_TDR[channel];
    tdr_field = USARTx_TDR_TDR;
  } else {
    isr_reg = UARTx_ISR[channel];
    isr_field = UARTx_ISR_TXE;
    tdr_reg = UARTx_TDR[channel];
    tdr_field = UARTx_TDR_TDR;
  }

  // Wait until the transmit FIFO is not full.
  while (READ_FIELD(isr_reg, isr_field) == 0) {
    if (count++ >= timeout) {
      return false; // Return false on timeout
    }
  }

  // Write the data to the transmit data register.
  // This automatically pushes the data into the FIFO.
  WRITE_FIELD(tdr_reg, tdr_field, data);

  // This is a blocking function, so we return immediately after the data is
  // placed in the FIFO. If you needed to ensure the data was completely sent,
  // you would wait for the TC (Transmission Complete) flag.
  return true;
}

bool uart_read_byte(uint8_t channel, uint8_t *data) {
  uint32_t count = 0;

  // Get the correct registers/fields.
  ro_reg32_t isr_reg;
  field32_t isr_field;
  ro_reg32_t rdr_reg;
  field32_t rdr_field;

  // Use the macro to select the correct register set for USART vs UART
  if (IS_USART_CHANNEL(channel)) {
    isr_reg = USARTx_ISR[channel];
    isr_field = USARTx_ISR_RXNE;
    rdr_reg = USARTx_RDR[channel];
    rdr_field = USARTx_RDR_RDR;
  } else {
    isr_reg = UARTx_ISR[channel];
    isr_field = UARTx_ISR_RXNE;
    rdr_reg = UARTx_RDR[channel];
    rdr_field = UARTx_RDR_RDR;
  }

  // Input validation: ensure the destination pointer is not NULL
  if (data == NULL) {
    return false;
  }

  // Wait until the receive FIFO is not empty.
  while (READ_FIELD(isr_reg, isr_field) == 0) {
    if (count++ >= timeout) {
      return false; // Return false on timeout
    }
  }

  // Read the data from the receive data register.
  // The hardware automatically retrieves the next available byte from the FIFO.
  *data = (uint8_t)READ_FIELD(rdr_reg, rdr_field);

  return true;
}

static inline bool verify_transfer_parameters(uart_channel_t channel, uint8_t *buff,
                                       size_t size) {

  if (buff == NULL) {
    // tal_raise(flag, "Buffer cannot be NULL");
    return false;
  }
  if (size == 0) {
    // tal_raise(flag, "Size cannot be zero");
    return false;
  }
  return true;
}

/**
 * Macro that generates cases for uart init
 * @author Owen Voskuhl Hayes, Lorde of the Isle, first of his name.
 *
 */
#define UART_FIELD_GENERATOR(uart, shift, reg)                                 \
  case uart: {                                                                 \
    field32_t n;                                                               \
    n.msk = (1 << shift);                                                      \
    n.pos = shift;                                                             \
    SET_FIELD(reg, n);                                                         \
    break;                                                                     \
  }

/**************************************************************************************************
 * @section Public Function Implementations
 **************************************************************************************************/
bool uart_init(uart_config_t *usart_config, dma_callback_t *callback,
               periph_dma_config_t *tx_stream, periph_dma_config_t *rx_stream) {
  // De-reference struct members for readability
  uart_channel_t channel = usart_config->channel;
  uint8_t tx_pin = usart_config->tx_pin;
  uint8_t rx_pin = usart_config->rx_pin;
  uint8_t ck_pin = usart_config->ck_pin;
  uint32_t baud_rate = usart_config->baud_rate;
  uart_parity_t parity = usart_config->parity;
  uart_datalength_t data_length = usart_config->data_length;

  // Enable usart clock
  switch (channel) {
    UART_FIELD_GENERATOR(UART1, 4, RCC_APB2ENR)
    UART_FIELD_GENERATOR(UART2, 17, RCC_APB1LENR)
    UART_FIELD_GENERATOR(UART3, 18, RCC_APB1LENR)
    UART_FIELD_GENERATOR(UART4, 19, RCC_APB1LENR)
    UART_FIELD_GENERATOR(UART5, 20, RCC_APB1LENR)
    UART_FIELD_GENERATOR(UART6, 5, RCC_APB2ENR)
    UART_FIELD_GENERATOR(UART7, 30, RCC_APB1LENR)
    UART_FIELD_GENERATOR(UART8, 31, RCC_APB1LENR)
  default:
    // Handle error or invalid USART number
    break;
  }

  // Set alternate-function mode
  tal_set_mode(tx_pin, 2);
  tal_set_mode(rx_pin, 2);

  tal_alternate_mode(tx_pin, 7);
  tal_alternate_mode(rx_pin, 7);

  // Set the right alternate function
  // if (!set_alternate_function(channel, tx_pin, rx_pin))
  //   return false;

  // // Enable the clocks for the gpio pins
  // tal_enable_clock(rx_pin);
  // tal_enable_clock(tx_pin);

  // Ensure the clock pin is disabled for asynchronous mode
  CLR_FIELD(USARTx_CR2[channel], USARTx_CR2_CLKEN);
  // CLR_FIELD(UART_MAP[channel].CR2, USARTx_CR2_CLKEN);
  
  
  // Set baud rate
  uint32_t clk_freq;
  if (IS_USART_CHANNEL(channel)) {
    clk_freq = clock_get_freq_ahb2();
  } else {
    clk_freq = clock_get_freq_ahb1();
  }

  
  uint32_t brr_value = clk_freq / baud_rate;
  WRITE_FIELD(USARTx_BRR[channel], USARTx_BRR_BRR_4_15, brr_value);



  // Set parity
  switch (parity) {
  case UART_PARITY_DISABLED:
    CLR_FIELD(USARTx_CR1[channel], USARTx_CR1_PCE);
    // CLR_FIELD(UART_MAP[channel].CR1, UARTx_CR1_PCE);
    break;
  case UART_PARITY_EVEN:
    SET_FIELD(USARTx_CR1[channel], USARTx_CR1_PCE);
    // SET_FIELD(UART_MAP[channel].CR1, UARTx_CR1_PCE);
    CLR_FIELD(USARTx_CR1[channel], USARTx_CR1_PS);
    // CLR_FIELD(UART_MAP[channel].CR1, UARTx_CR1_PS);
    break;
  case UART_PARITY_ODD:
    SET_FIELD(USARTx_CR1[channel], USARTx_CR1_PCE);
    SET_FIELD(USARTx_CR1[channel], USARTx_CR1_PS);
    // SET_FIELD(UART_MAP[channel].CR1, UARTx_CR1_PCE);
    // SET_FIELD(UART_MAP[channel].CR1, UARTx_CR1_PS);
    break;
  }


  // Set data length
  switch (data_length) {
  case UART_DATALENGTH_7:
    if (!parity) {
      // tal_raise(flag, "Invalid parity datasize combo");
      return false;
    }
    SET_FIELD(USARTx_CR1[channel], USARTx_CR1_Mx[0]);
    CLR_FIELD(USARTx_CR1[channel], USARTx_CR1_Mx[1]);
    // SET_FIELD(UART_MAP[channel].CR1, UARTx_CR1_Mx[0]);
    // CLR_FIELD(UART_MAP[channel].CR1, UARTx_CR1_Mx[1]);
    break;
  case UART_DATALENGTH_8:
    CLR_FIELD(USARTx_CR1[channel], USARTx_CR1_Mx[0]);
    CLR_FIELD(USARTx_CR1[channel], USARTx_CR1_Mx[1]);
    // CLR_FIELD(UART_MAP[channel].CR1, UARTx_CR1_Mx[0]);
    // CLR_FIELD(UART_MAP[channel].CR1, UARTx_CR1_Mx[1]);
    break;
  case UART_DATALENGTH_9:
    if (parity) {
      // tal_raise(flag, "Invalid parity datasize combo");
      return false;
    }
    SET_FIELD(USARTx_CR1[channel], USARTx_CR1_Mx[0]);
    SET_FIELD(USARTx_CR1[channel], USARTx_CR1_Mx[1]);
    break;
  }
  

  // Enable FIFOs
  SET_FIELD(USARTx_CR1[channel], USARTx_CR1_FIFOEN);
  // WRITE_FIELD(UART_MAP[channel].CR1, UARTx_CR1_FIFOEN, 1);

  dma_config_t dma_tx_stream = {
      .instance = tx_stream->instance,
      .stream = tx_stream->stream,
      .request_id = uart_dmamux_req[channel][1],
      .direction = tx_stream->direction,
      .src_data_size = tx_stream->src_data_size,
      .dest_data_size = tx_stream->dest_data_size,
      .priority = tx_stream->priority,
      .fifo_enabled = false, // FIFO disabled for tx
      .fifo_threshold = tx_stream->fifo_threshold,
      .callback = *callback, // We need to know if it failed.
  };
  dma_configure_stream(&dma_tx_stream);

  dma_config_t dma_rx_stream = {
      .instance = rx_stream->instance,
      .stream = rx_stream->stream,
      .request_id = uart_dmamux_req[channel][0],
      .direction = rx_stream->direction,
      .src_data_size = rx_stream->src_data_size,
      .dest_data_size = rx_stream->dest_data_size,
      .priority = rx_stream->priority,
      .fifo_enabled = false, // FIFO disabled for tx
      .fifo_threshold = rx_stream->fifo_threshold,
      .callback = *callback, // We need to know if it failed.
  };
  dma_configure_stream(&dma_rx_stream);

  // Save stream info
  dma_periph_streaminfo_t info = {.rx_instance = rx_stream->instance,
                                  .tx_instance = tx_stream->instance,
                                  .rx_stream = rx_stream->stream,
                                  .tx_stream = tx_stream->stream};
  uart_to_dma[channel] = info;

  // Set the timeout parameter
  timeout = usart_config->timeout;

  // Enable the peripheral

  SET_FIELD(USARTx_CR1[channel], USARTx_CR1_TE);
  SET_FIELD(USARTx_CR1[channel], USARTx_CR1_RE);
  SET_FIELD(USARTx_CR1[channel], USARTx_CR1_UE);

  // WRITE_FIELD(UART_MAP[channel].CR1, USARTx_CR1_UE, 1);
  // WRITE_FIELD(UART_MAP[channel].CR1, USARTx_CR1_TE, 1);
  // WRITE_FIELD(UART_MAP[channel].CR1, USARTx_CR1_RE, 1);

  return true;
}

bool uart_write_async(uart_channel_t channel, uint8_t *tx_buff, uint32_t size) {
  // Verify parameters
  verify_transfer_parameters(channel, tx_buff, size);

  // Check if usart channel is busy
  if (uart_busy[channel]) {
    // tal_raise(flag, "USART channel is busy");
    return false;
  }
  uart_busy[channel] = true;

  // Configure DMA stream
  uart_context_t context = {
      .busy = &uart_busy[channel],
      .channel = channel,
  };
  uart_contexts[channel] = context;
  dma_transfer_t tx_transfer = {
      .instance = uart_to_dma[channel].tx_instance,
      .stream = uart_to_dma[channel].tx_stream,
      .src = tx_buff,
      .dest = (void *)UART_MAP[channel].TDR, // maybe revisit the cast... in dma transfer struct
      .size = size,
      .context = &uart_contexts[channel],
      .disable_mem_inc = false,
  };
  dma_start_transfer(&tx_transfer);

  // Enable the dma requests
  SET_FIELD(UART_MAP[channel].CR3, UARTx_CR3_DMAT);

  return true;
}

bool uart_read_async(uart_channel_t channel, uint8_t *rx_buff, uint32_t size) {
  // Verify parameters
  verify_transfer_parameters(channel, rx_buff, size);

  // Check if usart channel is busy
  if (uart_busy[channel]) {
    // tal_raise(flag, "USART channel is busy");
    return false;
  }
  uart_busy[channel] = true;

  // Configure DMA stream
  uart_context_t context = {
      .busy = &uart_busy[channel],
      .channel = channel,
  };
  uart_contexts[channel] = context;
  dma_transfer_t tx_transfer = {
      .instance = uart_to_dma[channel].tx_instance,
      .stream = uart_to_dma[channel].tx_stream,
      .src = (void *)UART_MAP[channel].RDR,
      .dest = rx_buff,
      .size = size,
      .context = &uart_contexts[channel],
      .disable_mem_inc = false,
  };
  dma_start_transfer(&tx_transfer);

  // Enable the dma requests
  SET_FIELD(UART_MAP[channel].CR3, UARTx_CR3_DMAT);

  return true;
}

bool uart_write_blocking(uart_channel_t channel, uint8_t *tx_buff,
                         uint32_t size) {
  // Verify parameters
  verify_transfer_parameters(channel, tx_buff, size);

  // Check if usart channel is busy
  if (uart_busy[channel]) {
    // tal_raise(flag, "USART channel is busy");
    return false;
  }
  uart_busy[channel] = true;

  // Transmit the data byte by byte
  for (uint32_t i = 0; i < size; i++) {
    if (!uart_write_byte(channel, tx_buff[i])) {
      // tal_raise(flag, "USART write timeout");
      uart_busy[channel] = false;
      return false;
    }
  }

  uart_busy[channel] = false;
  return true;
}

bool uart_read_blocking(uart_channel_t channel, uint8_t *rx_buff,
                        uint32_t size) {
  // Verify parameters
  verify_transfer_parameters(channel, rx_buff, size);

  // Check if usart channel is busy
  if (uart_busy[channel]) {
    // tal_raise(flag, "USART channel is busy");
    return false;
  }
  uart_busy[channel] = true;

  // Receive the data byte by byte
  for (uint32_t i = 0; i < size; i++) {
    if (!uart_read_byte(channel, &rx_buff[i])) {
      // tal_raise(flag, "USART read timeout");
      uart_busy[channel] = false;
      return false;
    }
  }

  uart_busy[channel] = false;
  return true;
}
