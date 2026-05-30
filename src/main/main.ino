#include <esp32-hal-timer.h>
#include <esp32-hal-gpio.h>
#include <TFT_eSPI.h>  // Hardware-specific library

#include "runway.h"
#include "debouncer.h"

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library
Runway runway(tft);
Debouncer debouncer;

#define LOOP_PERIOD 30     // Display updates every 35 ms

volatile int counter = 0;
uint32_t updateTime = 0;  // time for next update
bool finished = false;

hw_timer_t* timer;

void initTimer(void) {
  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &timerHandler);
  timerAlarm(timer, 1000, true, 0);
  timerStop(timer);
}

void ARDUINO_ISR_ATTR timerHandler()
{
  debouncer.processButtons(GPIO.in);
  uint32_t pressedButtons = debouncer.buttonPressed(0b1111);
  if (pressedButtons)
  {
    runway.addPresses(pressedButtons, millis());
  }
}

void initPins(void) {
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
}


void setup(void) {
  Serial.begin(57600);  // For debug

  initPins();
  initTimer();
  tft.init();

  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  //dumpMacros();

  plotBorders();
  plotProgressBar(PROGRESS_X, PROGRESS_Y, PROGRESS_W, PROGRESS_H, 0.0);
  plotAccuracyMeter(ACC_METER_X, ACC_METER_Y, ACC_METER_W, ACC_METER_H);
  plotScoreboard(runway.getScore(), runway.getMultiplier(), runway.getAccuracy());

  timerStart(timer);

  updateTime = millis();  // Next update time
}


void loop() {


  if (updateTime <= millis()) {
    updateTime = millis() + LOOP_PERIOD;

    runway.calculateScore();
    runway.updateRunway();
    runway.plotRunway();

    plotScoreboard(runway.getScore(), runway.getMultiplier(), runway.getAccuracy());
    plotProgressBar(PROGRESS_X, PROGRESS_Y, PROGRESS_W, PROGRESS_H, runway.getProgress());
  }
  // remove below
  if (millis() > 8000 && !finished)
  {
    finished = true;
    BeatmapResult result = runway.getResult();
    char buffer[64];
    Serial.println("Result:");
    sprintf(buffer, "300: %d\n100: %d\n50: %d\nMiss: %d", result.num300, result.num100, result.num50, result.numMiss);
    Serial.println(buffer);
    sprintf(buffer, "Total object count: %d", result.num300 + result.num100 + result.num50 + result.numMiss);
    Serial.println(buffer);
    sprintf(buffer, "Accuracy: %f", result.accuracy);
    Serial.println(buffer);
  }
  //==============
}


/**
* Plots vertical progress bar
*
* @param x X offset
* @param y Y offset
* @param w Width of progress bar
* @param h Height of progress bar
* @param fraction_complete Value between 0 and 1 inclusive
*/
void plotProgressBar(int x, int y, int w, int h, float fraction_complete) {
  //if (fraction_complete < 0) fraction_complete = 0;
  //if (fraction_complete > 1) fraction_complete = 1;
  int height_complete = static_cast<int>(h * fraction_complete);
  tft.fillRect(x, y, w, h - height_complete, TFT_DARKGREY);
  tft.fillRect(x, y + h - height_complete + 1, w, height_complete, TFT_WHITE);
}

/**
* Plots vertical accuracy meter
*
* @param x X offset
* @param y Y offset
* @param w Width of accuracy meter bar
* @param h Height of accuracy meter bar
*/
void plotAccuracyMeter(int x, int y, int w, int h) {
  int goodHeight = 150;
  int greatHeight = GREAT_WINDOW / GOOD_WINDOW * goodHeight;
  int perfectHeight = PERFECT_WINDOW / GOOD_WINDOW * goodHeight;
  tft.fillRect(x, y, w, h, TFT_BLACK);
  tft.fillRect(x, y + (ACC_METER_H - goodHeight) / 2, w, goodHeight, TFT_ORANGE);
  tft.fillRect(x, y + (ACC_METER_H - greatHeight) / 2, w, greatHeight, TFT_GREENYELLOW);
  tft.fillRect(x, y + (ACC_METER_H - perfectHeight) / 2, w, perfectHeight, TFT_CYAN);
}



/**
* Plots white borders between panels
*/
void plotBorders(void) {
  // scoreboard border
  tft.fillRect(0, SB_H, SCREEN_WIDTH, 4, TFT_WHITE);

  // progress and accuracy meter borders
  tft.fillRect(PROGRESS_W, PROGRESS_Y, 2, PROGRESS_H, TFT_WHITE);
  tft.fillRect(ACC_METER_X - 2, ACC_METER_Y, 2, ACC_METER_H, TFT_WHITE);

  // lane lines
  tft.drawFastVLine(RUNWAY_X + 49, RUNWAY_Y, RUNWAY_HEIGHT, TFT_LIGHTGREY);
  tft.drawFastVLine(RUNWAY_X + 99, RUNWAY_Y, RUNWAY_HEIGHT, TFT_LIGHTGREY);
  tft.drawFastVLine(RUNWAY_X + 149, RUNWAY_Y, RUNWAY_HEIGHT, TFT_LIGHTGREY);
}

/**
* Plots the scoreboard at the top of the screen
*
* @param score Current score
* @param multiplier Current multiplier
* @param accuracy Current accuracy, between 0 and 1 inclusive
*/
void plotScoreboard(int score, int multiplier, float accuracy) {
  char buffer[32];
  sprintf(buffer, "%012d", score);
  tft.setTextPadding(0);
  tft.setTextDatum(BL_DATUM);
  tft.drawString(buffer, 3, 26, 4);

  sprintf(buffer, "x%d", multiplier);
  tft.setTextDatum(BL_DATUM);
  tft.drawString(buffer, 3, 38, 2);

  sprintf(buffer, "%.1f%%", accuracy * 100);
  tft.setTextDatum(BR_DATUM);
  tft.drawString(buffer, 236, 38, 2);
}


void dumpMacros(void) {
  char buffer[64];
  Serial.println("Scoreboard");
  sprintf(buffer, "X=%d, Y=%d, W=%d, H=%d", SB_X, SB_Y, SB_W, SB_H);
  Serial.println(buffer);
  Serial.println("Progress bar");
  sprintf(buffer, "X=%d, Y=%d, W=%d, H=%d", PROGRESS_X, PROGRESS_Y, PROGRESS_W, PROGRESS_H);
  Serial.println(buffer);
  Serial.println("Runway");
  sprintf(buffer, "X=%d, Y=%d, W=%d, H=%d", RUNWAY_X, RUNWAY_Y, RUNWAY_WIDTH, RUNWAY_HEIGHT);
  Serial.println(buffer);
  Serial.println("Accuracy meter");
  sprintf(buffer, "X=%d, Y=%d, W=%d, H=%d", ACC_METER_X, ACC_METER_Y, ACC_METER_W, ACC_METER_H);
  Serial.println(buffer);
}
