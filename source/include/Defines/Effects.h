#ifndef EXCDR_EFFECTS_H
#define EXCDR_EFFECTS_H

enum MoveType
{
    MOVE_TYPE_LINEAR     = 0,
    MOVE_TYPE_CIRCULAR   = 1,
    MOVE_TYPE_BEZIER     = 2,
    MAX_MOVE_TYPE
};

enum OffsetType
{
    OFFSET_TYPE_ABSOLUTE = 0,
    OFFSET_TYPE_RELATIVE = 1,
};

struct Effect
{
    Effect()
    {
        memset(this, 0, sizeof(Effect));
    }

    uint32* effectTimer;

    bool isBlocking;

    // movement data
    uint32* moveType;
    int32* startPos; // 2 coords
    int32* endPos;   // 2 coords
    uint32* offsetType;

    std::vector<std::wstring> *m_effectChain;
};

typedef std::map<const wchar_t*, Effect*> EffectMap;

#endif
