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

        static void CalculateEffectProgress(float &coef, uint8 progressType);

    private:
        void AnimateMoveLinear();
        void AnimateMoveCircular();
        void AnimateMoveBezier();
        void AnimateFadeIn();
        void AnimateFadeOut();

        // time coefficient could be reused
        float timeCoef;

        bool GetTimeCoef(float &target)
        {
            // If the time coefficient is equal or larger than 1, then we passed the end of effect
            // return false if finished, true if not

            target = (float(clock()-startTime)) / float(*effectProto->effectTimer);
            if (target >= 1.0f)
            {
                target = 1.0f;
                return false;
            }

            return true;
        }
        void CalculateEffectProgress(float &coef);

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

        // cached opacity
        uint8 startOpacity;

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
