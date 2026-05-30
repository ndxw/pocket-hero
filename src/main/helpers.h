#ifndef HELPERS_H
#define HELPERS_H

#include <TFT_eSPI.h>
#include <cstdlib>

uint16_t getRandomColour(void);

struct HitObject {
  int lane;
  int time;
  int type;
  int endTime;
};

struct ObjectPosition {
  int lane;
  int objectY;
};

#endif