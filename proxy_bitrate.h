#ifndef _PROXY_BITRATE
#define _PROXY_BITRATE

#include <time.h>

void update_bitrate(long long t1, long long t2, int size, int bit, char* chunkname, char* client_ip);

int next_bitrate(void);

int prev_bitrate(void);

#endif
