#include "song_select.h"

SongSelect::SongSelect(TFT_eSPI& tft, Pages& page) : Page(tft, page)
{
  Song song = { .title = "dear april", 
                .artist = "isobel", 
                .mapper = "Delis", 
                .difficulty = "7.27", 
                .difficultyName = "Choco's Nap Spot", 
                .lengthMs = 8000
              };
  Song song1 = { .title = "Voiceless Fish - Shin Kawasaki (Temporary)", 
                .artist = "TOGENASHI TOGEARI", 
                .mapper = "Sotarks", 
                .difficulty = "4.55", 
                .difficultyName = "Waterloo's Insane", 
                .lengthMs = 256780
              };
  songs.push_back(song);
  songs.push_back(song1);
  listHover = 0;
  prevListHover = -1;
}

void SongSelect::processInputs()
{
  // process each button press
  std::vector<ButtonPress>::iterator press;
  for (press = inputs.begin(); press < inputs.end(); press++)
  {
    switch (press->lane) 
    {
      case 0:
        // back
        prevListHover = -1; // keep current hover but ensure the hover box is still rendered if the user returns to this page
        currentPage = Pages::MainMenu;
        break;
      case 1:
        // up
        if (listHover <= 0) { continue; }
        else { listHover--; }
        break;
      case 2:
        // down
        if (listHover >= songs.size() - 1) { listHover = songs.size() - 1; }
        else { listHover++; }
        break;
      case 3:
        // ok
        prevListHover = -1; // keep current hover but ensure the hover box is still rendered if the user returns to this page
        currentPage = Pages::SongSelection;
        break;
    }
  }
  inputs.clear();
}

void SongSelect::plot() 
{
  int listItemHeight = 50;
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(TL_DATUM);
  tft.setFreeFont(CF_GGC16);
  std::string buffer;

  // hovered item changed, erase old hover box
  if (prevListHover != listHover) { tft.fillScreen(TFT_BLACK); }

  for (size_t songIndex = 0; songIndex < songs.size(); songIndex++)
  {
    int listItemY = songIndex * listItemHeight;

    // current song is hovered and hovered item has changed, redraw hover box
    if (songIndex == listHover && prevListHover != listHover)
    {
      prevListHover = listHover;
      tft.fillRect(0, listItemY, tft.width(), listItemHeight, TFT_VERYDARKGREY);
    }
    
    // reduced song info
    tft.drawString(songs[songIndex].title.c_str(), 3, listItemY += 1);
    tft.drawString(songs[songIndex].artist.c_str(), 3, listItemY += 16);
    buffer = songs[songIndex].difficulty + "* | " + msToClockTime(songs[songIndex].lengthMs);
    tft.drawString(buffer.c_str(), 3, listItemY += 16);
  }
}

void SongSelect::reset()
{
  listHover = 0;
  prevListHover = -1;
}

Song SongSelect::getCurrentSongInfo()
{
  return songs[listHover];
}