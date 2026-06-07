#ifndef SONG_SELECT_H
#define SONG_SELECT_H

#include <TFT_eSPI.h>
#include <vector>

#include "helpers.h"
#include "page.h"

class SongSelect : public Page
{
  private:
    size_t listHover;
    int prevListHover;
    std::vector<Song> songs;

  public:
    SongSelect(TFT_eSPI&, Pages&);
    void readSongs(); // from SD card
    void processInputs();
    void plot();
    void reset();
    Song getCurrentSongInfo();
};

#endif // SONG_SELECT_H