#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <TFT_eSPI.h>

#include "helpers.h"
#include "page.h"

class MainMenu : public Page {
  private:
    size_t hover;
    int prevHover;

  public:
    MainMenu(TFT_eSPI&, Pages&);
    void processInputs();
    void plot();
    void reset();
};

#endif // MAIN_MENU_H