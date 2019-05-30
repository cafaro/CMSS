/********************************************************************
 Count-Min Space-Saving
 
 An algorithm for tracking frequent items in data streams
 
 by Massimo Cafaro, Italo Epicoco and Marco Pulimeno
 University of Salento, Italy
 
 *********************************************************************/



#ifndef _GF_H
#define _GF_H

#include <stdint.h>

typedef uint64_t uint64;

extern uint64 gf_primTable[64];

extern uint64 gf_Add(uint64 a, uint64 b);
extern uint64 gf_Sub(uint64 a, uint64 b);
extern uint64 gf_Mul(uint64 a, uint64 b, int n, uint64 prim);
extern uint64 gf_Div(uint64 a, uint64 b);
extern uint64 gf_Mod(uint64 a, uint64 b);
extern uint64 gf_Inv(uint64 a, int n, uint64 prim);
extern uint64 gf_Poly(uint64 x, uint64 *a, int d, int n, uint64 prim);

#endif


