#ifndef BEATMAP_ATTEMPT_H
#define BEATMAP_ATTEMPT_H

#include <vector>
#include <TFT_eSPI.h>

#include "sample_song.h"
#include "helpers.h"
#include "page.h"

class BeatmapAttempt : public Page
{
  private:
    int prevAccPtrPos = ACC_METER_Y + ACC_METER_H / 2 - 2;
    uint16_t prevAccPtrRowColours[4] = { TFT_CYAN, TFT_CYAN, TFT_CYAN, TFT_CYAN };
    uint32_t startTimeMs;
    uint32_t finishTimeMs;
    uint32_t approachTimeMs; 
    uint32_t timeToLiveMs; // How long each object lives after reaching the hit line. Should be long enough such that the object moves off-screen before being killed.
    float objectYRatio; // for calculating distance from the top of the runway, px/ms
    uint32_t score;
    uint32_t maxCombo;
    uint32_t multiplier;
    float accuracy;
    uint32_t num300;
    uint32_t num100;
    uint32_t num50;
    uint32_t numMiss;
    bool scoreboardChanged = true;

    std::vector<Note> notes;
    std::vector<Note>::iterator nextNote;
    std::vector<Note> notesInPlay;
    std::vector<NotePosition> notePositions;
    std::vector<NotePosition> prevNotePositions;

    uint32_t completedCount;
    bool finished = false;

  public:
    BeatmapAttempt(TFT_eSPI&, Pages&);
    void loadBeatmap();

    void processInputs();
    void updateNotes();

    void plot();
    void plotNotes();
    void plotScoreboard();
    void plotProgressBar();
    void plotHitLine();
    void plotAccuracyPointer(float);

    void plotAccuracyMeter();
    void plotBorders();

    bool isFinished();
    BeatmapResult getResult();

};

#endif // BEATMAP_ATTEMPT_H