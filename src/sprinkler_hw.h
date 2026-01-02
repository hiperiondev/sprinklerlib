/**
 * @file sprinkler_hw.h
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

#ifndef SPRINKLER_HW_H_
#define SPRINKLER_HW_H_

#include <stdint.h>
#include <stdbool.h>

#include "sprinkler_data_types.h"

/**
 * @brief Delays execution for a specified number of milliseconds.
 *
 * This function introduces a precise delay in milliseconds, which is essential for timing-critical operations in the sprinkler system,
 * such as pump start delays (to prevent simultaneous high-current draws), relay overlap periods (to maintain water pressure during transitions),
 * and fine-grained pauses in queue processing. The delay can be implemented as blocking (e.g., busy-wait or sleep) or non-blocking depending on
 * the platform, but in most embedded contexts, it should allow other tasks to run if using an RTOS.
 *
 * Exhaustive functionality:
 * - The delay is calculated from the current system time and should be accurate within the limits of the hardware timer resolution.
 * - For values of 0, the function should return immediately without delay.
 * - Large values (e.g., > UINT32_MAX ms) are not expected, but if ms overflows, behavior is undefined—users should cap delays reasonably (e.g., < 1 hour).
 * - In simulated environments (as in the provided .c), it uses select() for sub-second precision, which is non-busy-waiting.
 * - On real hardware: Use timer peripherals (e.g., HAL_Delay on STM32, vTaskDelay on FreeRTOS) to avoid CPU wastage.
 * - Potential side effects: May affect system power consumption or allow interrupts/other threads to execute during the delay.
 * - Error cases: Typically always succeeds, but could fail on platforms with timer faults (though rare).
 * - Use cases: Called indirectly via sprinkler_main_loop for pump delays and overlaps; can be used manually for custom timing.
 *
 * @param ms The duration of the delay in milliseconds (0 to UINT32_MAX, though practical limits apply).
 * @return spr_err_t SPR_OK on successful delay completion, SPR_FAIL if a hardware timer error occurs (unlikely in most implementations).
 */
spr_err_t sprinkler_wait_ms(uint32_t ms);

/**
 * @brief Delays execution for a specified number of seconds.
 *
 * Similar to sprinkler_wait_ms but with second-level granularity, this function is used for longer pauses, such as queue pauses between relays,
 * per-relay pauses, or persistence save intervals. It provides a coarser delay suitable for operations where millisecond precision is unnecessary,
 * potentially allowing deeper sleep modes on power-constrained devices.
 *
 * Exhaustive functionality:
 * - Internally, it can be implemented by calling sprinkler_wait_ms(ms * 1000), but optimized versions use second-based sleeps (e.g., sleep() in POSIX).
 * - For s = 0, returns immediately.
 * - Handles large values, but practical use is for pauses up to hours (e.g., daily schedules); overflow not handled.
 * - In simulations, uses sleep() which is blocking but efficient.
 * - On embedded systems: Use RTOS delays or watchdog-aware sleeps to prevent timeouts.
 * - Side effects: Allows system idle time, potentially saving power or processing other events.
 * - Error cases: Rare, but could fail if sleep is interrupted abnormally (though typically ignored).
 * - Use cases: Queue pauses in sprinkler_main_loop, or manual long delays in custom code.
 *
 * @param s The duration of the delay in seconds (0 to UINT32_MAX, though practical limits apply).
 * @return spr_err_t SPR_OK on successful delay, SPR_FAIL on rare system errors.
 */
spr_err_t sprinkler_wait_seconds(uint32_t s);

