#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Parsers\EffectParser.h"
#include "Defines\Effects.h"

bool EffectParser::ParseFile(const char *path)
{
    if (!path)
        return false;

    FILE* efile = fopen(path, "r");
    if (!efile)
        return false;

    std::vector<std::string> efffile;
    char* ln = NULL;
    while ((ln = ReadLine(efile)) != NULL)
    {
        if (PrepareLine(ln))
            efffile.push_back(ln);
    }

    return Parse(&efffile);
}

bool EffectParser::Parse(std::vector<std::string> *input)
{
    if (!input)
        return false;

    char* left = NULL;
    char* right = NULL;

    char* effname = NULL;
    Effect* tmp = NULL;

    for (std::vector<std::string>::const_iterator itr = input->begin(); itr != input->end(); ++itr)
    {
        left = LeftSide(itr->c_str(), ' ');
        right = RightSide(itr->c_str(), ' ');

        // when parsing style definition
        if (effname)
        {
            if (!tmp)
                tmp = new Effect;

            // move type
            if (EqualString(left, "\\MOVE"))
            {
                if (EqualString(right, "linear"))
                    tmp->moveType = new uint32(MOVE_TYPE_LINEAR);
                else if (EqualString(right, "circular"))
                    tmp->moveType = new uint32(MOVE_TYPE_CIRCULAR);
                else if (EqualString(right, "bezier"))
                    tmp->moveType = new uint32(MOVE_TYPE_BEZIER);
                else
                    RAISE_ERROR("EffectParser: Unknown move type '%s'", right);
            }
            // starting position
            else if (EqualString(left, "\\START_POS"))
            {
                char* xpos = LeftSide(right, ',');
                char* ypos = LeftSide(right, ',');

                if (!IsNumeric(xpos) || !IsNumeric(ypos))
                    RAISE_ERROR("EffectParser: Non-numeric value supplied as position parameter");

                tmp->startPos = new uint32[2];
                tmp->startPos[0] = ToInt(xpos);
                tmp->startPos[1] = ToInt(ypos);
            }
            // end position
            else if (EqualString(left, "\\END_POS"))
            {
                char* xpos = LeftSide(right, ',');
                char* ypos = LeftSide(right, ',');

                if (!IsNumeric(xpos) || !IsNumeric(ypos))
                    RAISE_ERROR("EffectParser: Non-numeric value supplied as position parameter");

                tmp->endPos = new uint32[2];
                tmp->endPos[0] = ToInt(xpos);
                tmp->endPos[1] = ToInt(ypos);
            }
            // time for whole effect
            else if (EqualString(left, "\\TIMER"))
            {
                if (!IsNumeric(right))
                    RAISE_ERROR("EffectParser: Non-numeric value supplied as timer parameter");

                tmp->effectTimer = new uint32(ToInt(right));
            }
            // sets effect as blocking
            else if (EqualString(left, "\\BLOCKING"))
            {
                tmp->isBlocking = true;
            }
            // sets effect as non blocking (it is, by default, but some global config option can change that)
            else if (EqualString(left, "\\NOBLOCKING"))
            {
                tmp->isBlocking = false;
            }
            else if (EqualString(left, "\\DEF_END"))
            {
                sStorage->AddNewEffect(effname, tmp);
                effname = NULL;
                tmp = NULL;
            }

            continue;
        }

        // when not parsing effect definition
        // file version
        if (EqualString(left, "\\EXCEEDER_EFFECTS_FILE_VERSION"))
        {
            //
        }
        // start of definition
        else if (EqualString(left, "\\DEF_BEGIN"))
        {
            effname = right;
            continue;
        }
        // end
        else if (EqualString(left, "\\END"))
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
