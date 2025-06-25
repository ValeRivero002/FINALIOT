#include "WeightSensor.h"

HX711 scale;

const int LOADCELL_DOUT_PIN = 25;
const int LOADCELL_SCK_PIN = 33;
const float CALIBRATION_FACTOR = 470.95f;
const float THRESHOLD_EMPTY = 20.0f;
const float THRESHOLD_LITTLE = 40.0f;
const float THRESHOLD_ENOUGH = 200.0f;

String Weight = "";
String PastWeight = "";

void initScale() {
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(CALIBRATION_FACTOR);
  scale.tare();
}

String classifyWeight(float g) {
  if      (g <= THRESHOLD_EMPTY)  return "nothing";
  else if (g <= THRESHOLD_LITTLE) return "little";
  else if (g <= THRESHOLD_ENOUGH) return "enough";
  else                             return "overflowing";
}
