#ifndef EXCDR_EFFECTS_H
#define EXCDR_EFFECTS_H

enum MoveType
{
    MOVE_TYPE_LINEAR     = 0,
    MOVE_TYPE_CIRCULAR   = 1,
    MOVE_TYPE_BEZIER     = 2,
    MAX_MOVE_TYPE
};

struct Effect
{
    Effect()
    {
        memset(this, 0, sizeof(Style));
    }

    uint32* stepTimer;
    uint32* stepValue;

    bool isBlocking;

    // movement data
    uint32* moveType;
    uint32* startPos; // 2 coords
    uint32* endPos;   // 2 coords
};

typedef std::map<const char*, Effect*> EffectMap;

#endif
