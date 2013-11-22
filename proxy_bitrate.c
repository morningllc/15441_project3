#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "proxy_bitrate.h"
#include "proxy.h"

extern status_t *proxy_stat;

void update_bitrate(time_t t1, time_t t2, int size)
{
	float alpha = proxy_stat->alpha;
	float t = (float)size/((float)t2-(float)t1);
	proxy_stat->t = alpha*t + (1-alpha)*proxy_stat->t;
	
	int next = next_bitrate();
	int prev = prev_bitrate();

	if(proxy_stat->t > 1.5*next){
		proxy_stat->bitrate = next;
	}

	if(proxy_stat->t < 1.5*prev){
		proxy_stat->bitrate = prev;
	}
}

int next_bitrate()
{
  int cur_bitrate = proxy_stat->bitrate;
	int next_bitrate = proxy_stat->bitrate;
	for(int i=0; i<4; i++){
		if(proxy_stat->bitrates[i]>cur_bitrate){
			next_bitrate = proxy_stat->bitrates[i];
			break;
		}
	}
	return next_bitrate;
}

int prev_bitrate()
{
	int cur_bitrate = proxy_stat->bitrate;
	int prev_bitrate = proxy_stat->bitrate;
	for(int i=3; i>=0; i--){
		if(proxy_stat->bitrates[i]<cur_bitrate){
			prev_bitrate = proxy_stat->bitrates[i];
			break;
		}
	}
	return prev_bitrate;
}
