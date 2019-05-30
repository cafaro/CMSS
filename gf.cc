/********************************************************************
 Count-Min Space-Saving
 
 An algorithm for tracking frequent items in data streams
 
 by Massimo Cafaro, Italo Epicoco and Marco Pulimeno
 University of Salento, Italy
 
 *********************************************************************/


#include "gf.h"

/*---------------------------------------------------------------------



  Below are the table of primitive polynomials for
  GF(2^n), n = 0, ..., 63

  All the trinomials are taken from

     http://fchabaud.free.fr/English/default.php?COUNT=3&FILE0=Poly&FILE1=GF(2)&FILE2=Trinomials
     
  The other polynomials are taken from

     http://fchabaud.free.fr/English/default.php?COUNT=3&FILE0=Poly&FILE1=GF(2)&FILE2=LowSubDeg
	  
  1
  x + 1
  x2 + x + 1
  x3 + x + 1
  x4 + x + 1
  x5 + x2 + 1
  x6 + x + 1
  x7 + x + 1
  x8 + x4 + x3 + x + 1
  x9 + x + 1
  x10 + x3 + 1
  x11 + x2 + 1
  x12 + x3 + 1
  x13 + x4 + x3 + x + 1
  x14 + x5 + 1
  x15 + x + 1
  x16 + x5 + x3 + x + 1
  x17 + x3 + 1
  x18 + x3 + 1
  x19 + x5 + x2 + x + 1
  x20 + x3 + 1
  x21 + x2 + 1
  x22 + x + 1
  x23 + x5 + 1
  x24 + x4 + x3 + x + 1
  x25 + x3 + 1
  x26 + x4 + x3 + x + 1
  x27 + x5 + x2 + x + 1
  x28 + x + 1
  x29 + x2 + 1
  x30 + x + 1
  x31 + x3 + 1
  x32 + x7 + x3 + x2 + 1
  x33 + x10 + 1
  x34 + x7 + 1
  x35 + x2 + 1
  x36 + x9 + 1
  x37 + x5 + x4 + x3 + x2 + x + 1
  x38 + x6 + x5 + x + 1
  x39 + x4 + 1
  x40 + x5 + x4 + x3 + 1
  x41 + x3 + 1
  x42 + x7 + 1
  x43 + x6 + x4 + x3 + 1
  x44 + x5 + 1
  x45 + x4 + x3 + x + 1
  x46 + x + 1
  x47 + x5 + 1
  x48 + x5 + x3 + x2 + 1
  x49 + x9 + 1
  x50 + x4 + x3 + x2 + 1
  x51 + x6 + x3 + x + 1
  x52 + x3 + 1
  x53 + x6 + x2 + x + 1
  x54 + x9 + 1
  x55 + x7 + 1
  x56 + x7 + x4 + x2 + 1
  x57 + x4 + 1
  x58 + x19 + 1
  x59 + x6 + x5 + x4 + x3 + x + 1
  x60 + x + 1
  x61 + x5 + x2 + x1 + 1
  x62 + x29 + 1
  x63 + x + 1
*/
uint64 gf_primTable[64] = {
  1,
  (1ULL<<1) + 1,
  (1ULL<<2) + (1ULL<<1) + 1,
  (1ULL<<3) + (1ULL<<1) + 1,
  (1ULL<<4) + (1ULL<<1) + 1,
  (1ULL<<5) + (1ULL<<2) + 1,
  (1ULL<<6) + (1ULL<<1) + 1,
  (1ULL<<7) + (1ULL<<1) + 1,
  (1ULL<<8) + (1ULL<<4) + (1ULL<<3) + (1ULL<<1) + 1,
  (1ULL<<9) + (1ULL<<1) + 1,
  (1ULL<<10) + (1ULL<<3) + 1,
  (1ULL<<11) + (1ULL<<2) + 1,
  (1ULL<<12) + (1ULL<<3) + 1,
  (1ULL<<13) + (1ULL<<4) + (1ULL<<3) + (1ULL<<1) + 1,
  (1ULL<<14) + (1ULL<<5) + 1,
  (1ULL<<15) + (1ULL<<1) + 1,
  (1ULL<<16) + (1ULL<<5) + (1ULL<<3) + (1ULL<<1) + 1,
  (1ULL<<17) + (1ULL<<3) + 1,
  (1ULL<<18) + (1ULL<<3) + 1,
  (1ULL<<19) + (1ULL<<5) + (1ULL<<2) + (1ULL<<1) + 1,
  (1ULL<<20) + (1ULL<<3) + 1,
  (1ULL<<21) + (1ULL<<2) + 1,
  (1ULL<<22) + (1ULL<<1) + 1,
  (1ULL<<23) + (1ULL<<5) + 1,
  (1ULL<<24) + (1ULL<<4) + (1ULL<<3) + (1ULL<<1) + 1,
  (1ULL<<25) + (1ULL<<3) + 1,
  (1ULL<<26) + (1ULL<<4) + (1ULL<<3) + (1ULL<<1) + 1,
  (1ULL<<27) + (1ULL<<5) + (1ULL<<2) + (1ULL<<1) + 1,
  (1ULL<<28) + (1ULL<<1) + 1,
  (1ULL<<29) + (1ULL<<2) + 1,
  (1ULL<<30) + (1ULL<<1) + 1,
  (1ULL<<31) + (1ULL<<3) + 1,
  (1ULL<<32) + (1ULL<<7) + (1ULL<<3) + (1ULL<<2) + 1,
  (1ULL<<33) + (1ULL<<10) + 1,
  (1ULL<<34) + (1ULL<<7) + 1,
  (1ULL<<35) + (1ULL<<2) + 1,
  (1ULL<<36) + (1ULL<<9) + 1,
  (1ULL<<37) + (1ULL<<5) + (1ULL<<4) + (1ULL<<3) + (1ULL<<2) + (1ULL<<1) + 1,
  (1ULL<<38) + (1ULL<<6) + (1ULL<<5) + (1ULL<<1) + 1,
  (1ULL<<39) + (1ULL<<4) + 1,
  (1ULL<<40) + (1ULL<<5) + (1ULL<<4) + (1ULL<<3) + 1,
  (1ULL<<41) + (1ULL<<3) + 1,
  (1ULL<<42) + (1ULL<<7) + 1,
  (1ULL<<43) + (1ULL<<6) + (1ULL<<4) + (1ULL<<3) + 1,
  (1ULL<<44) + (1ULL<<5) + 1,
  (1ULL<<45) + (1ULL<<4) + (1ULL<<3) + (1ULL<<1) + 1,
  (1ULL<<46) + (1ULL<<1) + 1,
  (1ULL<<47) + (1ULL<<5) + 1,
  (1ULL<<48) + (1ULL<<5) + (1ULL<<3) + (1ULL<<2) + 1,
  (1ULL<<49) + (1ULL<<9) + 1,
  (1ULL<<50) + (1ULL<<4) + (1ULL<<3) + (1ULL<<2) + 1,
  (1ULL<<51) + (1ULL<<6) + (1ULL<<3) + (1ULL<<1) + 1,
  (1ULL<<52) + (1ULL<<3) + 1,
  (1ULL<<53) + (1ULL<<6) + (1ULL<<2) + (1ULL<<1) + 1,
  (1ULL<<54) + (1ULL<<9) + 1,
  (1ULL<<55) + (1ULL<<7) + 1,
  (1ULL<<56) + (1ULL<<7) + (1ULL<<4) + (1ULL<<2) + 1,
  (1ULL<<57) + (1ULL<<4) + 1,
  (1ULL<<58) + (1ULL<<19) + 1,
  (1ULL<<59) + (1ULL<<6) + (1ULL<<5) + (1ULL<<4) + (1ULL<<3) + (1ULL<<1) + 1,
  (1ULL<<60) + (1ULL<<1) + 1,
  (1ULL<<61) + (1ULL<<5) + (1ULL<<2) + (1ULL<<1) + 1,
  (1ULL<<62) + (1ULL<<29) + 1,
  (1ULL<<63) + (1ULL<<1) + 1,
};

