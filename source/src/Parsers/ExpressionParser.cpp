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

    // if it doesn't fit our defined integer or float pattern, let's call it string
    tmp->valueType = VT_STRING;
    tmp->value.asString = (wchar_t*)input;
    return tmp;
}

ValueType ExprTreeElem::getEvaluableType()
{
    switch (valueType)
    {
        // integer, float and string are final types
        case VT_INTEGER:
        case VT_FLOAT:
        case VT_STRING:
            return valueType;
        // expression have to contain some of final types
        case VT_EXPRESSION:
        {
            // not evaluable when no items are present
            if (items.empty())
                return VT_STRING;

            ValueType et = items[0]->getEvaluableType();
            if (et != VT_INTEGER && et != VT_FLOAT)
                return VT_STRING;

            for (uint32 i = 1; i < items.size(); i++)
            {
                if (items[i]->getEvaluableType() != et && (items[i]->getEvaluableType() == VT_INTEGER || items[i]->getEvaluableType() == VT_FLOAT))
                    et = VT_FLOAT;
                else if (items[i]->getEvaluableType() != et)
                    return VT_STRING;
            }

            return et;
        }
    }

    return VT_STRING;
}

void ExprTreeElem::SimplifyChildren()
{
    if (items.empty())
        return;

    // At first simplify recursively all the children to make it the simpliest it can be
    for (uint32 i = 0; i < items.size(); i++)
        if (items[i]->valueType == VT_EXPRESSION)
            items[i]->SimplifyChildren();

    ValueType et = items[0]->valueType;
    bool changed = false;

    // Fill initial simplifying base value
    valueUnion vun = {((operation == OP_ADD)?0:1)};
    if (et == VT_INTEGER)
    {
        changed = true;
        vun.asLong = items[0]->value.asLong*((items[0]->polarity)?1:(-1));
        items.erase(items.begin());
    }
    else if (et == VT_FLOAT)
    {
        changed = true;
        vun.asDouble = items[0]->value.asDouble*((items[0]->polarity)?1:(-1));
        items.erase(items.begin());
    }
    else
        et = VT_INTEGER;

    // Now iterate through all items and attempt to simplify every items - they have to be numerical (integer / float)
    for (uint32 i = 0; i < items.size(); i++)
    {
        if (items[i]->valueType == VT_INTEGER || items[i]->valueType == VT_FLOAT)
        {
            changed = true;

            // We need to change to floating point value type if some of operands are float
            if (items[i]->valueType != et)
            {
                et = VT_FLOAT;
                vun.asDouble = (double)vun.asLong;
            }

            // Calculate
            // involve polarity as multiplying right operand with 1 or -1
            switch (operation)
            {
                case OP_ADD:
                {
                    if (et == VT_INTEGER)
                        vun.asLong += items[i]->value.asLong*((items[i]->polarity)?1:(-1));
                    else
                        vun.asDouble += items[i]->value.asDouble*((items[i]->polarity)?1:(-1));
                    break;
                }
                case OP_MULTIPLY:
                {
                    if (et == VT_INTEGER)
                        vun.asLong *= items[i]->value.asLong*((items[i]->polarity)?1:(-1));
                    else
                        vun.asDouble *= items[i]->value.asDouble*((items[i]->polarity)?1:(-1));
                    break;
                }
                case OP_DIVIDE:
                {
                    // If the division of division is not integer type, we need to change to floating value
                    // i.e. division 3 by 2 requires it, but 6 by 3 not - it's still fine to divide it as integers
                    if (et == VT_INTEGER && ((vun.asLong % items[i]->value.asLong) != 0))
                    {
                        et = VT_FLOAT;
                        vun.asDouble = (double)vun.asLong;
                    }

                    if (et == VT_INTEGER)
                    {
                        if (items[i]->value.asLong != 0)
                            vun.asLong /= items[i]->value.asLong*((items[i]->polarity)?1:(-1));
                    }
                    else
                    {
                        if (items[i]->value.asDouble != 0)
                            vun.asDouble /= items[i]->value.asDouble*((items[i]->polarity)?1:(-1));
                    }
                    break;
                }
                case OP_MODULO:
                {
                    // modulo is applicable only on integer types
                    // TODO for future: round floating values if modulo is requested, or throw an error
                    if (et == VT_INTEGER)
                    {
                        if (items[i]->value.asLong != 0)
                            vun.asLong %= items[i]->value.asLong*((items[i]->polarity)?1:(-1));
                    }
                    /*else
                    {
                        if (items[i]->value.asDouble != 0)
                            vun.asDouble %= items[i]->value.asDouble*((items[i]->polarity)?1:(-1));
                    }*/
                    break;
                }
            }

            // And finally erase this element from map if it was properly evaluated with previous element(s)
            items.erase(items.begin()+i);
        }
    }

    // If we changed something (erased and element and evaluated it with result), we need to put the overall result to items back
    if (changed)
    {
        // decide polarity to keep uniformity
        bool pol = true;
        if (et == VT_FLOAT && vun.asDouble < 0.0)
        {
            vun.asDouble = -vun.asDouble;
            pol = false;
        }
        else if (et == VT_INTEGER && vun.asLong < 0)
        {
            vun.asLong = -vun.asLong;
            pol = false;
        }

        // and finally insert new element
        ExprTreeElem* tmp = new ExprTreeElem(pol, et, vun);

        items.push_back(tmp);
    }

    // If there is only one numerical item element, the whole expression is now not needed - change it to numerical type
    if (items.size() == 1 && (items[0]->valueType == VT_INTEGER || items[0]->valueType == VT_FLOAT))
    {
        valueType = items[0]->valueType;
        memcpy(&value, &(items[0]->value), sizeof(valueUnion));

        // equal polarities = positive value, otherwise it's negative value
        if (!polarity == items[0]->polarity)
            polarity = false;
        else
            polarity = true;

        items.clear();
    }

    // if there are only integer/float values left, we will simplify ourselves again to get only one-value
    for (uint32 i = 0; i < items.size(); i++)
        if (items[i]->valueType != VT_INTEGER && items[i]->valueType != VT_FLOAT)
            return;

    SimplifyChildren();
}
