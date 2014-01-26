#include <math.h>
#include "gaugeutil.h"

#define PI 3.14159265

int * GaugeUtil::getPointInArc(int a, int b, int arcDegrees, int arcStart, int arcRadius, float amount) {
  arcDegrees /= 16;
  arcStart /= 16;

  int degrees = (int (arcStart + (amount * arcDegrees)) + 0.5);

  int x = (int (arcRadius * cos(toRadians(degrees)) + a) + 0.5);
  int y = (int (arcRadius * sin(toRadians(degrees)) + b) + 0.5);

  int * result = new int[2];
  result[0] = x;
  result[1] = y;

  return result;
}

int * GaugeUtil::getAmountInArc(float from, float to) {
  int degreesFrom = (int (225 - (from * 270)) + 0.5) * 16;
        int degreesTo = (int (225 - (to * 270)) + 0.5) * 16;

  int * result = new int[2];
  result[0] = degreesFrom;
  result[1] = degreesTo;

  return result;
}

double GaugeUtil::toRadians(double degrees) {
  return degrees * PI / 180;
}

