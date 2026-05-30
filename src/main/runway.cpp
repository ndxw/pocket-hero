#include "runway.h"
#include <cstddef>
#include <TFT_eSPI.h>

Runway::Runway(TFT_eSPI& tft) : tft(tft)
{
  approachTime = 600.0;
  timeToLive = approachTime / 3;
  objectYRatio = HIT_LINE_Y / approachTime;

  buttonsPressed = 0;

  for (size_t i = 0; i < hit_objs.size(); i++)
  {
    objects.push_back(hit_objs[i]);
  }

  nextObj = objects.begin();

  completedCount = 0;
}

/**
* Clears bits corresponding to the pressed buttons in this->buttonsPressed
*
* @param buttons Bits 0 through 3 are set if buttons 0 through 3 have been pressed 
*/
void Runway::buttonsPressed(uint32_t buttons)
{
  buttonsPressed |= buttons;
}

/**
* Clears bits corresponding to the released buttons in this->buttonsPressed
*
* @param buttons Bits 0 through 3 are set if buttons 0 through 3 have been released 
*/
void Runway::buttonsReleased(uint32_t buttons)
{
  buttonsPressed &= !buttons;
}

void Runway::calculateScore()
{
  // TODO
}

void Runway::updateRunway()
{
  unsigned long currentTime = millis();

  // remove old object positions
  objectPositions.clear();

  // check if any new objects have come into play
  while (nextObj != objects.end())
  {
    if (nextObj->time <= currentTime + approachTime)
    {
      objectsInPlay.push_back(*nextObj);
      nextObj++;
    }
    else
    {
      break;
    }
  }

  // check if any objects have expired
  while (!objectsInPlay.empty() && currentTime > (*objectsInPlay.begin()).time + timeToLive)
  {
    objectsInPlay.erase(objectsInPlay.begin());
    completedCount++;
  }

  // calculate how far down each object has travelled
  for (size_t i = 0; i < objectsInPlay.size(); i++)
  {
    int timeInPlay = currentTime - (objectsInPlay[i].time - approachTime);
    int objectY = timeInPlay * objectYRatio;

    ObjectPosition pos{objectsInPlay[i].lane, objectY};
    objectPositions.push_back(pos);
  }
}

void Runway::plotRunway()
{
  // clear runway
  for (size_t i = 0; i < prevObjectPositions.size(); i++)
  {
    tft.fillRect(RUNWAY_X + 50 * prevObjectPositions[i].lane, RUNWAY_Y + prevObjectPositions[i].objectY, 49, 10, TFT_BLACK);
  }
  prevObjectPositions.clear();

  for (size_t i = 0; i < objectPositions.size(); i++)
  {
    prevObjectPositions.push_back(objectPositions[i]);
    tft.fillRect(RUNWAY_X + 50 * objectPositions[i].lane, RUNWAY_Y + objectPositions[i].objectY, 49, 10, TFT_MAGENTA);
  }

  plotHitLine();
}

/**
* Plots the hit line
*/
void Runway::plotHitLine(void)
{
  tft.drawFastHLine(RUNWAY_X, RUNWAY_Y + HIT_LINE_Y - 1, RUNWAY_WIDTH, TFT_WHITE);
  tft.drawFastHLine(RUNWAY_X, RUNWAY_Y + HIT_LINE_Y, RUNWAY_WIDTH, TFT_WHITE);
  tft.drawFastHLine(RUNWAY_X, RUNWAY_Y + HIT_LINE_Y + 1, RUNWAY_WIDTH, TFT_WHITE);
}

float Runway::getProgress(void)
{
  return completedCount / static_cast<float>(objects.size());
}

uint32_t Runway::getScore()
{
  return score;
}

uint32_t Runway::getMultiplier()
{
  return multiplier;
}

float Runway::getAccuracy()
{
  return accuracy;
}











