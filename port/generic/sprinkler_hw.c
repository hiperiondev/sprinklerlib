/**
 * @file sprinkler_hw.c
 * @brief Sprinkler hardware functions
 * @copyright 2026 Emiliano Augusto Gonzalez (hiperiondev). This project is released under MIT license. Contact: egonzalez.hiperion@gmail.com
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
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>

#include "sprinkler_data_types.h"
#include "sprinkler_hw.h"

// Simulated relay states (0: off, 1: on)
static uint8_t relay_states[32] = { 0 };

spr_err_t sprinkler_wait_ms(uint32_t ms) {
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    select(0, NULL, NULL, NULL, &tv);
    return SPR_OK;
}

spr_err_t sprinkler_wait_seconds(uint32_t s) {
    sleep(s);
    return SPR_OK;
}

spr_err_t sprinkler_start_relay(uint8_t relay) {
    if (relay >= 32) {
        return SPR_FAIL;
    }
    if (relay_states[relay] == 0) {
        relay_states[relay] = 1;
        printf("Starting relay %u\n", relay);
    }
    return SPR_OK;
}

spr_err_t sprinkler_stop_relay(uint8_t relay) {
    if (relay >= 32) {
        return SPR_FAIL;
    }
    if (relay_states[relay] == 1) {
        relay_states[relay] = 0;
        printf("Stopping relay %u\n", relay);
    }
    return SPR_OK;
}

spr_err_t sprinkler_persitence_get(sprinkler_t *spr) {
    FILE *fp = fopen("sprinkler.dat", "rb");
    if (fp == NULL) {
        memset(spr, 0, sizeof(sprinkler_t));
        return SPR_FAIL;
    }
    size_t read_size = fread(spr, sizeof(sprinkler_t), 1, fp);
    fclose(fp);
    if (read_size != 1) {
        memset(spr, 0, sizeof(sprinkler_t));
        return SPR_FAIL;
    }
    return SPR_OK;
}

spr_err_t sprinkler_persitence_put(sprinkler_t *spr) {
    FILE *fp = fopen("sprinkler.dat", "wb");
    if (fp == NULL) {
        return SPR_FAIL;
    }
    size_t write_size = fwrite(spr, sizeof(sprinkler_t), 1, fp);
    fclose(fp);
    if (write_size != 1) {
        return SPR_FAIL;
    }
    return SPR_OK;
}
