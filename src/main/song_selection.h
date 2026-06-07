#ifndef SONG_SELECTION_H
#define SONG_SELECTION_H

#include <TFT_eSPI.h>
#include <vector>

#include "helpers.h"
#include "page.h"

class SongSelection : public Page
{
  private:
    size_t hover = 1;
    int prevHover = 0;
    Song song;

  public:
    SongSelection(TFT_eSPI&, Pages&);
    void setSong(Song song);
    void processInputs();
    void plot();
};

#endif // SONG_SELECTION_H