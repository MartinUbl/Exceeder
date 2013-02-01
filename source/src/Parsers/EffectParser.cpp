#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Parsers/EffectParser.h"
#include "Handlers/EffectHandler.h"
#include "Defines/Effects.h"

bool EffectParser::ParseFile(const wchar_t *path)
{
    if (!path)
        return false;

#ifdef _WIN32
    FILE* efile = _wfopen(path, L"r, ccs=UTF-8");
#else
    FILE* efile = fopen(ToMultiByteString(path), "r, ccs=UTF-8");
#endif
    if (!efile)
        return false;

    std::vector<std::wstring> efffile;
    wchar_t* ln = NULL;
    while ((ln = ReadLine(efile)) != NULL)
    {
        if (PrepareLine(ln))
            efffile.push_back(ln);
    }

    return Parse(&efffile);
}

bool EffectParser::Parse(std::vector<std::wstring> *input)
{
    if (!input)
        return false;

    wchar_t* left = NULL;
    wchar_t* right = NULL;

    wchar_t* effname = NULL;
    Effect* tmp = NULL;

    for (std::vector<std::wstring>::const_iterator itr = input->begin(); itr != input->end(); ++itr)
    {
        left = LeftSide(itr->c_str(), ' ');
        right = RightSide(itr->c_str(), ' ');

        // when parsing style definition
        if (effname)
        {
            if (!tmp)
                tmp = new Effect;

            // move type
            if (EqualString(left, L"\\MOVE", true))
            {
                if (EqualString(right, L"linear", true))
                    tmp->moveType = new uint32(MOVE_TYPE_LINEAR);
                else if (EqualString(right, L"circle", true))
                    tmp->moveType = new uint32(MOVE_TYPE_CIRCULAR);
                else if (EqualString(right, L"circle+", true))
                {
                    tmp->moveType = new uint32(MOVE_TYPE_CIRCULAR);
                    tmp->circlePlus = true;
                }
                else if (EqualString(right, L"circle-", true))
                {
                    tmp->moveType = new uint32(MOVE_TYPE_CIRCULAR);
                    tmp->circlePlus = false;
                }
                else if (EqualString(right, L"bezier", true))
                    tmp->moveType = new uint32(MOVE_TYPE_BEZIER);
                else
                    RAISE_ERROR("EffectParser: Unknown move type '%s'", right);
            }
            // move/other offset from
            else if (EqualString(left, L"\\OFFSET", true))
            {
                if (EqualString(right, L"absolute", true))
                    tmp->offsetType = new uint32(OFFSET_TYPE_ABSOLUTE);
                else if (EqualString(right, L"relative", true))
                    tmp->offsetType = new uint32(OFFSET_TYPE_RELATIVE);
                else
                    RAISE_ERROR("EffectParser: Unknown offset type '%s'", right);
            }
            // effect progress
            else if (EqualString(left, L"\\PROGRESS", true))
            {
                if (EqualString(right, L"linear", true))
                    tmp->progressType = new uint32(EP_LINEAR);
                else if (EqualString(right, L"sinus", true))
                    tmp->progressType = new uint32(EP_SINUS);
                else if (EqualString(right, L"quadratic", true))
                    tmp->progressType = new uint32(EP_QUADRATIC);
                else
                    RAISE_ERROR("EffectParser: Unknown progress type '%s'", right);
            }
            // starting position
            else if (EqualString(left, L"\\START_POS", true))
            {
                wchar_t* xpos = LeftSide(right, ',');
                wchar_t* ypos = RightSide(right, ',');

                if (!IsNumeric(xpos) || !IsNumeric(ypos))
                    RAISE_ERROR("EffectParser: Non-numeric value supplied as position parameter");

                tmp->startPos = new int32[2];
                tmp->startPos[0] = ToInt(xpos);
                tmp->startPos[1] = ToInt(ypos);
            }
            // end position
            else if (EqualString(left, L"\\END_POS", true))
            {
                wchar_t* xpos = LeftSide(right, ',');
                wchar_t* ypos = RightSide(right, ',');

                if (!IsNumeric(xpos) || !IsNumeric(ypos))
                    RAISE_ERROR("EffectParser: Non-numeric value supplied as position parameter");

                tmp->endPos = new int32[2];
                tmp->endPos[0] = ToInt(xpos);
                tmp->endPos[1] = ToInt(ypos);
            }
            // in case of bezier movement, start vector is needed
            else if (EqualString(left, L"\\START_VECTOR", true))
            {
                wchar_t* ang = LeftSide(right, ' ');
                wchar_t* mag = RightSide(right, ' ');

                if (ang && !mag)
                {
                    float* vec = ParseVector2(ang, L',');
                    if (vec)
                    {
                        if (!tmp->bezierVector)
                            tmp->bezierVector = new CVector2[2];
                        tmp->bezierVector[0] = CVector2(vec[0], vec[1]);
                    }
                }
                else if (ang && IsNumeric(ang) && IsNumeric(mag))
                {
                    float angle = (float)ToInt(ang);
                    float magnitude = (float)ToInt(mag);

                    if (!tmp->bezierVector)
                        tmp->bezierVector = new CVector2[2];

                    tmp->bezierVector[0] = CVector2(magnitude*cos(angle*M_PI/180.0f), magnitude*sin(angle*M_PI/180.0f));
                }
                else
                    RAISE_ERROR("EffectParser: invalid start vector coordinates/parameters '%S' used", right);
            }
            // end vector
            else if (EqualString(left, L"\\END_VECTOR", true))
            {
                wchar_t* ang = LeftSide(right, ' ');
                wchar_t* mag = RightSide(right, ' ');

                if (ang && !mag)
                {
                    float* vec = ParseVector2(ang, L',');
                    if (vec)
                    {
                        if (!tmp->bezierVector)
                            tmp->bezierVector = new CVector2[2];

                        tmp->bezierVector[1] = CVector2(vec[0], vec[1]);
                    }
                }
                else if (ang && IsNumeric(ang) && IsNumeric(mag))
                {
                    float angle = (float)ToInt(ang);
                    float magnitude = (float)ToInt(mag);

                    if (!tmp->bezierVector)
                        tmp->bezierVector = new CVector2[2];

                    tmp->bezierVector[1] = CVector2(magnitude*cos(angle*M_PI/180.0f), magnitude*sin(angle*M_PI/180.0f));
                }
                else
                    RAISE_ERROR("EffectParser: invalid end vector coordinates/parameters '%S' used", right);
            }
            // time for whole effect
            else if (EqualString(left, L"\\TIMER", true))
            {
                if (!IsNumeric(right))
                    RAISE_ERROR("EffectParser: Non-numeric value supplied as timer parameter");

                tmp->effectTimer = new uint32(ToInt(right));
            }
            // move direction (in case of circle)
            if (EqualString(left, L"\\DIRECTION", true))
            {
                if (EqualString(right, L"plus", true) || EqualString(right, L"right", true))
                    tmp->circlePlus = true;
                else if (EqualString(right, L"minus", true) || EqualString(right, L"left", true))
                    tmp->circlePlus = false;
                else
                    RAISE_ERROR("EffectParser: Unknown move direction '%s'", right);
            }
            // sets effect as blocking
            else if (EqualString(left, L"\\BLOCKING", true))
            {
                tmp->isBlocking = true;
            }
            // sets effect as non blocking (it is, by default, but some global config option can change that)
            else if (EqualString(left, L"\\NOBLOCKING", true))
            {
                tmp->isBlocking = false;
            }
            else if (EqualString(left, L"\\NEXT_EFFECT", true))
            {
                if (tmp->m_effectChain == NULL)
                    tmp->m_effectChain = new std::vector<std::wstring>;

                tmp->m_effectChain->push_back(right);
            }
            else if (EqualString(left, L"\\DEF_END", true))
            {
                sStorage->AddNewEffect(effname, tmp);
                effname = NULL;
                tmp = NULL;
            }

            continue;
        }

        // when not parsing effect definition
        // file version
        if (EqualString(left, L"\\EXCEEDER_EFFECTS_FILE_VERSION", true))
        {
            //
        }
        // start of definition
        else if (EqualString(left, L"\\DEF_BEGIN", true))
        {
            effname = right;
            continue;
        }
        // end
        else if (EqualString(left, L"\\END", true))
        {
            return true;
        }
        else
        {
            RAISE_ERROR("EffectParser: Unrecognized key '%s'", left);
        }
    }

    return true;
}
