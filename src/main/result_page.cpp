#include "result_page.h"

ResultPage::ResultPage(TFT_eSPI& tft, Pages& page) : Page(tft, page)
{
}

void ResultPage::setResult(BeatmapResult result)
{
  this->result = result;
}

void ResultPage::processInputs()
{
  // process each button press
  std::vector<Input>::iterator press;
  for (press = inputs.begin(); press < inputs.end(); press++)
  {
    currentPage = Pages::SongSelect;
  }

  inputs.clear();
}

void ResultPage::plot() 
{
  tft.setFreeFont(CF_GGC20);
  tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Results", tft.width() / 2, 20);

  int resultY = 60;
  tft.setFreeFont(CF_GGC16);
  tft.setTextDatum(BL_DATUM);
  tft.drawString("Score", 3, resultY);
  tft.drawString("Combo", 3, resultY += 16);
  tft.drawString("Acc.", 3, resultY += 16);
  tft.drawString("300", 3, resultY += 16);
  tft.drawString("100", 3, resultY += 16);
  tft.drawString("50", 3, resultY += 16);
  tft.drawString("Miss", 3, resultY += 16);

  resultY = 60;
  int valueX = tft.width() / 2 - 20;
  char buffer[64];
  tft.drawString(std::to_string(result.score).c_str(), valueX, resultY);
  tft.drawString((std::to_string(result.maxCombo) + "x").c_str(), valueX, resultY += 16);
  sprintf(buffer, "%.1f%%", result.accuracy * 100);
  tft.drawString(buffer, valueX, resultY += 16);
  tft.drawString((std::to_string(result.num300) + "x").c_str(), valueX, resultY += 16);
  tft.drawString((std::to_string(result.num100) + "x").c_str(), valueX, resultY += 16);
  tft.drawString((std::to_string(result.num50) + "x").c_str(), valueX, resultY += 16);
  tft.drawString((std::to_string(result.numMiss) + "x").c_str(), valueX, resultY += 16);

}
