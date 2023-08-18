/**
 * @file sprinkler_hw.c
 * @brief Sprinkler hardware functions
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

#include <stdint.h>
#include <stdbool.h>

#include "sprinkler_data_types.h"
#include "sprinkler_hw.h"

spr_err_t sprinkler_wait_ms(uint32_t ms) {
    return SPR_OK;
}

spr_err_t sprinkler_wait_seconds(uint32_t s) {
    return SPR_OK;
}

spr_err_t sprinkler_start_relay(uint8_t relay) {
    return SPR_OK;
}

spr_err_t sprinkler_stop_relay(uint8_t relay) {
    return SPR_OK;
}

spr_err_t sprinkler_persitence_get(sprinkler_t **spr) {
    return SPR_OK;
}

spr_err_t sprinkler_persitence_put(sprinkler_t *spr) {
    return SPR_OK;
}
