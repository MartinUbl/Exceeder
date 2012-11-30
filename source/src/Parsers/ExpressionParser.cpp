#include "Global.h"
#include "Helpers.h"
#include "Parsers/ExpressionParser.h"

ExpressionVector* ExpressionParser::Parse(const wchar_t *input)
{
    if (!input || wcslen(input) == 0)
        return NULL;

    ExpressionVector* ret = new ExpressionVector;

    Entity en = EN_UNDEFINED;

    uint32 i, j;
    for (i = 0; i < wcslen(input); )
    {
        // try to determine, if it's an operator or parenthesis
        en = charEntity(input[i]);
        if (en != EN_UNDEFINED)
        {
            ret->push(en);
            i++;
            continue;
        }

        // it's a value
        for (j = i; j < wcslen(input); j++)
        {
            en = charEntity(input[j]);
            if (en != EN_UNDEFINED || j == wcslen(input)-1)
            {
                if (j == wcslen(input)-1)
                {
                    wchar_t* val = new wchar_t[j-i+2];
                    wcsncpy(val, &(input[i]), j-i+1);
                    val[j-i+1] = '\0';
                    ret->push(EN_VALUE, val);

                    i = j+1;
                }
                else
                {
                    wchar_t* val = new wchar_t[j-i+1];
                    wcsncpy(val, &(input[i]), j-i);
                    val[j-i] = '\0';
                    ret->push(EN_VALUE, val);
                    i = j;

                    goto continue_label;
                }
            }
        }
continue_label:;
    }

    return ret;
}

bool ExpressionParser::CheckSyntax(ExpressionVector* input)
{
    // empty or non-existant vectors are valid, and they are evaluated as zero in worker function, so we shall ignore it
    if (!input || input->empty())
        return true;

    // also input with only one element has to be containing only value element, otherwise it's evaluated as invalid
    if (input->size() == 1)
    {
        if ((*input)[0] == EN_VALUE)
            return true;
        else
            return false;
    }

    // There can't be more than one element type in a row (except parentheses)
    // We should say, that there are hypothetically three cathegories of element types:
    //   1. values (both numbers and strings, as references)
    //   2. operators
    //   3. parentheses
    int32 deepness = 0;
    for (uint32 i = 0; i < input->size()-1; i++)
    {
        if ((*input)[i] == EN_LEFT_PAR)
            deepness++;

        if ((*input)[i] == EN_RIGHT_PAR)
            deepness--;

        if (entityCathegory((*input)[i]) != EC_PARENTHESIS && entityCathegory((*input)[i]) == entityCathegory((*input)[i+1]))
            return false;
    }

    // parentheses aren't enclosed properly
    if (deepness != 0)
        return false;

    return true;
}

