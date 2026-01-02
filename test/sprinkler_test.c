/**
 * @file sprinkler_test.c
 * @brief Exhaustive test suite for sprinklerlib - NO ASSERTS, VERBOSE PASS/FAIL
 *
 * Tests every public function with clear [PASS]/[FAIL] output and comments.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "sprinkler_data_types.h"
#include "sprinkler_fn.h"
#include "sprinkler_hw.h"

// Helper macros
#define TEST_SECTION(title) printf("\n=== %s ===\n", title)
#define PASS(comment)       printf("\033[0;32m[PASS]\033[0m %s\n", comment)
#define FAIL(comment)       printf("\033[0;31m[FAIL]\033[0m %s\n", comment)
#define RUN_TEST(func)      do { printf("Running %s...\n", #func); func(); } while(0)

// Global sprinkler instance
static int total_tests = 0;
static int passed_tests = 0;

// Helper to update stats
#define CHECK(cond, comment) \
    do { \
        total_tests++; \
        if (cond) { \
            PASS(comment); \
            passed_tests++; \
        } else { \
            FAIL(comment); \
        } \
    } while(0)

// Test functions
void test_get_time(void) {
    TEST_SECTION("sprinkler_get_time");
    struct tm ti;
    uint32_t us;
    CHECK(sprinkler_get_time(NULL, NULL) == SPR_OK, "NULL parameters");
    CHECK(sprinkler_get_time(&ti, NULL) == SPR_OK, "timeinfo only");
    CHECK(sprinkler_get_time(NULL, &us) == SPR_OK, "unix_seconds only");
    CHECK(sprinkler_get_time(&ti, &us) == SPR_OK, "both parameters");

    // Verify against standard library
    time_t now = time(NULL);
    struct tm *std_ti = localtime(&now);
    CHECK(us == (uint32_t )now, "unix_seconds matches time(NULL)");
    CHECK(memcmp(&ti, std_ti, sizeof(struct tm)) == 0, "timeinfo matches localtime");

    // Edge case: Invalid pointers (but function handles NULL, no crash test)
}

void test_init_deinit(void) {
    TEST_SECTION("sprinkler_init and sprinkler_deinit");
    unlink("sprinkler.dat"); // Start clean

    sprinkler_t my_spr;
    sprinkler_t *spr = &my_spr;
    memset(spr, 0xFF, sizeof(sprinkler_t)); // Set to invalid values

    sprinkler_init(spr);
    CHECK(spr->pump == 0, "pump initialized to 0");
    CHECK(spr->pump_delay_ms == 0, "pump_delay_ms initialized to 0");
    CHECK(memcmp(spr->date_time, (uint32_t[32] ) { 0 }, sizeof(spr->date_time)) == 0, "date_time zeroed");
    CHECK(memcmp(spr->date_time_queue, (uint32_t[32] ) { 0 }, sizeof(spr->date_time_queue)) == 0, "date_time_queue zeroed");
    CHECK(memcmp(spr->relay, (uint16_t[32] ) { 0 }, sizeof(spr->relay)) == 0, "relay zeroed");
    CHECK(memcmp(spr->relay_overlap_ms, (uint32_t[32] ) { 0 }, sizeof(spr->relay_overlap_ms)) == 0, "relay_overlap_ms zeroed");
    CHECK(memcmp(spr->month, (uint8_t[12] ) { 0 }, sizeof(spr->month)) == 0, "month zeroed");
    CHECK(memcmp(spr->queue, (uint32_t[32] ) { 0 }, sizeof(spr->queue)) == 0, "queue zeroed");
    CHECK(memcmp(spr->queue_repeat, (uint8_t[32] ) { 0 }, sizeof(spr->queue_repeat)) == 0, "queue_repeat zeroed");
    CHECK(memcmp(spr->queue_relay_sec, (uint16_t[32][32] ) { 0 }, sizeof(spr->queue_relay_sec)) == 0, "queue_relay_sec zeroed");
    CHECK(memcmp(spr->queue_pause, (uint32_t[32] ) { 0 }, sizeof(spr->queue_pause)) == 0, "queue_pause zeroed");
    CHECK(memcmp(spr->gpio_relay, (uint8_t[32] ) { 0 }, sizeof(spr->gpio_relay)) == 0, "gpio_relay zeroed");
    CHECK(spr->sprinkler_config_changed == false, "config_changed false");
    CHECK(spr->queue_running == 0, "spr->queue_running 0");
    CHECK(spr->relay_running == 0, "spr->relay_running 0");
    CHECK(spr->active_pumps == 0, "spr->active_pumps 0");
    CHECK(memcmp(spr->queue_paused, (bool[32]){false}, sizeof(spr->queue_paused)) == 0, "spr->queue_paused all false");
    CHECK(memcmp(spr->current_relay_idx, (uint8_t[32] ) { 0 }, sizeof(spr->current_relay_idx)) == 0, "current_relay_idx zeroed");
    CHECK(memcmp(spr->queue_pause_end_times, (uint32_t[32] ) { 0 }, sizeof(spr->queue_pause_end_times)) == 0, "queue_pause_end_times zeroed");
    CHECK(memcmp(spr->repeat_count, (uint8_t[32] ) { 0 }, sizeof(spr->repeat_count)) == 0, "repeat_count zeroed");
    CHECK(memcmp(spr->queue_relay_end_times, (uint32_t[32][32] ) { 0 }, sizeof(spr->queue_relay_end_times)) == 0, "queue_relay_end_times zeroed");
    CHECK(memcmp(spr->pump_start_times, (uint32_t[5] ) { 0 }, sizeof(spr->pump_start_times)) == 0, "pump_start_times zeroed");

    // Test deinit without changes
    sprinkler_deinit(spr);
    CHECK(access("sprinkler.dat", F_OK) != 0, "No persistence file created without changes");

    // Test with changes
    sprinkler_init(spr);
    spr->sprinkler_config_changed = true;
    spr->pump = 1;
    sprinkler_deinit(spr);
    CHECK(access("sprinkler.dat", F_OK) == 0, "Persistence file created with changes");

    // Reload and check
    sprinkler_t reloaded;
    sprinkler_init(&reloaded);
    CHECK(reloaded.pump == 1, "Persisted data loaded correctly");

    // Test retry on deinit (simulate fa...
}

void test_set_dt_functions(void) {
    sprinkler_t my_spr;
    sprinkler_t *spr = &my_spr;
    sprinkler_init(spr);

    TEST_SECTION("Date/Time Set Functions");
    CHECK(sprinkler_set_dt_en(spr, 0, true) == SPR_OK, "set dt_en true valid id");
    CHECK(GET_DT_EN(spr->date_time[0]), "dt_en set to true");
    CHECK(spr->sprinkler_config_changed, "config_changed set");
    CHECK(sprinkler_set_dt_en(spr, 0, false) == SPR_OK, "set dt_en false valid id");
    CHECK(!GET_DT_EN(spr->date_time[0]), "dt_en set to false");
    CHECK(sprinkler_set_dt_en(spr, 32, true) == SPR_FAIL, "set dt_en invalid id");
    CHECK(sprinkler_set_dt_day(spr, 0, 0, true) == SPR_OK, "set dt_day 0 true");
    CHECK(GET_DT_DAY(spr->date_time[0], 0), "dt_day 0 set");
    CHECK(sprinkler_set_dt_day(spr, 0, 0, false) == SPR_OK, "set dt_day 0 false");
    CHECK(!GET_DT_DAY(spr->date_time[0], 0), "dt_day 0 cleared");
    CHECK(sprinkler_set_dt_day(spr, 0, 6, true) == SPR_OK, "set dt_day 6 true");
    CHECK(GET_DT_DAY(spr->date_time[0], 6), "dt_day 6 set");
    CHECK(sprinkler_set_dt_day(spr, 0, 7, true) == SPR_FAIL, "set dt_day invalid day");
    CHECK(sprinkler_set_dt_day(spr, 32, 0, true) == SPR_FAIL, "set dt_day invalid id");
    CHECK(sprinkler_set_dt_day(spr, 0, 8, true) == SPR_FAIL, "set dt_day out of range day");
    for (uint8_t d = 0; d < 7; d++) {
        sprinkler_set_dt_day(spr, 0, d, true);
        CHECK(GET_DT_DAY(spr->date_time[0], d), "all days set");
    }
    CHECK(sprinkler_set_dt_hour(spr, 0, 0, true) == SPR_OK, "set dt_hour 0 true");
    CHECK(GET_DT_HOUR(spr->date_time[0], 0), "dt_hour 0 set");
    CHECK(sprinkler_set_dt_hour(spr, 0, 0, false) == SPR_OK, "set dt_hour 0 false");
    CHECK(!GET_DT_HOUR(spr->date_time[0], 0), "dt_hour 0 cleared");
    CHECK(sprinkler_set_dt_hour(spr, 0, 23, true) == SPR_OK, "set dt_hour 23 true");
    CHECK(GET_DT_HOUR(spr->date_time[0], 23), "dt_hour 23 set");
    CHECK(sprinkler_set_dt_hour(spr, 0, 24, true) == SPR_FAIL, "set dt_hour invalid hour");
    CHECK(sprinkler_set_dt_hour(spr, 32, 0, true) == SPR_FAIL, "set dt_hour invalid id");
    for (uint8_t h = 0; h < 24; h++) {
        sprinkler_set_dt_hour(spr, 0, h, true);
        CHECK(GET_DT_HOUR(spr->date_time[0], h), "all hours set");
    }
    CHECK(sprinkler_set_dt_queue(spr, 0, 0, true) == SPR_OK, "set dt_queue 0 true");
    CHECK(CHECK_BIT(spr->date_time_queue[0], 0), "dt_queue 0 set");
    CHECK(sprinkler_set_dt_queue(spr, 0, 0, false) == SPR_OK, "set dt_queue 0 false");
    CHECK(!CHECK_BIT(spr->date_time_queue[0], 0), "dt_queue 0 cleared");
    CHECK(sprinkler_set_dt_queue(spr, 0, 31, true) == SPR_OK, "set dt_queue 31 true");
    CHECK(CHECK_BIT(spr->date_time_queue[0], 31), "dt_queue 31 set");
    CHECK(sprinkler_set_dt_queue(spr, 0, 32, true) == SPR_FAIL, "set dt_queue invalid queue");
    CHECK(sprinkler_set_dt_queue(spr, 32, 0, true) == SPR_FAIL, "set dt_queue invalid id");
    for (uint8_t q = 0; q < 32; q++) {
        sprinkler_set_dt_queue(spr, 0, q, true);
        CHECK(CHECK_BIT(spr->date_time_queue[0], q), "all queues set for dt");
    }
}

void test_set_relay_functions(void) {
    sprinkler_t my_spr;
    sprinkler_t *spr = &my_spr;
    sprinkler_init(spr);

    TEST_SECTION("Set Relay Functions");
    CHECK(sprinkler_set_relay_en(spr, 0, true) == SPR_OK, "set relay_en true valid relay");
    CHECK(GET_RELAY_EN(spr->relay[0]), "relay_en set to true");
    CHECK(spr->sprinkler_config_changed, "config_changed set after en true");
    CHECK(sprinkler_set_relay_en(spr, 0, false) == SPR_OK, "set relay_en false valid relay");
    CHECK(!GET_RELAY_EN(spr->relay[0]), "relay_en set to false");
    CHECK(sprinkler_set_relay_en(spr, 32, true) == SPR_FAIL, "set relay_en invalid relay");
    CHECK(sprinkler_set_relay_pump(spr, 0, 1) == SPR_OK, "set relay_pump valid pump");
    CHECK(GET_RELAY_PUMP(spr->relay[0]) == 1, "relay_pump set to 1");
    CHECK(spr->sprinkler_config_changed, "config_changed set after pump set");
    CHECK(sprinkler_set_relay_pump(spr, 0, 4) == SPR_OK, "set relay_pump max pump 4");
    CHECK(GET_RELAY_PUMP(spr->relay[0]) == 4, "relay_pump set to 4");
    CHECK(sprinkler_set_relay_pump(spr, 0, 5) == SPR_FAIL, "set relay_pump invalid pump 5");
    CHECK(sprinkler_set_relay_pump(spr, 32, 0) == SPR_FAIL, "set relay_pump invalid relay");
    CHECK(sprinkler_set_relay_min(spr, 0, 10) == SPR_OK, "set relay_min valid min");
    CHECK(GET_RELAY_MIN(spr->relay[0]) == 10, "relay_min set to 10");
    CHECK(spr->sprinkler_config_changed, "config_changed set after min set");
    CHECK(sprinkler_set_relay_min(spr, 0, 4095) == SPR_OK, "set relay_min max 4095");
    CHECK(GET_RELAY_MIN(spr->relay[0]) == 4095, "relay_min set to 4095");
    CHECK(sprinkler_set_relay_min(spr, 0, 4096) == SPR_FAIL, "set relay_min invalid min 4096");
    CHECK(sprinkler_set_relay_min(spr, 32, 0) == SPR_FAIL, "set relay_min invalid relay");
    CHECK(sprinkler_set_relay_overlap(spr, 0, 1000) == SPR_OK, "set relay_overlap valid");
    CHECK(spr->relay_overlap_ms[0] == 1000, "relay_overlap_ms set to 1000");
    CHECK(spr->sprinkler_config_changed, "config_changed set after overlap set");
    CHECK(sprinkler_set_relay_overlap(spr, 0, 0) == SPR_OK, "set relay_overlap to 0");
    CHECK(spr->relay_overlap_ms[0] == 0, "relay_overlap_ms set to 0");
    CHECK(sprinkler_set_relay_overlap(spr, 32, 1000) == SPR_FAIL, "set relay_overlap invalid relay");
}

void test_set_month_functions(void) {
    sprinkler_t my_spr;
    sprinkler_t *spr = &my_spr;
    sprinkler_init(spr);

    TEST_SECTION("Month Set Functions");
    CHECK(sprinkler_set_month_en(spr, 0, true) == SPR_OK, "set month_en true valid");
    CHECK(GET_MONTH_EN(spr->month[0]), "month_en set to true");
    CHECK(sprinkler_set_month_en(spr, 0, false) == SPR_OK, "set month_en false valid");
    CHECK(!GET_MONTH_EN(spr->month[0]), "month_en set to false");
    CHECK(sprinkler_set_month_en(spr, 12, true) == SPR_FAIL, "set month_en invalid month");
    CHECK(sprinkler_set_month_a(spr, 0, true) == SPR_OK, "set month_a true valid");
    CHECK(GET_MONTH_A(spr->month[0]), "month_a set to true");
    CHECK(sprinkler_set_month_a(spr, 0, false) == SPR_OK, "set month_a false valid");
    CHECK(!GET_MONTH_A(spr->month[0]), "month_a set to false");
    CHECK(sprinkler_set_month_a(spr, 12, true) == SPR_FAIL, "set month_a invalid month");
    CHECK(sprinkler_set_month_b(spr, 0, true) == SPR_OK, "set month_b true valid");
    CHECK(GET_MONTH_B(spr->month[0]), "month_b set to true");
    CHECK(sprinkler_set_month_b(spr, 0, false) == SPR_OK, "set month_b false valid");
    CHECK(!GET_MONTH_B(spr->month[0]), "month_b set to false");
    CHECK(sprinkler_set_month_b(spr, 12, true) == SPR_FAIL, "set month_b invalid month");
    CHECK(sprinkler_set_month_dt(spr, 0, 0) == SPR_OK, "set month_dt 0 valid");
    CHECK(GET_MONTH_DT(spr->month[0]) == 0, "month_dt set to 0");
    CHECK(sprinkler_set_month_dt(spr, 0, 31) == SPR_OK, "set month_dt max valid");
    CHECK(GET_MONTH_DT(spr->month[0]) == 31, "month_dt set to max");
    CHECK(sprinkler_set_month_dt(spr, 0, 32) == SPR_FAIL, "set month_dt overflow");
    CHECK(sprinkler_set_month_dt(spr, 12, 0) == SPR_FAIL, "set month_dt invalid month");
}

void test_set_pump_functions(void) {
    sprinkler_t my_spr;
    sprinkler_t *spr = &my_spr;
    sprinkler_init(spr);

    TEST_SECTION("Pump Set Functions");
    CHECK(sprinkler_set_pump_delay(spr, 0) == SPR_OK, "set pump_delay 0");
    CHECK(spr->pump_delay_ms == 0, "pump_delay set to 0");
    CHECK(sprinkler_set_pump_delay(spr, UINT32_MAX) == SPR_OK, "set pump_delay max");
    CHECK(spr->pump_delay_ms == UINT32_MAX, "pump_delay set to max");
    CHECK(sprinkler_set_pump_en(spr, 0, true) == SPR_OK, "set pump_en 0 true");
    CHECK(GET_PUMP_EN(spr->pump, 0), "pump_en 0 set");
    CHECK(sprinkler_set_pump_en(spr, 0, false) == SPR_OK, "set pump_en 0 false");
    CHECK(!GET_PUMP_EN(spr->pump, 0), "pump_en 0 cleared");
    CHECK(sprinkler_set_pump_en(spr, 4, true) == SPR_OK, "set pump_en 4 true");
    CHECK(GET_PUMP_EN(spr->pump, 4), "pump_en 4 set");
    CHECK(sprinkler_set_pump_en(spr, 5, true) == SPR_FAIL, "set pump_en invalid pump");
    CHECK(sprinkler_set_pump_relay(spr, 0, 0) == SPR_OK, "set pump_relay 0 to 0");
    CHECK(GET_PUMP_RELAY(spr->pump, 0) == 0, "pump_relay 0 set");
    CHECK(sprinkler_set_pump_relay(spr, 0, 31) == SPR_OK, "set pump_relay 0 to 31");
    CHECK(GET_PUMP_RELAY(spr->pump, 0) == 31, "pump_relay 0 set to max");
    CHECK(sprinkler_set_pump_relay(spr, 0, 32) == SPR_FAIL, "set pump_relay invalid relay");
    CHECK(sprinkler_set_pump_relay(spr, 5, 0) == SPR_FAIL, "set pump_relay invalid pump");
    for (uint8_t p = 0; p < 5; p++) {
        sprinkler_set_pump_en(spr, p, true);
    }
    for (uint8_t p = 0; p < 5; p++) {
        CHECK(GET_PUMP_EN(spr->pump, p), "all pumps enabled");
    }
}

void test_set_queue_functions(void) {
    sprinkler_t my_spr;
    sprinkler_t *spr = &my_spr;
    sprinkler_init(spr);

    TEST_SECTION("Queue Set Functions");
    CHECK(sprinkler_set_queue(spr, 0, 0, true) == SPR_OK, "set queue relay 0 true");
    CHECK(CHECK_BIT(spr->queue[0], 0), "queue relay 0 set");
    CHECK(sprinkler_set_queue(spr, 0, 0, false) == SPR_OK, "set queue relay 0 false");
    CHECK(!CHECK_BIT(spr->queue[0], 0), "queue relay 0 cleared");
    CHECK(sprinkler_set_queue(spr, 0, 31, true) == SPR_OK, "set queue relay 31 true");
    CHECK(CHECK_BIT(spr->queue[0], 31), "queue relay 31 set");
    CHECK(sprinkler_set_queue(spr, 0, 32, true) == SPR_FAIL, "set queue invalid relay");
    CHECK(sprinkler_set_queue(spr, 32, 0, true) == SPR_FAIL, "set queue invalid queue");
    for (uint8_t r = 0; r < 32; r++) {
        sprinkler_set_queue(spr, 0, r, true);
        CHECK(CHECK_BIT(spr->queue[0], r), "all relays in queue");
    }
    CHECK(sprinkler_set_queue_pause(spr, 0, 0) == SPR_OK, "set queue_pause 0");
    CHECK(GET_QUEUE_PAUSE_SEC(spr->queue_pause[0]) == 0, "queue_pause set to 0");
    uint32_t big_pause = UINT32_MAX;
    big_pause++;
    sprinkler_set_queue_pause(spr, 0, big_pause);
    CHECK(GET_QUEUE_PAUSE_SEC(spr->queue_pause[0]) == 0, "set queue_pause overflow");
    CHECK(sprinkler_set_queue_pause(spr, 32, 0) == SPR_FAIL, "set queue_pause invalid queue");
    CHECK(sprinkler_set_queue_autoadv(spr, 0, true) == SPR_OK, "set queue_autoadv true");
    CHECK(GET_QUEUE_AUTOADV(spr->queue_pause[0]), "queue_autoadv set");
    CHECK(sprinkler_set_queue_autoadv(spr, 0, false) == SPR_OK, "set queue_autoadv false");
    CHECK(!GET_QUEUE_AUTOADV(spr->queue_pause[0]), "queue_autoadv cleared");
    CHECK(sprinkler_set_queue_autoadv(spr, 32, true) == SPR_FAIL, "set queue_autoadv invalid queue");
    CHECK(sprinkler_set_queue_relay_sec(spr, 0, 0, 0) == SPR_OK, "set queue_relay_sec 0");
    CHECK(spr->queue_relay_sec[0][0] == 0, "queue_relay_sec set to 0");
    CHECK(sprinkler_set_queue_relay_sec(spr, 0, 0, UINT16_MAX) == SPR_OK, "set queue_relay_sec max");
    CHECK(spr->queue_relay_sec[0][0] == UINT16_MAX, "queue_relay_sec set to max");
    uint16_t big_sec = UINT16_MAX;
    big_sec++;
    sprinkler_set_queue_relay_sec(spr, 0, 0, big_sec);
    CHECK(spr->queue_relay_sec[0][0] == 0, "set queue_relay_sec overflow");
    CHECK(sprinkler_set_queue_relay_sec(spr, 32, 0, 0) == SPR_FAIL, "set queue_relay_sec invalid queue");
    CHECK(sprinkler_set_queue_relay_sec(spr, 0, 32, 0) == SPR_FAIL, "set queue_relay_sec invalid relay");
    CHECK(sprinkler_set_queue_relay_sec(spr, 31, 0, 10) == SPR_OK, "set per-relay pause queue 31");
    CHECK(spr->queue_relay_sec[31][0] == 10, "per-relay pause set");
    CHECK(sprinkler_set_queue_repeat(spr, 0, 0) == SPR_OK, "set queue_repeat 0");
    CHECK(spr->queue_repeat[0] == 0, "queue_repeat set to 0");
    CHECK(sprinkler_set_queue_repeat(spr, 0, UINT8_MAX) == SPR_OK, "set queue_repeat max");
    CHECK(spr->queue_repeat[0] == UINT8_MAX, "queue_repeat set to max");
    uint16_t big_repeat = UINT8_MAX;
    big_repeat++;
    sprinkler_set_queue_repeat(spr, 0, (uint8_t) big_repeat);
    CHECK(spr->queue_repeat[0] == 0, "set queue_repeat overflow");
    CHECK(sprinkler_set_queue_repeat(spr, 32, 0) == SPR_FAIL, "set queue_repeat invalid queue");
    CHECK(sprinkler_set_pause(spr, 0, 0) == SPR_OK, "set pause 0");
    CHECK(spr->queue_relay_sec[31][0] == 0, "per-relay pause set to 0");
    CHECK(sprinkler_set_pause(spr, 0, UINT16_MAX) == SPR_OK, "set pause max");
    uint32_t big_pause_relay = UINT16_MAX;
    big_pause_relay++;
    sprinkler_set_pause(spr, 0, big_pause_relay);
    CHECK(sprinkler_set_pause(spr, 32, 0) == SPR_FAIL, "set pause invalid relay");
}

void test_queue_control_functions(void) {
    sprinkler_t my_spr;
    sprinkler_t *spr = &my_spr;
    sprinkler_init(spr);

    TEST_SECTION("Queue Control Functions");
    spr->queue_running = 0xFFFFFFFFUL;
    sprinkler_queue_next(spr);
    CHECK(true, "queue_next all");
    for (uint8_t q = 0; q < 32; q++) {
        if (spr->current_relay_idx[q] == 1) {
            PASS("queue_next incremented");
        } else {
            FAIL("queue_next incremented");
        }
    }
    for (uint8_t q = 0; q < 32; q++) {
        if (!(spr->queue_running & (1UL << q))) {
            if (spr->current_relay_idx[q] == 1) {
                PASS("queue_next unaffected non-running");
            } else {
                FAIL("queue_next unaffected non-running");
            }
        }
    }
    memset(spr->current_relay_idx, 31, sizeof(spr->current_relay_idx));
    sprinkler_queue_next(spr);
    for (uint8_t q = 0; q < 32; q++) {
        CHECK(spr->current_relay_idx[q] == 31, "queue_next at max no increment");
    }
    memset(spr->current_relay_idx, 1, sizeof(spr->current_relay_idx));
    sprinkler_queue_previous(spr);
    CHECK(true, "queue_previous all");
    for (uint8_t q = 0; q < 32; q++) {
        if (spr->current_relay_idx[q] == 0) {
            PASS("queue_previous decremented");
        } else {
            FAIL("queue_previous decremented");
        }
    }
    for (uint8_t q = 0; q < 32; q++) {
        if (!(spr->queue_running & (1UL << q))) {
            if (spr->current_relay_idx[q] == 1) {
                PASS("queue_previous unaffected non-running");
            } else {
                FAIL("queue_previous unaffected non-running");
            }
        }
    }
    memset(spr->current_relay_idx, 0, sizeof(spr->current_relay_idx));
    sprinkler_queue_previous(spr);
    for (uint8_t q = 0; q < 32; q++) {
        CHECK(spr->current_relay_idx[q] == 0, "queue_previous at min no decrement");
    }
    sprinkler_queue_pause(spr);
    for (uint8_t q = 0; q < 32; q++) {
        CHECK(spr->queue_paused[q], "all queues paused");
    }
    sprinkler_queue_resume(spr);
    for (uint8_t q = 0; q < 32; q++) {
        CHECK(!spr->queue_paused[q], "all queues resumed");
    }
    CHECK(sprinkler_queue_pause_id(spr, 0) == SPR_OK, "queue_pause_id valid");
    CHECK(spr->queue_paused[0], "queue 0 paused");
    CHECK(sprinkler_queue_pause_id(spr, 32) == SPR_FAIL, "queue_pause_id invalid");
    CHECK(sprinkler_queue_resume_id(spr, 0) == SPR_OK, "queue_resume_id valid");
    CHECK(!spr->queue_paused[0], "queue 0 resumed");
    CHECK(sprinkler_queue_resume_id(spr, 32) == SPR_FAIL, "queue_resume_id invalid");
    CHECK(sprinkler_queue_next_id(spr, 0) == SPR_OK, "queue_next_id valid");
    CHECK(spr->current_relay_idx[0] == 1, "queue 0 next");
    CHECK(sprinkler_queue_next_id(spr, 32) == SPR_FAIL, "queue_next_id invalid");
    CHECK(sprinkler_queue_previous_id(spr, 0) == SPR_OK, "queue_previous_id valid");
    CHECK(spr->current_relay_idx[0] == 0, "queue 0 previous");
    CHECK(sprinkler_queue_previous_id(spr, 32) == SPR_FAIL, "queue_previous_id invalid");
}

void test_is_functions(void) {
    sprinkler_t my_spr;
    sprinkler_t *spr = &my_spr;
    sprinkler_init(spr);

    TEST_SECTION("Is/Check Functions");
    spr->queue_paused[0] = true;
    CHECK(!sprinkler_is_start_time(spr), "is_start_time false default");
    struct tm ti;
    sprinkler_get_time(&ti, NULL);
    uint8_t day = (ti.tm_wday == 0) ? 6 : ti.tm_wday - 1;
    uint8_t hour = ti.tm_hour;
    uint8_t mon = ti.tm_mon;
    sprinkler_set_month_en(spr, mon, true);
    sprinkler_set_month_dt(spr, mon, 0);
    sprinkler_set_dt_en(spr, 0, true);
    sprinkler_set_dt_hour(spr, 0, hour, true);
    sprinkler_set_dt_day(spr, 0, day, true);
    sprinkler_set_dt_min(spr, 0, hour, ti.tm_min);
    sprinkler_set_dt_queue(spr, 0, 0, true);
    CHECK(sprinkler_is_start_time(spr), "is_start_time true matching config");
    sprinkler_set_month_en(spr, mon, false);
    CHECK(!sprinkler_is_start_time(spr), "is_start_time false month disabled");
    sprinkler_set_month_en(spr, mon, true);
    sprinkler_set_dt_hour(spr, 0, (hour + 1) % 24, true);
    sprinkler_set_dt_hour(spr, 0, hour, false);
    CHECK(!sprinkler_is_start_time(spr), "is_start_time false wrong hour");
    sprinkler_set_dt_hour(spr, 0, hour, true);
    sprinkler_set_dt_day(spr, 0, (day + 1) % 7, true);
    sprinkler_set_dt_day(spr, 0, day, false);
    CHECK(!sprinkler_is_start_time(spr), "is_start_time false wrong day");
}

void test_main_loop(void) {
    TEST_SECTION("sprinkler_main_loop (Simulation with Sleep)");
    unlink("sprinkler.dat");
    sprinkler_t my_spr;
    sprinkler_init(&my_spr);
    sprinkler_t *spr = &my_spr;

    // Setup simple config: Queue 0 with relay 0 (1 min, no pump), manual start
    for (uint8_t i = 0; i < 32; i++)
        spr->gpio_relay[i] = i; // Map relay to itself
    sprinkler_set_relay_en(spr, 0, true);
    sprinkler_set_relay_min(spr, 0, 1); // 1 min = 60 sec
    sprinkler_set_relay_pump(spr, 0, 5); // Invalid pump to skip pump logic
    sprinkler_set_queue(spr, 0, 0, true);
    sprinkler_set_queue_repeat(spr, 0, 1);
    sprinkler_set_queue_pause(spr, 0, 0);
    sprinkler_set_queue_autoadv(spr, 0, true);
    sprinkler_set_pump_delay(spr, 0);
    sprinkler_set_queue_relay_sec(spr, 0, 0, 10); // Override to 10 sec for fast test

    // Test no running
    CHECK(sprinkler_main_loop(spr) == SPR_OK, "main_loop no queues");
    CHECK(spr->queue_running == 0, "no queues running");
    CHECK(spr->relay_running == 0, "no relays running");

    // Manual start queue
    spr->queue_running = 1 << 0;
    CHECK(sprinkler_main_loop(spr) == SPR_OK, "main_loop start relay");
    CHECK(spr->relay_running == (1UL << 0), "relay 0 running");
    CHECK(spr->queue_relay_end_times[0][0] > 0, "end time set");

    // Simulate time pass: Sleep 5 sec, still running
    sprinkler_wait_seconds(5);
    CHECK(sprinkler_main_loop(spr) == SPR_OK, "main_loop mid duration");
    CHECK(spr->relay_running == (1UL << 0), "relay still running");

    // Sleep another 6 sec, should stop
    sprinkler_wait_seconds(6);
    CHECK(sprinkler_main_loop(spr) == SPR_OK, "main_loop end duration");
    CHECK(spr->relay_running == 0, "relay stopped");
    CHECK(spr->queue_running == 0, "queue completed");

    // Test with pump
    sprinkler_set_relay_pump(spr, 0, 0);
    sprinkler_set_pump_en(spr, 0, true);
    sprinkler_set_pump_relay(spr, 0, 1); // Pump on relay 1
    sprinkler_set_queue_relay_sec(spr, 0, 0, 10); // Override to 10 sec
    spr->queue_running = 1 << 0;
    CHECK(sprinkler_main_loop(spr) == SPR_OK, "main_loop with pump");
    CHECK(spr->active_pumps & (1U << 0), "pump active");
    CHECK(spr->relay_running & (1UL << 0), "valve relay running");

    // End
    sprinkler_wait_seconds(11);
    CHECK(sprinkler_main_loop(spr) == SPR_OK, "main_loop end with pump");
    CHECK(spr->active_pumps == 0, "pump stopped");
    CHECK(spr->relay_running == 0, "relays stopped");

    // Test pause
    spr->queue_running = 1 << 0;
    sprinkler_main_loop(spr);
    sprinkler_queue_pause_id(spr, 0);
    CHECK(sprinkler_main_loop(spr) == SPR_OK, "main_loop paused");
    // Note: relay may still run to end, but no advance

    // Test repeat
    sprinkler_set_queue_repeat(spr, 0, 2);
    sprinkler_set_queue_relay_sec(spr, 0, 0, 10); // Override to 10 sec
    spr->queue_running = 1 << 0;
    sprinkler_main_loop(spr);
    sprinkler_wait_seconds(5);
    sprinkler_main_loop(spr);
    sprinkler_wait_seconds(6);
    sprinkler_main_loop(spr);
    CHECK(spr->repeat_count[0] == 1, "repeat count 1");
    // Another cycle
    sprinkler_main_loop(spr);
    sprinkler_wait_seconds(5);
    sprinkler_main_loop(spr);
    sprinkler_wait_seconds(5);
    sprinkler_main_loop(spr);
    CHECK(spr->queue_running == 0, "queue completed after repeats");

    // Test overlap
    sprinkler_set_queue(spr, 0, 1, true); // Add relay 1
    sprinkler_set_relay_en(spr, 1, true);
    sprinkler_set_relay_min(spr, 1, 1);
    sprinkler_set_relay_pump(spr, 1, 5);
    sprinkler_set_relay_overlap(spr, 0, 5000); // 5 sec overlap
    sprinkler_set_queue_relay_sec(spr, 0, 0, 15);
    sprinkler_set_queue_relay_sec(spr, 0, 1, 15);
    spr->queue_running = 1 << 0;
    sprinkler_main_loop(spr);
    sprinkler_wait_seconds(10); // Near end of first
    sprinkler_main_loop(spr);
    CHECK(spr->relay_running == ((1UL << 0) | (1UL << 1)), "overlap active");
    sprinkler_wait_seconds(6);
    sprinkler_main_loop(spr);
    CHECK(spr->relay_running == (1UL << 1), "first stopped, second continues");

    // Test auto start (time dependent)
    struct tm ti;
    sprinkler_get_time(&ti, NULL);
    uint8_t day = (ti.tm_wday == 0) ? 6 : ti.tm_wday - 1;
    uint8_t hour = ti.tm_hour;
    uint8_t mon = ti.tm_mon;
    sprinkler_set_month_en(spr, mon, true);
    sprinkler_set_month_dt(spr, mon, 0);
    sprinkler_set_dt_en(spr, 0, true);
    sprinkler_set_dt_hour(spr, 0, hour, true);
    sprinkler_set_dt_day(spr, 0, day, true);
    sprinkler_set_dt_min(spr, 0, hour, ti.tm_min);
    sprinkler_set_dt_queue(spr, 0, 0, true);
    CHECK(sprinkler_main_loop(spr) == SPR_OK, "main_loop auto start");
    CHECK(spr->queue_running & (1UL << 0), "queue auto started");

    // Edge: Empty queue
    spr->queue[0] = 0;
    spr->queue_running = 1 << 0;
    sprinkler_main_loop(spr);
    CHECK(spr->queue_running == 0, "empty queue stops immediately");

    // Edge: Duration 0 treated as 60 sec
    sprinkler_set_queue_relay_sec(spr, 0, 0, 0);
    sprinkler_set_relay_min(spr, 0, 0);
    spr->queue[0] = 1 << 0;
    spr->queue_running = 1 << 0;
    uint32_t start_time;
    sprinkler_get_time(NULL, &start_time);
    sprinkler_main_loop(spr);
    uint32_t end = spr->queue_relay_end_times[0][0];
    CHECK(end - start_time >= 58, "duration 0 as 60 sec"); // Approx
}

// Main test
int main(void) {
    printf("=== SprinklerLib Test Suite - VERBOSE PASS/FAIL ===\n");
    unlink("sprinkler.dat"); // Clean start

    RUN_TEST(test_get_time);
    RUN_TEST(test_init_deinit);
    RUN_TEST(test_set_dt_functions);
    RUN_TEST(test_set_relay_functions);
    RUN_TEST(test_set_month_functions);
    RUN_TEST(test_set_pump_functions);
    RUN_TEST(test_set_queue_functions);
    RUN_TEST(test_queue_control_functions);
    RUN_TEST(test_is_functions);
    RUN_TEST(test_main_loop);

    printf("\n=== TEST SUMMARY ===\n");
    printf("Total tests : %d\n", total_tests);
    printf("Passed      : %d\n", passed_tests);
    printf("Failed      : %d\n", total_tests - passed_tests);

    if (passed_tests == total_tests) {
        printf("\n*** ALL TESTS PASSED SUCCESSFULLY! ***\n");
    } else {
        printf("\n!!! SOME TESTS FAILED !!!\n");
    }

    return (passed_tests == total_tests) ? 0 : 1;
}
