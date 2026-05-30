#ifndef RUNWAY_H
#define RUNWAY_H

#include "sample_song.h"
#include "helpers.h"
#include <vector>

#define RUNWAY_X 21
#define RUNWAY_Y 44
#define RUNWAY_WIDTH 199
#define RUNWAY_HEIGHT 276

#define HIT_LINE_Y 220 // relative to top of runway

class Runway
{
  private:
    TFT_eSPI& tft;

    float approachTime; // ms
    float timeToLive; // ms. How long each object lives after reaching the hit line. Should be long enough such that the object moves off-screen before being killed.
    float objectYRatio; // for calculating distance from the top of the runway, px/ms
    uint32_t buttonsPressed;
    uint32_t score;
    uint32_t multiplier;
    float accuracy;

    std::vector<HitObject> objects;
    std::vector<HitObject>::iterator nextObj;
    std::vector<HitObject> objectsInPlay;
    std::vector<ObjectPosition> objectPositions;
    std::vector<ObjectPosition> prevObjectPositions;

    int completedCount;

  public:
    Runway(TFT_eSPI& tft);
    void buttonsPressed(uint32_t buttons);
    void buttonsReleased(uint32_t buttons);
    void calculateScore();
    void updateRunway();
    void plotRunway();
    void plotHitLine();
    void plotLaneLines();
    float getProgress();
    uint32_t getScore();
    uint32_t getMultiplier();
    float getAccuracy();

};

#endif