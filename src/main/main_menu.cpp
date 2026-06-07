#include "main_menu.h"

MainMenu::MainMenu(TFT_eSPI& tft, Pages& page) : Page(tft, page)
{
  hover = 0;
  prevHover = -1;
}

void MainMenu::processInputs()
{
  // process each button press
  std::vector<Input>::iterator press;
  for (press = inputs.begin(); press < inputs.end(); press++)
  {
    currentPage = Pages::SongSelect;
  }
  inputs.clear();
}


void MainMenu::plot() 
{
  bool hoverChanged = hover != prevHover;
  //tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(MC_DATUM);

  int center = tft.width() / 2;
  tft.setFreeFont(CF_GGC16);
  tft.drawString("Welcome to", center, 120);
  tft.setFreeFont(CF_GGC20);
  tft.drawString("POCKET HERO", center, 150);

  if (hoverChanged)
  {
    int startY = 230;
    if (hover == 0) { tft.fillRect(0, startY - 15, tft.width(), 30, TFT_VERYDARKGREY); }
    tft.setFreeFont(CF_GGC16);
    tft.drawString("Start", center, startY);
    prevHover = hover;
  }
}

void MainMenu::reset()
{
  hover = 0;
  prevHover = -1;
}

