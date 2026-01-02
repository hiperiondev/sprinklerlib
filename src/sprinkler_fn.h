/**
 * @file sprinkler_fn.h
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

#ifndef SPRINKLER_FN_H_
#define SPRINKLER_FN_H_

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "sprinkler_data_types.h"

#define TO_PERSISTENCE_SEC    15 // time between persistence saves

/// bitwise utils
#define SET_BIT(x,pos)          ((x) | ((uint32_t)(1U << (pos))))
#define CLEAR_BIT(x,pos)        ((x) & ((uint32_t)(~(1U << (pos)))))
#define CHECK_BIT(x,pos)        ((x) & ((uint32_t)(1U << (pos))))
#define SETMASK(b,p)            ((((uint32_t)(1U << (b))) - 1U) << (p))
#define UNSETMASK(b,p)          ((uint32_t)(~(SETMASK((b),(p)))))

/// sprinkler get values
#define GET_DT_EN(x)            (CHECK_BIT(x, 31))
#define GET_DT_HOURS(x)         ((x & SETMASK(24, 7)) >> 7)
#define GET_DT_HOUR(x,h)        (CHECK_BIT(GET_DT_HOURS(x), h))
#define GET_DT_DAYS(x)          (x & SETMASK(7,0))
#define GET_DT_DAY(x,d)         (CHECK_BIT(GET_DT_DAYS(x), d))

#define GET_RELAY_EN(x)         (CHECK_BIT(x, 15))
#define GET_RELAY_PUMP(x)       ((x & SETMASK(3, 12)) >> 12)
#define GET_RELAY_MIN(x)        (x & SETMASK(12, 0))

#define GET_MONTH_EN(x)         (CHECK_BIT(x, 7))
#define GET_MONTH_A(x)          (CHECK_BIT(x, 6))
#define GET_MONTH_B(x)          (CHECK_BIT(x, 5))
#define GET_MONTH_DT(x)         (x & SETMASK(5, 0))

#define GET_PUMP_EN(x,p)        (CHECK_BIT(x, (p + 25)))
#define GET_PUMP_RELAY(x, p)    (((x) >> ((p) * 5)) & 0x1F)  // 5 bits per pump → relay 0-31

/// sprinkler set values
#define SET_DT_EN(x,b)          x = b ? SET_BIT(x, 31) : CLEAR_BIT(x, 31)
#define SET_DT_HOUR(x,h,b)      x = b ? SET_BIT(x, (h + 7)) : CLEAR_BIT(x, (h + 7))
#define SET_DT_DAY(x,d,b)       x = b ? SET_BIT(x, d) : CLEAR_BIT(x, d)
#define SET_DT_QUEUE(x,q,b)     x = b ? SET_BIT(x, q) : CLEAR_BIT(x, q)

#define SET_RELAY_EN(x,b)       x = b ? SET_BIT(x, 15) : CLEAR_BIT(x, 15)
#define SET_RELAY_PUMP(x,v)     x = ((x & UNSETMASK(3, 12)) | (v << 12))
#define SET_RELAY_MIN(x,v)      x = (x & UNSETMASK(12, 0)) | (v)

#define SET_MONTH_EN(x,b)       x = b ? SET_BIT(x, 7) : CLEAR_BIT(x, 7)
#define SET_MONTH_A(x,b)        x = b ? SET_BIT(x, 6) : CLEAR_BIT(x, 6)
#define SET_MONTH_B(x,b)        x = b ? SET_BIT(x, 5) : CLEAR_BIT(x, 5)
#define SET_MONTH_DT(x,v)       x = (x & UNSETMASK(5, 0)) | (v)

#define SET_PUMP_EN(x,p,b)      x = b ? SET_BIT(x, (p + 25)) : CLEAR_BIT(x, (p + 25))
#define SET_PUMP_RELAY(x,p,v)   x = ((x & UNSETMASK(5, (p * 5))) | (v << (p * 5)))

#define SET_QUEUE(x,r,b)        x = (b ? SET_BIT(x, r) : CLEAR_BIT(x, r))
#define SET_QUEUE_AUTOADV(x,b)  x = (b ? SET_BIT(x, 31) : CLEAR_BIT(x, 31))
#define SET_QUEUE_RSEC(x,v)     x = (x & UNSETMASK(31, 0)) | (v)

#define GET_QUEUE_PAUSE_SEC(x)  ((x) & 0x7FFFFFFFUL)
#define GET_QUEUE_AUTOADV(x)    CHECK_BIT((x), 31)

// Wrap-safe time comparison macros
#define TIME_BEFORE(a, b)       ((int32_t)((a) - (b)) < 0)
#define TIME_AFTER(a, b)        ((int32_t)((a) - (b)) > 0)
#define TIME_AFTER_OR_EQ(a, b)  ((int32_t)((a) - (b)) >= 0)

/**
 * @brief Retrieves the current system time in both structured (tm) and Unix timestamp formats.
 *
 * This function fetches the current time from the system clock using time(NULL) and converts it into a local time structure if requested.
 * It handles the conversion safely, checking for errors in localtime conversion. The function is designed to provide time information
 * for scheduling and timing operations within the sprinkler system, such as determining if it's time to start a queue or calculating
 * end times for relays and pumps. If the timeinfo parameter is provided, it populates a struct tm with the broken-down local time.
 * If unix_seconds is provided, it stores the Unix timestamp (seconds since epoch) as a uint32_t. The function returns SPR_OK on success
 * or SPR_FAIL if the localtime conversion fails (e.g., due to invalid time or system issues). This is a foundational utility for all
 * time-dependent logic in the library, ensuring consistent time handling across features like automatic queue starting and duration calculations.
 *
 * @param timeinfo Pointer to a struct tm to store the broken-down local time. Can be NULL if not needed.
 * @param unix_seconds Pointer to a uint32_t to store the Unix timestamp. Can be NULL if not needed.
 * @return spr_err_t SPR_OK on success, SPR_FAIL if localtime conversion fails.
 */
