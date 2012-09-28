#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Presentation.h"
#include "Parsers\EffectParser.h"
#include "Defines\Effects.h"
#include "Defines\Slides.h"

EffectHandler::EffectHandler(SlideElement *parent, Effect *elementEffect)
{
    effectOwner = parent;
    effectProto = elementEffect;

    expired = false;

    startTime = clock();
}

EffectHandler::~EffectHandler()
{
}

void EffectHandler::Animate()
{
    // Dispatcher for effect types
    // Here we only determine the type and call appropriate function

    // Linear movement
    if (effectProto->moveType && (*effectProto->moveType) == MOVE_TYPE_LINEAR)
        AnimateMoveLinear();
}

void EffectHandler::AnimateMoveLinear()
{
    // Check for required things
    if (!(effectProto->effectTimer && effectProto->startPos && effectProto->endPos))
        return;

    // Calculate time coefficient to determine position
    float timeCoef = (float(clock()-startTime)) / float(*effectProto->effectTimer);
    if (timeCoef >= 1.0f)
    {
        // If the time coefficient is larger than 1, then we passed the end of effect, and finished him
        // We set the coefficient to 1 to avoid glitches, set effect as expired and if blocking, unblock the presentation
        timeCoef = 1.0f;
        SetExpired();
        if (effectProto->isBlocking)
            sPresentation->InterfaceEvent(IE_EFFECT_END);
    }

    for (uint32 i = 0; i <= 1; i++)
        effectOwner->position[i] = effectProto->startPos[i] + uint32(timeCoef * float(effectProto->endPos[i] - effectProto->startPos[i]));
}
