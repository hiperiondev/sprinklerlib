<a name="readme-top"></a>
<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/hiperiondev/sprinklerlib/">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">Sprinkler Library</h3>

  <p align="center">
    Sprinkler Library for Embedded Systems 
    <br />   
  </p>
</div>

<!-- ABOUT THE PROJECT -->
## About The Project

Sprinklerlib is an open-source C library designed for automating sprinkler systems in embedded environments. It provides a comprehensive framework for managing irrigation schedules, relay-controlled valves, pumps, and queues of operations. The library supports up to 32 relays (valves), 32 queues (sequences of relays), and 5 pumps, with flexible scheduling based on time (hours, minutes optional), days of the week, and months. Configurations are persisted to non-volatile storage, allowing the system to resume operations after restarts.

Key aspects include:
- **Time-Based Scheduling**: Automatic queue activation at specified times, with optional minute precision.
- **Queue Management**: Sequences of relays with overrides, repeats, pauses, and auto-advance options.
- **Pump Control**: Delayed starts to prevent power surges, associated with specific relays.
- **Relay Overlaps**: Millisecond-level overlaps for smooth transitions between relays to maintain water pressure.
- **Persistence**: Automatic saving of configurations with retries and timed intervals.
- **Manual Controls**: Functions for pausing, resuming, advancing, or reversing queues manually.
- **Hardware Abstraction**: Platform-agnostic hardware functions for delays, relay control, and storage.

The library is built for reliability in resource-constrained systems, using bit-packed structures for efficiency. It handles runtime states like active relays, paused queues, and end times using wrap-safe time comparisons to avoid overflow issues. While optimized for embedded use (e.g., with RTOS delays), it includes simulation-friendly implementations (e.g., using `select()` and `sleep()` for POSIX systems).

This project is released under the MIT License, making it suitable for both hobbyist and commercial applications. It is based on other open-source projects (contact original authors for details) and is hosted on GitHub for community contributions.

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

## Features  

- **Scheduling Flexibility**:
  - Up to 32 date/time slots for scheduling, each configurable with enable/disable, specific hours (0-23), days (0=Mon to 6=Sun, with potential tm_wday adjustment notes), and associated queues (bitmask for up to 32 queues).
  - Optional minute precision per hour via `ALLOW_MIN_PRECISION` define, allowing exact minute triggers.
  - Monthly configurations: Enable/disable per month (0-11), with A/B flags (placeholders, not used in current code but available for extensions like alternate schedules), and mapping to a date/time slot (0-31).

- **Relay Management**:
  - Up to 32 relays, each with: Enable/disable, associated pump (0-4 or none), on-duration in minutes (0-4095), overlap milliseconds for transitions (0-no overlap), and GPIO pin assignment.
  - Runtime tracking of active relays via bitmask.

- **Pump Management**:
  - Up to 5 pumps, each with: Enable/disable, dedicated relay assignment (0-31).
  - Global pump start delay in milliseconds to stagger activations and avoid inrush current.
  - Runtime states for starting, running, and stopping, with delayed starts and checks for necessity.

- **Queue System**:
  - Up to 32 queues (though `SPR_MAX_QUEUES` define suggests 8 for some contexts, code supports 32), each a bitmask of relays to activate in sequence.
  - Per-queue: Repeat count (0-255, 0 for single run), pause seconds after each relay (with auto-advance bit for automatic resumption).
  - Per-queue per-relay: Override seconds (0 to use default minutes, up to 65535; queue 31 reserved for per-relay pause configs).
  - Runtime management: Running bitmask, current relay index, pause end times, repeat counters, relay end times.
  - Manual controls: Next/previous relay (all or per-queue), pause/resume (all or per-queue).

- **Time and State Handling**:
  - Retrieval of current time as `struct tm` and Unix seconds, with error handling.
  - Wrap-safe comparisons for time (before/after/equal) to handle uint32_t overflows.
  - Detection of start times based on current hour/day/month, with exact matching via bitmasks.

- **Persistence and Configuration**:
  - Load/save entire `sprinkler_t` structure to/from storage (e.g., file, EEPROM).
  - Automatic persistence on changes after a timeout (`TO_PERSISTENCE_SEC = 15`), with retries in deinit.
  - Config change flag to trigger saves.