spr_err_t sprinkler_get_time(struct tm *timeinfo, uint32_t *unix_seconds);

/**
 * @brief Initializes the sprinkler system instance and loads persistent configuration if available.
 *
 * This function sets up a static instance of the sprinkler_t structure and assigns it to the provided pointer. It attempts to load
 * persisted configuration data using sprinkler_persitence_get(). If loading fails (e.g., no persisted data or corruption), it initializes
 * the structure to zero using memset, effectively resetting all configurations to default (disabled) states. It also resets global state
 * variables such as queue_running, relay_running, and various arrays for paused queues, current relay indices, pause end times, repeat counts,
 * relay end times, active pumps, and pump start times. This ensures the system starts in a clean, known state. The sprinkler_config_changed
 * flag is set to false initially. This function is typically called at the beginning of the program to prepare the sprinkler system for use,
 * allowing seamless resumption from persisted states or fresh starts. It does not perform any hardware initialization; that's handled separately.
 *
 * @param spr Pointer to sprinkler_t; will be set to point to the initialized instance.
 */
void sprinkler_init(sprinkler_t *spr);

/**
 * @brief Deinitializes the sprinkler system, saving configuration if changes were made.
 *
 * This function checks if the configuration has changed (via sprinkler_config_changed) and, if so, attempts to persist the current sprinkler_t
 * state using sprinkler_persitence_put() with up to 3 retries for reliability (e.g., in case of transient storage errors). This function does not free any memory
 * since the instance is static, but it ensures that any modified configurations are saved before shutdown. It should be called at program exit or when
 * the sprinkler system is no longer needed to preserve settings for future initializations. No hardware deinitialization is performed here.
 *
 * @param spr Pointer to sprinkler_t.
 */
void sprinkler_deinit(sprinkler_t *spr);

