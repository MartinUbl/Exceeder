#include "Global.h"
#include "Presentation.h"
#include "Log.h"
#include "Storage.h"
#include "Defines/Effects.h"
#include "Parsers/EffectParser.h"
#include "Handlers/EffectHandler.h"
#include "Defines/Slides.h"
#include "Vector.h"
#include "Position.h"

EffectHandler::EffectHandler(SlideElement *parent, Effect *elementEffect, bool fromQueue)
{
    effectOwner = parent;
    effectProto = elementEffect;

    m_queuePos = 0;

    int32 relativeOffset[2] = {0, 0};
    if (effectProto->offsetType)
    {
        if ((*effectProto->offsetType) == OFFSET_TYPE_RELATIVE)
        {
            for (uint32 i = 0; i <= 1; i++)
                relativeOffset[i] = int32(parent->position[i]);
        }
    }

    // reset start and end position
    for (uint32 i = 0; i <= 1; i++)
    {
        startPos[i] = parent->position[i];
        endPos[i] = parent->position[i];
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
            {
                m_effectQueue.push_back(tmp);
                m_queuePos++;
            }
        }
    }
    m_queuedEffectHandler = NULL;

    expired = false;
    runningQueue = false;

    // set parent final position for this effect (only if we are not listed as queued effect)
    if (!fromQueue)
    {
        for (uint32 i = 0; i <= 1; i++)
            parent->finalPosition[i] = endPos[i];

        Effect* tmp = NULL;
        for (std::list<Effect*>::const_reverse_iterator itr = m_effectQueue.rbegin(); itr != m_effectQueue.rend(); ++itr)
        {
            tmp = (*itr);
            if (tmp->moveType)
            {
                if (tmp->offsetType && (*(tmp->offsetType)) == OFFSET_TYPE_RELATIVE)
                {
                    for (uint32 i = 0; i <= 1; i++)
                        parent->finalPosition[i] = endPos[i] + tmp->endPos[i];
                }
                else
                    for (uint32 i = 0; i <= 1; i++)
                        parent->finalPosition[i] = tmp->endPos[i];
            }
        }
    }

    if (effectProto->moveType && (*(effectProto->moveType)) == MOVE_TYPE_CIRCULAR)
    {
        // phase is constant deviation from the mathematical "zero" angle
        // ..kind of magic here, yes.
        phase = acos(  ((startPos[0] - endPos[0])/2)  /  (sqrt(pow(float(startPos[0] - endPos[0]), 2) + pow(float(startPos[1] - endPos[1]), 2)) / 2 ) );
        // radius is the distance from any end point to center of the line between start and end points
        radius = sqrt(pow(float(endPos[0] - startPos[0]), 2) + pow(float(endPos[1] - startPos[1]), 2)) / 2;

        // this vector is used as radius vector - when computing coords, we have to move to the center of rotation
        movementVector[0].x = float(endPos[0] - startPos[0]) / 2.0f;
        movementVector[0].y = float(endPos[1] - startPos[1]) / 2.0f;
    }

    if (effectProto->srcOpacity)
        effectOwner->opacity = (*effectProto->srcOpacity);

    // save starting opacity for later calculation
    startOpacity = effectOwner->opacity;

    if (effectProto->srcScale)
        effectOwner->scale = (*effectProto->srcScale);

    startScale = effectOwner->scale;

    startTime = clock();
}

EffectHandler::~EffectHandler()
{
}

void EffectHandler::RollBack()
{
    if (effectOwner)
    {
        for (uint32 i = 0; i <= 1; i++)
            effectOwner->position[i] = startPos[i];

        effectOwner->opacity = startOpacity;
        effectOwner->scale = startScale;
    }

    startTime = clock();
    SetExpired(false);
    runningQueue = false;

    for (std::list<Effect*>::iterator itr = m_effectQueue.begin(); itr != m_effectQueue.end(); )
    {
        if (!(*itr))
        {
            itr = m_effectQueue.erase(itr);
            continue;
        }

        if (m_effectQueueFlags.find(*itr) != m_effectQueueFlags.end())
        {
            itr = m_effectQueue.erase(itr);
            continue;
        }

        itr++;
    }

    m_queuePos = m_effectQueue.size();
}