- **Hardware Primitives**:
  - Delays in milliseconds (precise, non-blocking where possible) and seconds.
  - Relay start/stop with validation and idempotency.
  - Extensible for real hardware (e.g., GPIO via HAL or direct).

- **Error Handling and Types**:
  - `spr_err_t` enum for OK, parameter/range/busy/HW/storage errors, and generic fail.
  - Bit manipulation macros for efficient setting/getting of packed fields.

- **Defines and Customization**:
  - `ALLOW_MIN_PRECISION`: Enables minute-level scheduling (adds 32x24 byte array).
  - `SPR_MAX_QUEUES` and `SPR_MAX_RELAYS`: Suggest limits (8/16), but code uses 32 for flexibility.
  - Bitwise utilities for set/clear/check/mask operations.

- **Runtime Loop**:
  - `sprinkler_main_loop`: Core function to call repeatedly; handles all automation, including start detection, queue processing, relay/pump control, pauses, overlaps, and cleanups.

The library emphasizes exhaustive validation (e.g., bounds checks on IDs, values), efficiency (bit-packing reduces memory), and extensibility (placeholders like month A/B). It avoids dynamic allocation, using fixed arrays for predictability in embedded systems.

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

## System Architecture  

The sprinklerlib architecture is centered around a single `sprinkler_t` structure that encapsulates all configuration and runtime state, ensuring thread-safety and ease of persistence. The system operates in a loop-based model, suitable for embedded main loops or tasks.

### Core Components:
1. **Data Structures** (`sprinkler_data_types.h`):
   - `sprinkler_t`: A packed struct with arrays for date_time (schedules), relays, months, queues, etc., and runtime fields like queue_running (bitmask), relay_running, etc.
   - Enums: `spr_err_t` for errors, `spr_pump_state_t` (not directly used in code but conceptual).
   - Bit-packed fields: E.g., `date_time` uses bit 31 for enable, 7-30 for hours, 0-6 for days; `relay` packs enable, pump, minutes.

2. **Hardware Abstraction** (`sprinkler_hw.h`):
   - Delay functions: `sprinkler_wait_ms` and `sprinkler_wait_seconds` for timing (e.g., overlaps, pauses).
   - Relay control: `sprinkler_start_relay` and `sprinkler_stop_relay` (GPIO-based).
   - Persistence: `sprinkler_persitence_get` and `sprinkler_persitence_put` for loading/saving config.

3. **Functional Logic** (`sprinkler_fn.h` and `.c`):
   - **Initialization**: `sprinkler_init` loads persistence or zeros struct; resets runtime states.
   - **Deinitialization**: `sprinkler_deinit` saves if changed, with retries.
   - **Time Utilities**: `sprinkler_get_time` fetches system time; macros for safe comparisons.
   - **Scheduling Check**: `sprinkler_is_start_time` matches current time against configs.
   - **Configuration Setters**: Numerous functions like `sprinkler_set_dt_day`, `sprinkler_set_relay_en`, etc., with validation and change flagging.
   - **Queue Controls**: Setters for queues, pauses, overrides, repeats; manual next/prev/pause/resume.
   - **Getters**: For states like paused queues, running masks, etc.
   - **Main Loop**: `sprinkler_main_loop`:
     - Fetches time.
     - Triggers queues on new start events (hour/min change).
     - Persists periodically if changed.
     - Processes pumps: Starts delayed if needed.
     - For each running queue:
       - Skips if paused or pause active.
       - Advances relay index, skips disabled/zero-duration.
       - Starts relays/pumps, sets end times.
       - Handles overlaps: Starts next relay early.
       - Stops on expiration, checks sharing, applies pauses.
       - Manages repeats, completes queues.
     - Cleans up if no queues running.

### Data Flow:
- **Startup**: Call `sprinkler_init` → Load config → Set up states.
- **Configuration**: Use setters to define schedules, queues, etc. → Flags change → Persisted in loop.
- **Runtime**: Repeatedly call `sprinkler_main_loop` (e.g., every second) → Checks time → Activates queues → Manages hardware via start/stop/wait.
- **Manual Intervention**: Call pause/next etc. as needed (e.g., from UI or sensors).
- **Shutdown**: Call `sprinkler_deinit` → Save if needed.

