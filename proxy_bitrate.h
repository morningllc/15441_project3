#ifndef _PROXY_BITRATE
#define _PROXY_BITRATE

#include <time.h>

void update_bitrate(time_t t1, time_t t2, int size);

int next_bitrate(void);

int prev_bitrate(void);

#endif
