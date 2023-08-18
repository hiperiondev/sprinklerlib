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

/// bitwise utils

#define SET_BIT(x,pos)    (x | (1U << pos))
#define CLEAR_BIT(x,pos)  (x & (~(1U << pos)))
#define CHECK_BIT(x,pos)  (x & (1UL << pos))
#define SETMASK(b,p)      (((2UL ^ b) - 1) << p)
#define UNSETMASK(b,p)    (~SETMASK(b,p))

/// sprinkler get values

#define GET_DT_EN(x)            (CHECK_BIT(x, 31))
#define GET_DT_HOURS(x)         ((x & SETMASK(24, 7)) >> 7)
#define GET_DT_HOUR(x,h)        (CHECK_BIT(GET_DT_HOURS(x), h))
#define GET_DT_DAYS(x)          (x & SETMASK(7,0))
#define GET_DT_DAY(x,d)         (CHECK_BIT(GET_DT_DAYS(x), d))

#define GET_RELAY_EN(x)         (CHECK_BIT(x, 15))
#define GET_RELAY_PUMP(x)       ((x & SETMASK(3, 12)) >> 12)
#define GET_RELAY_SEC(x)        (x & SETMASK(12, 0))

#define GET_MONTH_EN(x)         (CHECK_BIT(x, 7))
#define GET_MONTH_A(x)          (CHECK_BIT(x, 6))
#define GET_MONTH_B(x)          (CHECK_BIT(x, 5))
#define GET_MONTH_DT(x)         (x & SETMASK(5, 0))

#define GET_PUMP_EN(x,p)        (CHECK_BIT(x, (p + 25)))
#define GET_PUMP_RELAY(x,p)     (x & SETMASK(5, (p * 5))

/// sprinkler set values

#define SET_DT_EN(x,b)          x = b ? SET_BIT(x, 31) : CLEAR_BIT(x, 31)
#define SET_DT_HOUR(x,h,b)      x = b ? SET_BIT(x, (h + 7)) : CLEAR_BIT(x, (h + 7))
#define SET_DT_DAY(x,d,b)       x = b ? SET_BIT(x, d) : CLEAR_BIT(x, d)
#define SET_DT_QUEUE(x,q,b)     x = b ? SET_BIT(x, q) : CLEAR_BIT(x, q)

#define SET_RELAY_EN(x,b)       x = b ? SET_BIT(x, 15) : CLEAR_BIT(x, 15)
#define SET_RELAY_PUMP(x,v)     x = ((x & UNSETMASK(3, 12)) | (v << 12))
#define SET_RELAY_SEC(x,v)      x = (x & UNSETMASK(12, 0)) | (v)

#define SET_MONTH_EN(x,b)       x = b ? SET_BIT(x, 7) : CLEAR_BIT(x, 7)
#define SET_MONTH_A(x,b)        x = b ? SET_BIT(x, 6) : CLEAR_BIT(x, 6)
#define SET_MONTH_B(x,b)        x = b ? SET_BIT(x, 5) : CLEAR_BIT(x, 5)
#define SET_MONTH_DT(x,v)       x = (x & UNSETMASK(5, 0)) | (v)

#define SET_PUMP_EN(x,p,b)      x = b ? SET_BIT(x, (p + 25)) : CLEAR_BIT(x, (p + 25))
#define SET_PUMP_RELAY(x,p,v)   x = ((x & UNSETMASK(5, (p * 5))) | (v << (p * 5)))

#define SET_QUEUE(x,r,b)        x = (b ? SET_BIT(x, r) : CLEAR_BIT(x, r))
#define SET_QUEUE_AUTOADV(x,b)  x = (b ? SET_BIT(x, 31) : CLEAR_BIT(x, 31))
#define SET_QUEUE_RSEC(x,v)     x = (x & UNSETMASK(31, 0)) | (v)

/// sprinkler data types

typedef enum SPRINKLER_ERROR {
	SPR_OK   = 0x00,
	SPR_FAIL = 0xff
} spr_err_t;

typedef struct sprinkler_s {
    uint32_t pump;                    // xxABCDEaaaaabbbbbcccccdddddeeeee ABCD:enabled pump5,4,3,2,1; abcde:relay pump5,4,3,2,1 x:?
    uint32_t date_time[32];           // EHHHHHHHHHHHHHHHHHHHHHHHHDDDDDDD E:enabled, H:23-0, D:0=Mon-6=Sun
    uint32_t date_time_queue[32];     // qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq q: enabled queue
    uint16_t relay[32];               // EPPPMMMMMMMMMMMM E:enabled; P-> 0:pump1,2,3,4,5; M: on seconds
    uint32_t relay_overlap;           // current relay and next relay open simultaneously for the duration specified (milliseconds)
     uint8_t month[12];               // EABDDDDD E:enabled; A:?; B:?; D:date_time
    uint32_t pump_delay_ms;           // delay to start pump (milliseconds)
    uint32_t queue[32];               // rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr r:relay
     uint8_t queue_repeat[32];        // times queue cycle repeat
    uint16_t queue_relay_sec[32][32]; // [queue] [relay] if not 0 override relay on seconds
    uint32_t queue_pause[32];         // asssssssssssssssssssssssssssssss a:autoadvance; s:pause to next relay (seconds)
     uint8_t gpio_relay[32];          // gpio relay
} sprinkler_t;

#endif /* SPRINKLER_DATA_TYPES_H_ */