/**
 * @brief Executes the main control loop for the sprinkler system, handling scheduling, relay/pump control, and state updates.
 *
 * This is the core function that drives the sprinkler system's operation. It retrieves the current time and checks if it's a scheduled start time
 * using sprinkler_is_start_time(). If a new start event is detected, it enables relevant queues based on the current month's date_time configuration.
 * It periodically persists changes if the config has changed and sufficient time (TO_PERSISTENCE_SEC) has passed. The loop processes all active queues,
 * managing relay activations, durations (with overrides), pauses, overlaps, repeats, and pump delays. For each queue, it advances through enabled relays,
 * starts pumps with delays if needed, handles relay overlaps for smooth transitions, and stops relays/pumps when durations expire or queues complete.
 * If no queues are running, it resets all runtime states and stops any active pumps/relays. The function supports auto-advance after pauses and per-relay
 * pause configurations (using queue 31). It also checks for paused queues and skips them unless auto-advance is enabled. Pump starts are delayed if configured,
 * and overlaps allow simultaneous operation of consecutive relays for a specified millisecond duration to prevent pressure drops. The loop is designed to be
 * called repeatedly (e.g., in a main program loop) without blocking, using current time for all decisions. It returns SPR_OK on normal operation or SPR_FAIL
 * if time retrieval fails. This function encapsulates the entire runtime logic, making the system autonomous once configured.
 *
 * @param spr Pointer to the initialized sprinkler_t structure.
 * @return spr_err_t SPR_OK on success, SPR_FAIL if time retrieval fails.
 */
spr_err_t sprinkler_main_loop(sprinkler_t *spr);

/**
 * @brief Sets or clears a specific day enable bit in a date_time schedule entry.
 *
 * This function modifies the date_time array in the sprinkler structure for a given schedule ID. It uses bitwise operations (SET_DT_DAY or CLEAR_DT_DAY)
 * to enable or disable a specific day of the week (0=Monday to 6=Sunday) in the 7-bit day mask. If enabled, the schedule can trigger on that day if other
 * conditions (hour, enable flag) match. The function validates the ID (0-31) and day (0-6), returning SPR_FAIL on invalid inputs. It sets the config changed
 * flag to true, triggering future persistence. This is used to configure weekly recurring schedules for sprinkler queues, allowing fine-grained control over
 * which days a particular time slot is active. Note that day 0 is Monday, which may differ from tm_wday (0=Sunday).
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param id Schedule ID (0-31).
 * @param day Day of the week (0=Monday to 6=Sunday).
 * @param en True to enable, false to disable.
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid parameters.
 */
spr_err_t sprinkler_set_dt_day(sprinkler_t *spr, uint8_t id, uint8_t day, bool en);

/**
 * @brief Sets or clears a specific hour enable bit in a date_time schedule entry.
 *
 * Similar to sprinkler_set_dt_day, this function modifies the 24-bit hour mask in the date_time array for a given ID. It enables or disables a specific
 * hour (0-23) using SET_DT_HOUR or CLEAR_DT_HOUR. This allows schedules to trigger only at selected hours on enabled days. Validation ensures ID <=31 and
 * hour <=23. The config changed flag is set. This function is key for time-of-day scheduling, combining with days and minutes for precise activation times.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param id Schedule ID (0-31).
 * @param hour Hour of the day (0-23).
 * @param en True to enable, false to disable.
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid parameters.
 */
spr_err_t sprinkler_set_dt_hour(sprinkler_t *spr, uint8_t id, uint8_t hour, bool en);

/**
 * @brief Sets the minute value for a specific hour in a date_time schedule entry.
 *
 * This function updates the date_time_min 2D array for a given schedule ID and hour with a specific minute (0-59). It only compiles if ALLOW_MIN_PRECISION
 * is defined, providing sub-hour precision for start times. Validation checks ID <=31, hour <=23, min <=59. Sets config changed flag. This enhances scheduling
 * accuracy beyond hourly granularity, allowing starts at exact minutes within enabled hours.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param id Schedule ID (0-31).
 * @param hour Hour of the day (0-23).
 * @param min Minute (0-59).
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid parameters.
 */
