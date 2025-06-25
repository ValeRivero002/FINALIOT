#ifndef WEIGHT_SENSOR_H
#define WEIGHT_SENSOR_H

#include "HX711.h"

extern HX711 scale;

extern const int LOADCELL_DOUT_PIN;
extern const int LOADCELL_SCK_PIN;
extern const float CALIBRATION_FACTOR;
extern const float THRESHOLD_EMPTY;
extern const float THRESHOLD_LITTLE;
extern const float THRESHOLD_ENOUGH;

extern String Weight;
extern String PastWeight;

void initScale();
String classifyWeight(float g);

#endif
