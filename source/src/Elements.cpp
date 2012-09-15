#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Defines\Slides.h"
#include "Defines\Styles.h"
#include "Defines\Effects.h"

void SlideElement::Draw()
{
    switch (elemType)
    {
        case SLIDE_ELEM_TEXT:
            typeText.Draw(this);
            break;
        default:
            break;
    }
}

void SlideElement::elemTextData::Draw(SlideElement* parent)
{
    if (parent->elemStyle.size() > 0)
    {
        Style* myStyle = sStorage->GetStyle(parent->elemStyle.c_str());

        if (myStyle->fontId >= 0)
            sSimplyFlat->Drawing->PrintText(myStyle->fontId, parent->typeText.position[0], parent->typeText.position[1], parent->typeText.text.c_str());
    }
}
