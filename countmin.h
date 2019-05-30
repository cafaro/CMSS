/********************************************************************
 Count-Min Space-Saving
 
 An algorithm for tracking frequent items in data streams
 
 by Massimo Cafaro, Italo Epicoco and Marco Pulimeno
 University of Salento, Italy
 
 *********************************************************************/


#include <iostream>
#include <unordered_map>
#include <set>
#include <cmath>
#include "massdal.h"
#include "prng.h"




using namespace std;

#define min(x,y)	fmin(x, y)
#define max(x,y)	fmax(x, y)


typedef struct CMF_type{
    double eps;
    double delta;
    unsigned int depth; // number of hash functions
    unsigned int width;
    unsigned int **items;
    long **frequencies;
    unsigned int *hashInit; //hash function init seed
} CMF_type;


extern CMF_type *CMF_Init(double eps, double delta, unsigned int width, unsigned int depth, int seed);
extern void CMF_Destroy(CMF_type * cm);
extern long CMF_Size(CMF_type * cm);
extern void CMF_Update(CMF_type * cm, unsigned int item, long packet_size);
extern long CMF_PointEst(CMF_type * cm, unsigned int query);
extern set< pair<unsigned int, long> > *CMF_FindHH(CMF_type *cmf, long threshold);
