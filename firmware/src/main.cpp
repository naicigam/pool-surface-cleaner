// Pool Surface Cleaner — main controller
//
// Hardware: ESP32-C3 SuperMini, IRF520 MOSFET driving a 12 V bilge pump,
// 12 V LiFePO4 battery charged from a 10 W solar panel via a PWM controller.
//
// Behaviour:
//   - On boot or wake, sample battery + PV.
//   - If battery is OK and it's daylight (or the user pressed the button),
//     run the pump until either battery drops to LOW, MAX_RUN_MS elapses,
//     the user presses the button again, or the watchdog fires.
//   - Otherwise, deep-sleep until DEEP_SLEEP_INTERVAL_S or button wake.
//   - When WiFi is enabled, expose a tiny dashboard + JSON status endpoint
//     while the pump is running.

#include <Arduino.h>
#include "config.h"

#if __has_include("secrets.h")
  #include "secrets.h"
#else
  #define WIFI_SSID ""
  #define WIFI_PASSWORD ""
#endif

#ifndef WIFI_DISABLED
  #include <WiFi.h>
  #include <WebServer.h>
  static WebServer server(HTTP_PORT);
  static bool wifiAssociated = false;
#endif

#include <esp_sleep.h>
#include <driver/rtc_io.h>

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------
enum class State : uint8_t {
  BOOT,
  IDLE,
  PUMPING,
  SLEEPING_LOW_BAT,
  FAULT
};

static State        state             = State::BOOT;
static uint32_t     pumpStartMs       = 0;
static uint32_t     lastCommandMs     = 0;
static uint32_t     lastButtonEdgeMs  = 0;
static bool         lastButtonReading = HIGH;
static bool         buttonPressed     = false;

// -----------------------------------------------------------------------------
// Sensors
// -----------------------------------------------------------------------------
static uint16_t readBatteryMillivolts() {
  uint32_t acc = 0;
  for (int i = 0; i < 16; ++i) acc += analogRead(PIN_BAT_ADC);
  float adc = acc / 16.0f;
  return (uint16_t)ADC_TO_MILLIVOLTS(adc);
}

static uint16_t readPvMillivolts() {
  uint32_t acc = 0;
  for (int i = 0; i < 16; ++i) acc += analogRead(PIN_PV_ADC);
  float adc = acc / 16.0f;
  return (uint16_t)ADC_TO_MILLIVOLTS(adc);
}

static bool isBasketFull() {
  // Reserved: float switch on PIN_BASKET_FULL pulls to GND when full.
  // Currently always returns false until v2.
  return false;
}

// -----------------------------------------------------------------------------
// Pump control
// -----------------------------------------------------------------------------
static bool pumpRunning = false;

static void pumpStop() {
  ledcWrite(PUMP_PWM_CHANNEL, 0);
  pumpRunning = false;
}

static void pumpStart() {
  // Soft-start ramp to reduce inrush spike on the buck converter rail.
  for (uint16_t duty = 0; duty <= PUMP_DUTY_RUN; duty += 8) {
    ledcWrite(PUMP_PWM_CHANNEL, duty);
    delay(PUMP_SOFTSTART_MS / (PUMP_DUTY_RUN / 8 + 1));
  }
  ledcWrite(PUMP_PWM_CHANNEL, PUMP_DUTY_RUN);
  pumpRunning   = true;
  pumpStartMs   = millis();
  lastCommandMs = millis();
}

// -----------------------------------------------------------------------------
// Button (debounced edge detection)
// -----------------------------------------------------------------------------
static void scanButton() {
  bool reading = digitalRead(PIN_BUTTON);
  if (reading != lastButtonReading) {
    lastButtonEdgeMs  = millis();
    lastButtonReading = reading;
  }
  if ((millis() - lastButtonEdgeMs) > BUTTON_DEBOUNCE_MS) {
    static bool stable = HIGH;
    if (reading != stable) {
      stable = reading;
      if (stable == LOW) buttonPressed = true;   // press edge
    }
  }
}

// -----------------------------------------------------------------------------
// WiFi + tiny web dashboard
// -----------------------------------------------------------------------------
#ifndef WIFI_DISABLED

