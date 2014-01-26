#ifndef GAUGEUTIL_H
#define GAUGEUTIL_H

class GaugeUtil
{
public:
    static int * getPointInArc(int a, int b, int arcDegrees, int arcStart, int arcRadius, float amount);
    static int * getAmountInArc(float from, float to);
    static double toRadians(double degrees);
};

#endif // GAUGEUTIL_H
