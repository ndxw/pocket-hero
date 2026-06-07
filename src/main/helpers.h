#ifndef HELPERS_H
#define HELPERS_H

#include <TFT_eSPI.h>
#include <cstdlib>

#define TFT_VERYDARKGREY 0x2104

#define CF_GGC20 &Gamegirl_Classic_20
#define CF_GGC16 &Gamegirl_Classic_16

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

enum class InputType {
  Press,
  Release
};

enum class NoteType {
  Hit,
  Slider
};

enum class HitJudgement {
  Miss,
  _50,
  _100,
  _300
};

enum class Pages {
  MainMenu,
  SongSelect,
  SongSelection,
  BeatmapAttempt,
  ResultPage,
  Leaderboard
};

struct Song {
  std::string title;
  std::string artist;
  std::string mapper;
  std::string difficulty;
  std::string difficultyName;
  uint32_t lengthMs;
};

struct Note {
  uint8_t lane;
  uint32_t timeMs;
  NoteType type;
  uint32_t endTimeMs;
  bool pending;
  bool clicked;
  HitJudgement judgement;
  uint32_t multiplier;
};

struct NoteData {
  uint8_t lane;
  uint32_t timeMs;
  NoteType type;
  uint32_t endTimeMs;
};

struct NotePosition {
  uint8_t lane;
  int noteY;
};

struct Input {
  InputType type;
  uint8_t lane;
  uint32_t timeMs;
};

struct BeatmapResult {
  uint32_t score;
  uint32_t maxCombo;
  uint32_t multiplier;
  float accuracy;
  uint32_t num300;
  uint32_t num100;
  uint32_t num50;
  uint32_t numMiss;
  // add date and time
};



uint16_t getRandomColour(void);
std::string msToClockTime(uint32_t ms);
std::string padLeftZeros(std::string oldStr, size_t nZero);
std::string noteToString(Note note);
void resultToString (BeatmapResult, char*);
// void printButtonPress(ButtonPress* press, char* buffer);

#endif // HELPERS_H