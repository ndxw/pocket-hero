#ifndef RUNWAY_H
#define RUNWAY_H

#include "sample_song.h"
#include "helpers.h"
#include <vector>


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

#define RUNWAY_X 21
#define RUNWAY_Y 44
#define RUNWAY_WIDTH 199
#define RUNWAY_HEIGHT 276

#define ACC_METER_X 222
#define ACC_METER_Y PROGRESS_Y
#define ACC_METER_W SCREEN_WIDTH - ACC_METER_X
#define ACC_METER_H 276

// windows for hitting 300, 100, and 50 in milliseconds
// approx. OD 9 in osu!
#define PERFECT_WINDOW 50.0
#define GREAT_WINDOW 120.0
#define GOOD_WINDOW 220.0

#define HIT_LINE_Y 220 // relative to top of runway

class Runway
{
  private:
    TFT_eSPI& tft;

    int prevAccPtrPos = ACC_METER_Y + ACC_METER_H / 2 - 2;
    uint16_t prevAccPtrRowColours[4] = { TFT_CYAN, TFT_CYAN, TFT_CYAN, TFT_CYAN };
    float approachTime = 600.0; // ms
    float timeToLive = approachTime / 3; // ms. How long each object lives after reaching the hit line. Should be long enough such that the object moves off-screen before being killed.
    float objectYRatio = HIT_LINE_Y / approachTime; // for calculating distance from the top of the runway, px/ms
    std::vector<ButtonPress> buttonPresses;
    uint32_t score;
    uint32_t multiplier;
    float accuracy;
    uint32_t num300;
    uint32_t num100;
    uint32_t num50;
    uint32_t numMiss;
    uint32_t numDidntClick;

    std::vector<HitObject> objects;
    std::vector<HitObject>::iterator nextObj;
    std::vector<HitObject> objectsInPlay;
    std::vector<ObjectPosition> objectPositions;
    std::vector<ObjectPosition> prevObjectPositions;

    uint32_t completedCount;

  public:
    Runway(TFT_eSPI& tft);
    void addPresses(uint32_t buttons, unsigned long currentTime);
    void calculateScore();
    void updateRunway();
    void plotRunway();
    void plotHitLine();
    void plotAccuracyPointer(float hitDeviation);
    float getProgress();
    uint32_t getScore();
    uint32_t getMultiplier();
    float getAccuracy();
    BeatmapResult getResult();

};

#endif