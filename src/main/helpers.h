#ifndef HELPERS_H
#define HELPERS_H

#include <TFT_eSPI.h>
#include <cstdlib>

struct HitObject {
  int lane;
  int time;
  int type;
  int endTime;
  bool clicked;
};

struct ObjectPosition {
  int lane;
  int objectY;
};

struct ButtonPress {
  int lane;
  int time;
};

struct BeatmapResult {
  float approachTime; // ms
  float timeToLive; // ms. How long each object lives after reaching the hit line. Should be long enough such that the object moves off-screen before being killed.
  uint32_t score;
  uint32_t multiplier;
  float accuracy;
  uint32_t num300;
  uint32_t num100;
  uint32_t num50;
  uint32_t numMiss;
};

uint16_t getRandomColour(void);
// void printHitObject(HitObject* obj, char* buffer);
// void printButtonPress(ButtonPress* press, char* buffer);

#endif