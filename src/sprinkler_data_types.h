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

#define ALLOW_MIN_PRECISION // allow minutes precision

/// sprinkler data types

typedef enum SPRINKLER_ERROR {
	SPR_OK   = 0x00,
	SPR_FAIL = 0xff
} spr_err_t;

typedef struct sprinkler_s {
    uint32_t pump;                    // xxABCDEaaaaabbbbbcccccdddddeeeee ABCD:enabled pump5,4,3,2,1; abcde:relay pump5,4,3,2,1 x:?
    uint32_t date_time[32];           // EHHHHHHHHHHHHHHHHHHHHHHHHDDDDDDD E:enabled, H:23-0, D:0=Mon-6=Sun
#ifdef ALLOW_MIN_PRECISION
     uint8_t date_time_min[32][24];   // minutes of every hour
#endif
    uint32_t date_time_queue[32];     // qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq q: enabled queue
    uint16_t relay[32];               // EPPPMMMMMMMMMMMM E:enabled; P-> 0:pump1,2,3,4,5; M: on seconds
    uint32_t relay_overlap_ms;        // current relay and next relay open simultaneously for the duration specified (milliseconds)
     uint8_t month[12];               // EABDDDDD E:enabled; A:?; B:?; D:date_time
    uint32_t pump_delay_ms;           // delay to start pump (milliseconds)
    uint32_t queue[32];               // rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr r:relay
     uint8_t queue_repeat[32];        // times queue cycle repeat
    uint16_t queue_relay_sec[32][32]; // [queue] [relay] if not 0 override relay on seconds
    uint32_t queue_pause[32];         // asssssssssssssssssssssssssssssss a:autoadvance; s:pause to next relay (seconds)
     uint8_t gpio_relay[32];          // gpio relay
} sprinkler_t;

#endif /* SPRINKLER_DATA_TYPES_H_ */
