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
    if (effectProto->moveType && (*effectProto->moveType) == MOVE_TYPE_LINEAR)
        AnimateMoveLinear();
}

void EffectHandler::AnimateMoveLinear()
{
    if (!(effectProto->effectTimer && effectProto->startPos && effectProto->endPos))
        return;

    float timeCoef = (float(clock()-startTime)) / float(*effectProto->effectTimer);
    if (timeCoef >= 1.0f)
    {
        timeCoef = 1.0f;
        SetExpired();
        if (effectProto->isBlocking)
            sPresentation->InterfaceEvent(IE_EFFECT_END);
    }

    if (effectOwner->elemType == SLIDE_ELEM_TEXT)
    {
        for (uint32 i = 0; i <= 1; i++)
            effectOwner->typeText.position[i] = effectProto->startPos[i] + uint32(timeCoef * float(effectProto->endPos[i] - effectProto->startPos[i]));
    }
}
