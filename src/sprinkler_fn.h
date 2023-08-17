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

/**
 */
extern bool sprinkler_config_changed;

/**
 * @fn void sprinkler_init(sprinkler_t **spr)
 * @brief
 *
 * @param spr
 */
void sprinkler_init(sprinkler_t **spr);

/**
 * @fn void sprinkler_deinit(sprinkler_t **spr)
 * @brief
 *
 * @param spr
 */
void sprinkler_deinit(sprinkler_t **spr);

/**
 * @fn spr_err_t sprinkler_start_cicle(sprinkler_t *spr)
 * @brief
 *
 * @param spr
 * @return
 */
spr_err_t sprinkler_start_cicle(sprinkler_t *spr);

/**
 * @fn bool sprinkler_is_start_time(sprinkler_t *spr)
 * @brief
 *
 * @param spr
 * @return
 */
bool sprinkler_is_start_time(sprinkler_t *spr);

/**
 * @fn spr_err_t sprinkler_setdt_day(sprinkler_t **spr, uint8_t id, uint8_t day, bool en)
 * @brief
 *
 * @param spr
 * @param id
 * @param day
 * @param en
 * @return
 */
spr_err_t sprinkler_setdt_day(sprinkler_t **spr, uint8_t id, uint8_t day, bool en);

/**
 * @fn sprinkler_setdt_hour(sprinkler_t **spr, uint8_t id, uint8_t hour, bool en)
 * @brief
 *
 * @param spr
 * @param id
 * @param hour
 * @param en
 * @return
 */
spr_err_t sprinkler_setdt_hour(sprinkler_t **spr, uint8_t id, uint8_t hour, bool en);

/**
 * @fn spr_err_t sprinkler_setdt_en(sprinkler_t **spr, uint8_t id, bool en)
 * @brief
 *
 * @param spr
 * @param id
 * @param en
 * @return
 */
spr_err_t sprinkler_setdt_en(sprinkler_t **spr, uint8_t id, bool en);

/**
 * @fn spr_err_t sprinkler_setmonth_en(sprinkler_t **spr, uint8_t month, bool en)
 * @brief
 *
 * @param spr
 * @param month
 * @param en
 * @return
 */
spr_err_t sprinkler_setmonth_en(sprinkler_t **spr, uint8_t month, bool en);

/**
 * @fn spr_err_t sprinkler_setmonth_a(sprinkler_t **spr, uint8_t month, uint8_t a)
 * @brief
 *
 * @param spr
 * @param month
 * @param a
 * @return
 */
spr_err_t sprinkler_setmonth_a(sprinkler_t **spr, uint8_t month, uint8_t a);

/**
 * @fn spr_err_t sprinkler_setmonth_b(sprinkler_t **spr, uint8_t month, uint8_t b)
 * @brief
 *
 * @param spr
 * @param month
 * @param b
 * @return
 */
spr_err_t sprinkler_setmonth_b(sprinkler_t **spr, uint8_t month, uint8_t b);

/**
 * @fn spr_err_t sprinkler_setmonth_dt(sprinkler_t **spr, uint8_t month, uint8_t dt)
 * @brief
 *
 * @param spr
 * @param month
 * @param dt
 * @return
 */
spr_err_t sprinkler_setmonth_dt(sprinkler_t **spr, uint8_t month, uint8_t dt);

/**
 * @fn spr_err_t sprinkler_setrelay_en(sprinkler_t **spr, uint8_t relay, bool en)
 * @brief
 *
 * @param spr
 * @param relay
 * @param en
 * @return
 */
spr_err_t sprinkler_setrelay_en(sprinkler_t **spr, uint8_t relay, bool en);

/**
 * @fn spr_err_t sprinkler_setrelay_pump(sprinkler_t **spr, uint8_t relay, bool pump)
 * @brief
 *
 * @param spr
 * @param relay
 * @param pump
 * @return
 */
spr_err_t sprinkler_setrelay_pump(sprinkler_t **spr, uint8_t relay, bool pump);

/**
 * @fn spr_err_t sprinkler_setrelay_min(sprinkler_t **spr, uint8_t relay, uint8_t min)
 * @brief
 *
 * @param spr
 * @param relay
 * @param min
 * @return
 */
spr_err_t sprinkler_setrelay_min(sprinkler_t **spr, uint8_t relay, uint8_t min);

/**
 * @fn spr_err_t sprinkler_setrelay_gpio(sprinkler_t **spr, uint8_t relay, uint8_t gpio)
 * @brief
 *
 * @param spr
 * @param relay
 * @param gpio
 * @return
 */
spr_err_t sprinkler_setrelay_gpio(sprinkler_t **spr, uint8_t relay, uint8_t gpio);

/**
 * @fn spr_err_t sprinkler_setpause(sprinkler_t **spr, uint8_t relay, uint32_t seconds)
 * @brief
 *
 * @param spr
 * @param relay
 * @param seconds
 * @return
 */
spr_err_t sprinkler_setpause(sprinkler_t **spr, uint8_t relay, uint32_t seconds);

/**
 * @fn spr_err_t sprinkler_setbeforepump(sprinkler_t **spr, uint32_t ms)
 * @brief
 *
 * @param spr
 * @param ms
 * @return
 */
spr_err_t sprinkler_setbeforepump(sprinkler_t **spr, uint32_t ms);

/**
 * @fn spr_err_t sprinkler_set_pump_en(sprinkler_t **spr, bool pump, bool en)
 * @brief
 *
 * @param spr
 * @param pump
 * @param en
 * @return
 */
spr_err_t sprinkler_set_pump_en(sprinkler_t **spr, bool pump, bool en);

/**
 * @fn spr_err_t sprinkler_set_pump_relay(sprinkler_t **spr, bool pump, uint8_t relay)
 * @brief
 *
 * @param spr
 * @param pump
 * @param relay
 * @return
 */
spr_err_t sprinkler_set_pump_relay(sprinkler_t **spr, bool pump, uint8_t relay);

#endif /* SPRINKLER_FN_H_ */
