#include "Beacon.h"

CBeacon::CBeacon()
{

}

CBeacon::~CBeacon()
{

}

void CBeacon::addBeacon(float x, float y, float reliability)
{
    struct SBeacon b;
    b.x = x;
    b.y = y;
    b.reliability = reliability;
    beacon.push_back(b);
}
