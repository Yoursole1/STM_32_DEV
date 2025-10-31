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
 * @file common/platform/pwm.h
 * @authors Ethan Rad
 * @brief Driver for pulse-width modulation
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  int pin;            // Pin number on MCU
  int32_t *pinreg;    // Pin reg pointer
  int num_in_group;   // Pin # in GPIO group
  int channel;        // Timer channel number
  int32_t *base;      // Base register of timer channel
  int frequency;      // PWM frequency on pin
  uint16_t dutyCycle; // Duty cycle of PWM signal on pin
  bool running;       // Whether pin is outputting a PWM signal
} pwm_pin_t;

/**
 * @param pin: The integer value of the pin
 * @param frequency: The PWM frequency of the pin
 * @param dutyCycle: The duty cycle of the pwm signal on a scale of 0 to 65535
 */
void tal_pwm_init(int pin, int frequency, uint16_t dutyCycle, bool *const err);

/**
 * @param pin: The integer value of the pin
 * @param frequency: The PWM frequency of the pin
 */
void tal_pwm_pin_set_channel_freq(int pin, int frequency, bool *const err);

/**
 * @param pin: The integer value of the pin
 * @param dutyCycle: The duty cycle of the pwm signal on a scale of 0 to 255
 */
void tal_pwm_pin_set_channel_duty_cycle(int pin, uint8_t dutyCycle,
                                        bool *const err);

/**
 * @param pin: The pin to enable PWM on
 */
void tal_pwm_pin_enable(int pin, bool *const err);

/**
 * @param pin: The pin to disable PWM on
 */
void tal_pwm_pin_disable(int pin, bool *const err);

/**
 * @param pin: The pin which is to be checked if it is running
 *
 * @return: true if pwm on the pin is on, false if it is off
 */
bool tal_pwm_is_running(int pin, bool *const err);

/**
 * @param pin: The pin which is to be checked if it is valid
 * @param pin_info: Pointer to struct which should be filled with pin info
 *
 * @return: true if pwm on the pin is valid, false if it is not
 */
bool get_pin_info(int pin, pwm_pin_t *pin_info);
