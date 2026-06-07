#include <esp32-hal-timer.h>
#include <esp32-hal-gpio.h>
#include <TFT_eSPI.h>  // Hardware-specific library

#include "main_menu.h"
#include "song_select.h"
#include "song_selection.h"
#include "beatmap_attempt.h"
#include "result_page.h"
#include "leaderboard.h"
#include "debouncer.h"
#include "helpers.h"

Pages currentPage = Pages::MainMenu;

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library
MainMenu mainMenu(tft, currentPage);
SongSelect songSelect(tft, currentPage);
SongSelection songSelection(tft, currentPage);
BeatmapAttempt beatmapAttempt(tft, currentPage);
ResultPage resultPage(tft, currentPage);
Leaderboard leaderboard(tft, currentPage);
Debouncer debouncer;

#define LOOP_PERIOD 30     // Display updates every 35 ms

volatile int counter = 0;
uint32_t updateTime = 0;  // time for next update

hw_timer_t* timer;

void setup(void) {
  Serial.begin(57600);  // For debug

  initPins();
  initTimer();
  tft.init();

  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  timerStart(timer);
  updateTime = millis();  // Next update time
}


void loop() {

  if (updateTime <= millis()) 
  {
    updateTime = millis() + LOOP_PERIOD;

    switch (currentPage) 
    {
      case Pages::MainMenu:
        tft.fillScreen(TFT_BLACK);
        mainMenu.reset();
        while (currentPage == Pages::MainMenu)
        {
          mainMenu.plot();
          mainMenu.processInputs();
        }
        break;

      case Pages::SongSelect:
        tft.fillScreen(TFT_BLACK);
        while (currentPage == Pages::SongSelect)
        {
          songSelect.plot();
          songSelect.processInputs();
        }
        break;

      case Pages::SongSelection:
        tft.fillScreen(TFT_BLACK);
        songSelection.setSong(songSelect.getCurrentSongInfo());
        while (currentPage == Pages::SongSelection)
        {
          songSelection.plot();
          songSelection.processInputs();
        }
        break;

      case Pages::BeatmapAttempt:
        tft.fillScreen(TFT_BLACK);

        beatmapAttempt.plotBorders();
        beatmapAttempt.plotAccuracyMeter();

        beatmapAttempt.loadBeatmap();

        while(!beatmapAttempt.isFinished())
        {
          beatmapAttempt.processInputs();
          beatmapAttempt.updateNotes();
          beatmapAttempt.plot();
        }
        break;

      case Pages::ResultPage:
        tft.fillScreen(TFT_BLACK);

        resultPage.setResult(beatmapAttempt.getResult());
        resultPage.plot();

        while (currentPage == Pages::ResultPage)
        {
          resultPage.processInputs();
        }
        break;

      case Pages::Leaderboard:
        tft.fillScreen(TFT_BLACK);

        leaderboard.plot();

        while (currentPage == Pages::Leaderboard)
        {
          leaderboard.processInputs();
        }
        break;
    }
  }
}

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
  uint32_t releasedButtons = debouncer.buttonReleased(0b1111);
  if (pressedButtons || releasedButtons)
  {
    switch (currentPage)
    {
      case Pages::MainMenu:
        mainMenu.queuePresses(pressedButtons); break;
      case Pages::SongSelect:
        songSelect.queuePresses(pressedButtons); break;
      case Pages::SongSelection:
        songSelection.queuePresses(pressedButtons); break;
      case Pages::BeatmapAttempt:
        beatmapAttempt.queueInputs(pressedButtons, releasedButtons); break;
      case Pages::ResultPage:
        resultPage.queuePresses(pressedButtons); break;
      case Pages::Leaderboard:
        leaderboard.queuePresses(pressedButtons); break;

    }
  }
}

void initPins(void) {
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
}

void dumpMacros(void) {
  char buffer[64];
  Serial.println("Scoreboard");
  sprintf(buffer, "X=%d, Y=%d, W=%d, H=%d", SB_X, SB_Y, SB_W, SB_H);
  Serial.println(buffer);
  Serial.println("Progress bar");
  sprintf(buffer, "X=%d, Y=%d, W=%d, H=%d", PROGRESS_X, PROGRESS_Y, PROGRESS_W, PROGRESS_H);
  Serial.println(buffer);
  Serial.println("beatmapAttempt");
  sprintf(buffer, "X=%d, Y=%d, W=%d, H=%d", RUNWAY_X, RUNWAY_Y, RUNWAY_WIDTH, RUNWAY_HEIGHT);
  Serial.println(buffer);
  Serial.println("Accuracy meter");
  sprintf(buffer, "X=%d, Y=%d, W=%d, H=%d", ACC_METER_X, ACC_METER_Y, ACC_METER_W, ACC_METER_H);
  Serial.println(buffer);
}
