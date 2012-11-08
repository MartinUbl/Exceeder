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

#ifndef VK_RETURN
#define VK_RETURN  0x0D
#define VK_SPACE   0x20
#define VK_CONTROL 0x11
#define VK_SHIFT   0x10
#endif

#endif
