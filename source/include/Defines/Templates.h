#ifndef EXCDR_TEMPLATES_H
#define EXCDR_TEMPLATES_H

#include "Defines/Slides.h"

#define TEMPLATE_ID_DELIMITER L"__--__"

struct SlideTemplate
{
    SlideTemplate()
    {
    }

    SlideElementVector m_elements;
};

typedef std::map<const wchar_t*, SlideTemplate*> TemplateMap;

#endif
