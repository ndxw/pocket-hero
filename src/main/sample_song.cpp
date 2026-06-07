#include "sample_song.h"

std::vector<NoteData> sampleSong = {
  {0, 3000, NoteType::Hit, 3000},
  {1, 3250, NoteType::Hit, 3250},
  {2, 3500, NoteType::Hit, 3500},
  {3, 3750, NoteType::Hit, 3750},
  {0, 4000, NoteType::Hit, 4000},
  {2, 4500, NoteType::Hit, 4500},
  {1, 5000, NoteType::Hit, 5000},
  {3, 5333, NoteType::Hit, 5333},
  {2, 5666, NoteType::Hit, 5666},
  {1, 6000, NoteType::Hit, 6000}
};

std::vector<NoteData> sampleLong = {
  {0, 3000, NoteType::Hit, 3000},
  {1, 3250, NoteType::Hit, 3250},
  {2, 3500, NoteType::Hit, 3500},
  {3, 3750, NoteType::Hit, 3750},
  {0, 4000, NoteType::Hit, 4000},
  {2, 4500, NoteType::Hit, 4500},
  {1, 5000, NoteType::Hit, 5000},
  {3, 5333, NoteType::Hit, 5333},
  {2, 5666, NoteType::Hit, 5666},
  {0, 6000, NoteType::Hit, 6000},
  {1, 6250, NoteType::Hit, 6250},
  {2, 6500, NoteType::Hit, 6500},
  {3, 6750, NoteType::Hit, 6750},
  {0, 7000, NoteType::Hit, 7000},
  {2, 7500, NoteType::Hit, 7500},
  {1, 8000, NoteType::Hit, 8000},
  {3, 8333, NoteType::Hit, 8333},
  {2, 8666, NoteType::Hit, 8666},
  {1, 9000, NoteType::Hit, 9000}
};

std::vector<NoteData> sampleHold = {
  {0, 3000, NoteType::Slider, 3500},
  {2, 4000, NoteType::Hit   , 4000},
  {3, 4500, NoteType::Hit   , 4500},
  {2, 5000, NoteType::Slider, 5500},
  {0, 6000, NoteType::Hit   , 6000},
  {1, 6500, NoteType::Hit   , 6500},
  {1, 7000, NoteType::Slider, 7500},
  {2, 8000, NoteType::Hit   , 8000},
  {3, 8500, NoteType::Hit   , 8500},
  {0, 9000, NoteType::Hit   , 9000}
};

// std::vector<Note> sampleHoldOverlap = {
//   {0, 3000, NoteType::Hit, 3000},
//   {1, 3250, NoteType::Hit, 4000},
//   {2, 3500, NoteType::Hit, 5000},
//   {3, 3750, NoteType::Hit, 6000},
//   {0, 4000, NoteType::Hit, 7000},
//   {2, 4500, NoteType::Hit, 8000},
//   {1, 5000, NoteType::Hit, 9000},
//   {3, 5333, NoteType::Hit, 10000},
//   {2, 5666, NoteType::Hit, 11000},
//   {1, 6000, NoteType::Hit, 12000}
// };

// std::vector<Note> sampleMix = {
//   {0, 3000, NoteType::Hit, 3000},
//   {1, 3250, NoteType::Hit, 4000},
//   {2, 3500, NoteType::Hit, 5000},
//   {3, 3750, NoteType::Hit, 6000},
//   {0, 4000, NoteType::Hit, 7000},
//   {2, 4500, NoteType::Hit, 8000},
//   {1, 5000, NoteType::Hit, 9000},
//   {3, 5333, NoteType::Hit, 10000},
//   {2, 5666, NoteType::Hit, 11000},
//   {1, 6000, NoteType::Hit, 12000}
// };