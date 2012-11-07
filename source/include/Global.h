#ifndef EXCDR_GLOBAL_H
#define EXCDR_GLOBAL_H

#ifdef _WIN32
  #include <windows.h>
  #include <direct.h>
#else
  #include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <ctime>

using namespace std;

#include <SimplyFlat.h>
#include "Helpers.h"

#endif
