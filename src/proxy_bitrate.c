#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "proxy_bitrate.h"
#include "proxy.h"
#include "proxy_log.h"

extern status_t *proxy_stat;
extern int verbal;
void update_bitrate(long long t1, long long t2, int size, int bit, char* chunkname, char *client_ip)
{
	float alpha = proxy_stat->alpha;
	float t = (float)(((float)size)/((float)(t2-t1))) * 8000;
	proxy_stat->t = alpha*t + (1.0-alpha)*proxy_stat->t;
	
	int next = next_bitrate();
	int prev = prev_bitrate();

	if(proxy_stat->t > 1.5*next*1000){
		proxy_stat->bitrate = next;
	}

	if(proxy_stat->t < proxy_stat->bitrate*1000){
		proxy_stat->bitrate = prev;
	}
	if(verbal)
		fprintf(stdout,"---------------------throughput = %lf -------------------\n",t/1000);

	fprintf(stdout,"%.2f    %.2f   %d\n",t/1000,proxy_stat->t/1000,bit);
	logWrite((float)(t2-t1)/1000, t/1000, proxy_stat->t/1000, bit, client_ip, chunkname);
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