ExpressionTreeElement* ExpressionParser::BuildTree(ExpressionVector *input, uint32 start, uint32 count)
{
    if (!input || input->empty() || count == 0)
        return NULL;

    if (count == 1)
    {
        if ((*input)[start] == EN_VALUE)
            return BuildValueElement((*input).getValue(start));
        else
            return NULL;
    }
    if (count == 2 && ((*input)[start] == EN_PLUS || (*input)[start] == EN_MINUS))
    {
        if ((*input)[start+1] == EN_VALUE)
        {
            ExpressionTreeElement* tmp = BuildValueElement((*input).getValue(start+1));

            // remain from expression preparsing, does it have to be there anyways?
            // polarity of single element is decided when parsing addition elements one by one
            /*if ((*input)[start] == EN_PLUS)
                tmp->polarity = true;
            else if ((*input)[start] == EN_MINUS)
                tmp->polarity = false;*/

            return tmp;
        }
    }

    ExpressionTreeElement* tmp = new ExpressionTreeElement;
    tmp->valueType = VT_EXPRESSION;
    tmp->items.clear();

    if (((*input)[start] == EN_MINUS || (*input)[start] == EN_PLUS) && (*input)[start+1] == EN_LEFT_PAR && (*input)[start+count-1] == EN_RIGHT_PAR)
    {
        start += 2;
        count -= 3;
    }
    else if ((*input)[start] == EN_LEFT_PAR && (*input)[start+count-1] == EN_RIGHT_PAR)
    {
        start += 1;
        count -= 2;
    }

    uint32 i;
    uint32 lastBreak = start;
    int32 deepness = 0;

    // thanks to saving polarity of every element, it does not depend on the order of addition elements

    ExpressionTreeElement* child = NULL;

    // at first parse the simple addition elements (only values)
    for (i = start; i < input->size() && i < start+count; i++)
    {
        // if the string doesn't begin with polarity unary operator and we are on the top scope
        if (i != start && deepness == 0 && ((*input)[i] == EN_PLUS || (*input)[i] == EN_MINUS || (lastBreak != start && (i == input->size()-1 || i == start+count-1)) ))
        {
            if (i == input->size()-1 || i == start+count-1)
            {
                child = BuildTree(input, lastBreak, i-lastBreak+1);
                if ((*input)[lastBreak] == EN_MINUS)
                    child->polarity = false;

                tmp->items.push_back(child);
            }
            else
            {
                child = BuildTree(input, lastBreak, i-lastBreak);
                if ((*input)[lastBreak] == EN_MINUS)
                    child->polarity = false;

                tmp->items.push_back(child);
            }

            lastBreak = i;
        }

        if ((*input)[i] == EN_LEFT_PAR)
            deepness++;

        if ((*input)[i] == EN_RIGHT_PAR)
            deepness--;
    }

    // If parser already found some dividable elements, we won't care about them anymore, call the operation "addition", and parse inner expressions
    if (lastBreak != start)
        tmp->operation = OP_ADD;
    else
    {
        for (i = start; i < input->size() && i < start+count; i++)
        {
            // if the string doesn't begin with polarity unary operator and we are on the top scope
            if (i != start && deepness == 0 && ((*input)[i] == EN_MULT || (lastBreak != start && (i == input->size()-1 || i == start+count-1))))
            {
                if (i == input->size()-1 || i == start+count-1)
                    tmp->items.push_back(BuildTree(input, lastBreak+1, i-lastBreak));
                else
                    tmp->items.push_back(BuildTree(input, lastBreak, i-lastBreak));

                lastBreak = i;
            }

            if ((*input)[i] == EN_LEFT_PAR)
                deepness++;

            if ((*input)[i] == EN_RIGHT_PAR)
                deepness--;
        }

        if (lastBreak != start)
            tmp->operation = OP_MULTIPLY;
        else
        {
            for (i = start; i < input->size() && i < start+count; i++)
            {
                // if the string doesn't begin with polarity unary operator and we are on the top scope
                if (i != start && deepness == 0 && ((*input)[i] == EN_MODULO || (lastBreak != start && (i == input->size()-1 || i == start+count-1))))
                {
                    if (i == input->size()-1 || i == start+count-1)
                        tmp->items.push_back(BuildTree(input, lastBreak+1, i-lastBreak));
                    else
                        tmp->items.push_back(BuildTree(input, lastBreak, i-lastBreak));

                    lastBreak = i;
                }

                if ((*input)[i] == EN_LEFT_PAR)
                    deepness++;

                if ((*input)[i] == EN_RIGHT_PAR)
                    deepness--;
            }

            if (lastBreak != start)
                tmp->operation = OP_MODULO;
            else
            {
                for (i = start; i < input->size() && i < start+count; i++)
                {
                    // if the string doesn't begin with polarity unary operator and we are on the top scope
                    if (i != start && deepness == 0 && ((*input)[i] == EN_DIV || (lastBreak != start && (i == input->size()-1 || i == start+count-1))))
                    {
                        if (i == input->size()-1 || i == start+count-1)
                            tmp->items.push_back(BuildTree(input, lastBreak+1, i-lastBreak));
                        else
                            tmp->items.push_back(BuildTree(input, lastBreak, i-lastBreak));

                        lastBreak = i;
                    }

                    if ((*input)[i] == EN_LEFT_PAR)
                        deepness++;

                    if ((*input)[i] == EN_RIGHT_PAR)
                        deepness--;
                }

                if (lastBreak != start)
                    tmp->operation = OP_DIVIDE;
            }

        }
    }

    return tmp;
}

ExpressionTreeElement* ExpressionParser::BuildValueElement(const wchar_t* input)
{
    if (!input || wcslen(input) == 0)
        return NULL;

    ExpressionTreeElement* tmp = new ExpressionTreeElement();

    // integer type
    if (IsNumeric(input))
    {
        tmp->valueType = VT_INTEGER;
        tmp->value.asLong = ToInt(input);
        return tmp;
    }

    // floating type
    wchar_t *left = NULL, *right = NULL;
    left = LeftSide(input, L'.');
    right = RightSide(input, L'.');

    if (left && right && IsNumeric(left) && IsNumeric(right))
    {
        tmp->valueType = VT_FLOAT;
        tmp->value.asDouble = atof(ToMultiByteString(input));
        return tmp;
    }

    tmp->valueType = VT_STRING;
    tmp->value.asString = (wchar_t*)input;
    return tmp;
}
