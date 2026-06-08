#include <string>

#include "helpers.h"

uint16_t getRandomColour(void)
{
  uint16_t colours[] = {TFT_BLUE, TFT_GREEN, TFT_CYAN, TFT_RED, TFT_MAGENTA, TFT_YELLOW, TFT_ORANGE, TFT_GREENYELLOW, TFT_PINK, TFT_GOLD, TFT_SILVER, TFT_SKYBLUE, TFT_VIOLET};
  int randomIndex = rand() % 13;
  return colours[randomIndex];
}

std::string msToClockTime(uint32_t ms)
{
    uint32_t lengthSeconds = ms / 1000;
    uint32_t minutes = lengthSeconds / 60;
    uint32_t seconds = lengthSeconds % 60;
    return padLeftZeros(std::to_string(minutes), 2) + ":" + padLeftZeros(std::to_string(seconds), 2);
}

std::string padLeftZeros(std::string oldStr, size_t nZero)
{
  return std::string(nZero - std::min(nZero, oldStr.length()), '0') + oldStr;
}

std::string noteToString(Note note)
{
  std::string judgement;
  switch (note.judgement) {
    case HitJudgement::_300:
      judgement = "300"; break;
    case HitJudgement::_100:
      judgement = "100"; break;
    case HitJudgement::_50:
      judgement = "50"; break;
    case HitJudgement::Miss:
      judgement = "Miss"; break;
  }
  return "{lane: " + std::to_string(note.lane) + ", timeMs: " + std::to_string(note.timeMs) + ", type: " + (note.type == NoteType::Hit ? "Hit" : "Slider") + ", endTimeMs: " + std::to_string(note.endTimeMs) + 
          ", active: " + std::to_string(note.active) + ", pending: " + std::to_string(note.pending) + ", clicked: " + std::to_string(note.clicked) + 
          ", judgement: " + judgement + ", multiplier: " + std::to_string(note.multiplier) + "}";
}

std::string notePosToString(NotePosition pos)
{
  return "{lane:" + std::to_string(pos.lane) + ", noteY: " + std::to_string(pos.noteY) + ", active: " + std::to_string(pos.active) + "}";
}


void resultToString (BeatmapResult result, char* buffer)
{
  sprintf(buffer, "{score:%d, maxCombo:%d, multiplier:%d, accuracy:%.3f, num300:%d, num100:%d, num50:%d, numMiss:%d}", result.score, result.maxCombo, result.multiplier, result.accuracy, result.num300, result.num100, result.num50, result.numMiss);
}