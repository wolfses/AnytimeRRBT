#ifndef CBEACON_H
#define CBEACON_H

#include <vector>

struct SBeacon{
    float x;
    float y;
    float reliability;
};

class CBeacon
{
public:
    CBeacon();
    ~CBeacon();
    void addBeacon(float x, float y, float reliability=0.1);

public:
    std::vector<struct SBeacon> beacon;
};

#endif // CBEACON_H
