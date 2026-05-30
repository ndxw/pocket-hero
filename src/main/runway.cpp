#include "runway.h"
#include <cstddef>
#include <TFT_eSPI.h>

Runway::Runway(TFT_eSPI& tft) : tft(tft)
{
  score = 0;
  multiplier = 1;
  accuracy = 1;
  num300 = 0;
  num100 = 0;
  num50 = 0;
  numMiss = 0;
  numDidntClick = 0;

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
void Runway::addPresses(uint32_t buttons, unsigned long currentTime)
{
  for (int i = 0; i < 4; i++)
  {
    if ((buttons >> i) & 0x1)
    {
      ButtonPress press = {i, currentTime};
      buttonPresses.push_back(press);
      // Serial.print(i);
      // Serial.print(", ");
      // Serial.println(currentTime);
    }
  }
}

void Runway::calculateScore()
{
  // for each press, find the nearest object (in its lane)
  for (size_t pressIndex = 0; pressIndex < buttonPresses.size(); pressIndex++)
  {
    bool found = false;
    size_t objIndex;
    for (objIndex = 0; objIndex < objectsInPlay.size(); objIndex++)
    {
      if (!objectsInPlay[objIndex].clicked && 
          buttonPresses[pressIndex].lane == objectsInPlay[objIndex].lane && 
          std::abs(buttonPresses[pressIndex].time - objectsInPlay[objIndex].time) <= GOOD_WINDOW / 2.0)
      {
        found = true;
        multiplier++;
        objectsInPlay[objIndex].clicked = true;
        int hitDeviation = buttonPresses[pressIndex].time - objectsInPlay[objIndex].time;
        plotAccuracyPointer(hitDeviation);
        if (std::abs(hitDeviation) <= PERFECT_WINDOW / 2.0)
        {
          num300++;
        }
        else if (std::abs(hitDeviation) <= GREAT_WINDOW / 2.0)
        {
          num300++;
        }
        else 
        {
          num300++;
        }
        break;
      }
    }

    if (!found && objectsInPlay.size() != 0)
    {
      char buffer[128];

      sprintf(buffer, "buttonPresses size=%d, objInPlay size=%d", buttonPresses.size(), objectsInPlay.size());
      Serial.println(buffer);

      // sprintf(buffer, "lane=%d, timePressed=%d", buttonPresses[pressIndex].lane, buttonPresses[pressIndex].time); 
      // Serial.println(buffer);
      // sprintf(buffer, "lane=%d, time=%d, type=%d, endTime=%d, clicked=%d", objectsInPlay[objIndex].lane, objectsInPlay[objIndex].time, objectsInPlay[objIndex].type, objectsInPlay[objIndex].endTime, objectsInPlay[objIndex].clicked);
      // Serial.println(buffer);
      

      numMiss++;
      multiplier = 1;
    }
  }

  buttonPresses.clear();

  score = 300 * num300 + 100 * num100 + 50 * num50;
  accuracy = (300 * num300 + 100 * num100 + 50 * num50) / static_cast<float>(300 * (num300 + num100 + num50 + numMiss));
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
  while (!objectsInPlay.empty() && currentTime > objectsInPlay.begin()->time + timeToLive)
  {
    // if object expired without being clicked, a miss is counted
    if (!objectsInPlay.begin()->clicked)
    {
      numMiss++;
      multiplier = 1;
    }
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

/**
* Plots accuracy meter pointer
*
* @param hitDeviation Number of milliseconds the hit was away from a perfect (0 ms) hit. Can be negative.
*/
void Runway::plotAccuracyPointer(float hitDeviation) {
  // first erase the old pointer by overwriting it with meter colours
  for (int i = 0; i < 4; i++) {
    tft.drawFastHLine(ACC_METER_X, prevAccPtrPos + i, ACC_METER_W, prevAccPtrRowColours[i]);
  }

  // calculate new pointer position
  int accMeterCenter = 180;
  int ptrPosition = accMeterCenter + hitDeviation * 150.0 / 220.0;  // height of good window (px) / good window (ms)
  if (ptrPosition > SCREEN_HEIGHT - 4) ptrPosition = SCREEN_HEIGHT - 4;
  else if (ptrPosition < ACC_METER_Y) ptrPosition = ACC_METER_Y;

  // save the new pointer position and meter colours
  prevAccPtrPos = ptrPosition;
  for (int i = 0; i < 4; i++) {
    prevAccPtrRowColours[i] = tft.readPixel(ACC_METER_X, i + ptrPosition);
  }

  tft.fillRect(ACC_METER_X, ptrPosition, ACC_METER_W, 4, TFT_RED);
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

BeatmapResult Runway::getResult()
{
  BeatmapResult result;
  result.approachTime = this->approachTime;
  result.timeToLive = this->timeToLive;
  result.score = this->score;
  result.multiplier = this->multiplier;
  result.accuracy = this->accuracy;
  result.num300 = this->num300;
  result.num100 = this->num100;
  result.num50 = this->num50;
  result.numMiss = this->numMiss;
  return result;
}