### Key Mechanisms:
- **Bitmasks**: For efficient storage/checks (e.g., queue relays, running states).
- **Time Management**: Uses `time_t` as uint32_t for simplicity, assuming no Y2038 issues in embedded context.
- **Pauses and Overlaps**: Timestamp-based end times for precise control.
- **Pump Delays**: Schedules future starts to avoid simultaneous activations.
- **Persistence**: Whole-struct dump/load for simplicity; add CRC for robustness if needed.
- **Error Propagation**: Most functions return `spr_err_t`; main loop returns on time fail.

This architecture allows scalable, autonomous operation with minimal external dependencies, ideal for microcontrollers like ESP32 or STM32.

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

## Usage  

### Prerequisites:
- C compiler (e.g., GCC for embedded).
- System time source (e.g., RTC or NTP).
- Hardware: Relays on GPIOs, storage (e.g., flash/file).
- Define `ALLOW_MIN_PRECISION` if needed (increases memory by ~768 bytes).

### Basic Example:
```c
#include "sprinkler_fn.h"
#include <stdio.h>  // For printf in simulation

int main() {
    sprinkler_t spr_inst;
    sprinkler_t *spr = &spr_inst;

    // Initialize
    sprinkler_init(spr);

    // Configure example: Enable relay 0, assign to pump 0, 5 min duration, GPIO 10
    sprinkler_set_relay_en(spr, 0, true);
    sprinkler_set_relay_pump(spr, 0, 0);
    sprinkler_set_relay_min(spr, 0, 5);
    sprinkler_set_relay_gpio(spr, 0, 10);

    // Enable pump 0 on relay 32? Wait, relays 0-31; example relay 5 for pump
    sprinkler_set_pump_en(spr, 0, true);
    sprinkler_set_pump_relay(spr, 0, 5);  // Assume relay 5 is pump control

    // Set pump delay 500ms
    sprinkler_set_pump_delay(spr, 500);

    // Create queue 0 with relays 0 and 1
    sprinkler_set_queue(spr, 0, 0, true);
    sprinkler_set_queue(spr, 0, 1, true);
    sprinkler_set_queue_pause(spr, 0, 10);  // 10s pause after each
    sprinkler_set_queue_autoadv(spr, 0, true);
    sprinkler_set_queue_repeat(spr, 0, 2);  // Repeat twice

    // Schedule: Date/time slot 0, enable, hour 8, day 0 (Mon)
    sprinkler_set_dt_en(spr, 0, true);
    sprinkler_set_dt_hour(spr, 0, 8, true);
    sprinkler_set_dt_day(spr, 0, 0, true);
#ifdef ALLOW_MIN_PRECISION
    sprinkler_set_dt_min(spr, 0, 8, 0);  // At 8:00
#endif
    sprinkler_set_dt_queue(spr, 0, 0, true);  // Start queue 0

    // Month 0 (Jan), enable, map to dt 0
    sprinkler_set_month_en(spr, 0, true);
    sprinkler_set_month_dt(spr, 0, 0);

    // Main loop (simulate)
    while (1) {
        if (sprinkler_main_loop(spr) != SPR_OK) {
            printf("Error in main loop\n");
            break;
        }
        sprinkler_wait_seconds(1);  // Simulate tick
    }

    // Deinit
    sprinkler_deinit(spr);
    return 0;
}
```

### Advanced Usage:
- **Manual Control**: `sprinkler_queue_pause_id(spr, 0);` to pause queue 0.
- **State Queries**: `if (sprinkler_is_queue_paused_id(spr, 0)) ...`
- **Per-Relay Pause**: `sprinkler_set_queue_relay_sec(spr, 31, 0, 30);` for 30s after relay 0.
- **Overlap**: `sprinkler_set_relay_overlap(spr, 0, 2000);` for 2s overlap with next.
- **Custom Hardware**: Implement `sprinkler_start_relay` etc. for your platform (e.g., GPIO write).
- **Persistence**: Override `sprinkler_persitence_put/get` for EEPROM/flash.

Compile with `-DALLOW_MIN_PRECISION` if needed. For embedded, integrate into main task loop.

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

## Troubleshooting  

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

## Contributing  

Contributions are welcome, especially given the project’s early stage. To contribute:  
- Submit issues or feature requests on the [GitHub Issues](https://github.com/hiperiondev/sprinklerlib/issues) page.  
- Propose code or documentation improvements via pull requests.  
- Engage in discussions on the repository to share ideas.  

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

## License  

Distributed under the MIT License. See `LICENSE` file for more information.  

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>
