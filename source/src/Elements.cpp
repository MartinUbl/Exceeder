#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Parsers/StyleParser.h"
#include "Defines/Slides.h"
#include "Defines/Styles.h"
#include "Defines/Effects.h"
#include "Handlers/EffectHandler.h"

void SlideElement::CreateEffectIfAny()
{
    myEffect = NULL;

    if (elemEffect.size() > 0)
    {
        Effect* tmp = sStorage->GetEffect(elemEffect.c_str());
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
    if (parent->elemStyle.size() > 0)
    {
        Style* myStyle = sStorage->GetStyle(parent->elemStyle.c_str());

        // Set color if any
        if (myStyle && myStyle->fontColor)
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
            sSimplyFlat->Drawing->PrintStyledText(parent->position[0], parent->position[1], wrap, outlist);
        else if (myStyle && myStyle->fontId >= 0)
            sSimplyFlat->Drawing->PrintText(myStyle->fontId, parent->position[0], parent->position[1], GetFeatureArrayIndexOf(myStyle), wrap, parent->typeText.text.c_str());
        else
            sSimplyFlat->Drawing->PrintText(sStorage->GetDefaultFontId(), parent->position[0], parent->position[1], FA_NORMAL, wrap, parent->typeText.text.c_str());

        // Set color back to white if necessary
        if (myStyle && myStyle->fontColor)
            glColor3ub(255, 255, 255);
    }
}

void SlideElement::elemImageData::Draw(SlideElement* parent)
{
    if (parent->typeImage.resourceId > 0)
    {
        ResourceEntry* res = sStorage->GetResource(parent->typeImage.resourceId);
        if (res && res->image)
            sSimplyFlat->Drawing->DrawRectangle(parent->position[0], parent->position[1], parent->typeImage.size[0], parent->typeImage.size[1], 0, res->image->textureId);
    }
}
