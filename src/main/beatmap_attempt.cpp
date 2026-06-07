#include "beatmap_attempt.h"
#include <cstddef>
#include <TFT_eSPI.h>

BeatmapAttempt::BeatmapAttempt(TFT_eSPI& tft, Pages& page) : Page(tft, page)
{
}

void BeatmapAttempt::loadBeatmap()
{
  approachTimeMs = 600;
  timeToLiveMs = static_cast<uint32_t>(approachTimeMs / 3.0f); // How long each object lives after reaching the hit line. Should be long enough such that the object moves off-screen before being killed.
  objectYRatio = HIT_LINE_Y / static_cast<float>(approachTimeMs); // for calculating distance from the top of the runway, px/ms

  score = 0;
  maxCombo = 1;
  multiplier = 1;
  accuracy = 1;
  num300 = 0;
  num100 = 0;
  num50 = 0;
  numMiss = 0;

  notes.clear();
  std::vector<NoteData>::iterator noteData;
  for (noteData = sampleSong.begin(); noteData < sampleSong.end(); noteData++)
  {
    Note note = {noteData->lane, noteData->timeMs, noteData->type, noteData->endTimeMs, false, false, HitJudgement::Miss, NULL};
    notes.push_back(note);
  }

  nextNote = notes.begin();

  completedCount = 0;

  startTimeMs = millis();
  finishTimeMs = 0;
  finished = false;
}

void BeatmapAttempt::processInputs()
{
  uint32_t noteScore = 0;

  for (size_t pressIndex = 0; pressIndex < inputs.size(); pressIndex++)
  {
    bool found = false;
    size_t noteIndex;
    for (noteIndex = 0; noteIndex < notesInPlay.size(); noteIndex++)
    {
      int32_t pressTimeMs = inputs[pressIndex].timeMs - startTimeMs; // convert input global timestamp to beatmap timestamp
      /*
      Conditions for matching an input to a note:
      1. Note has not yet been clicked
      2. Note is in the same lane as the input
      3. Input is within the active window of the note
      */
      if (!notesInPlay[noteIndex].clicked && 
          inputs[pressIndex].lane == notesInPlay[noteIndex].lane && 
          std::abs(pressTimeMs - static_cast<int32_t>(notesInPlay[noteIndex].timeMs)) <= GOOD_WINDOW / 2.0f)
      {
        found = true;
        notesInPlay[noteIndex].clicked = true;

        // update accuracy meter pointer
        int32_t hitDeviation = pressTimeMs - notesInPlay[noteIndex].timeMs;
        plotAccuracyPointer(hitDeviation);

        // score input 
        if (std::abs(hitDeviation) <= PERFECT_WINDOW / 2.0f)
        {
          num300++;
          noteScore = 300 * multiplier;
        }
        else if (std::abs(hitDeviation) <= GREAT_WINDOW / 2.0)
        {
          num100++;
          noteScore = 100 * multiplier;
        }
        else 
        {
          num50++;
          noteScore = 50 * multiplier;
        }

        multiplier++;
        break;
      }
    }

    if (!found && notesInPlay.size() != 0)
    {
      numMiss++;
      if (multiplier > maxCombo) { maxCombo = multiplier; } // update max combo
      multiplier = 1;
    }
  }

  inputs.clear();

  score += noteScore;
  if (completedCount != 0) { accuracy = (300 * num300 + 100 * num100 + 50 * num50) / static_cast<float>(300 * (num300 + num100 + num50 + numMiss)); }
  else { accuracy = 1.0f; }
  
}

void BeatmapAttempt::updateNotes()
{
  unsigned long beatmapTimeMs = millis() - startTimeMs;

  // remove old note positions
  notePositions.clear();

  // check if any new notes have come into play
  while (nextNote != notes.end() && nextNote->timeMs <= beatmapTimeMs + approachTimeMs)
  {
    notesInPlay.push_back(*nextNote);
    nextNote++;
  }

  // check if any notes have expired
  while (!notesInPlay.empty() && beatmapTimeMs > notesInPlay.begin()->timeMs + timeToLiveMs)
  {
    // if note expired without being clicked, a miss is counted
    if (!notesInPlay.begin()->clicked)
    {
      numMiss++;
      if (multiplier > maxCombo) { maxCombo = multiplier; } // update max combo
      multiplier = 1;
    }
    notesInPlay.erase(notesInPlay.begin());
    completedCount++;
  }

  // calculate how far down each note has travelled
  for (size_t i = 0; i < notesInPlay.size(); i++)
  {
    int timeInPlayMs = beatmapTimeMs - (notesInPlay[i].timeMs - approachTimeMs);
    int noteY = timeInPlayMs * objectYRatio;

    NotePosition pos{notesInPlay[i].lane, noteY};
    notePositions.push_back(pos);
  }

  // 3 second pause at the end of the beatmap
  if (nextNote == notes.end() && notesInPlay.empty())
  {
    if (finishTimeMs == 0) { finishTimeMs = millis() + 3000; }
    else if (millis() >= finishTimeMs) 
    { 
      if (multiplier > maxCombo) { maxCombo = multiplier; } // update max combo
      finished = true;
      currentPage = Pages::ResultPage;
    }
  }
}

/**
* Plots changing elements.
*/
void BeatmapAttempt::plot()
{
  plotNotes();
  plotScoreboard();
  plotProgressBar();
  plotHitLine();
}

