#ifndef EXCDR_PARSER_H
#define EXCDR_PARSER_H

class Parser
{
    public:
        static wchar_t* ReadLine(FILE* f);
        static bool PrepareLine(wchar_t *&input);
};

#endif
