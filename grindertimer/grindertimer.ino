#include "TM1637.h"
#include "ezButton.h"

// Pins definitions for TM1637 and can be changed to other ports
const int LCD_CLK = SCL;
const int LCD_DIO = SDA;

const int GRIND_BTN_PIN = A0;
const int RESET_BTN_PIM = A1;
const int UP_BTN_PIN = A2;
const int DOWN_BTN_PIN = A3;

TM1637 tm1637(LCD_CLK, LCD_DIO);

const int STATE_READY = 1;
const int STATE_RUNNING = 2;
const int STATE_PAUSED = 3;

int state = STATE_READY;
long timerPreset = 15000;
long timerStart;
long timerRemaining;

void setup() {
    tm1637.init();
    tm1637.set(BRIGHT_TYPICAL);
    tm1637.point(1);
}

void displayTime(long timeMillis) {
  float timeValue = ((float)timeMillis / 1000);
  tm1637.displayNum(timeValue, 2);
}

void handleReady() {
  timerRemaining = timerPreset;
  timerStart = millis();
  state = STATE_RUNNING;
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

}

void loop() {
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
