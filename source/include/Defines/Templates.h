#ifndef EXCDR_TEMPLATES_H
#define EXCDR_TEMPLATES_H

#include "Defines/Slides.h"

struct SlideTemplate
{
    SlideTemplate()
    {
    }

    SlideElementVector m_elements;
};

typedef std::map<const wchar_t*, SlideTemplate*> TemplateMap;

#endif
