/**
 * @file sprinkler_fn.c
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

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "sprinkler_data_types.h"
#include "sprinkler_fn.h"

bool sprinkler_config_changed;

/// system dependent functions ///
extern void sprinkler_wait_ms(uint32_t ms);
extern void sprinkler_wait_seconds(uint32_t s);
extern void sprinkler_start_relay(uint8_t relay);
extern void sprinkler_stop_relay(uint8_t relay);
extern void sprinkler_start_pump(bool pump);
extern void sprinkler_stop_pump(bool pump);
extern void sprinkler_persitence_get(sprinkler_t **spr);
extern void sprinkler_persitence_put(sprinkler_t **spr);

//////////////////////////////////////////////////////////////

void sprinkler_init(sprinkler_t **spr) {
    *spr = malloc(sizeof(struct sprinkler_s));
    sprinkler_persitence_get(spr);
    sprinkler_config_changed = false;
}

void sprinkler_deinit(sprinkler_t **spr) {
    if (sprinkler_config_changed)
        sprinkler_persitence_put(spr);
    free(*spr);
}

spr_err_t sprinkler_start_cicle(sprinkler_t *spr) {
    for (uint8_t relay = 0; relay < 7; relay++) {
        if (GET_PUMP1_RELAY(spr->pump) == relay || GET_PUMP2_RELAY(spr->pump) == relay) // relay is pump
            continue;

        sprinkler_start_relay(relay);
        sprinkler_wait_ms(spr->ms_before_pump);
        sprinkler_start_pump(GET_RELAY_PUMP(relay));
        sprinkler_wait_seconds(GET_RELAY_MIN(spr->relay[relay]) * 60);

        sprinkler_stop_pump(GET_RELAY_PUMP(relay));
        sprinkler_wait_ms(spr->ms_before_pump);
        sprinkler_stop_relay(relay);
    }

    return SPR_OK;
}

bool sprinkler_is_start_time(sprinkler_t *spr) {
    uint32_t start_time = 0;
    struct tm timeinfo;
    time_t now;

    time(&now);
    localtime_r(&now, &timeinfo);

    SET_DT_EN(start_time, true);
    SET_DT_HOUR(start_time, timeinfo.tm_hour, true);
    SET_DT_DAY(start_time, timeinfo.tm_wday, true);

    return (GET_MONTH_EN(spr->month[timeinfo.tm_mon]) && ((spr->date_time[GET_MONTH_DT(spr->month[timeinfo.tm_mon])] & start_time) == start_time));
}

//////////////////////////////////////////////////////////////

spr_err_t sprinkler_setdt_day(sprinkler_t **spr, uint8_t id, uint8_t day, bool en) {
    if (id > MAX_DT - 1 || day > 6)
        return SPR_FAIL;

    SET_DT_DAY((*spr)->date_time[id], day, en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_setdt_hour(sprinkler_t **spr, uint8_t id, uint8_t hour, bool en) {
    if (id > MAX_DT - 1 || hour > 23)
        return SPR_FAIL;

    SET_DT_HOUR((*spr)->date_time[id], hour, en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_setdt_en(sprinkler_t **spr, uint8_t id, bool en) {
    if (id > MAX_DT - 1)
        return SPR_FAIL;

    SET_DT_EN((*spr)->date_time[id], en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

/////////////////////

spr_err_t sprinkler_setmonth_en(sprinkler_t **spr, uint8_t month, bool en) {
    if (month > 11)
        return SPR_FAIL;

    SET_MONTH_EN((*spr)->month[month], en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_setmonth_a(sprinkler_t **spr, uint8_t month, uint8_t a) {
    if (month > 11)
        return SPR_FAIL;

    SET_MONTH_A((*spr)->month[month], a);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_setmonth_b(sprinkler_t **spr, uint8_t month, uint8_t b) {
    if (month > 11)
        return SPR_FAIL;

    SET_MONTH_B((*spr)->month[month], b);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_setmonth_dt(sprinkler_t **spr, uint8_t month, uint8_t dt) {
    if (month > 11 || dt > MAX_DT - 1)
        return SPR_FAIL;

    SET_MONTH_DT((*spr)->month[month], dt);

    sprinkler_config_changed = true;
    return SPR_OK;
}

/////////////////////

spr_err_t sprinkler_setrelay_en(sprinkler_t **spr, uint8_t relay, bool en) {
    if (relay > 7)
        return SPR_FAIL;

    SET_RELAY_EN((*spr)->relay[relay], en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_setrelay_pump(sprinkler_t **spr, uint8_t relay, bool pump) {
    if (relay > 7)
        return SPR_FAIL;

    SET_RELAY_PUMP((*spr)->relay[relay], pump);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_setrelay_min(sprinkler_t **spr, uint8_t relay, uint8_t min) {
    if (relay > 7 || min > 63)
        return SPR_FAIL;

    SET_RELAY_MIN((*spr)->relay[relay], min);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_setrelay_gpio(sprinkler_t **spr, uint8_t relay, uint8_t gpio) {
    if (relay > 7)
        return SPR_FAIL;

    (*spr)->gpio_relay[relay] = gpio;

    sprinkler_config_changed = true;
    return SPR_OK;
}

/////////////////////

spr_err_t sprinkler_setpause(sprinkler_t **spr, uint8_t relay, uint32_t seconds) {
    if (relay > 7)
        return SPR_FAIL;

    (*spr)->pause[relay] = seconds;

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_setbeforepump(sprinkler_t **spr, uint32_t ms) {
    (*spr)->ms_before_pump = ms;

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_pump_en(sprinkler_t **spr, bool pump, bool en) {
    if (pump)
        SET_PUMP1_EN((*spr)->pump, en);
    else
        SET_PUMP2_EN((*spr)->pump, en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_pump_relay(sprinkler_t **spr, bool pump, uint8_t relay) {
    if (relay > 7)
        return SPR_FAIL;

    if (pump)
        SET_PUMP1_RELAY((*spr)->pump, relay);
    else
        SET_PUMP2_RELAY((*spr)->pump, relay);

    sprinkler_config_changed = true;
    return SPR_OK;
}
