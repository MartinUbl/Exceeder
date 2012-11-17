#ifndef EXCDR_EFFECT_HANDLER_H
#define EXCDR_EFFECT_HANDLER_H

struct SlideElement;

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

        clock_t startTime;

        SlideElement* effectOwner;
        Effect* effectProto;
        std::list<Effect*> m_effectQueue;
        EffectHandler* m_queuedEffectHandler;
};

#endif
