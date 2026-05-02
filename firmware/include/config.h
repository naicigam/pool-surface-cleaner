#pragma once

// -----------------------------------------------------------------------------
// Pin assignments (ESP32-C3 SuperMini)
// -----------------------------------------------------------------------------
#define PIN_BAT_ADC          0     // Battery voltage divider (100k / 22k)
#define PIN_PV_ADC           1     // Solar PV voltage divider (100k / 22k)
#define PIN_BUTTON           2     // IP67 button to GND, INPUT_PULLUP
#define PIN_BASKET_FULL      3     // Reserved: float switch, INPUT_PULLUP
#define PIN_PUMP_GATE        4     // IRF520 gate (LEDC PWM)
#define PIN_LED              8     // On-board LED (active low on SuperMini)

// -----------------------------------------------------------------------------
// ADC scaling
//   Divider: V_in --[100k]--+--[22k]-- GND
//                          ADC pin
//   Ratio: V_in = V_adc * (100 + 22) / 22 = V_adc * 5.545
//   ESP32-C3 ADC: 12-bit (0..4095), default attenuation maps ~0..2.5 V at the pin
// -----------------------------------------------------------------------------
#define ADC_REF_MV           2500.0f
#define ADC_MAX              4095.0f
#define DIVIDER_RATIO        5.545f
#define ADC_TO_MILLIVOLTS(x) ((x) * (ADC_REF_MV / ADC_MAX) * DIVIDER_RATIO)

// -----------------------------------------------------------------------------
// Battery thresholds (LiFePO4 4S, 12.8 V nominal)
// -----------------------------------------------------------------------------
#define BAT_FULL_MV          14000   // ~3.50 V/cell — fully charged
#define BAT_OK_MV            13000   // ~3.25 V/cell — start pump allowed
#define BAT_LOW_MV           11600   // ~2.90 V/cell — refuse to start, sleep
#define BAT_CRITICAL_MV      11000   // ~2.75 V/cell — stop immediately

// Solar PV "is it daytime?" threshold (open-circuit panel V > this = sun is up)
#define PV_DAYLIGHT_MV       10000

// -----------------------------------------------------------------------------
// Pump control
// -----------------------------------------------------------------------------
#define PUMP_PWM_FREQ_HZ     1000
#define PUMP_PWM_RES_BITS    8
#define PUMP_PWM_CHANNEL     0
#define PUMP_DUTY_RUN        255   // full-on, 0..255 (we PWM only for soft-start)
#define PUMP_SOFTSTART_MS    500   // ramp 0 -> RUN over this period

// Maximum continuous run per wake cycle (safety)
#define MAX_RUN_MS           (60UL * 60UL * 1000UL)   // 1 hour

// -----------------------------------------------------------------------------
// Timing
// -----------------------------------------------------------------------------
#define BUTTON_DEBOUNCE_MS   50
#define WATCHDOG_TIMEOUT_MS  (5UL * 60UL * 1000UL)   // 5 min without command → stop
#define WIFI_CONNECT_TIMEOUT_MS  10000
#define DEEP_SLEEP_INTERVAL_S    (30UL * 60UL)        // wake every 30 min by default
#define LOOP_PERIOD_MS       10                       // ~100 Hz main loop

// -----------------------------------------------------------------------------
// Web server
// -----------------------------------------------------------------------------
#define HTTP_PORT            80

// -----------------------------------------------------------------------------
// Build-time switches
// -----------------------------------------------------------------------------
// Define WIFI_DISABLED to build a fully offline firmware (saves ~70 % power)
// #define WIFI_DISABLED