static const char DASHBOARD_HTML[] PROGMEM = R"HTML(
<!doctype html><html><head><meta charset=utf-8>
<title>Pool Surface Cleaner</title>
<style>
 body{font-family:system-ui;margin:2em;color:#222}
 h1{margin-bottom:0}
 .row{display:flex;gap:1em;margin:1em 0}
 .card{padding:1em;border:1px solid #ddd;border-radius:8px;flex:1;min-width:8em}
 .card .v{font-size:1.6em;font-weight:600}
 button{padding:.6em 1.2em;font-size:1em;border-radius:6px;border:1px solid #888;cursor:pointer}
 button.start{background:#3a7;color:#fff;border-color:#283}
 button.stop {background:#c33;color:#fff;border-color:#a22}
</style></head>
<body>
<h1>Pool Surface Cleaner</h1>
<p>Status: <span id=state>—</span></p>
<div class=row>
  <div class=card><div>Battery</div><div class=v id=bat>— V</div></div>
  <div class=card><div>Solar</div>  <div class=v id=pv>— V</div></div>
  <div class=card><div>Run time</div><div class=v id=run>—</div></div>
</div>
<div class=row>
  <button class=start onclick="cmd('start')">START</button>
  <button class=stop  onclick="cmd('stop')">STOP</button>
</div>
<script>
async function cmd(c){await fetch('/'+c,{method:'POST'});refresh()}
async function refresh(){
  let r=await fetch('/status');let s=await r.json();
  document.getElementById('state').innerText=s.state;
  document.getElementById('bat').innerText=(s.bat_mv/1000).toFixed(2)+' V';
  document.getElementById('pv').innerText =(s.pv_mv /1000).toFixed(2)+' V';
  document.getElementById('run').innerText=s.run_s+' s';
}
setInterval(refresh,1000);refresh();
</script></body></html>
)HTML";

static const char* stateName() {
  switch (state) {
    case State::BOOT:             return "BOOT";
    case State::IDLE:             return "IDLE";
    case State::PUMPING:          return "PUMPING";
    case State::SLEEPING_LOW_BAT: return "LOW_BAT";
    case State::FAULT:            return "FAULT";
  }
  return "?";
}

static void handleRoot() {
  server.send_P(200, "text/html", DASHBOARD_HTML);
}

static void handleStatus() {
  uint32_t runS = pumpRunning ? (millis() - pumpStartMs) / 1000 : 0;
  char buf[160];
  snprintf(buf, sizeof(buf),
           "{\"state\":\"%s\",\"bat_mv\":%u,\"pv_mv\":%u,\"run_s\":%lu,\"basket_full\":%s}",
           stateName(),
           readBatteryMillivolts(),
           readPvMillivolts(),
           (unsigned long)runS,
           isBasketFull() ? "true" : "false");
  server.send(200, "application/json", buf);
}

static void handleStart() {
  lastCommandMs = millis();
  if (state == State::IDLE && readBatteryMillivolts() >= BAT_OK_MV) {
    pumpStart();
    state = State::PUMPING;
  }
  server.send(200, "text/plain", "ok");
}

static void handleStop() {
  lastCommandMs = millis();
  pumpStop();
  if (state == State::PUMPING) state = State::IDLE;
  server.send(200, "text/plain", "ok");
}

static void wifiBegin() {
  if (strlen(WIFI_SSID) == 0) return;
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - t0) < WIFI_CONNECT_TIMEOUT_MS) {
    delay(100);
  }
  wifiAssociated = (WiFi.status() == WL_CONNECTED);
  if (wifiAssociated) {
    Serial.print("WiFi: ");
    Serial.println(WiFi.localIP());
    server.on("/",       HTTP_GET,  handleRoot);
    server.on("/status", HTTP_GET,  handleStatus);
    server.on("/start",  HTTP_POST, handleStart);
    server.on("/stop",   HTTP_POST, handleStop);
    server.begin();
  } else {
    Serial.println("WiFi: not connected (offline mode)");
  }
}

#endif  // WIFI_DISABLED

// -----------------------------------------------------------------------------
// Serial command parser
// -----------------------------------------------------------------------------
static void scanSerial() {
  if (!Serial.available()) return;
  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toLowerCase();
  if (cmd == "start") {
    lastCommandMs = millis();
    if (state == State::IDLE && readBatteryMillivolts() >= BAT_OK_MV) {
      pumpStart();
      state = State::PUMPING;
      Serial.println("pump: start");
    } else {
      Serial.println("pump: refused (battery low or wrong state)");
    }
  } else if (cmd == "stop") {
    pumpStop();
    if (state == State::PUMPING) state = State::IDLE;
    Serial.println("pump: stop");
  } else if (cmd == "status") {
    Serial.printf("state=%s bat=%u mV pv=%u mV run=%lu ms\n",
                  state == State::PUMPING ? "PUMPING" : "IDLE",
                  readBatteryMillivolts(),
                  readPvMillivolts(),
                  pumpRunning ? (unsigned long)(millis() - pumpStartMs) : 0UL);
  } else if (cmd == "sleep") {
    Serial.println("entering deep sleep");
    delay(50);
    esp_deep_sleep_start();
  }
}

// -----------------------------------------------------------------------------
// Deep sleep
// -----------------------------------------------------------------------------
static void enterDeepSleep(uint32_t seconds) {
  Serial.printf("deep sleep for %lus\n", (unsigned long)seconds);
  pumpStop();
  esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
  // Wake on button press: GPIO 2 going LOW.
  esp_deep_sleep_enable_gpio_wakeup(1ULL << PIN_BUTTON, ESP_GPIO_WAKEUP_GPIO_LOW);
  delay(50);
  esp_deep_sleep_start();
}

// -----------------------------------------------------------------------------
// Setup / loop
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println();
  Serial.println("pool-surface-cleaner boot");

  pinMode(PIN_BUTTON,      INPUT_PULLUP);
  pinMode(PIN_BASKET_FULL, INPUT_PULLUP);
  pinMode(PIN_LED,         OUTPUT);
  digitalWrite(PIN_LED, HIGH);   // active-low: off

  ledcSetup(PUMP_PWM_CHANNEL, PUMP_PWM_FREQ_HZ, PUMP_PWM_RES_BITS);
  ledcAttachPin(PIN_PUMP_GATE, PUMP_PWM_CHANNEL);
  ledcWrite(PUMP_PWM_CHANNEL, 0);

  uint16_t bat = readBatteryMillivolts();
  uint16_t pv  = readPvMillivolts();
  Serial.printf("boot: bat=%u mV pv=%u mV\n", bat, pv);

  if (bat < BAT_LOW_MV) {
    Serial.println("battery low at boot — going back to sleep");
    state = State::SLEEPING_LOW_BAT;
    enterDeepSleep(DEEP_SLEEP_INTERVAL_S);
  }

  bool wokeFromButton =
      esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_GPIO;
  bool isDaylight = pv >= PV_DAYLIGHT_MV;

#ifndef WIFI_DISABLED
  wifiBegin();
#endif

  if (wokeFromButton || isDaylight) {
    pumpStart();
    state         = State::PUMPING;
    lastCommandMs = millis();
    Serial.println("pump: started (button or daylight)");
  } else {
    state = State::IDLE;
    Serial.println("idle (no daylight, no button) — will sleep shortly");
  }
}

void loop() {
  uint32_t now = millis();

#ifndef WIFI_DISABLED
  if (wifiAssociated) server.handleClient();
#endif
  scanSerial();
  scanButton();

  // Button toggles between IDLE and PUMPING.
  if (buttonPressed) {
    buttonPressed = false;
    lastCommandMs = now;
    if (state == State::PUMPING) {
      pumpStop();
      state = State::IDLE;
      Serial.println("pump: stop (button)");
    } else if (state == State::IDLE) {
      if (readBatteryMillivolts() >= BAT_OK_MV) {
        pumpStart();
        state = State::PUMPING;
        Serial.println("pump: start (button)");
      } else {
        Serial.println("pump: refused (battery low)");
      }
    }
  }

  // Pump-running safety checks.
  if (state == State::PUMPING) {
    digitalWrite(PIN_LED, (now / 500) % 2 ? LOW : HIGH);   // 1 Hz heartbeat

    uint16_t bat = readBatteryMillivolts();
    if (bat < BAT_CRITICAL_MV) {
      Serial.println("pump: stop (critical low battery)");
      pumpStop();
      state = State::SLEEPING_LOW_BAT;
      enterDeepSleep(DEEP_SLEEP_INTERVAL_S);
    }
    if ((now - pumpStartMs) >= MAX_RUN_MS) {
      Serial.println("pump: stop (max run time reached)");
      pumpStop();
      state = State::IDLE;
    }
    if ((now - lastCommandMs) >= WATCHDOG_TIMEOUT_MS) {
      // No external command in a long time. That's fine — autonomous mode —
      // but bump the watchdog forward so we don't spam this branch.
      lastCommandMs = now;
    }
  } else {
    digitalWrite(PIN_LED, HIGH);   // off
  }

  // After ~30 s of idle, give up and deep-sleep.
  if (state == State::IDLE && (now - lastCommandMs) > 30000UL) {
    enterDeepSleep(DEEP_SLEEP_INTERVAL_S);
  }

  delay(LOOP_PERIOD_MS);
}
