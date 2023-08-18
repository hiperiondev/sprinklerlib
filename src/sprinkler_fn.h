/**
 * @file sprinkler_fn.h
 * @brief Sprinkler functions
 * @copyright 2023 Emiliano Augusto Gonzalez (hiperiondev). This project is released under MIT license. Contact: egonzalez.hiperion@gmail.com
 * @see Project Site: https://github.com/hiperiondev/sprinklerlib
 * @note This is based on other projects. Please contact their authors for more information.
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef SPRINKLER_FN_H_
#define SPRINKLER_FN_H_

#include <stdint.h>
#include <stdbool.h>

extern bool sprinkler_config_changed;

void sprinkler_init(sprinkler_t **spr);

void sprinkler_deinit(sprinkler_t **spr);

spr_err_t sprinkler_start_cicle(sprinkler_t *spr);

bool sprinkler_is_start_time(sprinkler_t *spr);

spr_err_t sprinkler_set_dt_day(sprinkler_t **spr, uint8_t id, uint8_t day, bool en);

spr_err_t sprinkler_set_dt_hour(sprinkler_t **spr, uint8_t id, uint8_t hour, bool en);

spr_err_t sprinkler_set_dt_en(sprinkler_t **spr, uint8_t id, bool en);

spr_err_t sprinkler_set_dt_queue(sprinkler_t **spr, uint8_t id, uint8_t queue, bool en);

spr_err_t sprinkler_set_month_en(sprinkler_t **spr, uint8_t month, bool en);

spr_err_t sprinkler_set_month_a(sprinkler_t **spr, uint8_t month, uint8_t a);

spr_err_t sprinkler_set_month_b(sprinkler_t **spr, uint8_t month, uint8_t b);

spr_err_t sprinkler_set_month_dt(sprinkler_t **spr, uint8_t month, uint8_t dt);

spr_err_t sprinkler_set_relay_en(sprinkler_t **spr, uint8_t relay, bool en);

spr_err_t sprinkler_set_relay_pump(sprinkler_t **spr, uint8_t relay, uint8_t pump);

spr_err_t sprinkler_set_relay_sec(sprinkler_t **spr, uint8_t relay, uint8_t min);

spr_err_t sprinkler_set_relay_overlap(sprinkler_t **spr, uint8_t relay, uint32_t ms);

spr_err_t sprinkler_set_relay_gpio(sprinkler_t **spr, uint8_t relay, uint8_t gpio);

spr_err_t sprinkler_set_queue(sprinkler_t **spr, uint8_t queue, uint32_t relay, bool en) ;

spr_err_t sprinkler_set_queue_pause(sprinkler_t **spr, uint8_t queue, uint32_t seconds);

spr_err_t sprinkler_set_queue_autoadv(sprinkler_t **spr, uint8_t queue, bool en);

spr_err_t sprinkler_set_queue_relay_sec(sprinkler_t **spr, uint8_t queue, uint8_t relay, uint16_t seconds);

spr_err_t sprinkler_set_queue_repeat(sprinkler_t **spr, uint8_t queue, uint8_t times);

spr_err_t sprinkler_set_pause(sprinkler_t **spr, uint8_t relay, uint32_t seconds);

spr_err_t sprinkler_set_pump_delay(sprinkler_t **spr, uint32_t ms);

spr_err_t sprinkler_set_pump_en(sprinkler_t **spr, uint8_t pump, bool en);

spr_err_t sprinkler_set_pump_relay(sprinkler_t **spr, uint8_t pump, uint8_t relay);

bool sprinkler_is_queue_paused(void);

spr_err_t sprinkler_is_queue_running(void);

spr_err_t sprinkler_is_relay_running(void);

#endif /* SPRINKLER_FN_H_ */
