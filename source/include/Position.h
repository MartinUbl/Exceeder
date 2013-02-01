#ifndef EXCDR_POSITION_H
#define EXCDR_POSITION_H

#include "Vector.h"

struct Position2
{
    Position2() { x = 0; y = 0; };
    Position2(float px, float py) { x = px; y = py; };
    Position2(float* coords) { x = coords[0]; y = coords[1]; };
    Position2(int32* coords) { x = (float)coords[0]; y = (float)coords[1]; };

    Position2 operator+(const CVector2 &vec)
    {
        return Position2(x+vec.x, y+vec.y);
    }

    Position2 operator-(const CVector2 &vec)
    {
        return Position2(x-vec.x, y-vec.y);
    }

    CVector2 makeVector(Position2 &pos)
    {
        return CVector2(pos.x - x, pos.y - y);
    }

    static CVector2 makeVector(Position2 &a, Position2 &b)
    {
        return CVector2(b.x - a.x, b.y - a.y);
    }

    float x;
    float y;
};

#endif