spr_err_t sprinkler_set_dt_min(sprinkler_t *spr, uint8_t id, uint8_t hour, uint8_t min);

/**
 * @brief Enables or disables an entire date_time schedule entry.
 *
 * This function sets or clears the enable bit (bit 31) in the date_time array for the specified ID using SET_DT_EN or CLEAR_DT_EN. An enabled entry can
 * trigger queues if day/hour/minute match current time. Validation for ID <=31. Sets config changed. This acts as a master switch for each of the 32 schedule slots.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param id Schedule ID (0-31).
 * @param en True to enable, false to disable.
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid ID.
 */
spr_err_t sprinkler_set_dt_en(sprinkler_t *spr, uint8_t id, bool en);

/**
 * @brief Associates or disassociates a queue with a date_time schedule entry.
 *
 * This function modifies the date_time_queue array, setting or clearing a bit for a specific queue (0-31) in the given schedule ID using SET_DT_QUEUE.
 * When the schedule triggers, all associated queues will start if the month enables it. Validates ID and queue <=31. Sets config changed. This links time schedules
 * to action queues, allowing multiple queues to activate on the same schedule.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param id Schedule ID (0-31).
 * @param queue Queue ID (0-31).
 * @param en True to associate, false to disassociate.
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid parameters.
 */
spr_err_t sprinkler_set_dt_queue(sprinkler_t *spr, uint8_t id, uint8_t queue, bool en);

/**
 * @brief Enables or disables scheduling for a specific month.
 *
 * Modifies the month array (0-11 for Jan-Dec), setting or clearing the enable bit (bit 7) using SET_MONTH_EN. Only enabled months allow their associated
 * date_time schedules to trigger. Validates month <=11. Sets config changed. This provides seasonal control, e.g., disabling watering in winter months.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param month Month (0=Jan to 11=Dec).
 * @param en True to enable, false to disable.
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid month.
 */
spr_err_t sprinkler_set_month_en(sprinkler_t *spr, uint8_t month, bool en);

/**
 * @brief Sets the 'A' flag for a month (purpose application-specific, bit 6).
 *
 * Updates bit 6 in the month array entry using SET_MONTH_A. The exact meaning of 'A' is not defined in the core library and may be used for custom
 * extensions (e.g., alternate schedule A). Validates month <=11. Sets config changed.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param month Month (0-11).
 * @param a Value (0 or 1, but as bool; true sets bit).
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid month.
 */
spr_err_t sprinkler_set_month_a(sprinkler_t *spr, uint8_t month, uint8_t a);

/**
 * @brief Sets the 'B' flag for a month (purpose application-specific, bit 5).
 *
 * Similar to sprinkler_set_month_a, but for bit 5 using SET_MONTH_B. For custom or alternate configurations (e.g., schedule B).
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param month Month (0-11).
 * @param b Value (0 or 1, bool).
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid month.
 */
spr_err_t sprinkler_set_month_b(sprinkler_t *spr, uint8_t month, uint8_t b);

/**
 * @brief Sets the date_time schedule ID associated with a month.
 *
 * Updates the lower 5 bits (0-4) of the month array entry with a date_time ID (0-31) using SET_MONTH_DT. This links each month to a specific schedule slot.
 * Validates month <=11 and dt <=31. Sets config changed. Essential for monthly varying schedules.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param month Month (0-11).
 * @param dt Date_time ID (0-31).
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid parameters.
 */
spr_err_t sprinkler_set_month_dt(sprinkler_t *spr, uint8_t month, uint8_t dt);

/**
 * @brief Enables or disables a specific relay.
 *
 * Sets or clears the enable bit (bit 15) in the relay array entry using SET_RELAY_EN. Disabled relays are skipped in queues. Validates relay <=31.
 * Sets config changed. Relays control individual sprinkler zones or valves.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param relay Relay ID (0-31).
 * @param en True to enable, false to disable.
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid relay.
 */