/* compute a+b */
uint64 gf_Add(uint64 a, uint64 b) 
{
  return a ^ b;
}

/* compute a-b */
uint64 gf_Sub(uint64 a, uint64 b) 
{
  return a ^ b;
}

/* compute a*b */
uint64 gf_Mul(uint64 a, uint64 b, int n, uint64 prim) 
{
  uint64 bitn = 1ULL << n;
  uint64 p = a;
  uint64 r = 0;
  
  while (b) {
    if (b & 1) r ^= p;
    b >>= 1;
    p <<= 1;
    if (p & bitn) p ^= prim;
  }
  return r;
}

void gf_LongDiv(uint64 a, uint64 b, uint64 *qp, uint64 *rp) 
{
  int i, alen, blen;
  uint64 q, r;
  
  for(alen = 0; (1ULL<<alen) <= a; alen++);
  alen--;
  
  for(blen = 0; (1ULL<<blen) <= b; blen++);
  blen--;
  
  q = 0; r = a;
  for (i = alen-blen; i >= 0; i--) {
    q <<= 1;
    if ( r & (1ULL<<(blen+i)) ) {
      r ^= (b<<i);
      q += 1;
    }
  }

  *qp = q;
  *rp = r;
}

/* compute a/b */
uint64 gf_Div(uint64 a, uint64 b)
{
  uint64 q, r;
  gf_LongDiv(a, b, &q, &r);
  return q;
}

