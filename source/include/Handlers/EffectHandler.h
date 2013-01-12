#ifndef EXCDR_EFFECT_HANDLER_H
#define EXCDR_EFFECT_HANDLER_H

struct SlideElement;

enum EffectProgress
{
    // Assuming x is from <0;1>
    EP_LINEAR    = 0, // y = x
    EP_SINUS     = 1, // y = sin(x*PI/2)
    EP_QUADRATIC = 2, // y = x^2
};

class EffectHandler
{
    public:
        EffectHandler(SlideElement* parent, Effect* elementEffect, bool fromQueue = false);
        ~EffectHandler();

        void Animate();
        bool isExpired() { return expired; };
        bool isRunningQueue() { return runningQueue; };

        void QueueEffect(Effect* eff);

        Effect* getEffectProto() { return effectProto; };

    private:
        void AnimateMoveLinear();
        void AnimateMoveCircular();

        void UnblockPresentationIfNeeded();
        void SetExpired()
        {
            expired = true;
            UnblockPresentationIfNeeded();
        };
        void SetSelfExpired()
        {
            if (m_effectQueue.empty())
                SetExpired();
            else
                runningQueue = true;
        };
        void QueuedEffectExpired();
        bool expired;
        bool runningQueue;

        // cached position coords
        int32 startPos[2];
        int32 endPos[2];

        // For circular and bezier movement
        CVector2 movementVector[2];
        // For circular movement
        float phase;
        float radius;

        clock_t startTime;

        SlideElement* effectOwner;
        Effect* effectProto;
        std::list<Effect*> m_effectQueue;
        EffectHandler* m_queuedEffectHandler;
};

#endif