spr_err_t sprinkler_set_relay_en(sprinkler_t *spr, uint8_t relay, bool en);

/**
 * @brief Assigns a pump to a relay.
 *
 * Updates the 3-bit pump field (bits 12-14) in the relay entry using SET_RELAY_PUMP. Pumps (0-4) are associated with relays for water supply control.
 * Validates relay <=31, pump <=4 (though macro allows up to 7). Sets config changed.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param relay Relay ID (0-31).
 * @param pump Pump ID (0-4).
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid parameters.
 */
spr_err_t sprinkler_set_relay_pump(sprinkler_t *spr, uint8_t relay, uint8_t pump);

/**
 * @brief Sets the default duration in minutes for a relay.
 *
 * Updates the 12-bit minute field (bits 0-11) in the relay entry using SET_RELAY_MIN. Duration is minutes * 60 seconds, min 0-4095. 0 means skip or use override.
 * Validates relay <=31, minutes <=4095. Sets config changed.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param relay Relay ID (0-31).
 * @param minutes Duration in minutes (0-4095).
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid relay.
 */
spr_err_t sprinkler_set_relay_min(sprinkler_t *spr, uint8_t relay, uint16_t minutes);

/**
 * @brief Sets the overlap duration in milliseconds between this relay and the next.
 *
 * Updates relay_overlap_ms array for the relay. Overlap allows simultaneous operation to maintain pressure. Validates relay <=31. Sets config changed.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param relay Relay ID (0-31).
 * @param ms Overlap milliseconds (0 to disable).
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid relay.
 */
spr_err_t sprinkler_set_relay_overlap(sprinkler_t *spr, uint8_t relay, uint32_t ms);

/**
 * @brief Sets the GPIO pin associated with a relay.
 *
 * Updates gpio_relay array. Used for hardware control via sprinkler_start/stop_relay. Validates relay <=31. Sets config changed.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param relay Relay ID (0-31).
 * @param gpio GPIO pin number.
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid relay.
 */
spr_err_t sprinkler_set_relay_gpio(sprinkler_t *spr, uint8_t relay, uint8_t gpio);

/**
 * @brief Adds or removes a relay from a queue.
 *
 * Modifies the queue array bitmask, setting or clearing the bit for the relay using SET_QUEUE. Queues are sequences of relays to activate in order.
 * Validates queue <=31, relay <=31 (though relay is uint32_t, but bit ops limit to 0-31). Sets config changed.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param queue Queue ID (0-31).
 * @param relay Relay ID (0-31).
 * @param en True to add, false to remove.
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid parameters.
 */
spr_err_t sprinkler_set_queue(sprinkler_t *spr, uint8_t queue, uint32_t relay, bool en);

/**
 * @brief Sets the pause duration after each relay in a queue.
 *
 * Updates queue_pause array with seconds (bits 0-30), capped at 0x7FFFFFFF. Validates queue <=31. Sets config changed. Pause occurs after relay duration.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param queue Queue ID (0-31).
 * @param seconds Pause seconds.
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid queue.
 */
spr_err_t sprinkler_set_queue_pause(sprinkler_t *spr, uint8_t queue, uint32_t seconds);

/**
 * @brief Enables or disables auto-advance after pause in a queue.
 *
 * Sets bit 31 in queue_pause using SET_QUEUE_AUTOADV. If enabled, queue resumes automatically after pause; else, pauses until manual resume.
 * Validates queue <=31. Sets config changed.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param queue Queue ID (0-31).
 * @param en True for auto-advance, false for manual.
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid queue.
 */
spr_err_t sprinkler_set_queue_autoadv(sprinkler_t *spr, uint8_t queue, bool en);

