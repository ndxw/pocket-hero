#include "leaderboard.h"

Leaderboard::Leaderboard(TFT_eSPI& tft, Pages& page) : Page(tft, page)
{
  hover = 1;
  prevHover = 0;
}

void Leaderboard::processInputs()
{
  // process each button press
  std::vector<ButtonPress>::iterator input;
  for (input = inputs.begin(); input < inputs.end(); input++)
  {
    switch (input->lane) 
    {
      case 0:
        // back
        currentPage = Pages::SongSelection;
        break;
      case 1 ... 2:
        // up
        if (hover == 0) { hover = 1; }
        else { hover = 0; }
        break;
      case 3:
        // ok
        // if (hover == 1) { currentPage = Pages::BeatmapAttempt; }
        // else { currentPage = Pages::Leaderboard; }
        break;
    }
  }

  inputs.clear();
}

void Leaderboard::plot() 
{
  tft.setFreeFont(CF_GGC20);
  tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Leaderboard", tft.width() / 2, 20);
  if (hover != prevHover)
  {
    prevHover = hover;
  }
}
