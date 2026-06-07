#include "song_selection.h"

SongSelection::SongSelection(TFT_eSPI& tft, Pages& page) : Page(tft, page)
{
  hover = 1;
  prevHover = 0;
}

void SongSelection::setSong(Song song)
{
  this->song = song;
}

void SongSelection::processInputs()
{
  // process each button press
  std::vector<Input>::iterator press;
  for (press = inputs.begin(); press < inputs.end(); press++)
  {
    switch (press->lane) 
    {
      case 0:
        // back
        hover = 1;
        prevHover = 0;
        currentPage = Pages::SongSelect;
        break;
      case 1 ... 2:
        // up
        if (hover == 0) { hover = 1; }
        else { hover = 0; }
        break;
      case 3:
        // ok
        if (hover == 1) 
        { 
          hover = 1;
          prevHover = 0;
          currentPage = Pages::BeatmapAttempt;  
        }
        else 
        { 
          hover = 0;
          prevHover = 1;
          currentPage = Pages::Leaderboard; 
        }
        break;
    }
  }

  inputs.clear();
}

void SongSelection::plot() 
{
  if (hover != prevHover)
  {
    prevHover = hover;

    // convert milliseconds to mm:ss format
    std::string lengthStr = msToClockTime(song.lengthMs);

    // plot song info
    tft.setTextDatum(TL_DATUM);
    tft.setTextWrap(true, false);

    tft.setFreeFont(CF_GGC20);
    tft.drawString(song.title.c_str(), 3, 3);

    tft.setFreeFont(CF_GGC16);
    tft.drawString(song.artist.c_str(), 3, 29);
    tft.drawString(("[" + song.difficultyName + "]").c_str(), 3, 47);
    tft.drawString(("Mapped by " + song.mapper).c_str(), 3, 63);
    tft.drawString((song.difficulty + "* | " + lengthStr).c_str(), 3, 79);
  
    if (hover == 0)
    {
      tft.fillRect(tft.width() / 2 + 1, tft.height() - 50, tft.width() / 2, 50, TFT_BLACK);
      tft.fillRect(0, tft.height() - 50, tft.width() / 2, 50, TFT_VERYDARKGREY);
    }
    else
    {
      tft.fillRect(tft.width() / 2 + 1, tft.height() - 50, tft.width() / 2, 50, TFT_VERYDARKGREY);
      tft.fillRect(0, tft.height() - 50, tft.width() / 2, 50, TFT_BLACK);
    }

    // leaderboard icon
    tft.fillRect(40, 288, 10, 24, TFT_WHITE);
    tft.fillRect(55, 278, 10, 34, TFT_WHITE);
    tft.fillRect(70, 300, 10, 12, TFT_WHITE);
    // play icon
    tft.fillTriangle(170, 280, 170, 310, 190, 295, TFT_GREEN);
  }
}
