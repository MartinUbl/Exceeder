#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Parsers/StyleParser.h"
#include "Defines/Slides.h"
#include "Defines/Styles.h"
#include "Defines/Effects.h"
#include "Handlers/EffectHandler.h"
#include "Presentation.h"

void SlideElement::CreateEffectIfAny()
{
    myEffect = NULL;

    if (wcslen(elemEffect) > 0)
    {
        Effect* tmp = sStorage->GetEffect(elemEffect);
        if (tmp)
            myEffect = new EffectHandler(this, tmp);
    }
}

void SlideElement::PlayEffect(const wchar_t* effectId)
{
    Effect* tmp = sStorage->GetEffect(effectId);
    if (!tmp)
        return;

    if (myEffect && !myEffect->isExpired())
        myEffect->QueueEffect(tmp);
    else
    {
        if (myEffect)
            delete myEffect;

        myEffect = new EffectHandler(this, tmp);
    }
}

void SlideElement::Draw()
{
    if (myEffect && !myEffect->isExpired())
        myEffect->Animate();

    switch (elemType)
    {
        case SLIDE_ELEM_TEXT:
            typeText.Draw(this);
            break;
        case SLIDE_ELEM_IMAGE:
            typeImage.Draw(this);
        default:
            break;
    }
}

uint8 SlideElement::elemTextData::GetFeatureArrayIndexOf(Style* style)
{
    return (style->bold << 0 | style->italic << 1 | style->underline << 2 | style->strikeout << 3);
}

void SlideElement::elemTextData::Draw(SlideElement* parent)
{
    Style* myStyle = NULL;

    if (wcslen(parent->elemStyle) > 0)
        myStyle = sStorage->GetStyle(parent->elemStyle);
    else
        myStyle = sStorage->GetDefaultStyle();

    if (myStyle)
    {
        // Set color if any
        if (myStyle->fontColor)
        {
            uint32 color = (*(myStyle->fontColor));
            glColor3ub(COLOR_R(color),COLOR_G(color),COLOR_B(color));
        }

        int32 wrap = WW_NO_WRAP;
        if (parent->typeText.wrapSign == WW_PREWRAP)
        {
            if (parent->finalPosition[0] < 0)
                wrap = WW_WRAP_CANVAS;
            else
                wrap = -int32(parent->finalPosition[0]);
        }
        else
            wrap = parent->typeText.wrapSign;

        // draw text with own font. If not set, use default font
        if (outlist && outlist->size() > 0)
        {
            StyledTextList *tmp = new StyledTextList(outlist->begin(), outlist->end());
            for (ExprMap::iterator itr = outlistExpressions.begin(); itr != outlistExpressions.end(); ++itr)
            {
                int64 val = sPresentation->NumerateExpression(&(*itr).second);
                swprintf(((*tmp)[(*itr).first])->text, 255, L"%li", val);
            }

            sSimplyFlat->Drawing->PrintStyledText(parent->position[0], parent->position[1], wrap, tmp);
        }
        else if (myStyle->fontId >= 0)
            sSimplyFlat->Drawing->PrintText(myStyle->fontId, parent->position[0], parent->position[1], GetFeatureArrayIndexOf(myStyle), wrap, parent->typeText.text);
        else
            sSimplyFlat->Drawing->PrintText(sStorage->GetDefaultFontId(), parent->position[0], parent->position[1], FA_NORMAL, wrap, parent->typeText.text);

        // Set color back to white if necessary
        if (myStyle->fontColor)
            glColor3ub(255, 255, 255);
    }
}

void SlideElement::elemImageData::Draw(SlideElement* parent)
{
    Style* myStyle = NULL;

    if (wcslen(parent->elemStyle) > 0)
        myStyle = sStorage->GetStyle(parent->elemStyle);
    else
        myStyle = sStorage->GetDefaultStyle();

    if (parent->typeImage.resourceId > 0)
    {
        ResourceEntry* res = sStorage->GetResource(parent->typeImage.resourceId);

        uint32 color = 0;
        if (myStyle->overlayColor)
            color = (*(myStyle->overlayColor));

        if (res && res->image)
            sSimplyFlat->Drawing->DrawRectangle(parent->position[0], parent->position[1], parent->typeImage.size[0], parent->typeImage.size[1], 0, res->image->textureId);

        sSimplyFlat->Drawing->DrawRectangle(parent->position[0], parent->position[1], parent->typeImage.size[0], parent->typeImage.size[1], color, 0);
    }
}
