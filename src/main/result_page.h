#ifndef RESULT_PAGE_H
#define RESULT_PAGE_H

#include <TFT_eSPI.h>
#include <vector>

#include "helpers.h"
#include "page.h"

class ResultPage : public Page {
  private:
    BeatmapResult result;

  public:
    ResultPage(TFT_eSPI& tft, Pages& page);
    void setResult(BeatmapResult result);
    void processInputs();
    void plot();
};

#endif // RESULT_PAGE_H