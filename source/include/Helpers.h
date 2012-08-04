#ifndef EXCDR_HELPERS_H
#define EXCDR_HELPERS_H

typedef std::list<std::pair<const char*, std::string>> ParsedDefs;

extern const char* CharVectorToString(std::vector<char>* vect);
extern char* ExtractFolderFromPath(const char* input);
extern char* ExtractFilenameFromPath(const char* input);
extern char* MakeFilePath(const char* dir, const char* filename);

extern char* LeftSide(const char* input, const char delim);
extern char* RightSide(const char* input, const char delim);

extern void ParseInputDefinitions(char* input, ParsedDefs* output);
extern const char* GetDefinitionKeyValue(ParsedDefs* input, const char* key);
extern void GetPositionDefinitionKeyValue(ParsedDefs* input, const char* key, uint32* destX, uint32* destY);

extern char* RemoveBeginningSpaces(const char* input);

extern bool EqualString(const char* first, const char* second);
extern bool IsNumeric(const char* inp);
extern int ToInt(const char* inp);
extern const char* ToUppercase(const char* input);
extern const char* ToLowercase(const char* input);

#endif
