#ifndef _FETCH_H_
#define _FETCH_H_

struct FetchParms
{
    void (*OnGotData)(char *incomingBuffer, char* output);
    char message[300];
    char url[300];
};

void fetch(struct FetchParms *fetchParms);

#endif