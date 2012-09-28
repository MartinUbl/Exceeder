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

    uint32* effectTimer;

    bool isBlocking;

    // movement data
    uint32* moveType;
    uint32* startPos; // 2 coords
    uint32* endPos;   // 2 coords
};

typedef std::map<const char*, Effect*> EffectMap;

struct SlideElement;

class EffectHandler
{
    public:
        EffectHandler(SlideElement* parent, Effect* elementEffect);
        ~EffectHandler();

        void Animate();
        bool isExpired() { return expired; };

        Effect* getEffectProto() { return effectProto; };

    private:
        void AnimateMoveLinear();

        void SetExpired() { expired = true; };
        bool expired;

        clock_t startTime;

        SlideElement* effectOwner;
        Effect* effectProto;
};

#endif
