#include "page.h"

Page::Page(TFT_eSPI& tft, Pages& page) : tft(tft), currentPage(page)
{
  inputs.clear();
}

void Page::queueInputs(uint32_t pressedButtons, uint32_t releasedButtons)
{
  uint32_t currentTime = millis();
  for (size_t i = 0; i < 4; i++)
  {
    if ((pressedButtons >> i) & 0x1)
    {
      Input press = {InputType::Press, i, currentTime};
      inputs.push_back(press);
    }
    if ((releasedButtons >> i) & 0x1)
    {
      Input release = {InputType::Release, i, currentTime};
      inputs.push_back(release);
    }
  }
}

