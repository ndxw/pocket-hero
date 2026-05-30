#include <esp32-hal-timer.h>
#include <esp32-hal-gpio.h>
#include <TFT_eSPI.h>  // Hardware-specific library

#include "runway.h"
#include "debouncer.h"

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library
Runway runway(tft);
Debouncer debouncer;

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define SB_X 0
#define SB_Y 0
#define SB_W SCREEN_WIDTH
#define SB_H 40
#define PROGRESS_X 0
#define PROGRESS_Y SB_Y + SB_H + 4
#define PROGRESS_W 19
#define PROGRESS_H 276
#define ACC_METER_X 222
#define ACC_METER_Y PROGRESS_Y
#define ACC_METER_W SCREEN_WIDTH - ACC_METER_X
#define ACC_METER_H 276


// windows for hitting 300, 100, and 50 in milliseconds
// approx. OD 9 in osu!
#define PERFECT_WINDOW 50.0
#define GREAT_WINDOW 120.0
#define GOOD_WINDOW 220.0

#define LOOP_PERIOD 30     // Display updates every 35 ms
#define DEBOUNCE_DELAY 10  // ms

//#define OVERWRITE_METER


volatile unsigned long lastPressTime0 = 0;
volatile unsigned long lastPressTime1 = 0;
volatile unsigned long lastPressTime2 = 0;
volatile unsigned long lastPressTime3 = 0;
volatile uint8_t buttonsState = 0xff;
volatile int counter = 0;
uint32_t updateTime = 0;  // time for next update
float value = 0;
float hit_dev = -300.0;

// pointer begins in center of meter
int prev_acc_pointer_pos = ACC_METER_Y + ACC_METER_H / 2 - 2;
uint16_t prev_acc_pointer_row_colours[] = { TFT_CYAN, TFT_CYAN, TFT_CYAN, TFT_CYAN };


void initTimer(void) {
  hw_timer_t* timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &timerHandler);
  timerAlarm(timer, 1000, true, 0);
}

void ARDUINO_ISR_ATTR timerHandler()
{
  debouncer.processButtons(GPIO.in);
  if (debouncer.buttonPressed(BIT(0)))
  {
    counter++;
  }
  else if (debouncer.buttonPressed(BIT(1)))
  {
    counter+=2;
  }
  else if (debouncer.buttonPressed(BIT(2)))
  {
    counter+=3;
  }
  else if (debouncer.buttonPressed(BIT(3)))
  {
    counter+=4;
  }
}

void initPins(void) {
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
}


void setup(void) {
  initPins();
  initTimer();
  tft.init();
  tft.setRotation(0);
  Serial.begin(57600);  // For debug
  tft.fillScreen(TFT_BLACK);

  //dumpMacros();

  plotBorders();
  plotProgressBar(PROGRESS_X, PROGRESS_Y, PROGRESS_W, PROGRESS_H, 0.0);
  plotAccuracyMeter(ACC_METER_X, ACC_METER_Y, ACC_METER_W, ACC_METER_H);
  plotScoreboard(5318008, 727, 0.996);

  updateTime = millis();  // Next update time
}


void loop() {


  if (updateTime <= millis()) {
    updateTime = millis() + LOOP_PERIOD;

    //unsigned long t = micros();
    plotAccuracyPointer(hit_dev);
    //Serial.print(micros()-t); // Print time taken for meter update
    //Serial.println(" us");
    hit_dev += 1.0;
    if (hit_dev > 300.0) hit_dev = -300.0;

    runway.updateRunway();
    runway.plotRunway();

    plotScoreboard(counter, 727, 0.996);
    plotProgressBar(PROGRESS_X, PROGRESS_Y, PROGRESS_W, PROGRESS_H, runway.getProgress());
  }
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
* Plots accuracy meter pointer
*
* @param hit_deviation Number of milliseconds the hit was away from a perfect (0 ms) hit. Can be negative.
*/
void plotAccuracyPointer(float hit_deviation) {
  // first erase the old pointer by overwriting it with meter colours
  for (int i = 0; i < 4; i++) {
    tft.drawFastHLine(ACC_METER_X, prev_acc_pointer_pos + i, ACC_METER_W, prev_acc_pointer_row_colours[i]);
  }

  // calculate new pointer position
  int acc_meter_center = 180;
  int pointer_position = acc_meter_center + hit_deviation * 150.0 / 220.0;  // height of good window (px) / good window (ms)
  if (pointer_position > SCREEN_HEIGHT - 4) pointer_position = SCREEN_HEIGHT - 4;
  else if (pointer_position < ACC_METER_Y) pointer_position = ACC_METER_Y;

  // save the new pointer position and meter colours
  prev_acc_pointer_pos = pointer_position;
  for (int i = 0; i < 4; i++) {
    prev_acc_pointer_row_colours[i] = tft.readPixel(ACC_METER_X, i + pointer_position);
  }

  tft.fillRect(ACC_METER_X, pointer_position, ACC_METER_W, 4, TFT_RED);
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
  tft.drawString(buffer, 3, 2, 4);
  sprintf(buffer, "x%d", multiplier);
  tft.drawString(buffer, 3, 24, 2);
  sprintf(buffer, "%.1f%%", accuracy * 100);
  tft.drawString(buffer, 198, 24, 2);
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