/**
 * @brief Sets an override duration in seconds for a specific relay in a queue.
 *
 * Updates queue_relay_sec 2D array. If >0, overrides the relay's default minutes. Note: Queue 31 is reserved for per-relay pauses.
 * Validates queue <=31, relay <=31, seconds <=65535. Sets config changed.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param queue Queue ID (0-31, but not 31 for standard use).
 * @param relay Relay ID (0-31).
 * @param seconds Override seconds (0 to use default).
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid parameters.
 */
spr_err_t sprinkler_set_queue_relay_sec(sprinkler_t *spr, uint8_t queue, uint8_t relay, uint16_t seconds);

/**
 * @brief Sets the number of times a queue should repeat its cycle.
 *
 * Updates queue_repeat array. 0 means no repeat (run once). Validates queue <=31, times <=255. Sets config changed.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param queue Queue ID (0-31).
 * @param times Repeat count (0-255).
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid parameters.
 */
spr_err_t sprinkler_set_queue_repeat(sprinkler_t *spr, uint8_t queue, uint8_t times);

/**
 * @brief Sets a per-relay pause duration (uses queue 31's relay_sec).
 *
 * This configures pauses specific to each relay, independent of queue, by setting queue_relay_sec[31][relay]. Capped at 65535 seconds.
 * Validates relay <32. Sets config changed. Applied after the relay in any queue.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param relay Relay ID (0-31).
 * @param seconds Pause seconds.
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid relay.
 */
spr_err_t sprinkler_set_pause(sprinkler_t *spr, uint8_t relay, uint32_t seconds);

/**
 * @brief Sets the global delay in milliseconds before starting a pump.
 *
 * Updates pump_delay_ms. Used to stagger pump starts. Sets config changed.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param ms Delay milliseconds.
 * @return spr_err_t Always SPR_OK.
 */
spr_err_t sprinkler_set_pump_delay(sprinkler_t *spr, uint32_t ms);

/**
 * @brief Enables or disables a specific pump.
 *
 * Sets enable bit for pump (0-4) in pump field using SET_PUMP_EN (bits 25-29). Validates pump <=4. Sets config changed.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param pump Pump ID (0-4).
 * @param en True to enable, false to disable.
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid pump.
 */
spr_err_t sprinkler_set_pump_en(sprinkler_t *spr, uint8_t pump, bool en);

/**
 * @brief Assigns a relay to a pump.
 *
 * Updates the 5-bit relay field for the pump in the pump uint32_t using SET_PUMP_RELAY (5 bits per pump, positions 0-24). Validates pump <=4, relay <=31.
 * Sets config changed. Pumps use dedicated relays for control.
 *
 * @param spr Pointer to the sprinkler_t structure.
 * @param pump Pump ID (0-4).
 * @param relay Relay ID (0-31).
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid parameters.
 */
spr_err_t sprinkler_set_pump_relay(sprinkler_t *spr, uint8_t pump, uint8_t relay);

/**
 * @brief Advances all running queues to the next relay.
 *
 * Iterates over running queues and increments current_relay_idx if <31. No validation beyond running check. Used for manual control.
 *
 * @param spr Pointer to sprinkler_t.
 * @return spr_err_t Always SPR_OK.
 */
spr_err_t sprinkler_queue_next(sprinkler_t *spr);

/**
 * @brief Moves all running queues to the previous relay.
 *
 * Similar to next, but decrements current_relay_idx if >0.
 *
 * @param spr Pointer to sprinkler_t.
 * @return spr_err_t Always SPR_OK.
 */
spr_err_t sprinkler_queue_previous(sprinkler_t *spr);

/**
 * @brief Pauses all queues.
 *
 * Sets queue_paused[0-31] to true. Does not affect running relays immediately.
 *
 * @param spr Pointer to sprinkler_t.
 * @return spr_err_t Always SPR_OK.
 */
spr_err_t sprinkler_queue_pause(sprinkler_t *spr);

/**
 * @brief Resumes all queues.
 *
 * Sets queue_paused[0-31] to false.
 *
 * @param spr Pointer to sprinkler_t.
 * @return spr_err_t Always SPR_OK.
 */
