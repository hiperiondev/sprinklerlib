/**
 * @file sprinkler_fn.c
 * @brief Sprinkler functions
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

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "sprinkler_data_types.h"
#include "sprinkler_hw.h"
#include "sprinkler_fn.h"

//////////////////////////////////////////////////////////////

spr_err_t sprinkler_get_time(struct tm *timeinfo, uint32_t *unix_seconds) {
    time_t now_raw = time(NULL);
    if (timeinfo) {
        struct tm *tmp = localtime(&now_raw);
        if (tmp == NULL) {
            return SPR_FAIL;
        }
        *timeinfo = *tmp;
    }
    if (unix_seconds) {
        *unix_seconds = (uint32_t) now_raw;
    }
    return SPR_OK;
}

void sprinkler_init(sprinkler_t *spr) {
    memset(spr, 0, sizeof(*spr));

    if (sprinkler_persitence_get(spr) != SPR_OK) {
        memset(spr, 0, sizeof(sprinkler_t));
    }
}

void sprinkler_deinit(sprinkler_t *spr) {
    if (spr->sprinkler_config_changed) {
        int retries = 3;
        while (retries-- > 0) {
            if (sprinkler_persitence_put(spr) == SPR_OK) {
                break;
            }
        }
    }
}

spr_err_t sprinkler_queue_next(sprinkler_t *spr) {
    uint32_t running = spr->queue_running;
    while (running) {
        uint8_t q = 0;
        while (!(running & (1UL << q)))
            q++;
        running &= ~(1UL << q);
        if (spr->current_relay_idx[q] < 31)
            spr->current_relay_idx[q]++;
    }

    return SPR_OK;
}

spr_err_t sprinkler_queue_previous(sprinkler_t *spr) {
    uint32_t running = spr->queue_running;
    while (running) {
        uint8_t q = 0;
        while (!(running & (1UL << q)))
            q++;
        running &= ~(1UL << q);
        if (spr->current_relay_idx[q] > 0)
            spr->current_relay_idx[q]--;
    }

    return SPR_OK;
}

spr_err_t sprinkler_queue_pause(sprinkler_t *spr) {
    for (uint8_t q = 0; q < 32; q++) {
        spr->queue_paused[q] = true;
    }

    return SPR_OK;
}

spr_err_t sprinkler_queue_resume(sprinkler_t *spr) {
    for (uint8_t q = 0; q < 32; q++) {
        spr->queue_paused[q] = false;
    }

    return SPR_OK;
}

bool sprinkler_is_queue_paused_id(sprinkler_t *spr, uint8_t q) {
    if (q > 31) {
        return false;
    }

    return spr->queue_paused[q];
}

spr_err_t sprinkler_queue_pause_id(sprinkler_t *spr, uint8_t q) {
    if (q > 31) {
        return SPR_FAIL;
    }
    spr->queue_paused[q] = true;

    return SPR_OK;
}

spr_err_t sprinkler_queue_resume_id(sprinkler_t *spr, uint8_t q) {
    if (q > 31) {
        return SPR_FAIL;
    }
    spr->queue_paused[q] = false;

    return SPR_OK;
}

spr_err_t sprinkler_queue_next_id(sprinkler_t *spr, uint8_t q) {
    if (q > 31)
        return SPR_FAIL;
    if (spr->current_relay_idx[q] < 31)
        spr->current_relay_idx[q]++;

    return SPR_OK;
}

spr_err_t sprinkler_queue_previous_id(sprinkler_t *spr, uint8_t q) {
    if (q > 31)
        return SPR_FAIL;
    if (spr->current_relay_idx[q] > 0)
        spr->current_relay_idx[q]--;

    return SPR_OK;
}

uint32_t sprinkler_get_paused_queues(sprinkler_t *spr) {
    uint32_t mask = 0;
    for (uint8_t i = 0; i < 32; i++) {
        if (spr->queue_paused[i]) {
            mask |= (1UL << i);
        }
    }

    return mask;
}

bool sprinkler_is_start_time(sprinkler_t *spr) {
    uint32_t start_time = 0;
    struct tm timeinfo;

    if (sprinkler_get_time(&timeinfo, NULL) != SPR_OK) {
        return false;
    }

    uint8_t day = (timeinfo.tm_wday == 0 ? 6 : timeinfo.tm_wday - 1);
    SET_DT_EN(start_time, true);
    SET_DT_HOUR(start_time, timeinfo.tm_hour, true);
    SET_DT_DAY(start_time, day, true);  // tm_wday: 0=Sun ... 6=Sat, but your code uses 0=Mon? Wait!

    return ((GET_MONTH_EN(spr->month[timeinfo.tm_mon])) && ((spr->date_time[GET_MONTH_DT(spr->month[timeinfo.tm_mon])] & start_time) == start_time)
#ifdef ALLOW_MIN_PRECISION
            && timeinfo.tm_min == spr->date_time_min[GET_MONTH_DT(spr->month[timeinfo.tm_mon])][timeinfo.tm_hour]
#endif
    );
}

//////////////////////////////////////////////////////////////

spr_err_t sprinkler_set_dt_day(sprinkler_t *spr, uint8_t id, uint8_t day, bool en) {
    if (id > 31 || day > 6)
        return SPR_FAIL;

    SET_DT_DAY(spr->date_time[id], day, en);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_dt_hour(sprinkler_t *spr, uint8_t id, uint8_t hour, bool en) {
    if (id > 31 || hour > 23)
        return SPR_FAIL;

    SET_DT_HOUR(spr->date_time[id], hour, en);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_dt_min(sprinkler_t *spr, uint8_t id, uint8_t hour, uint8_t min) {
    if (id > 31 || hour > 23 || min > 59) {
        return SPR_FAIL;
    }

    spr->date_time_min[id][hour] = min;
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_dt_en(sprinkler_t *spr, uint8_t id, bool en) {
    if (id > 31)
        return SPR_FAIL;

    SET_DT_EN(spr->date_time[id], en);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_dt_queue(sprinkler_t *spr, uint8_t id, uint8_t queue, bool en) {
    if (id > 31)
        return SPR_FAIL;

    if (queue >= 32)
        return SPR_FAIL;

    SET_DT_QUEUE(spr->date_time_queue[id], queue, en);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

/////////////////////

spr_err_t sprinkler_set_month_en(sprinkler_t *spr, uint8_t month, bool en) {
    if (month > 11)
        return SPR_FAIL;

    SET_MONTH_EN(spr->month[month], en);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_month_a(sprinkler_t *spr, uint8_t month, uint8_t a) {
    if (month > 11)
        return SPR_FAIL;

    SET_MONTH_A(spr->month[month], a);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_month_b(sprinkler_t *spr, uint8_t month, uint8_t b) {
    if (month > 11)
        return SPR_FAIL;

    SET_MONTH_B(spr->month[month], b);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_month_dt(sprinkler_t *spr, uint8_t month, uint8_t dt) {
    if (month > 11) {
        return SPR_FAIL;
    }

    if (dt > 31) {
        return SPR_FAIL;
    }

    SET_MONTH_DT(spr->month[month], dt);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

/////////////////////

spr_err_t sprinkler_set_relay_en(sprinkler_t *spr, uint8_t relay, bool en) {
    if (relay > 31)
        return SPR_FAIL;

    SET_RELAY_EN(spr->relay[relay], en);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_relay_pump(sprinkler_t *spr, uint8_t relay, uint8_t pump) {
    if (relay > 31 || pump > 4)
        return SPR_FAIL;

    SET_RELAY_PUMP(spr->relay[relay], pump);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_relay_min(sprinkler_t *spr, uint8_t relay, uint16_t min) {
    if (relay > 31)
        return SPR_FAIL;
    if (min > 4095)
        return SPR_FAIL;

    SET_RELAY_MIN(spr->relay[relay], min);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_relay_overlap(sprinkler_t *spr, uint8_t relay, uint32_t ms) {
    if (relay > 31)
        return SPR_FAIL;

    spr->relay_overlap_ms[relay] = ms;
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_relay_gpio(sprinkler_t *spr, uint8_t relay, uint8_t gpio) {
    if (relay > 31)
        return SPR_FAIL;

    spr->gpio_relay[relay] = gpio;
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

/////////////////////

spr_err_t sprinkler_set_queue(sprinkler_t *spr, uint8_t queue, uint32_t relay, bool en) {
    if (queue >= 32 || relay >= 32) {
        return SPR_FAIL;
    }

    SET_QUEUE(spr->queue[queue], (uint8_t )relay, en);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_queue_pause(sprinkler_t *spr, uint8_t queue, uint32_t seconds) {
    if (queue >= 32) {
        return SPR_FAIL;
    }

    uint32_t pause_sec = seconds & 0x7FFFFFFFUL;
    SET_QUEUE_RSEC(spr->queue_pause[queue], pause_sec);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_queue_autoadv(sprinkler_t *spr, uint8_t queue, bool en) {
    if (queue >= 32) {
        return SPR_FAIL;
    }

    SET_QUEUE_AUTOADV(spr->queue_pause[queue], en);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_queue_relay_sec(sprinkler_t *spr, uint8_t queue, uint8_t relay, uint16_t seconds) {
    if (queue > 31 || relay > 31)
        return SPR_FAIL;

    spr->queue_relay_sec[queue][relay] = seconds;
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_queue_repeat(sprinkler_t *spr, uint8_t queue, uint8_t times) {
    if (queue > 31)
        return SPR_FAIL;

    if (times > 255)
        return SPR_FAIL;

    spr->queue_repeat[queue] = times;
    spr->sprinkler_config_changed = true;
    return SPR_OK;
}

/////////////////////

spr_err_t sprinkler_set_pump_delay(sprinkler_t *spr, uint32_t ms) {
    spr->pump_delay_ms = ms;
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_pump_en(sprinkler_t *spr, uint8_t pump, bool en) {
    if (pump > 4)
        return SPR_FAIL;

    SET_PUMP_EN(spr->pump, pump, en);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_pump_relay(sprinkler_t *spr, uint8_t pump, uint8_t relay) {
    if (pump > 4 || relay > 31)
        return SPR_FAIL;

    SET_PUMP_RELAY(spr->pump, pump, relay);
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

spr_err_t sprinkler_set_pause(sprinkler_t *spr, uint8_t relay, uint32_t seconds) {
    if (relay > 31)
        return SPR_FAIL;

    if (seconds > UINT16_MAX)
        return SPR_ERR_RANGE;

    spr->queue_relay_sec[31][relay] = seconds;
    spr->sprinkler_config_changed = true;

    return SPR_OK;
}

/////////////////////

bool sprinkler_is_queue_paused(sprinkler_t *spr) {
    for (uint8_t i = 0; i < 32; i++) {
        if (spr->queue_paused[i])
            return true;
    }

    return false;
}

uint32_t sprinkler_is_queue_running(sprinkler_t *spr) {
    return spr->queue_running;
}

uint32_t sprinkler_is_relay_running(sprinkler_t *spr) {
    return spr->relay_running;
}

spr_err_t start_pump_if_needed(sprinkler_t *spr, uint8_t pump, uint32_t now) {
    if (pump >= 5 || !GET_PUMP_EN(spr->pump, pump)) {
        return SPR_OK;  // No pump needed or invalid
    }
    if ((spr->active_pumps & (1U << pump)) != 0) {
        return SPR_OK;  // Already active
    }
    if (spr->pump_start_times[pump] != 0) {
        if (now >= spr->pump_start_times[pump]) {
            uint8_t pump_relay = GET_PUMP_RELAY(spr->pump, pump);
            sprinkler_start_relay(pump_relay);
            spr->active_pumps |= (1U << pump);
            spr->pump_start_times[pump] = 0;
            return SPR_OK;
        } else {
            return SPR_FAIL;  // Still waiting for delay
        }
    }
    uint32_t delay_ms = spr->pump_delay_ms;
    if (delay_ms == 0) {
        uint8_t pump_relay = GET_PUMP_RELAY(spr->pump, pump);
        sprinkler_start_relay(pump_relay);
        spr->active_pumps |= (1U << pump);
        return SPR_OK;
    } else {
        uint32_t delay_sec = (delay_ms + 999UL) / 1000UL;
        spr->pump_start_times[pump] = now + delay_sec;
        return SPR_FAIL;  // Delay initiated, not ready yet
    }
}

/////////////////////

spr_err_t sprinkler_main_loop(sprinkler_t *spr) {
    struct tm timeinfo;
    uint32_t now;

    if (sprinkler_get_time(&timeinfo, &now) != SPR_OK) {
        return SPR_FAIL;
    }

    bool is_start = sprinkler_is_start_time(spr);
#ifdef ALLOW_MIN_PRECISION
    static int last_minute = -1;
    int current_minute = timeinfo.tm_min;
    if (current_minute != last_minute && is_start) {
#else
    static int last_hour = -1;
    int current_hour = timeinfo.tm_hour;
    if (current_hour != last_hour && is_start) {
#endif
        uint8_t dt_id = GET_MONTH_DT(spr->month[timeinfo.tm_mon]);
        if (GET_DT_EN(spr->date_time[dt_id])) {
            for (uint8_t q = 0; q < 32; ++q) {
                if (CHECK_BIT(spr->date_time_queue[dt_id], q)) {
                    spr->queue_running |= (1UL << q);
                }
            }
        }
    }
#ifdef ALLOW_MIN_PRECISION
    last_minute = current_minute;
#else
    last_hour = current_hour;
#endif
    static uint32_t last_persist_time = 0;
    if (spr->sprinkler_config_changed && TIME_AFTER_OR_EQ(now, last_persist_time + TO_PERSISTENCE_SEC)) {
        if (sprinkler_persitence_put(spr) == SPR_OK) {
            spr->sprinkler_config_changed = false;
            last_persist_time = now;
        }
    }
    for (uint8_t p = 0; p < 5; p++) {
        if (spr->pump_start_times[p] != 0 && TIME_AFTER_OR_EQ(now, spr->pump_start_times[p])) {
            uint8_t pump_relay = GET_PUMP_RELAY(spr->pump, p);
            sprinkler_start_relay(pump_relay);
            spr->active_pumps |= (1U << p);
            spr->pump_start_times[p] = 0;
        }
    }
    if (spr->queue_running == 0) {
        spr->relay_running = 0;
        memset(spr->current_relay_idx, 0, sizeof(spr->current_relay_idx));
        memset(spr->queue_pause_end_times, 0, sizeof(spr->queue_pause_end_times));
        memset(spr->queue_paused, 0, sizeof(spr->queue_paused));
        memset(spr->repeat_count, 0, sizeof(spr->repeat_count));
        memset(spr->queue_relay_end_times, 0, sizeof(spr->queue_relay_end_times));
        if (spr->active_pumps != 0) {
            for (uint8_t p = 0; p < 5; p++) {
                if (spr->active_pumps & (1U << p)) {
                    uint8_t pump_relay = GET_PUMP_RELAY(spr->pump, p);
                    sprinkler_stop_relay(pump_relay);
                }
            }
            spr->active_pumps = 0;
        }
        return SPR_OK;
    }
    for (uint8_t current_queue = 0; current_queue < 32; current_queue++) {
        if (!(spr->queue_running & (1UL << current_queue)))
            continue;
        uint32_t queue_mask = spr->queue[current_queue];
        if (queue_mask == 0) {
            spr->queue_running &= ~(1UL << current_queue);
            spr->repeat_count[current_queue] = 0;
            continue;
        }
        uint8_t idx = spr->current_relay_idx[current_queue];
        while (idx < 32 && !(queue_mask & (1UL << idx)))
            idx++;
        if (idx >= 32) {
            if (spr->queue_repeat[current_queue] == 0 || ++spr->repeat_count[current_queue] > spr->queue_repeat[current_queue]) {
                spr->queue_running &= ~(1UL << current_queue);
                spr->repeat_count[current_queue] = 0;
            }
            spr->current_relay_idx[current_queue] = 0;
            continue;
        }
        spr->current_relay_idx[current_queue] = idx;
        uint8_t relay = idx;
        if (spr->queue_pause_end_times[current_queue] > 0 && TIME_BEFORE(now, spr->queue_pause_end_times[current_queue]))
            continue;

        if (spr->queue_pause_end_times[current_queue] > 0 && TIME_AFTER_OR_EQ(now, spr->queue_pause_end_times[current_queue]))
            spr->queue_pause_end_times[current_queue] = 0;

        if (spr->queue_paused[current_queue] && !CHECK_BIT(spr->queue_pause[current_queue], 31)) {
            continue;
        }
        if (!GET_RELAY_EN(spr->relay[relay])) {
            spr->current_relay_idx[current_queue]++;
            continue;
        }
        uint32_t duration_sec = (uint32_t) GET_RELAY_MIN(spr->relay[relay]) * 60UL;
        uint16_t override_sec = spr->queue_relay_sec[current_queue][relay];
        if (override_sec > 0)
            duration_sec = override_sec;
        if (duration_sec == 0) {
            spr->current_relay_idx[current_queue]++;
            continue;
        }
        if (spr->queue_relay_end_times[current_queue][relay] == 0) {
            uint8_t pump = GET_RELAY_PUMP(spr->relay[relay]);

            if (start_pump_if_needed(spr, pump, now) != SPR_OK) {
                continue;
            }

            spr->queue_relay_end_times[current_queue][relay] = now + duration_sec;
            if ((spr->relay_running & (1UL << relay)) == 0) {
                sprinkler_start_relay(spr->gpio_relay[relay]);
                spr->relay_running |= (1UL << relay);
            }
        }
        if (TIME_AFTER_OR_EQ(now, spr->queue_relay_end_times[current_queue][relay])) {
            bool relay_still_needed = false;

            for (uint8_t o = 0; o < 32; o++) {
                if (o != current_queue && (spr->queue_running & (1UL << o)) && spr->current_relay_idx[o] == relay
                        && spr->queue_relay_end_times[o][relay] > now) {
                    relay_still_needed = true;
                    break;
                }
            }
            if (!relay_still_needed) {
                sprinkler_stop_relay(spr->gpio_relay[relay]);
                spr->relay_running &= ~(1UL << relay);
            }
            uint8_t pump = GET_RELAY_PUMP(spr->relay[relay]);
            bool pump_still_needed = false;
            for (uint8_t o = 0; o < 32; o++) {
                if ((spr->relay_running & (1UL << o)) && GET_RELAY_PUMP(spr->relay[o]) == pump) {
                    pump_still_needed = true;
                    break;
                }
            }
            if (pump < 5 && !pump_still_needed && (spr->active_pumps & (1U << pump))) {
                uint8_t pump_relay = GET_PUMP_RELAY(spr->pump, pump);
                sprinkler_stop_relay(pump_relay);
                spr->active_pumps &= ~(1U << pump);
            }
            spr->queue_relay_end_times[current_queue][relay] = 0;
            uint16_t per_relay_pause = spr->queue_relay_sec[31][relay];
            uint32_t pause_sec = (per_relay_pause > 0) ? per_relay_pause : (spr->queue_pause[current_queue] & 0x7FFFFFFFUL);
            if (pause_sec > 0) {
                spr->queue_pause_end_times[current_queue] = now + pause_sec;
            }
            if (!CHECK_BIT(spr->queue_pause[current_queue], 31)) {
                spr->queue_paused[current_queue] = true;
            }
            spr->current_relay_idx[current_queue]++;
            idx = spr->current_relay_idx[current_queue];
            while (idx < 32 && !(queue_mask & (1UL << idx))) {
                idx++;
            }
            if (idx >= 32) {
                if (spr->queue_repeat[current_queue] == 0 || ++spr->repeat_count[current_queue] >= spr->queue_repeat[current_queue]) {
                    spr->queue_running &= ~(1UL << current_queue);
                    spr->repeat_count[current_queue] = 0;
                }
                spr->current_relay_idx[current_queue] = 0;
            }
        }
        if (spr->relay_running & (1UL << relay)) {
            uint32_t overlap_ms = spr->relay_overlap_ms[relay];
            if (overlap_ms > 0) {
                uint32_t overlap_sec = (overlap_ms + 999UL) / 1000UL;
                uint32_t intended_start = spr->queue_relay_end_times[current_queue][relay] - overlap_sec;
                if (TIME_AFTER_OR_EQ(now, intended_start)) {
                    uint8_t next_idx = relay + 1;

                    while (next_idx < 32 && !(queue_mask & (1UL << next_idx)))
                        next_idx++;
                    if (next_idx < 32 && GET_RELAY_EN(spr->relay[next_idx])) {
                        uint8_t next_relay = next_idx;
                        uint32_t next_duration_sec = (uint32_t) GET_RELAY_MIN(spr->relay[next_relay]) * 60UL;
                        uint16_t next_override = spr->queue_relay_sec[current_queue][next_relay];
                        if (next_override > 0)
                            next_duration_sec = next_override;
                        if (next_duration_sec > 0) {
                            uint8_t next_pump = GET_RELAY_PUMP(spr->relay[next_relay]);
                            if (start_pump_if_needed(spr, next_pump, now) == SPR_OK) {
                                spr->queue_relay_end_times[current_queue][next_relay] = intended_start + next_duration_sec;
                                if ((spr->relay_running & (1UL << next_relay)) == 0) {
                                    sprinkler_start_relay(spr->gpio_relay[next_relay]);
                                    spr->relay_running |= (1UL << next_relay);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return SPR_OK;
}
