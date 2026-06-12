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
  for (noteData = sampleHoldOverlap.begin(); noteData < sampleHoldOverlap.end(); noteData++)
  {
    Note note = {noteData->lane, noteData->timeMs, noteData->type, noteData->endTimeMs, false, false, false, HitJudgement::Miss, 1};
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
  bool noteMatched;
  uint32_t noteScore;
  int32_t pressTimeMs, hitDeviation;

  for (auto& input : inputs)
  {
    noteMatched = false;
    noteScore = 0;

    if (input.type == InputType::Press) 
    {
      for (auto& note : notesVisible[input.lane])
      {
        pressTimeMs = input.timeMs - startTimeMs; // convert input global timestamp to beatmap timestamp
        hitDeviation = pressTimeMs - static_cast<int32_t>(note.timeMs);
        /*
        Conditions for matching a press to a hit note:
        1. Note has not yet been clicked
        2. Note is active
        */
        if (!note.clicked && note.active)
        {
          if (note.type == NoteType::Hit)
          {
            noteMatched = true;
            note.clicked = true;
            
            // update accuracy meter pointer
            plotAccuracyPointer(hitDeviation);

            // score input 
            if (std::abs(hitDeviation) <= PERFECT_DEVIATION)
            {
              num300++;
              noteScore = 300 * multiplier;
              note.judgement = HitJudgement::_300;
            }
            else if (std::abs(hitDeviation) <= GREAT_DEVIATION)
            {
              num100++;
              noteScore = 100 * multiplier;
              note.judgement = HitJudgement::_100;
            }
            else 
            {
              num50++;
              noteScore = 50 * multiplier;
              note.judgement = HitJudgement::_50;
            }

            note.multiplier = multiplier;
            multiplier++;

            Serial.println("\nMatched press to hit:");
            char buffer[64];
            sprintf(buffer, "pressTime: %d, dev: %d", pressTimeMs, hitDeviation);
            Serial.println(buffer);
            const char* noteStr = noteToString(note).c_str();
            Serial.println(noteStr);

            break;
          }
          // note.type == NoteType::Slider
          else
          {
            /*
            Conditions for matching a press to a slider note:
            1. Note has not yet been clicked
            2. Note is in first active window
            3. Note is not pending
            */
            if (!note.pending && pressTimeMs <= note.timeMs + GOOD_DEVIATION)
            {
              noteMatched = true;
              note.pending = true;

              Serial.println("\nMatched press to slider:");
              const char* noteStr = noteToString(note).c_str();
              Serial.println(noteStr);

              break;
            }
          }
        }
      }
    }
    // input.type == InputType::Release
    else
    {
      for (auto& note : notesVisible[input.lane])
      {
        pressTimeMs = input.timeMs - startTimeMs; // convert input global timestamp to beatmap timestamp
        hitDeviation = pressTimeMs - static_cast<int32_t>(note.endTimeMs);

        /*
        Conditions for matching a release to a slider note:
        1. Note has not yet been clicked
        2. Note is in second active window
        3. Note is pending
        */
        if (!note.clicked && note.active && pressTimeMs >= note.endTimeMs - GOOD_DEVIATION && note.pending)
        {
          if (note.type == NoteType::Slider)
          {
            noteMatched = true;
            note.clicked = true;
            note.pending = false;
            
            // update accuracy meter pointer
            plotAccuracyPointer(hitDeviation);

            // score input 
            if (std::abs(hitDeviation) <= PERFECT_DEVIATION)
            {
              num300++;
              noteScore = 300 * multiplier;
              note.judgement = HitJudgement::_300;
            }
            else if (std::abs(hitDeviation) <= GREAT_DEVIATION)
            {
              num100++;
              noteScore = 100 * multiplier;
              note.judgement = HitJudgement::_100;
            }
            else 
            {
              num50++;
              noteScore = 50 * multiplier;
              note.judgement = HitJudgement::_50;
            }

            note.multiplier = multiplier;
            multiplier++;

            Serial.println("\nMatched release to slider:");
            char buffer[64];
            sprintf(buffer, "releaseTime: %d, dev: %d", pressTimeMs, hitDeviation);
            Serial.println(buffer);
            const char* noteStr = noteToString(note).c_str();
            Serial.println(noteStr);

            break;
          }
        }
        // ignore InputType::Release for NoteType::Hit
        else if (note.type == NoteType::Hit && note.clicked)
        {
          noteMatched = true;

          Serial.println("\nMatched release to hit: ignoring...");
          const char* noteStr = noteToString(note).c_str();
          Serial.println(noteStr);
        }
      }
    }

    // if input outside all active windows, reset multiplier (and combo)
    if (!noteMatched && notesVisible[input.lane].size() != 0)
    {
      if (multiplier > maxCombo) { maxCombo = multiplier; } // update max combo
      multiplier = 1;
    }

    score += noteScore;
    if (completedCount != 0) { accuracy = (300 * num300 + 100 * num100 + 50 * num50) / static_cast<float>(300 * (num300 + num100 + num50 + numMiss)); }
    else { accuracy = 1.0f; }
  }

  inputs.clear();
  //Serial.println("exit processInput\n");
}

