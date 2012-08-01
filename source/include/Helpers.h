#ifndef EXCDR_HELPERS_H
#define EXCDR_HELPERS_H

extern const char* CharVectorToString(std::vector<char>* vect);
extern char* ExtractFolderFromPath(const char* input);
extern char* ExtractFilenameFromPath(const char* input);
extern char* MakeFilePath(const char* dir, const char* filename);

extern char* LeftSide(const char* input, const char delim);
extern char* RightSide(const char* input, const char delim);

extern char* RemoveBeginningSpaces(const char* input);

extern bool EqualString(const char* first, const char* second);
extern bool IsNumeric(const char* inp);
extern int ToInt(const char* inp);

#endif
