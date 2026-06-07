#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <TFT_eSPI.h>
#include <vector>

#include "helpers.h"
#include "page.h"

class Leaderboard : public Page
{
  private:
    size_t hover = 1;
    int prevHover = 0;

  public:
    Leaderboard(TFT_eSPI&, Pages&);
    void processInputs();
    void plot();
};

#endif // LEADERBOARD_H