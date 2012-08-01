#ifndef EXCDR_PARSER_H
#define EXCDR_PARSER_H

class Parser
{
    public:
        static char* ReadLine(FILE* f);
        static bool PrepareLine(char *&input);
};

#endif
