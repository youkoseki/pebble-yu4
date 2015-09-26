#pragma once

#include "pebble.h"

//#define NUM_CLOCK_TICKS 11

static const GPathInfo MINUTE_HAND_POINTS = {
  4,
  (GPoint []) {
    {-5, 0},
    {5, 0},
    {5, -35},
    {-5, -35}
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
  4, (GPoint []){
    {-5, 0},
    {5, 0},
    {5, -35},
    {-5, -35}
  }
};
