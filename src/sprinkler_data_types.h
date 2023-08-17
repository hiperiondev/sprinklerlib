/**
 * @file sprinkler_data_types.h
 * @brief Sprinkler data types
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

#ifndef SPRINKLER_DATA_TYPES_H_
#define SPRINKLER_DATA_TYPES_H_

#include <stdint.h>

#define MAX_DT 32 // should never be bigger than 32

/// bitwise utils

#define SET_BIT(x, pos)    (x | (1U << pos))
#define CLEAR_BIT(x, pos)  (x & (~(1U << pos)))
#define CHECK_BIT(x, pos)  (x & (1UL << pos))

/// sprinkler get values

#define GET_DT_EN(x)           (CHECK_BIT(x, 31))
#define GET_DT_HOURS(x)        ((x & 0x7fffff80) >> 7)
#define GET_DT_HOUR(x, h)      (CHECK_BIT(GET_DT_HOURS(x), h))
#define GET_DT_DAYS(x)         (x & 0x7f)
#define GET_DT_DAY(x, d)       (CHECK_BIT(GET_DT_DAYS(x), d))

#define GET_RELAY_EN(x)        (CHECK_BIT(x, 7))
#define GET_RELAY_PUMP(x)      (CHECK_BIT(x, 6))
#define GET_RELAY_MIN(x)       (x & 0x3f)

#define GET_MONTH_EN(x)        (CHECK_BIT(x, 7))
#define GET_MONTH_A(x)         (CHECK_BIT(x, 6))
#define GET_MONTH_B(x)         (CHECK_BIT(x, 5))
#define GET_MONTH_DT(x)        (x & 0x1f)

#define GET_PUMP1_EN(x)        (CHECK_BIT(x, 7))
#define GET_PUMP2_EN(x)        (CHECK_BIT(x, 6))
#define GET_PUMP1_RELAY(x)     ((x & 0x38) >> 3)
#define GET_PUMP2_RELAY(x)     (x & 0x07)

/// sprinkler set values

#define SET_DT_EN(x, b)        x = b ? SET_BIT(x, 31) : CLEAR_BIT(x, 31)
#define SET_DT_HOUR(x, h, b)   x = b ? SET_BIT(x, (h + 7)) : CLEAR_BIT(x, (h + 7))
#define SET_DT_DAY(x, d, b)    x = b ? SET_BIT(x, d) : CLEAR_BIT(x, d)

#define SET_RELAY_EN(x,b)      x = b ? SET_BIT(x, 7) : CLEAR_BIT(x, 7)
#define SET_RELAY_PUMP(x, b)   x = b ? SET_BIT(x, 6) : CLEAR_BIT(x, 6)
#define SET_RELAY_MIN(x, v)    x = (x & 0xc0) | (v)

#define SET_MONTH_EN(x, b)     x = b ? SET_BIT(x, 7) : CLEAR_BIT(x, 7)
#define SET_MONTH_A(x, b)      x = b ? SET_BIT(x, 6) : CLEAR_BIT(x, 6)
#define SET_MONTH_B(x, b)      x = b ? SET_BIT(x, 5) : CLEAR_BIT(x, 5)
#define SET_MONTH_DT(x, v)     x = (x & 0xe0) | (v)

#define SET_PUMP1_EN(x, b)     x = b ? SET_BIT(x, 7) : CLEAR_BIT(x, 7)
#define SET_PUMP2_EN(x, b)     x = b ? SET_BIT(x, 6) : CLEAR_BIT(x, 6)
#define SET_PUMP1_RELAY(x, v)  x = (x & 0xc7) | (v << 3)
#define SET_PUMP2_RELAY(x, v)  x = (x & 0xf8) | (v)

/// sprinkler data types

typedef enum SPRINKLER_ERROR {
	SPR_OK,
	SPR_FAIL
} spr_err_t;

typedef struct sprinkler_s {
	 uint8_t pump;              // EeRRRrrr E:enabled pump1; e: enabled pump2; R: relay pump1; r relay pump2
    uint32_t date_time[MAX_DT]; // EHHHHHHHHHHHHHHHHHHHHHHHHDDDDDDD E:enabled, H: 23-0, D:0=Mon-6=Sun
     uint8_t relay[8];          // EPMMMMMM E:enabled; P-> 0:pump1, 1:pump2; M: minutes
    uint32_t pause[8];          // pause to next relay (seconds)
     uint8_t month[12];         // EABDDDDD E: enabled; A:?; B:?; D: date_time
    uint32_t ms_before_pump;    // time enable pump before valve (milliseconds)
     uint8_t gpio_relay[8];     // gpio of relay
} sprinkler_t;

#endif /* SPRINKLER_DATA_TYPES_H_ */
