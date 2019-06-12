#ifndef UTILITIES_H
#define UTILITIES_H

#include "Log.h"


#define StrNCmp(x, y)   strncmp(x,y, strlen(y))

#define TIMEPOINT chrono::high_resolution_clock::time_point
#define NOW chrono::high_resolution_clock::now()

extern bool operator<(struct sockaddr_in a, struct sockaddr_in b);

#endif // UTILITIES_H