/**
* Plots notes.
*/
void BeatmapAttempt::plotNotes()
{
  // clear old notes
  for (uint32_t i = 0; i < prevNotePositions.size(); i++)
  {
    tft.fillRect(RUNWAY_X + 50 * prevNotePositions[i].lane, RUNWAY_Y + prevNotePositions[i].noteY, 49, 10, TFT_BLACK);
  }
  prevNotePositions.clear();

  // plot new notes 
  for (uint32_t i = 0; i < notePositions.size(); i++)
  {
    prevNotePositions.push_back(notePositions[i]);
    tft.fillRect(RUNWAY_X + 50 * notePositions[i].lane, RUNWAY_Y + notePositions[i].noteY, 49, 10, TFT_MAGENTA);
  }
}

/**
* Plots scoreboard.
*/
void BeatmapAttempt::plotScoreboard() 
{
  char buffer[32];
  tft.setTextColor(TFT_WHITE, TFT_BLACK, true);

  tft.setTextDatum(TL_DATUM);
  tft.setFreeFont(CF_GGC20);
  sprintf(buffer, "%010d", score);
  tft.drawString(buffer, 3, 3);

  tft.setTextDatum(BL_DATUM);
  tft.setFreeFont(CF_GGC16);
  sprintf(buffer, "x%d", multiplier);
  tft.drawString(buffer, 3, 38);

  int accX = 236;
  int accY = 38;
  tft.fillRect(accX - 100, accY - 16, 100, 16, TFT_BLACK);

  tft.setTextDatum(BR_DATUM);
  sprintf(buffer, "%.1f%%", accuracy * 100);
  tft.drawString(buffer, 236, 38);
}

/**
* Plots progress bar.
*/
void BeatmapAttempt::plotProgressBar() 
{
  float progress = completedCount / static_cast<float>(notes.size()); // [0-1]
  int heightComplete = static_cast<int>(PROGRESS_H * progress);
  tft.fillRect(PROGRESS_X, PROGRESS_Y, PROGRESS_W, PROGRESS_H - heightComplete, TFT_DARKGREY);
  tft.fillRect(PROGRESS_X, PROGRESS_Y + PROGRESS_H - heightComplete + 1, PROGRESS_W, heightComplete, TFT_WHITE);
}

/**
* Plots the hit line
*/
void BeatmapAttempt::plotHitLine(void)
{
  tft.drawFastHLine(RUNWAY_X, RUNWAY_Y + HIT_LINE_Y - 1, RUNWAY_WIDTH, TFT_WHITE);
  tft.drawFastHLine(RUNWAY_X, RUNWAY_Y + HIT_LINE_Y    , RUNWAY_WIDTH, TFT_WHITE);
  tft.drawFastHLine(RUNWAY_X, RUNWAY_Y + HIT_LINE_Y + 1, RUNWAY_WIDTH, TFT_WHITE);
}

/**
* Plots accuracy meter.
*/
void BeatmapAttempt::plotAccuracyMeter() 
{
  int goodHeight = 150;
  int greatHeight = GREAT_WINDOW / GOOD_WINDOW * goodHeight;
  int perfectHeight = PERFECT_WINDOW / GOOD_WINDOW * goodHeight;
  tft.fillRect(ACC_METER_X, ACC_METER_Y, ACC_METER_W, ACC_METER_H, TFT_BLACK);
  tft.fillRect(ACC_METER_X, ACC_METER_Y + (ACC_METER_H - goodHeight) / 2, ACC_METER_W, goodHeight, TFT_ORANGE);
  tft.fillRect(ACC_METER_X, ACC_METER_Y + (ACC_METER_H - greatHeight) / 2, ACC_METER_W, greatHeight, TFT_GREENYELLOW);
  tft.fillRect(ACC_METER_X, ACC_METER_Y + (ACC_METER_H - perfectHeight) / 2, ACC_METER_W, perfectHeight, TFT_CYAN);
}

/**
* Plots accuracy meter pointer.
*
* @param hitDeviation Number of milliseconds the hit was away from a perfect (0 ms) hit. A negative value denotes an early hit, a positive value a late hit.
*/
void BeatmapAttempt::plotAccuracyPointer(float hitDeviation) 
{
  // first erase the old pointer by overwriting it with meter colours
  for (uint32_t i = 0; i < 4; i++) {
    tft.drawFastHLine(ACC_METER_X, prevAccPtrPos + i, ACC_METER_W, prevAccPtrRowColours[i]);
  }

  // calculate new pointer position
  int accMeterCenter = 180;
  int ptrPosition = accMeterCenter + hitDeviation * 150.0 / GOOD_WINDOW;  // height of good window (px) / good window (ms)
  if (ptrPosition > SCREEN_HEIGHT - 4) ptrPosition = SCREEN_HEIGHT - 4;
  else if (ptrPosition < ACC_METER_Y) ptrPosition = ACC_METER_Y;

  // save the new pointer position and meter colours
  prevAccPtrPos = ptrPosition;
  for (uint32_t i = 0; i < 4; i++) {
    prevAccPtrRowColours[i] = tft.readPixel(ACC_METER_X, i + ptrPosition);
  }

  // draw new pointer
  tft.fillRect(ACC_METER_X, ptrPosition, ACC_METER_W, 4, TFT_RED);
}

/**
* Plots white borders between elements of the beatmap HUD.
*/
void BeatmapAttempt::plotBorders(void) 
{
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
* Checks if the beatmap is finished.
* 
* @return Whether the beatmap is finished.
*/
bool BeatmapAttempt::isFinished(void)
{
  return finished;
}

BeatmapResult BeatmapAttempt::getResult()
{
  BeatmapResult result;
  result.score = this->score;
  result.maxCombo = this->maxCombo;
  result.multiplier = this->multiplier;
  result.accuracy = this->accuracy;
  result.num300 = this->num300;
  result.num100 = this->num100;
  result.num50 = this->num50;
  result.numMiss = this->numMiss;
  return result;
}