/**
 * @brief Activates (starts) a specific relay, turning it on.
 *
 * This function sets the specified relay to an active state, typically by driving its associated GPIO pin to the appropriate level (high or low,
 * depending on relay module—active-low common). Relays control sprinkler valves or pumps, and this is called when a relay's duration starts in a queue.
 * It checks if the relay is already on to avoid redundant operations.
 *
 * Exhaustive functionality:
 * - Validates relay ID (0-31); returns SPR_FAIL for invalid IDs to prevent out-of-bounds access.
 * - In hardware: Uses GPIO output to toggle the pin; may include debouncing or current limiting if needed.
 * - State management: Tracks relay states internally if simulation is used, but on real HW, reads back GPIO if verification is required.
 * - Idempotent: If already started, does nothing and returns SPR_OK.
 * - Side effects: May consume power, activate solenoids, or trigger external events (e.g., water flow).
 * - Error cases: Fails on invalid relay, GPIO faults, or if HW is not initialized.
 * - Use cases: Called in sprinkler_main_loop when starting a relay or pump; can be used manually for testing.
 *
 * @param relay The relay ID to start (0-31, mapped to GPIO via sprinkler_t::gpio_relay).
 * @return spr_err_t SPR_OK if started successfully (or already on), SPR_FAIL on invalid relay or HW error.
 */
spr_err_t sprinkler_start_relay(uint8_t relay);

/**
 * @brief Deactivates (stops) a specific relay, turning it off.
 *
 * Symmetric to sprinkler_start_relay, this function sets the relay to inactive, stopping the associated valve or pump. Called when durations expire
 * or queues complete. Checks current state to avoid unnecessary operations.
 *
 * Exhaustive functionality:
 * - Validates relay ID (0-31).
 * - In hardware: Sets GPIO to inactive level.
 * - Idempotent: If already stopped, returns SPR_OK.
 * - Side effects: Stops water flow, reduces power usage.
 * - Error cases: Invalid relay or GPIO errors.
 * - Use cases: End of relay duration in main loop, or manual control.
 *
 * @param relay The relay ID to stop (0-31).
 * @return spr_err_t SPR_OK if stopped successfully (or already off), SPR_FAIL on invalid relay or HW error.
 */
spr_err_t sprinkler_stop_relay(uint8_t relay);

/**
 * @brief Loads the sprinkler configuration from persistent storage.
 *
 * Retrieves the entire sprinkler_t structure from non-volatile storage (e.g., file, EEPROM, flash). If loading fails (e.g., no data, corruption),
 * initializes the structure to defaults (all zero/disabled). This is called during sprinkler_init to restore previous settings.
 *
 * Exhaustive functionality:
 * - Storage medium: Platform-dependent—file in simulations, binary blob in embedded flash.
 * - Validation: Checks read size; if mismatch, resets to zero and returns SPR_FAIL.
 * - Integrity: No built-in CRC/checksum; users can add if needed.
 * - Side effects: Overwrites *spr; assumes spr is valid pointer.
 * - Error cases: Storage not found, read errors, size mismatch.
 * - Use cases: System startup to resume schedules.
 *
 * @param spr Pointer to sprinkler_t to load data into.
 * @return spr_err_t SPR_OK on successful load, SPR_FAIL if load failed (structure reset to defaults).
 */
spr_err_t sprinkler_persitence_get(sprinkler_t *spr);

/**
 * @brief Saves the sprinkler configuration to persistent storage.
 *
 * Stores the entire sprinkler_t structure to non-volatile storage. Called when configurations change (via sprinkler_config_changed) after a timeout,
 * or during deinit. Retries not built-in here, but handled in caller.
 *
 * Exhaustive functionality:
 * - Overwrites existing data; atomicity depends on platform (e.g., temp file then rename for safety).
 * - Validation: Checks write size.
 * - Side effects: Commits changes persistently.
 * - Error cases: Write failures, storage full.
 * - Use cases: After any config setter, periodically in main loop.
 *
 * @param spr Pointer to sprinkler_t to save.
 * @return spr_err_t SPR_OK on successful save, SPR_FAIL on write error.
 */
spr_err_t sprinkler_persitence_put(sprinkler_t *spr);

#endif /* SPRINKLER_HW_H_ */