void EffectHandler::RollBackLastQueued()
{
    if (m_queuePos == m_effectQueue.size())
        return;

    uint32 i;
    std::list<EffectHandler*>::iterator itr = m_effectQueueHandlers.end();
    --itr;
    for (i = 1; i < m_queuePos; i++)
        if (itr != m_effectQueueHandlers.begin())
            --itr;

    for (i = 0; i <= 1; i++)
        effectOwner->position[i] = (*itr)->startPos[i];

    effectOwner->opacity = (*itr)->startOpacity;
    effectOwner->scale = (*itr)->startScale;

    if (m_queuePos != 0)
        m_queuePos--;
}

void EffectHandler::QueueEffect(Effect* eff)
{
    if (isExpired())
        SetExpired(false);

    m_effectQueue.push_back(eff);
    m_queuePos++; // due to moving every single element by one

    m_effectQueueFlags[eff] = EFFECT_QF_ADDED_LATER;

    if (eff->moveType)
    {
        if (eff->offsetType && (*(eff->offsetType)) == OFFSET_TYPE_RELATIVE)
        {
            for (uint32 i = 0; i <= 1; i++)
                effectOwner->finalPosition[i] = endPos[i] + eff->endPos[i];
        }
        else
            for (uint32 i = 0; i <= 1; i++)
                effectOwner->finalPosition[i] = eff->endPos[i];
    }
}

void EffectHandler::QueuedEffectExpired()
{
    if (m_effectQueue.empty())
    {
        SetExpired();
        return;
    }

    // remove last effect from queue
    //m_effectQueue.erase(--m_effectQueue.end());
    m_queuePos--;
    //delete m_queuedEffectHandler;
    m_queuedEffectHandler = NULL;

    if (m_queuePos == 0)
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
        if (m_queuePos == 0)
        {
            SetSelfExpired();
            return;
        }

        // If no effect handler defined, create one from last available effect
        if (!m_queuedEffectHandler)
        {
            std::list<Effect*>::iterator it = m_effectQueue.end();
            for (uint32 i = 0; i < m_queuePos; i++)
                it--;

            m_queuedEffectHandler = new EffectHandler(effectOwner ,(*it), true);
            m_effectQueueHandlers.push_back(m_queuedEffectHandler);
        }

        if (m_queuedEffectHandler->isExpired())
            QueuedEffectExpired();
        else
            m_queuedEffectHandler->Animate();

        return;
    }

    // Calculate time coefficient to determine position
    // if this method returns false, the effect just finished
    if (!(effectProto->effectTimer))
    {
        timeCoef = 1.0;
        SetSelfExpired();
    }
    else
    {
        if (!GetTimeCoef(timeCoef))
            SetSelfExpired();

        CalculateEffectProgress(timeCoef);
    }

    // movement
    if (effectProto->moveType)
    {
        // Linear movement
        if ((*effectProto->moveType) == MOVE_TYPE_LINEAR)
            AnimateMoveLinear();
        // Circle movement
        else if ((*effectProto->moveType) == MOVE_TYPE_CIRCULAR)
            AnimateMoveCircular();
        // Bezier movement (cubic)
        else if ((*effectProto->moveType) == MOVE_TYPE_BEZIER)
            AnimateMoveBezier();
    }

    // fade
    if (effectProto->fadeType)
    {
        // Fade in
        if ((*effectProto->fadeType) == FADE_TYPE_IN)
            AnimateFadeIn();
        // Fade out
        else if ((*effectProto->fadeType) == FADE_TYPE_OUT)
            AnimateFadeOut();
    }

    // scale
    if (effectProto->scaleType)
    {
        if ((*effectProto->scaleType) == SCALE_TYPE_SCALE)
            AnimateScaleScale();
    }

    // Synchronization
    if (isExpired())
    {
        // Synchronize ending position with demanded coordinates
        if (effectProto->moveType && (effectOwner->position[0] != endPos[0] || effectOwner->position[1] != endPos[1]))
        {
            effectOwner->position[0] = endPos[0];
            effectOwner->position[1] = endPos[1];
        }

        // Synchronize also opacity
        if (effectProto->fadeType && effectProto->destOpacity && effectOwner->opacity != (*effectProto->destOpacity))
            effectOwner->opacity = (*effectProto->destOpacity);
    }
}

