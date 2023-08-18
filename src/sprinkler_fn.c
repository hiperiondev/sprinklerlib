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
#include "sprinkler_hw.h"
#include "sprinkler_fn.h"

bool sprinkler_config_changed;
uint8_t queue_running;
bool queue_paused;
uint8_t relay_running;

//////////////////////////////////////////////////////////////

void sprinkler_init(sprinkler_t **spr) {
    *spr = malloc(sizeof(struct sprinkler_s));
    sprinkler_persitence_get(spr);
    sprinkler_config_changed = false;
    queue_paused = false;
    queue_running = SPR_FAIL;
    relay_running = SPR_FAIL;
}

void sprinkler_deinit(sprinkler_t **spr) {
    if (sprinkler_config_changed)
        sprinkler_persitence_put(*spr);
    free(*spr);
}

spr_err_t sprinkler_start_cicle(sprinkler_t *spr) {

    return SPR_OK;
}

spr_err_t sprinkler_queue_next(sprinkler_t *spr) {

    return SPR_OK;
}

spr_err_t sprinkler_queue_previous(sprinkler_t *spr) {

    return SPR_OK;
}

spr_err_t sprinkler_queue_pause(sprinkler_t *spr) {

    return SPR_OK;
}

spr_err_t sprinkler_queue_resume(sprinkler_t *spr) {

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

spr_err_t sprinkler_set_dt_day(sprinkler_t **spr, uint8_t id, uint8_t day, bool en) {
    if (id > 31 || day > 6)
        return SPR_FAIL;

    SET_DT_DAY((*spr)->date_time[id], day, en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_dt_hour(sprinkler_t **spr, uint8_t id, uint8_t hour, bool en) {
    if (id > 31 || hour > 23)
        return SPR_FAIL;

    SET_DT_HOUR((*spr)->date_time[id], hour, en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_dt_en(sprinkler_t **spr, uint8_t id, bool en) {
    if (id > 31)
        return SPR_FAIL;

    SET_DT_EN((*spr)->date_time[id], en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_dt_queue(sprinkler_t **spr, uint8_t id, uint8_t queue, bool en) {
    if (id > 31 || queue > 31)
        return SPR_FAIL;

    SET_DT_QUEUE((*spr)->date_time_queue[id], queue, en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

/////////////////////

spr_err_t sprinkler_set_month_en(sprinkler_t **spr, uint8_t month, bool en) {
    if (month > 11)
        return SPR_FAIL;

    SET_MONTH_EN((*spr)->month[month], en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_month_a(sprinkler_t **spr, uint8_t month, uint8_t a) {
    if (month > 11)
        return SPR_FAIL;

    SET_MONTH_A((*spr)->month[month], a);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_month_b(sprinkler_t **spr, uint8_t month, uint8_t b) {
    if (month > 11)
        return SPR_FAIL;

    SET_MONTH_B((*spr)->month[month], b);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_month_dt(sprinkler_t **spr, uint8_t month, uint8_t dt) {
    if (month > 11 || dt > 31)
        return SPR_FAIL;

    SET_MONTH_DT((*spr)->month[month], dt);

    sprinkler_config_changed = true;
    return SPR_OK;
}

/////////////////////

spr_err_t sprinkler_set_relay_en(sprinkler_t **spr, uint8_t relay, bool en) {
    if (relay > 31)
        return SPR_FAIL;

    SET_RELAY_EN((*spr)->relay[relay], en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_relay_pump(sprinkler_t **spr, uint8_t relay, uint8_t pump) {
    if (relay > 31 || pump > 4)
        return SPR_FAIL;

    SET_RELAY_PUMP((*spr)->relay[relay], pump);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_relay_sec(sprinkler_t **spr, uint8_t relay, uint8_t min) {
    if (relay > 31 || min > 63)
        return SPR_FAIL;

    SET_RELAY_SEC((*spr)->relay[relay], min);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_relay_overlap(sprinkler_t **spr, uint8_t relay, uint32_t ms) {
    if (relay > 31)
        return SPR_FAIL;

    (*spr)->relay_overlap = ms;

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_relay_gpio(sprinkler_t **spr, uint8_t relay, uint8_t gpio) {
    if (relay > 31)
        return SPR_FAIL;

    (*spr)->gpio_relay[relay] = gpio;

    sprinkler_config_changed = true;
    return SPR_OK;
}

/////////////////////

spr_err_t sprinkler_set_queue(sprinkler_t **spr, uint8_t queue, uint32_t relay, bool en) {
    if (queue > 31 || relay > 31)
        return SPR_FAIL;

    SET_QUEUE((*spr)->queue[queue], relay, en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_queue_pause(sprinkler_t **spr, uint8_t queue, uint32_t seconds) {
    if (queue > 31 || seconds > (0x2UL ^ 31))
        return SPR_FAIL;

    SET_QUEUE_RSEC((*spr)->queue_pause[queue], seconds);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_queue_autoadv(sprinkler_t **spr, uint8_t queue, bool en) {
    if (queue > 31)
        return SPR_FAIL;

    SET_QUEUE_AUTOADV((*spr)->queue[queue], en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_queue_relay_sec(sprinkler_t **spr, uint8_t queue, uint8_t relay, uint16_t seconds) {
    if (queue > 31 || relay > 31)
        return SPR_FAIL;

    (*spr)->queue_relay_sec[queue][relay] = seconds;

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_queue_repeat(sprinkler_t **spr, uint8_t queue, uint8_t times) {
    if (queue > 31)
        return SPR_FAIL;

    (*spr)->queue_repeat[queue] = times;

    sprinkler_config_changed = true;
    return SPR_OK;
}

/////////////////////

spr_err_t sprinkler_set_pump_delay(sprinkler_t **spr, uint32_t ms) {
    (*spr)->pump_delay_ms = ms;

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_pump_en(sprinkler_t **spr, uint8_t pump, bool en) {
    if (pump > 4)
        return SPR_FAIL;

    SET_PUMP_EN((*spr)->pump, pump, en);

    sprinkler_config_changed = true;
    return SPR_OK;
}

spr_err_t sprinkler_set_pump_relay(sprinkler_t **spr, uint8_t pump, uint8_t relay) {
    if (pump > 4 || relay > 31)
        return SPR_FAIL;

    SET_PUMP_RELAY((*spr)->pump, pump, relay);

    sprinkler_config_changed = true;
    return SPR_OK;
}

/////////////////////

bool sprinkler_is_queue_paused(void) {
    return queue_paused;
}

spr_err_t sprinkler_is_queue_running(void) {
    return queue_running;
}

spr_err_t sprinkler_is_relay_running(void) {
    return relay_running;
}

