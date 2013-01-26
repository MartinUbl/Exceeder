#ifndef EXCDR_HELPERS_H
#define EXCDR_HELPERS_H

typedef std::list< std::pair<const wchar_t*, std::wstring> > ParsedDefs;

extern wchar_t* CharVectorToString(std::vector<wchar_t>* vect);
extern wchar_t* ExtractFolderFromPath(const wchar_t* input);
extern wchar_t* ExtractFilenameFromPath(const wchar_t* input);
extern wchar_t* MakeFilePath(const wchar_t* dir, const wchar_t* filename);

extern wchar_t* LeftSide(const wchar_t* input, const wchar_t delim);
extern wchar_t* RightSide(const wchar_t* input, const wchar_t delim);

extern void ParseInputDefinitions(wchar_t* input, ParsedDefs* output);
extern wchar_t* GetDefinitionKeyValue(ParsedDefs* input, const wchar_t* key);
extern void GetPositionDefinitionKeyValue(ParsedDefs* input, const wchar_t* key, int32* destX, int32* destY);
extern float* ParseVector2(wchar_t* input, wchar_t delim);

extern wchar_t* RemoveBeginningSpaces(const wchar_t* input);

extern bool EqualString(const wchar_t* first, const wchar_t* second, bool caseInsensitive = false);
extern bool EqualString(const char* first, const char* second);
extern int ContainsString(const wchar_t* str, const wchar_t* substr);
extern bool IsNumeric(const wchar_t* inp);
extern int ToInt(const wchar_t* inp);
extern wchar_t UpperChar(wchar_t inp);
extern wchar_t LowerChar(wchar_t inp);
extern const wchar_t* ToUppercase(const wchar_t* input);
extern const wchar_t* ToLowercase(const wchar_t* input);
extern const wchar_t* ToWideString(const char* input);
extern const char*    ToMultiByteString(const wchar_t* input);

#define IN_SQUARE(x, y, a, b, c, d) (x >= a && x <= c && y >= b && y <= d)

#endif
