#include "TM1637.h"
#include <AceButton.h>
using namespace ace_button;

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

ButtonConfig noRepeatButtonConfig;
ButtonConfig repeatButtonConfig;

AceButton grindButton(&noRepeatButtonConfig);
AceButton resetButton(&noRepeatButtonConfig);
AceButton upButton(&repeatButtonConfig);
AceButton downButton(&repeatButtonConfig);
void handleButtonEvent(AceButton*, uint8_t, uint8_t);

void setup() {
  // serial console for debugging
  Serial.begin(9600);

  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);
  tm1637.point(1);

  pinMode(GRIND_BTN_PIN, INPUT_PULLUP);
  pinMode(RESET_BTN_PIM, INPUT_PULLUP);
  pinMode(UP_BTN_PIN, INPUT_PULLUP);
  pinMode(DOWN_BTN_PIN, INPUT_PULLUP);

  // setup button configs
  noRepeatButtonConfig.setEventHandler(handleButtonEvent);
  noRepeatButtonConfig.setDebounceDelay(50);
  repeatButtonConfig.setEventHandler(handleButtonEvent);
  repeatButtonConfig.setFeature(ButtonConfig::kFeatureRepeatPress);
  repeatButtonConfig.setDebounceDelay(50);

  // setup buttons
  grindButton.init(GRIND_BTN_PIN);
  resetButton.init(RESET_BTN_PIM);
  upButton.init(UP_BTN_PIN);
  downButton.init(DOWN_BTN_PIN);

}

void displayTime(long timeMillis) {
  float timeValue = ((float)timeMillis / 1000);
  tm1637.displayNum(timeValue, 2);
}

void handleReady() {
  displayTime(timerPreset);
}

void handleRunning() {
  long currentRemaining = timerRemaining - (millis() - timerStart);
  if (currentRemaining <= 0) {
    state = STATE_READY;
    return;
  }
  displayTime(currentRemaining);
}

void handlePaused() {
  displayTime(timerRemaining);
}

void buttonsLoop() {
  grindButton.check();
  resetButton.check();
  upButton.check();
  downButton.check();
}

void handleButtonEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  switch (eventType) {
    case AceButton::kEventPressed:
    case AceButton::kEventRepeatPressed:
      if (button->getPin() == UP_BTN_PIN && state == STATE_READY) {
        timerPreset = timerPreset + TIMER_PRESET_DELTA;
        if (timerPreset > TIMER_PRESET_MAX) {
          timerPreset = TIMER_PRESET_MAX;
        }
      }
      if (button->getPin() == DOWN_BTN_PIN && state == STATE_READY) {
        timerPreset = timerPreset - TIMER_PRESET_DELTA;
        if (timerPreset < TIMER_PRESET_MIN) {
          timerPreset = TIMER_PRESET_MIN;
        }
      }
      if (button->getPin() == RESET_BTN_PIM && state == STATE_PAUSED) {
        state = STATE_READY;
      }
      if (button->getPin() == GRIND_BTN_PIN && state == STATE_PAUSED) {
        timerStart = millis();
        state = STATE_RUNNING;
      }
      if (button->getPin() == GRIND_BTN_PIN && state == STATE_READY) {
        timerRemaining = timerPreset;
        timerStart = millis();
        state = STATE_RUNNING;
      }
      break;
    case AceButton::kEventReleased:
      if (button->getPin() == GRIND_BTN_PIN && state == STATE_RUNNING) {
        state = STATE_PAUSED;
        timerRemaining = timerRemaining - (millis() - timerStart);
      }
      break;
  }
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