void EffectHandler::CalculateEffectProgress(float &coef, uint8 progressType)
{
    switch (progressType)
    {
        case EP_LINEAR:
        default:
            break;
        case EP_SINUS:
            coef = sin(coef * M_PI / 2);
            break;
        case EP_QUADRATIC:
            coef = pow(coef, 2);
            break;
    }
}

void EffectHandler::CalculateEffectProgress(float &coef)
{
    if (effectProto->progressType)
        CalculateEffectProgress(coef, *effectProto->progressType);
}

void EffectHandler::AnimateMoveLinear()
{
    // Check for required things
    if (!(effectProto->startPos && effectProto->endPos))
        return;

    for (uint32 i = 0; i <= 1; i++)
        effectOwner->position[i] = int32(startPos[i]) + int32(timeCoef * float(int32(endPos[i]) - int32(startPos[i])));
}

void EffectHandler::AnimateMoveCircular()
{
    // Check for required things
    if (!(effectProto->startPos && effectProto->endPos))
        return;

    float angle = phase;

    if (effectProto->circlePlus && (*effectProto->circlePlus))
        angle -= timeCoef*M_PI;
    else
        angle += timeCoef*M_PI;

    effectOwner->position[0] = int32(startPos[0] + movementVector[0].x + cos(angle)*radius);
    effectOwner->position[1] = int32(startPos[1] + movementVector[0].y + sin(angle)*radius);
}

void EffectHandler::AnimateMoveBezier()
{
    // Check for required things
    if (!(effectProto->startPos && effectProto->endPos && effectProto->bezierVector))
        return;

    CVector2 AC = effectProto->bezierVector[0];
    CVector2 BD = effectProto->bezierVector[1];

    Position2 A(startPos);
    Position2 B(endPos);
    Position2 C = A + AC;
    Position2 D = B + BD;

    CVector2 CD = Position2::makeVector(C,D);

    Position2 M = A + AC * timeCoef;
    Position2 N = B + BD * (1 - timeCoef);
    Position2 P = C + CD * timeCoef;

    CVector2 MP = Position2::makeVector(M,P);
    CVector2 NP = Position2::makeVector(N,P);

    Position2 Q = M + MP * timeCoef;
    Position2 R = N + NP * (1 - timeCoef);

    CVector2 QR = Position2::makeVector(Q,R);

    effectOwner->position[0] = int32(Q.x + QR.x * timeCoef);
    effectOwner->position[1] = int32(Q.y + QR.y * timeCoef);
}

void EffectHandler::AnimateFadeIn()
{
    // Check for required things
    if (!(effectProto->destOpacity))
        return;

    effectOwner->opacity = uint8(startOpacity + float((*effectProto->destOpacity) - startOpacity)*timeCoef);
}

void EffectHandler::AnimateFadeOut()
{
    // Check for required things
    if (!(effectProto->destOpacity))
        return;

    effectOwner->opacity = uint8(startOpacity - float(startOpacity - (*effectProto->destOpacity))*timeCoef);
}

void EffectHandler::AnimateScaleScale()
{
    if (!(effectProto->destScale))
        return;

    effectOwner->scale = startScale + ((*effectProto->destScale) - startScale)*timeCoef;
}
