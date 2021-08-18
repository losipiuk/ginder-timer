#include "TM1637.h"
#include "ezButton.h"

// Pins definitions for TM1637 and can be changed to other ports
const int LCD_CLK = SCL;
const int LCD_DIO = SDA;

const int GRIND_BTN_PIN = 5;
const int RESET_BTN_PIM = 4;
const int UP_BTN_PIN = 3;
const int DOWN_BTN_PIN = 2;

const int STATE_READY = 1;
const int STATE_RUNNING = 2;
const int STATE_PAUSED = 3;

const int BUTTON_PRESSED = LOW;
const int BUTTON_RELEASED = HIGH;

const long TIMER_PRESET_DEFAULT = 5000;
const long TIMER_PRESET_MIN = 1000;
const long TIMER_PRESET_MAX = 60000;
const long TIMER_PRESET_DELTA = 500;

int state = STATE_READY;
long timerPreset = TIMER_PRESET_DEFAULT;
long timerStart;
long timerRemaining;

// UI objects
TM1637 tm1637(LCD_CLK, LCD_DIO);
ezButton grindButton(GRIND_BTN_PIN);
ezButton resetButton(RESET_BTN_PIM);
ezButton upButton(UP_BTN_PIN);
ezButton downButton(DOWN_BTN_PIN);

void setup() {
  // serial console for debugging
  Serial.begin(9600);

  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);
  tm1637.point(1);

  grindButton.setDebounceTime(50);
  resetButton.setDebounceTime(50);
  upButton.setDebounceTime(50);
  downButton.setDebounceTime(50);
}

void displayTime(long timeMillis) {
  float timeValue = ((float)timeMillis / 1000);
  tm1637.displayNum(timeValue, 2);
}

void handleReady() {
  displayTime(timerPreset);
  if (grindButton.isPressed()) {
    timerRemaining = timerPreset;
    timerStart = millis();
    state = STATE_RUNNING;
    return;
  }
  if (upButton.isPressed()) {
    Serial.println("up!");
    timerPreset = timerPreset + TIMER_PRESET_DELTA;
    if (timerPreset > TIMER_PRESET_MAX) {
      timerPreset = TIMER_PRESET_MAX;
    }
    return;
  }
  if (downButton.isPressed()) {
    timerPreset = timerPreset - TIMER_PRESET_DELTA;
    if (timerPreset < TIMER_PRESET_MIN) {
      timerPreset = TIMER_PRESET_MIN;
    }
  }

}

void handleRunning() {
  long currentRemaining = timerRemaining - (millis() - timerStart);
  displayTime(currentRemaining);
  if (currentRemaining <= 0) {
    state = STATE_READY;
    return;
  }
  if (grindButton.getState() == BUTTON_RELEASED) {
    state = STATE_PAUSED;
    timerRemaining = currentRemaining;
    return;
  }
}

void handlePaused() {
  displayTime(timerRemaining);
  if (resetButton.isPressed()) {
    state = STATE_READY;
    return;
  }
  if (grindButton.isPressed()) {
    timerStart = millis();
    state = STATE_RUNNING;
    return;
  }
}

void buttonsLoop() {
  grindButton.loop();
  resetButton.loop();
  upButton.loop();
  downButton.loop();
}

void loop() {
  buttonsLoop();

  switch (state) {
  case STATE_READY:
    handleReady();
    break;
  case STATE_RUNNING:
    handleRunning();
    break;
  case STATE_PAUSED:
    handlePaused();
    break;
  }
}