/* compute a%b */
uint64 gf_Mod(uint64 a, uint64 b) 
{
  uint64 q, r;
  gf_LongDiv(a, b, &q, &r);
  return r;
}


void gf_ExtendedEuclid(uint64 a, uint64 b,
                       uint64 *g, uint64 *u, uint64 *v,
                       int n, uint64 prim) 
{
  uint64 g0, g1, u0, u1, v0, v1, q, r, tmp;

  g0 = a; g1 = b;
  u0 = 1; u1 = 0;
  v0 = 0; v1 = 1;
  while (g1 != 0) {
    gf_LongDiv(g0, g1, &q, &r);
    g0 = g1; g1 = r;
    tmp = u0; u0 = u1; u1 = gf_Sub(tmp, gf_Mul(u1, q, n, prim));
    tmp = v0; v0 = v1; v1 = gf_Sub(tmp, gf_Mul(v1, q, n, prim));
  }
  
  *g = g0 ; *u = u0 ; *v = v0;
}

/* find b s.t. Mul(b,a) = 1 */
uint64 gf_Inv(uint64 a, int n, uint64 prim)
{
  uint64 g, u, v;
  
  /* a*u + prim*v = 1 => u = 1/a */
  gf_ExtendedEuclid(a, prim, &g, &u, &v, n, prim);
  return u;
}

/* find a[0]*x^{d-1} + a[1]*x^{d-2} + ... a[d-1]*x^0 */
uint64 gf_Poly(uint64 x, uint64 *a, int d, int n, uint64 prim)
{
  int i;
  uint64 p, mask;

  p = 0;
  mask = (1ULL << n) - 1;
  for (i = 0; i < d; i++) {
    p = gf_Add(gf_Mul(p, x, n, prim), (a[i] & mask));
  }
  return p;
}

#if 0

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int i;
  uint64 j;
  int n, N;
  uint64 prim;

  n = 8;
  prim = gf_primTable[n];
  N = 32;
  
  for (i = 1; i < n && i < N; i++) {
    j = gf_Inv((uint64)i, n, prim);
    fprintf(stderr, "GaloisField(2^%d): Inv(%d)=%llu \tMul(%d,%llu)=%llu\n",
            n, i, j, i, j, gf_Mul(i,j,n,prim));
  }
}

#endif
