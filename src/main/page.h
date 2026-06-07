#ifndef PAGE_H
#define PAGE_H

#include <TFT_eSPI.h>
#include <vector>

#include "helpers.h"

class Page {
  protected:
    TFT_eSPI& tft;
    Pages& currentPage;
    std::vector<Input> inputs;

    virtual void processInputs() = 0;
    virtual void plot() = 0;

  public:
    Page(TFT_eSPI&, Pages&);
    void queueInputs(uint32_t, uint32_t);
};

#endif // PAGE_H