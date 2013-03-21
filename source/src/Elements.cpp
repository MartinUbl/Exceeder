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
        {
            if (elemType != SLIDE_ELEM_PLAY_EFFECT)
                myEffect = new EffectHandler(this, tmp);
        }
    }
}

void SlideElement::OnCreate()
{
    needRecalc = false;
    CalculatePosition();
}

void SlideElement::CalculatePosition()
{
    needRecalc = false;

    Style* myStyle = NULL;

    if (wcslen(elemStyle) > 0)
        myStyle = sStorage->GetStyle(elemStyle);

    if (!myStyle)
        myStyle = sStorage->GetDefaultStyle();

    uint32 width = 0, height = 0;
    if (elemType == SLIDE_ELEM_TEXT)
    {
        if (typeText.outlist && typeText.outlist->size() > 0)
        {
            for (StyledTextList::const_iterator itr = typeText.outlist->begin(); itr != typeText.outlist->end(); ++itr)
            {
                if ((*itr)->fontId >= 0)
                    if (SF->Drawing->GetFontHeight((*itr)->fontId) > height)
                        height = SF->Drawing->GetFontHeight((*itr)->fontId);

                if ((*itr)->fontId >= 0)
                    width += SF->Drawing->GetTextWidth((*itr)->fontId, (*itr)->feature, (*itr)->text);
                else
                    needRecalc = true;
            }
        }
        else
        {
            if (myStyle->fontId >= 0)
            {
                width = SF->Drawing->GetTextWidth(myStyle->fontId, elemTextData::GetFeatureArrayIndexOf(myStyle), typeText.text);
                height = SF->Drawing->GetFontHeight(myStyle->fontId);
            }
            else
                needRecalc = true;
        }
    }
    else if (elemType == SLIDE_ELEM_IMAGE)
    {
        width = typeImage.size[0];
        height = typeImage.size[1];
    }

    if (!needRecalc)
    {
        if (position[0] == POS_CENTER)
            position[0] = (sStorage->GetOriginalScreenWidth()-width) / 2;
        else if (position[0] == POS_LEFT)
            position[0] = 0;
        else if (position[0] == POS_RIGHT)
            position[0] = sStorage->GetOriginalScreenWidth()-width;

        if (position[1] == POS_CENTER)
            position[1] = (sStorage->GetOriginalScreenHeight()-height) / 2;
        else if (position[1] == POS_TOP)
            position[1] = 0;
        else if (position[1] == POS_BOTTOM)
            position[1] = sStorage->GetOriginalScreenHeight()-height;
    }
}

void SlideElement::PlayEffect(const wchar_t* effectId)
{
    Effect* tmp = sStorage->GetEffect(effectId);
    if (!tmp)
        return;

    if (myEffect)
        myEffect->QueueEffect(tmp);
    else
        myEffect = new EffectHandler(this, tmp);
}

void SlideElement::Draw()
{
    if (needRecalc)
        CalculatePosition();

    sSimplyFlat->Drawing->PushMatrix();

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

    sSimplyFlat->Drawing->PopMatrix();
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
        uint32 color = 0;
        if (myStyle->fontColor)
        {
            color = (*(myStyle->fontColor));

            if (!myStyle->overlayColor)
                glColor4ub(COLOR_R(color),COLOR_G(color),COLOR_B(color), parent->opacity);
            else
                glColor4ub(uint8(COLOR_R(color)+(1-(float)COLOR_A((*myStyle->overlayColor))/255.0f)*(COLOR_R(color)-COLOR_R((*myStyle->overlayColor)))),
                           uint8(COLOR_G(color)+(1-(float)COLOR_A((*myStyle->overlayColor))/255.0f)*(COLOR_G(color)-COLOR_G((*myStyle->overlayColor)))),
                           uint8(COLOR_B(color)+(1-(float)COLOR_A((*myStyle->overlayColor))/255.0f)*(COLOR_B(color)-COLOR_B((*myStyle->overlayColor)))),
                           parent->opacity);
        }
        else if (myStyle->overlayColor)
        {
            color = (*(myStyle->overlayColor));
            glColor4ub(COLOR_R(color),COLOR_G(color),COLOR_B(color), parent->opacity);
        }

        glTranslatef((GLfloat)parent->position[0], (GLfloat)parent->position[1], 0);
        glScalef(parent->scale, parent->scale, parent->scale);
        glTranslatef(-(GLfloat)parent->position[0], -(GLfloat)parent->position[1], 0);

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

            for (StyledTextList::iterator itr = tmp->begin(); itr != tmp->end(); ++itr)
            {
                (*itr)->color &= 0xFFFFFF00; // remove alpha value
                (*itr)->color |= uint8(parent->opacity); // and add parent opacity
            }

            sSimplyFlat->Drawing->PrintStyledText(parent->position[0], parent->position[1], wrap, tmp);

            delete tmp;
        }
        else if (myStyle->fontId >= 0)
            sSimplyFlat->Drawing->PrintText(myStyle->fontId, parent->position[0], parent->position[1], GetFeatureArrayIndexOf(myStyle), wrap, parent->typeText.text);
        else
            sSimplyFlat->Drawing->PrintText(sStorage->GetDefaultFontId(), parent->position[0], parent->position[1], FA_NORMAL, wrap, parent->typeText.text);

        // Set color back to white if necessary
        if (myStyle->fontColor || myStyle->overlayColor)
            glColor4ub(255, 255, 255, 255);
    }
}

void SlideElement::elemImageData::Draw(SlideElement* parent)
{
    Style* myStyle = NULL;

    if (wcslen(parent->elemStyle) > 0)
        myStyle = sStorage->GetStyle(parent->elemStyle);

    if (parent->typeImage.resourceId > 0)
    {
        ResourceEntry* res = sStorage->GetResource(parent->typeImage.resourceId);

        uint32 color = 0;
        if (res->image->colorOverlay)
            color = res->image->colorOverlay;
        else if (myStyle && myStyle->overlayColor)
            color = (*(myStyle->overlayColor));

        uint8 newOpacity = uint8( float(COLOR_A(color)) * ((float)parent->opacity)/255.0f );

        color = (color & 0xFFFFFF00) | newOpacity;

        if (res && res->image)
            sSimplyFlat->Drawing->DrawRectangle(parent->position[0], parent->position[1], parent->typeImage.size[0], parent->typeImage.size[1], MAKE_COLOR_RGBA(255,255,255,parent->opacity), res->image->textureId);

        sSimplyFlat->Drawing->DrawRectangle(parent->position[0], parent->position[1], parent->typeImage.size[0], parent->typeImage.size[1], color, 0);
    }
}
