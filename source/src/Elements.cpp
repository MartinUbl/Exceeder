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
    
}