void BeatmapAttempt::updateNotes()
{
  uint32_t beatmapTimeMs = millis() - startTimeMs;
  // Serial.print("Current beatmap time: ");
  // Serial.println(beatmapTimeMs);

  // check if any new notes have become visible
  while (nextNote != notes.end() && nextNote->timeMs <= beatmapTimeMs + approachTimeMs)
  {
    // Serial.println("new note:");
    // const char* noteStr = noteToString(*nextNote).c_str();
    // Serial.println(noteStr);
    notesVisible[nextNote->lane].push_back(*nextNote);
    nextNote++;
  }

  // check if any notes' "activeness" has changed 
  for (size_t lane = 0; lane < 4; lane++)
  {
    // Serial.print("active/inactive check lane: ");
    // Serial.println(lane);
    for (auto& note : notesVisible[lane])
    {
      // note entered first (head) active window
      if (beatmapTimeMs >= note.timeMs - GOOD_DEVIATION && beatmapTimeMs <= note.timeMs + GOOD_DEVIATION && !note.active)
      {
        note.active = true;
      }
      // note between head and tail windows
      else if (beatmapTimeMs > note.timeMs + GOOD_DEVIATION && beatmapTimeMs < note.endTimeMs - GOOD_DEVIATION && note.active)
      {
        note.active = false;
      }
      // note entered second (tail) active window
      else if (beatmapTimeMs >= note.endTimeMs - GOOD_DEVIATION && beatmapTimeMs <= note.endTimeMs + GOOD_DEVIATION && !note.active)
      {
        note.active = true;
      }
      // note left tail window, and is now out of play
      else if (beatmapTimeMs > note.endTimeMs + GOOD_DEVIATION && note.active)
      {
        note.active = false;
        note.pending = false;
        completedCount++;

        // if note leaves tail window without being clicked, a miss is counted
        if (!note.clicked)
        {
          note.judgement = HitJudgement::Miss;
          numMiss++;
          if (multiplier > maxCombo) { maxCombo = multiplier; }
          multiplier = 1;
        }
      }
    }
  }

  // check if any notes have moved off screen
  for (size_t lane = 0; lane < 4; lane++)
  {
    // Serial.print("expiry check lane: ");
    // Serial.println(lane);
    while (!notesVisible[lane].empty() && beatmapTimeMs > notesVisible[lane].front().endTimeMs + timeToLiveMs)
    {
      // Serial.println("note expired:");
      // const char* noteStr = noteToString(notesVisible[lane].front()).c_str();
      // Serial.println(noteStr);
      notesVisible[lane].pop_front();
    }
  }
  
  int timeVisibleMs, noteY;
  uint32_t noteHeight;
  notePositions.clear(); // remove old note positions

  // calculate how far down each note has travelled
  for (size_t lane = 0; lane < 4; lane++)
  {
    // Serial.print("movement calc lane: ");
    // Serial.println(lane);
    for (auto& note : notesVisible[lane])
    {
      timeVisibleMs = beatmapTimeMs - (note.timeMs - approachTimeMs);
      noteY = timeVisibleMs * objectYRatio;
      noteHeight = (note.endTimeMs - note.timeMs) * objectYRatio;
      noteHeight = std::min(static_cast<int>(noteHeight), noteY);

      NotePosition pos{lane, noteY, noteHeight, note.active};
      notePositions.push_back(pos);
      // Serial.println("note moved:");
      // Serial.println(notePosToString(pos).c_str());
    }
  }

  // 3 second pause at the end of the beatmap
  if (nextNote == notes.end() && notesVisible[0].empty() && notesVisible[1].empty() && notesVisible[2].empty() && notesVisible[3].empty())
  {
    if (finishTimeMs == 0) { finishTimeMs = millis() + 1000; }
    else if (millis() >= finishTimeMs) 
    { 
      Serial.println("song FINISHED");
      if (multiplier > maxCombo) { maxCombo = multiplier; } // update max combo
      finished = true;
      currentPage = Pages::ResultPage;
    }
  }
      //Serial.println("exit updateNotes\n");

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
  //Serial.println("clearing old notes");
  // clear old notes
  for (auto& prevPos : prevNotePositions)
  {
    if (prevPos.height) // slider
    {
      tft.fillRect(RUNWAY_X + 50 * prevPos.lane, RUNWAY_Y + prevPos.y - prevPos.height, 49, prevPos.height, TFT_BLACK);
    }
    else // hit
    {
      tft.fillRect(RUNWAY_X + 50 * prevPos.lane, RUNWAY_Y + prevPos.y, 49, 10, TFT_BLACK);
    }
  }
  prevNotePositions.clear();

  //Serial.println("plotting new notes");
  // plot new notes 
  uint16_t noteColour;
  for (auto& pos : notePositions)
  {
    prevNotePositions.push_back(pos);

    // Serial.println("plotting note:");
    // const char* posStr = notePosToString(pos).c_str();
    //   Serial.println(posStr);

    if (pos.active) { noteColour = TFT_GREENYELLOW; }
    else { noteColour = TFT_MAGENTA; }

    if (pos.height) // slider
    {
      //Serial.println("slider^");
      tft.fillRect(RUNWAY_X + 50 * pos.lane, RUNWAY_Y + pos.y - pos.height, 49, pos.height, noteColour);
    }
    else // hit
    {
      //Serial.println("hit^");
      tft.fillRect(RUNWAY_X + 50 * pos.lane, RUNWAY_Y + pos.y, 49, 10, noteColour);
    }
  }
  //Serial.println("exit plotNotes\n");
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
  int greatHeight = (GREAT_DEVIATION * 2) / (GOOD_DEVIATION * 2) * goodHeight;
  int perfectHeight = (PERFECT_DEVIATION * 2) / (GOOD_DEVIATION * 2) * goodHeight;
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
  int ptrPosition = accMeterCenter + hitDeviation * 150.0 / (GOOD_DEVIATION * 2);  // height of good window (px) / good window (ms)
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