spr_err_t sprinkler_queue_resume(sprinkler_t *spr);

/**
 * @brief Pauses a specific queue.
 *
 * Sets queue_paused[q] to true. Validates q <=31.
 *
 * @param spr Pointer to sprinkler_t.
 * @param q Queue ID (0-31).
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid q.
 */
spr_err_t sprinkler_queue_pause_id(sprinkler_t *spr, uint8_t q);

/**
 * @brief Resumes a specific queue.
 *
 * Sets queue_paused[q] to false. Validates q <=31.
 *
 * @param spr Pointer to sprinkler_t.
 * @param q Queue ID (0-31).
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid q.
 */
spr_err_t sprinkler_queue_resume_id(sprinkler_t *spr, uint8_t q);

/**
 * @brief Advances a specific queue to the next relay.
 *
 * Increments current_relay_idx[q] if <31. Validates q <=31.
 *
 * @param spr Pointer to sprinkler_t.
 * @param q Queue ID (0-31).
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid q.
 */
spr_err_t sprinkler_queue_next_id(sprinkler_t *spr, uint8_t q);

/**
 * @brief Moves a specific queue to the previous relay.
 *
 * Decrements current_relay_idx[q] if >0. Validates q <=31.
 *
 * @param spr Pointer to sprinkler_t.
 * @param q Queue ID (0-31).
 * @return spr_err_t SPR_OK on success, SPR_FAIL on invalid q.
 */
spr_err_t sprinkler_queue_previous_id(sprinkler_t *spr, uint8_t q);

/**
 * @brief Gets a bitmask of all paused queues.
 *
 * Constructs a uint32_t mask where bits 0-31 indicate paused queues.
 *
 * @param spr Pointer to sprinkler_t.
 * @return uint32_t Bitmask of paused queues.
 */
uint32_t sprinkler_get_paused_queues(sprinkler_t *spr);

/**
 * @brief Checks if the current time matches a scheduled start time.
 *
 * Retrieves current time, computes day (adjusting tm_wday: 0=Sun -> 6=Sat, but code sets 0=Mon? Note: potential mismatch). Builds a temp date_time value
 * with enable, current hour, and day bits set. Checks if current month is enabled and if the associated date_time matches exactly (bits and optionally minute).
 * Returns true if all conditions match, triggering queue starts.
 *
 * @param spr Pointer to sprinkler_t.
 * @return bool True if it's a start time, false otherwise.
 */
bool sprinkler_is_start_time(sprinkler_t *spr);

/**
 * @brief Checks if any queue is paused.
 *
 * Scans queue_paused[0-31] for any true value.
 *
 * @param spr Pointer to sprinkler_t.
 * @return bool True if at least one queue is paused.
 */
bool sprinkler_is_queue_paused(sprinkler_t *spr);

/**
 * @brief Gets the bitmask of running queues.
 *
 * Directly returns the global queue_running uint32_t.
 *
 * @param spr Pointer to sprinkler_t.
 * @return uint32_t Bitmask of running queues (bits 0-31).
 */
uint32_t sprinkler_is_queue_running(sprinkler_t *spr);

/**
 * @brief Gets the bitmask of running relays.
 *
 * Directly returns the global relay_running uint32_t.
 *
 * @param spr Pointer to sprinkler_t.
 * @return uint32_t Bitmask of active relays (bits 0-31).
 */
uint32_t sprinkler_is_relay_running(sprinkler_t *spr);

/**
 * @brief Checks if a specific queue is paused.
 *
 * Returns queue_paused[q] if q <=31, else false.
 *
 * @param spr Pointer to sprinkler_t.
 * @param q Queue ID (0-31).
 * @return bool True if paused, false otherwise.
 */
bool sprinkler_is_queue_paused_id(sprinkler_t *spr, uint8_t q);

#endif /* SPRINKLER_FN_H_ */
