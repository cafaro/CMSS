/********************************************************************
 Count-Min Space-Saving
 
 An algorithm for tracking frequent items in data streams
 
 by Massimo Cafaro, Italo Epicoco and Marco Pulimeno
 University of Salento, Italy
 
 *********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include "countmin.h"
#include "xxhash.h"


CMF_type * CMF_Init(double eps, double delta, unsigned int width, unsigned int depth, int seed)
{     // Initialize the sketch based on user-supplied size
    CMF_type *cm = NULL;
    prng_type *prng = NULL;
    
    cm=(CMF_type *) malloc(sizeof(CMF_type));
    
    prng=prng_Init(-abs(seed),2);
    // initialize the generator to pick the hash functions
    
    if (cm && prng) {

        if (depth <= 0) {
            depth = ceil(- log(delta));
        }

        if (width <=0) {
            width = ceil(exp(1) / (2 * eps));
        }

        cm->eps = eps;
        cm->delta = delta;
        cm->depth=depth;
        cm->width=width;
        cm->items=(unsigned int **)calloc(sizeof(unsigned int *),cm->depth);
        cm->frequencies=(long **)calloc(sizeof(long *),cm->depth);
        cm->hashInit = (unsigned int *)calloc(sizeof(unsigned int),cm->depth);
        

        if (cm->hashInit && cm->items && cm->frequencies) {
            for (unsigned int j = 0; j < depth; j++) {
                cm->hashInit[j] = prng_int(prng) & MOD;
                cm->items[j]=(unsigned int *) calloc(2*cm->width, sizeof(unsigned int));
                cm->frequencies[j]=(long *) calloc(2*cm->width, sizeof(long));
            }
        } else {
            fprintf(stdout,"Memory allocation failed\n");
            free(cm); cm = NULL;
            return NULL;
        }
    }
    
    return cm;
}

void CMF_Destroy(CMF_type * cm)
{     // get rid of a sketch and free up the space
    if (!cm) return;
    
    if (cm->items) {
        for(int j = 0; j<cm->depth; j++)
            free(cm->items[j]);
        
        free(cm->items); cm->items=NULL;
    }
    if (cm->frequencies) {
        for(int j = 0; j<cm->depth; j++)
            free(cm->frequencies[j]);
        
        free(cm->frequencies); cm->frequencies=NULL;
    }
    
    free(cm);  cm = NULL;
}

long CMF_Size(CMF_type * cm)
{ // return the size of the sketch in bytes
    int items, frequencies;
    if (!cm) return 0;
    
    //fprintf(stderr,"bucket: %lu bytes\n", 2 *(sizeof(unsigned int) + sizeof(long)));
    
    items = 2*cm->width*cm->depth*sizeof(unsigned int);
    frequencies = 2*cm->width*cm->depth*sizeof(long);
    
    return(items + frequencies);
}



void CMF_Update(CMF_type * cm, unsigned int item, long packet_size)
{
    if (!cm) return;
    
#pragma unroll
#pragma clang vectorize(enable) interleave(enable)
    for (unsigned int j = 0; j < cm->depth; j++) {
        
        unsigned int bucket = (XXH32(&item, sizeof(unsigned int), cm->hashInit[j]) % cm->width) << 1;
       
        // apply space saving
        unsigned int *stored_items = &(cm->items[j][bucket]);
        long *stored_freqs = &(cm->frequencies[j][bucket]);
        
        int max_offset = (stored_freqs[0] > stored_freqs[1])? 0 : 1;
        
        if(stored_items[max_offset] == item){
            stored_freqs[max_offset] += packet_size;
        }
        else {
            stored_items[1 - max_offset] = item;
            stored_freqs[1 - max_offset] += packet_size;
        }
    }
}



long CMF_PointEst(CMF_type * cm, unsigned int query)
{
    // return an estimate of the count of an item by taking the minimum
    int j;
    long ans;
    
    if (!cm) return 0;
    
    ans = LONG_MAX;
    
    for (j=0;j<cm->depth;j++){
        
        int bucket = XXH32(&query, sizeof(unsigned int), cm->hashInit[j]) % cm->width;
        
        unsigned int stored_item1 = cm->items[j][2*bucket];
        unsigned int stored_item2 = cm->items[j][2*bucket+1];
        long stored_freq1 = cm->frequencies[j][2*bucket];
        long stored_freq2 = cm->frequencies[j][2*bucket+1];
        long minimum = min(stored_freq1, stored_freq2);
        
        if(query == stored_item1)
            ans=min(ans,stored_freq1);
        else {
            if(query == stored_item2)
                ans=min(ans,stored_freq2);
            else
                ans=min(ans,minimum);
        }
    }
    
    return (ans);
}



set< pair<unsigned int, long> > *CMF_FindHH(CMF_type *cmf, long threshold)
{ // find all items whose estimated count is greater than threshold
    
    
    set< pair<unsigned int, long> > *results = new  set< pair<unsigned int, long> >[cmf->width];
    
    pair<unsigned int, long> chhpair; // candidate heavy hitter and its frequency
    
    long point_query = 0;
    
    // find heavy hitters
    // scan all of the buckets
    for (int j = 0; j < cmf->depth; j++) {
        for (int i = 0; i < cmf->width; i++){
            
            unsigned int stored_item1 = cmf->items[j][2*i];
            unsigned int stored_item2 = cmf->items[j][2*i+1];
            long stored_freq1 = cmf->frequencies[j][2*i];
            long stored_freq2 = cmf->frequencies[j][2*i+1];
            
            long maximum = max(stored_freq1, stored_freq2);
            
            if(maximum > threshold){
                
                if(maximum == stored_freq1){
                    point_query = CMF_PointEst(cmf, stored_item1);
                    chhpair =  make_pair(stored_item1, point_query);
                }
                else{
                    point_query = CMF_PointEst(cmf, stored_item2);
                    chhpair =  make_pair(stored_item2, point_query);
                }

                if(point_query > threshold){
                    results->insert(chhpair);
                }
            }
        }
    }
    
    return (results);
}
