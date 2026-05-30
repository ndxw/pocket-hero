#include "helpers.h"

uint16_t getRandomColour(void)
{
  uint16_t colours[] = {TFT_BLUE, TFT_GREEN, TFT_CYAN, TFT_RED, TFT_MAGENTA, TFT_YELLOW, TFT_ORANGE, TFT_GREENYELLOW, TFT_PINK, TFT_GOLD, TFT_SILVER, TFT_SKYBLUE, TFT_VIOLET};
  int randomIndex = rand() % 13;
  return colours[randomIndex];
}
