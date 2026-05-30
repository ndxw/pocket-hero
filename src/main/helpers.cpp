#include "helpers.h"

uint16_t getRandomColour(void)
{
  uint16_t colours[] = {TFT_BLUE, TFT_GREEN, TFT_CYAN, TFT_RED, TFT_MAGENTA, TFT_YELLOW, TFT_ORANGE, TFT_GREENYELLOW, TFT_PINK, TFT_GOLD, TFT_SILVER, TFT_SKYBLUE, TFT_VIOLET};
  int randomIndex = rand() % 13;
  return colours[randomIndex];
}

// void printHitObject(HitObject* obj, char* buffer)
// {
//   sprintf(buffer, "lane=%d, time=%d, type=%d, endTime=%d, clicked=%d", obj->lane, obj->time, obj->type, obj->endTime, obj->clicked);
// }

// void printButtonPress(ButtonPress* press, char* buffer)
// {
//   sprintf(buffer, "lane=%d, timePressed=%d", press->lane, press->time);
// }