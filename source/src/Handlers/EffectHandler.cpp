#include "Global.h"
#include "Presentation.h"
#include "Log.h"
#include "Storage.h"
#include "Defines\Effects.h"
#include "Parsers\EffectParser.h"
#include "Defines\Slides.h"

EffectHandler::EffectHandler(SlideElement *parent, Effect *elementEffect)
{
    effectOwner = parent;
    effectProto = elementEffect;

    int32 relativeOffset[2] = {0, 0};
    if (effectProto->offsetType)
    {
        if ((*effectProto->offsetType) == OFFSET_TYPE_RELATIVE)
        {
            for (uint32 i = 0; i <= 1; i++)
                relativeOffset[i] = int32(parent->position[i]);
        }
    }

    // cache start and end positions to make us able to handle them indepentently of parent effect
    if (effectProto->startPos)
    {
        for (uint32 i = 0; i <= 1; i++)
            startPos[i] = effectProto->startPos[i] + relativeOffset[i];
    }
    if (effectProto->endPos)
    {
        for (uint32 i = 0; i <= 1; i++)
            endPos[i] = effectProto->endPos[i] + relativeOffset[i];
    }

    // Fill effect queue from chained effects defined in effect def
    // for future: allow recursive chaining, but don't forget to avoid endless loop! (effect 1 -> effect 2 -> effect 1 -> ...)
    // note: reversed to make queue building easier from ordered vector of chained effects
    if (effectProto->m_effectChain)
    {
        Effect* tmp = NULL;
        for (std::vector<std::wstring>::reverse_iterator itr = effectProto->m_effectChain->rbegin(); itr != effectProto->m_effectChain->rend(); ++itr)
        {
            const wchar_t* name = (*itr).c_str();
            tmp = sStorage->GetEffect(itr->c_str());
            if (tmp)
                m_effectQueue.push_back(tmp);
        }
    }
    m_queuedEffectHandler = NULL;

    expired = false;
    runningQueue = false;

    startTime = clock();
}

EffectHandler::~EffectHandler()
{
}

void EffectHandler::QueueEffect(Effect* eff)
{
    if (isExpired())
        return;

    m_effectQueue.push_front(eff);
}

void EffectHandler::QueuedEffectExpired()
{
    if (m_effectQueue.empty())
    {
        SetExpired();
        return;
    }

    // remove last effect from queue
    m_effectQueue.erase(--m_effectQueue.end());
    delete m_queuedEffectHandler;
    m_queuedEffectHandler = NULL;

    if (m_effectQueue.empty())
        SetExpired();
}

void EffectHandler::UnblockPresentationIfNeeded()
{
    if (effectProto->isBlocking)
        sPresentation->InterfaceEvent(IE_EFFECT_END);
}

void EffectHandler::Animate()
{
    // Dispatcher for effect types
    // Here we only determine the type and call appropriate function

    if (isRunningQueue())
    {
        if (m_effectQueue.empty())
        {
            SetSelfExpired();
            return;
        }

        // If no effect handler defined, create one from last available effect
        if (!m_queuedEffectHandler)
            m_queuedEffectHandler = new EffectHandler(effectOwner ,(*(--m_effectQueue.end())));

        if (m_queuedEffectHandler->isExpired())
            QueuedEffectExpired();
        else
            m_queuedEffectHandler->Animate();

        return;
    }

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
        SetSelfExpired();
    }

    for (uint32 i = 0; i <= 1; i++)
        effectOwner->position[i] = int32(startPos[i]) + int32(timeCoef * float(int32(endPos[i]) - int32(startPos[i])));
}
