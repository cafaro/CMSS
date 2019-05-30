/********************************************************************
 Count-Min Space-Saving
 
 An algorithm for tracking frequent items in data streams
 
 by Massimo Cafaro, Italo Epicoco and Marco Pulimeno
 University of Salento, Italy
 
 *********************************************************************/


#ifndef _HASH_H
#define _HASH_H
#include <stdlib.h>             
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include "gf.h"

//Provides 5-universal hash functions.
//Both a direct polynomial based one, 
//and the much faster tabulation based one.


typedef uint8_t INT8;
typedef uint16_t INT16;
typedef uint32_t INT32;
typedef uint64_t INT64;
typedef INT64 * INT96;

#define Prime8 257
#define Prime16 65537

typedef INT64 * Hash32bit2univID;
typedef INT64 * Hash48bit2univID;
typedef INT64 * Hash64bit2univID;
typedef INT32 Hash32bitUnivID;
typedef INT64 Hash48bitUnivID;
typedef INT64 Hash64bitUnivID;
typedef INT64 * Hash61bit5univID;
typedef INT64** Hash89bit5univID;
typedef INT32** HashShortTab5univID_32to32;
typedef INT64** HashShortTab5univID_32to64;
typedef INT64** HashShortTab5univID_48to64;
typedef INT64** HashShortTab5univID_64to64;
typedef INT32** HashCharTab2univID_32to32;
typedef INT32** HashCharTab5univID_32to32;
typedef INT64** HashCharTab5univID_32to64;
typedef INT64** HashCharTab5univID_40to64;
typedef INT64** HashCharTab5univID_48to64;
typedef INT64** HashCharTab5univID_56to64;
typedef INT64** HashCharTab2univID_64to64;
typedef struct {
  INT64 h;    // hash value
  INT64 v0;   // vector for parallel addition
  INT32 v1;   // vector for parallel addition
} charTab_64to64_s;
typedef struct {
  charTab_64to64_s t0[8][256];
#ifdef COMPRESS  
  INT64 t1[7][256+7*(Prime8-256)];
#else
  INT64 t1[7][(Prime8-2)*8+1];
#endif
} HashCharTab5univID_64to64_s;
typedef HashCharTab5univID_64to64_s* HashCharTab5univID_64to64;

// different views of a 32-bit integer
typedef union {
  INT32 as_int32; 
  INT16 as_int16s[2];
  INT8 as_int8s[4];
} int32views;

// different views of a 64-bit double word
typedef union {
  INT64 as_int64;
  INT32 as_int32s[2];
  INT16 as_int16s[4];
  INT8 as_int8s[8];
} int64views;

const INT64 LowOnes = (((INT64)1)<<32)-1;
const INT64 Ones48  = (((INT64)1)<<48)-1;

#define HIGH(x)  ((x)>>32)        // extract higher 32 bits from INT64
#define LOW(x)   ((x)&LowOnes)    // extract lower 32 bits from INT64
#define LOW48(x) ((x)&Ones48)     // extract lower 48 bits from INT64

const INT64 Prime61 = (((INT64)1)<<61) - 1;

const INT64 Prime89[3] = {
  (((INT64)1)<<32)-1, (((INT64)1)<<32)-1, (((INT64)1)<<25)-1
};

const INT64 Prime89_0 = (((INT64)1)<<32)-1;
const INT64 Prime89_1 = (((INT64)1)<<32)-1;
const INT64 Prime89_2 = (((INT64)1)<<25)-1;
const INT64 Prime89_21 = (((INT64)1)<<57)-1;

//Random numbers from random.org based on atmostpheric noise. 
INT64 Random64[125]= {0x285eb7722a62ce6eull,
                      0xa84c7463e2b7856bull,
		      0xb29100d6abcc8666ull,
		      0xf9bfca5b7461fb1full,
		      0x51c8dafc30c88dadull,
		      0x0687468c365ec51dull,
		      0x2bf2cd3ad64b6218ull,
		      0xc20565a4d1f00f9eull,
		      0x7d533575d313c658ull,
		      0xbf2fba6b00725b85ull,
		      0x4cfc2f6557e722beull,
		      0xedbce818556dfb2bull,
		      0xd9df508027db1bbeull,
		      0x21f2d922f712f48bull,
		      0xcd8b289d83a65804ull,
		      0x4a19cfb02445a6d2ull,
		      0xc95e56b1e19a4f94ull,
		      0xcfeeeaccaf477248ull,
		      0x4eec3378b73bb25cull,
		      0x18a3f38d1c48b2beull,
		      0x71b79ab5cb1e3730ull,
		      0x79cdb30e2f38309dull,
		      0xb41d4983bdbc8d6full,
		      0xba9f57c01b66b7e3ull,
		      0xe400503c95c16568ull,
                      0x5977bfd4630294f1ull,
                      0x57d4d7940099676full,
                      0xd945de9268f4b191ull,
		      0x4034711421eaf806ull,
		      0x6d8108a4a6d58c22ull,
		      0x5c421818ddbdd4eeull,
		      0xbd9b7e4071713c13ull,
		      0xa60d1d6e793e5eb2ull,
		      0x7443fb031b8ec6c7ull,
		      0xd8290c7120e05d4aull,
		      0x797fb1d9a6a8d27full,
		      0x543ec268ab1f2e45ull,
		      0xcaf2a6139701f320ull,
		      0x9519611d130bee47ull,
		      0x19bbc533f018be1aull,
		      0xdbfacdfeb573133dull,
		      0x3255dacc4c7bfe12ull,
		      0xbc6c9228e5518f6eull,
		      0xb5c1681037347178ull,
		      0xbaaa2cfef186bfadull,
		      0x1834b8ab0f9e876eull,
		      0x9d7b7f228433e0f7ull,
		      0xa99cc292d003dd09ull,
		      0xc0cb8037046b5295ull,
		      0xa6ffa3d4671aa3d2ull,
		      0xc27023fbee2862e6ull,
		      0x5a9877bcc4bd3172ull,
		      0xfcb0da3caf9fcfe0ull,
		      0xc35ef57e1866ceaeull,
		      0xd4f7c927d169a115ull,
		      0x699054518fc74756ull,
		      0xa75cbf617fc9db8dull,
		      0x7f3adf4369665a9cull,
		      0x6b98eeeb4c517f42ull,
		      0xa12e44f5de954f24ull,
		      0x5789ded4dced978eull,
		      0xe4dd20ed27cd3567ull,
		      0x9b4e90c365b8790bull,
		      0xd486ed6e099f499bull,
		      0x3f3d0ccfeaa0c0dcull,
		      0x548c746cdb192adaull,
		      0x8ce636d8469fe2dcull,
		      0xca59ec929549a6a1ull,
		      0x647b9878deaba1f0ull,
		      0xebbb4b2641c54d34ull,
		      0x7be6d2918b680abdull,
		      0x02ad265fb4733490ull,
		      0xfe1053044faf3486ull,
		      0x539ea358ff6b6df3ull,
		      0x025d73224a2b5826ull,
		      0x7daad302451f41b3ull,
		      0x6038455ddb535976ull,
		      0x8d6d00a9a728a067ull,
		      0xe9f03d61d4965d59ull,
		      0x38314b8102daff3bull,
		      0x56b335e7893a76f1ull,
		      0x1048ca2f415712abull,
		      0xa9bc989a891dc173ull,
		      0xb741df3ae02836c2ull,
		      0x7711e6c6f5830783ull,
		      0x8edbf2be9226e24bull,
		      0xe4a4b8ba310fc2e2ull,
		      0xbc7b67f4a02f23c8ull,
		      0x5669b1a9d6d8df17ull,
		      0xdd3ebf2e3c516e26ull,
		      0x77bdd6def5236c4full,
		      0x9aeb54bdffacd65eull,
		      0xab676483404a21b8ull,
		      0xf7270f77a9d1b3a3ull,
		      0x3794e1cdcc7de433ull,
		      0x8e2b74d3a06aa56aull,
		      0x572698d05b901d40ull,
		      0x7bd6c265c1dd5cdfull,
		      0xd2f68a53970db82eull,
		      0x0e1d5f5dd9bd23bdull,
                      0x92cde6c9ba6f142dull,
                      0x771e249e953e58bfull,
                      0x10c283cd9f3d013dull,
                      0xfcb621f3be0010f0ull,
                      0xed26d55db69495c8ull,
                      0x9cb048b2ced79aaaull,
                      0x789d722605e49f5dull,
                      0x8844e35e0dd4abf8ull,
                      0x471147bc50351474ull,
                      0x5f37ef038ab4ecd5ull,
                      0xa7afec130eac62ecull,
                      0xefb7b55bacb20b49ull,
                      0xccbf0f0117f6280full,
                      0xce8964c421d9fcb4ull,
                      0x449059c87bb6a1bbull,
                      0x33735e2e819dda26ull,
                      0xe5a36c81f568631dull,
                      0x2767aa44aa3e418bull,
                      0x1c03b25783ea9652ull,
                      0x469b5b18f2310c8bull,
                      0x329f42358cbaaf0full,
                      0x726f9e805aab76ffull,
                      0xecba7d19f3dff63bull,
                      0xc846ef9f06f008d8ull,
                      0xdc403ac0d9292141ull};

#ifndef NEXTRAND
#define NEXTRAND 0
#endif

int next64random = NEXTRAND;
int end64random = 125;

INT32 Random32[125]= {0xa7b7856bu,
                      0xa84c7463u,
		      0xb29100d6u,
		      0xf9bfca5bu,
		      0x51c8dafcu,
		      0x0687468cu,
		      0x2bf2cd3au,
		      0xc20565a4u,
		      0x7d533575u,
		      0xbf2fba6bu,
		      0x4cfc2f65u,
		      0xedbce818u,
		      0xd9df5080u,
		      0x21f2d922u,
		      0xcd8b289du,
		      0x4a19cfb0u,
		      0xc95e56b1u,
		      0xcfeeeaccu,
		      0x4eec3378u,
		      0x18a3f38du,
		      0x71b79ab5u,
		      0x79cdb30eu,
		      0xb41d4983u,
		      0xba9f57c0u,
		      0xe400503cu,
                      0x5977bfd4u,
                      0x57d4d794u,
                      0xd945de92u,
		      0x40347114u,
		      0x6d8108a4u,
		      0x5c421818u,
		      0xbd9b7e40u,
		      0xa60d1d6eu,
		      0x7443fb03u,
		      0xd8290c71u,
		      0x797fb1d9u,
		      0x543ec268u,
		      0xcaf2a613u,
		      0x9519611du,
		      0x19bbc533u,
		      0xdbfacdfeu,
		      0x3255daccu,
		      0xbc6c9228u,
		      0xb5c16810u,
		      0xbaaa2cfeu,
		      0x1834b8abu,
		      0x9d7b7f22u,
		      0xa99cc292u,
		      0xc0cb8037u,
		      0xa6ffa3d4u,
		      0xc27023fbu,
		      0x5a9877bcu,
		      0xfcb0da3cu,
		      0xc35ef57eu,
		      0xd4f7c927u,
		      0x69905451u,
		      0xa75cbf61u,
		      0x7f3adf43u,
		      0x6b98eeebu,
		      0xa12e44f5u,
		      0x5789ded4u,
		      0xe4dd20edu,
		      0x9b4e90c3u,
		      0xd486ed6eu,
		      0x3f3d0ccfu,
		      0x548c746cu,
		      0x8ce636d8u,
		      0xca59ec92u,
		      0x647b9878u,
		      0xebbb4b26u,
		      0x7be6d291u,
		      0x02ad265fu,
		      0xfe105304u,
		      0x539ea358u,
		      0x025d7322u,
		      0x7daad302u,
		      0x6038455du,
		      0x8d6d00a9u,
		      0xe9f03d61u,
		      0x38314b81u,
		      0x56b335e7u,
		      0x1048ca2fu,
		      0xa9bc989au,
		      0xb741df3au,
		      0x7711e6c6u,
		      0x8edbf2beu,
		      0xe4a4b8bau,
		      0xbc7b67f4u,
		      0x5669b1a9u,
		      0xdd3ebf2eu,
		      0x77bdd6deu,
		      0x9aeb54bdu,
		      0xab676483u,
		      0xf7270f77u,
		      0x3794e1cdu,
		      0x8e2b74d3u,
		      0x572698d0u,
		      0x7bd6c265u,
		      0xd2f68a53u,
		      0x0e1d5f5du,
                      0xba6f142du,
                      0x953e58bfu,
                      0x9f3d013du,
                      0xbe0010f0u,
                      0xb69495c8u,
                      0xced79aaau,
                      0x05e49f5du,
                      0x0dd4abf8u,
                      0x50351474u,
                      0x8ab4ecd5u,
                      0x0eac62ecu,
                      0xacb20b49u,
                      0x17f6280fu,
                      0x21d9fcb4u,
                      0x7bb6a1bbu,
                      0x819dda26u,
                      0xf568631du,
                      0xaa3e418bu,
                      0x83ea9652u,
                      0xf2310c8bu,
                      0x8cbaaf0fu,
                      0x5aab76ffu,
                      0xf3dff63bu,
                      0x06f008d8u,
                      0xd9292141u};

int next32random = NEXTRAND;
int end32random = 125;

// The first "endrandom" numbers returned are truely random INT64.
// They are perfect for bases of new hash functions, and currently
// we have just enough such random numbers for tabulation based
// 5-universal hashing, or one of the other schemes provided.
// The remaining ones are based on the pseudo-random mrand48() which
// returns a signed 32-bit number. We do not use the unsigned lrand48() 
// which only returns 31-bit.
inline static INT64 RandINT64() {
  INT64 r,r1;
  if (next64random<end64random) r=Random64[next64random++];
  else {
    if (next64random==end64random) {
      srand48(Random64[0]);
#ifndef NDEBUG          
      fprintf(stderr,"Switching from random to pseudo-random in Rand64\n");
#endif
    }
    r =(INT64) mrand48();  
    r1 = (INT64) mrand48();
    r=r<<32;
    r=r^r1;
  }
  return r;
}
inline static INT32 RandINT32() {
  INT32 r;
  if (next32random<end32random) r=Random32[next32random++];
  else {
    if (next32random==end32random) {
      srand48(Random64[0]);     
#ifndef NDEBUG    
      fprintf(stderr,"Switching from random to pseudo-random in Rand32\n");
#endif
    }
    r =(INT32) mrand48();  
  }
  return r;
}

inline static INT64 gfInv(INT64 x, INT64 p) 
{
  INT64 p0, p1;
  p0 = x;
  p1 = (p0*x) % p;
  while (p1 != 1) {
    p0 = p1;
    p1 = (p0*x) % p;
  }
  return p0;
}

/* c is a coefficient vector with length k */
inline static INT32 kwise_32to32(INT32 x, INT64 *c, int k) {
  int n;
  INT64 prim, h;
  n = 32;
  prim = gf_primTable[n];
  h = gf_Poly((INT64) x, c, k, n, prim);
  return (INT32) h;
}

/* c0 and c1 are coefficient vectors with length k */
inline static INT64 kwise_32to64(INT32 x, INT64 *c0, INT64 *c1, int k) {
  int n;
  INT64 prim, h0, h1;
  
  n = 32;
  prim = gf_primTable[n];
  h0 = gf_Poly((INT64) x, c0, k, n, prim);
  h1 = gf_Poly((INT64) x, c1, k, n, prim);
  
  return ((h0<<32) | h1);
}

/* Computes ax+b mod Prime, possibly plus Prime,  
   exploiting the structure of Prime.  */
inline static INT64 MultAddPrime61_x32(INT32 x, INT64 a, INT64 b) { 
  INT64 a0,a1,c0,c1,c;
  a0 = LOW(a)*x;
  a1 = HIGH(a)*x;
  c0 = a0+(a1<<32);
  c1 = (a0>>61)+(a1>>29);
  c  = (c0&Prime61)+c1+b;
  return c;
} // 12 instructions


/* Computes ax+b mod Prime, possibly plus Prime
   exploiting the structure of Prime.
   Assuming x: 61-bit, b: 61-bit, a: < 2*Prime => at most 62-bit
*/
inline static INT64 MultAddPrime61_x61(INT64 x, INT64 a, INT64 b) { 
  INT64 x0,x1,a0,a1,ax00,ax01_10,ax11,c0,c1,c;

  x0 = LOW(x);
  x1 = HIGH(x);
  a0 = LOW(a);
  a1 = HIGH(a);

  ax00 = a0*x0;
  ax11 = a1*x1;  /* at most 62+61-64=59 bits */

  /* there is no overflow because
        a0*x1 has at most 32+29=61 bits
        a1*x0 has at most 30+32=62 bits */
  ax01_10 = a0*x1 + a1*x0;

  /*
   * c =
   *       ax11<<3  + 
   *       ax01>>29 + Prime&(ax01<<32) +
   *       ax10>>29 + Prime&(ax10<<32) +
   *       ax00>>61 + Prime&ax00 +
   *       b
   */

  c0 = ax00 + (ax01_10<<32); /* overflow does not matter here */
  c1 = (ax00>>61) + (ax01_10>>29) + (ax11<<3); /* at most 63 bits */
  c  = (c0&Prime61) + c1 + b; /* there is no overflow */
  c  = (c&Prime61) + (c>>61); /* c < 2*Prime */

  return c;
}

inline static INT64 Mod64Prime89(INT96 r)
{
  /* Computes (r mod Prime89) mod 2^64, exploiting the structure of Prime89 */
  INT64 r0, r1, r2;

  /* r2r1r0 = r&Prime + r>>89 */  
  r2 = r[2];
  r1 = r[1];
  r0 = r[0] + (r2>>25);
  r2 &= Prime89_2;
  
  return
    (r2 == Prime89_2 && r1 == Prime89_1 && r0 >= Prime89_0) ?
    (r0 - Prime89_0) :
    (r0 + (r1<<32));
}

/* Computes ax+b mod Prime, possibly plus Prime (x is 64 bits)
   exploiting the structure of Prime.  */
inline void MultAddPrime89(INT96 r, INT64 x, INT96 a, INT96 b) { 
  INT64 x1, x0, c21, c20, c11, c10, c01, c00;
  INT64 d0, d0b, d1, d2;
  INT64 s0, s1, carry;

  x1 = HIGH(x);
  x0 = LOW(x);
  
  c21 = a[2]*x1;
  c20 = a[2]*x0;
  c11 = a[1]*x1;
  c10 = a[1]*x0;
  c01 = a[0]*x1;
  c00 = a[0]*x0;

  /*
   * r =
   *       c21<<7 +
   *       c20>>25 + Prime&(c20<<64) +
   *       c11>>25 + Prime&(c11<<64) +
   *       c10>>57 + Prime&(c10<<32) +
   *       c01>>57 + Prime&(c01<<32) +
   *       c00 + b
   */
  
  d0 = (c20>>25)+(c11>>25)+(c10>>57)+(c01>>57);
  d0b = (c21<<7);
  d1 = (c10&Prime89_21) + (c01&Prime89_21);
  d2 = (c20&Prime89_2) + (c11&Prime89_2) + (c21>>57);


  /* r = b + c00 + d0 + d0b + d1<<32 + d2<<64 */

  s0 = b[0] + LOW(c00) + LOW(d0) + LOW(d0b);
  r[0] = LOW(s0);
  carry = HIGH(s0);

  s1 = b[1] + HIGH(c00) + HIGH(d0) + HIGH(d0b) + LOW(d1) + carry;
  r[1] = LOW(s1);
  carry = HIGH(s1);

  r[2] = b[2] + HIGH(d1) + d2 + carry;
    
}

/* CWtrick for 32-bit key x with prime 2^61-1 */
inline static INT32 Hash61bit4univ_32to32(INT32 x, Hash61bit5univID index) {
  INT64 h;
  int i;

  h = index[0];
  for (i=1;i<4;i++) h = MultAddPrime61_x32(x,h,index[i]);
  h = (h&Prime61)+(h>>61);
  if (h>=Prime61) h-=Prime61;
  return ((INT32) h);
} 

/* CWtrick for 32-bit key x with prime 2^61-1 */
inline static INT32 Hash61bit5univ_32to32(INT32 x, Hash61bit5univID index) {
  INT64 h;
  int i;

  h = index[0];
  for (i=1;i<5;i++) h = MultAddPrime61_x32(x,h,index[i]);
  h = (h&Prime61)+(h>>61);
  if (h>=Prime61) h-=Prime61;
  return ((INT32) h);
} 

/* CWtrick for 32-bit key x with prime 2^61-1 */
inline static INT64 Hash61bit4univ_32to61(INT32 x, Hash61bit5univID index) {
  INT64 h;
  int i;

  h = index[0];
  for (i=1;i<4;i++) h = MultAddPrime61_x32(x,h,index[i]);
  h = (h&Prime61)+(h>>61);
  if (h>=Prime61) h-=Prime61;
  return (h);
}

/* CWtrick for 32-bit key x with prime 2^61-1 */
inline static INT64 Hash61bit5univ_32to61(INT32 x, Hash61bit5univID index) {
  INT64 h;
  int i;

  h = index[0];
  for (i=1;i<5;i++) h = MultAddPrime61_x32(x,h,index[i]);
  h = (h&Prime61)+(h>>61);
  if (h>=Prime61) h-=Prime61;
  return (h);
} 

/* CWtrick for 48-bit key x with prime 2^61-1 */
inline static INT64 Hash61bit4univ_48to61(INT64 x, Hash61bit5univID index) {
  INT64 h;
  int i;

  h = index[0];
  for (i=1;i<4;i++) h = MultAddPrime61_x61(x,h,index[i]);
  h = (h&Prime61)+(h>>61);
  if (h>=Prime61) h-=Prime61;
  return (h);
} 

/* CWtrick for 48-bit key x with prime 2^61-1 */
inline static INT64 Hash61bit5univ_48to61(INT64 x, Hash61bit5univID index) {
  INT64 h;
  int i;

  h = index[0];
  for (i=1;i<5;i++) h = MultAddPrime61_x61(x,h,index[i]);
  h = (h&Prime61)+(h>>61);
  if (h>=Prime61) h-=Prime61;
  return (h);
} 

inline static INT64* InitHash61bit5univ() {
  int i;

  Hash61bit5univID hid = (INT64*) malloc(5*sizeof(INT64));
  for (i=0;i<5;i++) {
    hid[i]=RandINT64();
    hid[i] = (hid[i]&Prime61)+(hid[i]>>61);
    if (hid[i]>=Prime61) hid[i]-=Prime61;
  }
  return hid;
}

/* CWtrick for 64-bit key x with prime 2^89-1 */
inline static INT64 Hash89bit4univ_64to64(INT64 x, Hash89bit5univID index) {
  INT64 r[3], *i0;
  INT64 h;
  int i;

  i0   = index[0];
  r[0] = i0[0];
  r[1] = i0[1];
  r[2] = i0[2];
  for (i=1;i<4;i++) MultAddPrime89(r,x,r,index[i]);
  h = Mod64Prime89(r);
  return (h);
} 

/* CWtrick for 64-bit key x with prime 2^89-1 */
inline static INT64 Hash89bit5univ_64to64(INT64 x, Hash89bit5univID index) {
  INT64 r[3], *i0;
  INT64 h;
  int i;

  i0   = index[0];
  r[0] = i0[0];
  r[1] = i0[1];
  r[2] = i0[2];
  for (i=1;i<5;i++) MultAddPrime89(r,x,r,index[i]);
  h = Mod64Prime89(r);
  return (h);
} 

inline void RandomBelowPrime89(INT96 x) {
  /* Generates a random number < Prime89 exploiting the structure of Prime89 */
  INT32 i;
  
  for (;;) {
    for (i=0;i<3;i++)
      x[i] = RandINT64() & Prime89[i];
    for (i=0;i<3;i++)
      if (x[i] != Prime89[i]) return;
  }
}

inline static INT96* InitHash89bit5univ() {
  int i;

  Hash89bit5univID hid = (INT96*) malloc(5*sizeof(INT96));
  for (i=0;i<5;i++) {
    hid[i] = (INT96) malloc(3*sizeof(INT64));
    RandomBelowPrime89(hid[i]);
  }
  return hid;
}

inline static Hash32bitUnivID InitHash32bitUniv() {
  return (RandINT32() | 1u);  // it needs to be an odd number
}

inline INT32 Hash32bitUniv(INT32 x, Hash32bitUnivID hid) {
  //One may only use high order bits of this output..
  return hid*x;
}

inline static Hash48bitUnivID InitHash48bitUniv() {
  return ((RandINT64()>>16) | 1ull);  // it needs to be an odd number
}

inline INT64 Hash48bitUniv(INT64 x, Hash48bitUnivID hid) {
  //One may only use high order bits of this output..
  return LOW48(hid*x);
}

inline static Hash64bitUnivID InitHash64bitUniv() {
  return (RandINT64() | 1ull);  // it needs to be an odd number
}

inline INT64 Hash64bitUniv(INT64 x, Hash64bitUnivID hid) {
  //One may only use high order bits of this output..
  return hid*x;
}


inline static Hash32bit2univID InitHash32bit2univ() {
  Hash32bit2univID hid = (INT64*) malloc(2*sizeof(INT64));
  hid[0]=RandINT64();
  hid[1]=RandINT64();
  return hid;
}

inline INT32 Hash32bit2univ(INT32 x, Hash32bit2univID hid) {
  INT64 H;
  INT32 h; 
  H = (x*hid[0])+hid[1];
  h = H >> 32;
  return h;
} 

inline static Hash48bit2univID InitHash48bit2univ() {
  Hash48bit2univID hid = (INT64*) malloc(4*sizeof(INT64));
  hid[0]=RandINT32();
  hid[1]=RandINT64();
  hid[2]=RandINT32();
  hid[3]=RandINT64();
  return hid;
}

/*
 * Let A0 = hid[0]; A1 = hid[1]; B0 = hid[2]; B1 = hid[3];
 * Let x1 = HIGH(x); x0 = LOW(x);
 *
 * Let x0A0 = x0*A0. Let carry = HIGH(LOW(x0A0)+B0).
 *
 * Then the higher 64-bit of A*x + B can be computed as:
 * x*A1 + x1*A0 + HIGH(x0A0) + B1 + carry
 *
 */
inline INT64 Hash48bit2univ(INT64 x, Hash48bit2univID hid) {
  INT64 H, x0A0, carry;
  x0A0 = LOW(x)*hid[0];
  carry = HIGH(LOW(x0A0)+hid[2]);
  H = x*hid[1] + HIGH(x)*hid[0] + HIGH(x0A0) + hid[3] + carry;
  return (H>>16);
} 

inline static Hash64bit2univID InitHash64bit2univ() {
  Hash64bit2univID hid = (INT64*) malloc(5*sizeof(INT64));
  hid[0]=RandINT64();
  hid[1]=RandINT64();
  hid[2]=RandINT64();
  hid[3]=RandINT64();
  hid[4]=RandINT64();
  return hid;
}

/*
 * Let
 *    A00 = hid[0]; A01 = hid[1]; A10 = hid[2]; A11 = hid[3]; B = hid[4]
 *    x0 = LOW(x); x1 = HIGH(x);
 *
 * We compute H = (((x0+A00)*(x1+A01))>>32) + (x0+A10)*(x1+A11) + B
 */
inline INT64 Hash64bit2univ(INT64 x, Hash64bit2univID hid) {
  INT64 H, x0, x1;
  x0 = LOW(x);
  x1 = HIGH(x);
  H = (x0+hid[0])*(x1+hid[1]);
  H >>= 32;
  H += (x0+hid[2])*(x1+hid[3]);
  H += hid[4];
  return H;
}


inline static HashShortTab5univID_32to32 InitHashShortTab5univ_32to32() {
  unsigned int i,j,jmax;
  HashShortTab5univID_32to32 htab;
  INT64 coef[3][5];
  
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 5; j++) {
      coef[i][j] = (INT64) RandINT32();
    }
  }

  htab = (HashShortTab5univID_32to32) malloc(3*sizeof(INT32*));

  for (i=0; i<3; i++) {
#ifdef COMPRESS
    jmax = (i < 2) ? 65536 : 65538;
#else
    jmax = (i < 2) ? 65536 : 65536*2;
#endif    
    htab[i] = (INT32*) malloc(jmax*sizeof(INT32));
    for (j = 0; j < jmax; j++) {
      htab[i][j] = kwise_32to32(j, coef[i], 5);
    }
  }

  return htab;
}


/* tabulation based hashing for 32-bit key x using 16-bit characters.*/
inline INT32 HashShortTab5univ_32to32(INT32 x, HashShortTab5univID_32to32 htab) {
  INT32 x0, x1, x2;
  x0 = x&65535;
  x1 = x>>16;
  x2 = x0 + x1;
#ifdef COMPRESS  
  x2 = 2 - (x2>>16) + (x2&65535);  // optional compression
#endif  
  return htab[0][x0]^htab[1][x1]^htab[2][x2];
} // 8 + 4 = 12 instructions


inline static HashShortTab5univID_32to64 InitHashShortTab5univ_32to64() {
  unsigned int i,j,jmax;
  HashShortTab5univID_32to64 htab;
  INT64 coef[3*2][5], rv;
  
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 5; j++) {
      rv = RandINT64();
      coef[2*i][j]   = HIGH(rv);
      coef[2*i+1][j] = LOW(rv);
    }
  }

  htab = (HashShortTab5univID_32to64) malloc(3*sizeof(INT64*));

  for (i=0; i<3; i++) {
#ifdef COMPRESS
    jmax = (i < 2) ? 65536 : 65538;
#else
    jmax = (i < 2) ? 65536 : 65536*2;
#endif    
    htab[i] = (INT64*) malloc(jmax*sizeof(INT64));
    for (j = 0; j < jmax; j++) {
      htab[i][j] = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
    }
  }

  return htab;
}


/* tabulation based hashing for 32-bit key x using 16-bit characters.*/
inline INT64 HashShortTab5univ_32to64(INT32 x, HashShortTab5univID_32to64 htab) {
  INT32 x0, x1, x2;
  x0 = x&65535;
  x1 = x>>16;
  x2 = x0 + x1;
#ifdef COMPRESS  
  x2 = 2 - (x2>>16) + (x2&65535);  // optional compression
#endif  
  return htab[0][x0]^htab[1][x1]^htab[2][x2];
} // 8 + 4 = 12 instructions


inline static HashShortTab5univID_48to64 InitHashShortTab5univ_48to64() {
  unsigned int i,j,jmax;
  HashShortTab5univID_48to64 htab;
  INT64 coef[5*2][5], rv, prime, invtbl[5];
  
  for (i = 0; i < 5; i++) {
    for (j = 0; j < 5; j++) {
      rv = RandINT64();
      coef[2*i][j]   = HIGH(rv);
      coef[2*i+1][j] = LOW(rv);
    }
  }

  htab = (HashShortTab5univID_48to64) malloc(5*sizeof(INT64*));

  prime = Prime16;
  for (i=0; i<5; i++)
    invtbl[i] = gfInv(i+1, prime);
  
  for (i=0; i<3; i++) {
    jmax = 65536;
    htab[i] = (INT64*) malloc(jmax*2*sizeof(INT64));
    for (j = 0; j < jmax; j++) {
      htab[i][2*j] = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
      htab[i][2*j+1] =
        (((j*invtbl[i+1])%prime)) +
        (((j*invtbl[i+2])%prime)<<18);
    }
  }

  for (i=3; i<5; i++) {
#ifdef COMPRESS
    jmax = 65536+3*(prime-65536);
#else
    jmax = 3*(prime-1)+1;
#endif
    htab[i] = (INT64*) malloc(jmax*sizeof(INT64));
    for (j = 0; j < jmax; j++) {
      htab[i][j] = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
    }
  }

  return htab;
}

/* tabulation based hashing for 48-bit key x using 16-bit characters.*/
inline INT64 HashShortTab5univ_48to64(INT64 x, HashShortTab5univID_48to64 htab) {
  INT64 *a0, *a1, *a2, c;

#ifdef COMPRESS  
  const INT64 m1 = (65535ull<<18) + (65535ull);
  const INT64 m2 =     (3ull<<34) +     (3ull<<16);
  const INT64 m3 =     (3ull<<18) +     (3ull);
#endif
  
  a0 = htab[0] + ((x&65535)<<1);
  a1 = htab[1] + (((x>>16)&65535)<<1);
  a2 = htab[2] + ((x>>32)<<1);

  c = a0[1] + a1[1] + a2[1];

#ifdef COMPRESS  
  c = m3 - ((c&m2)>>16) + (c&m1);
#endif

  return a0[0] ^ a1[0] ^ a2[0] ^
    htab[3][c&262143ull] ^
    htab[4][c>>18];
}

inline static HashShortTab5univID_64to64 InitHashShortTab5univ_64to64() {
  unsigned int i,j,jmax;
  HashShortTab5univID_64to64 htab;
  INT64 coef[7*2][5], rv, prime, invtbl[7];
  
  for (i = 0; i < 7; i++) {
    for (j = 0; j < 5; j++) {
      rv = RandINT64();
      coef[2*i][j]   = HIGH(rv);
      coef[2*i+1][j] = LOW(rv);
    }
  }

  htab = (HashShortTab5univID_64to64) malloc(7*sizeof(INT64*));

  prime = Prime16;
  for (i=0; i<7; i++)
    invtbl[i] = gfInv(i+1, prime);
  
  for (i=0; i<4; i++) {
    jmax = 65536;
    htab[i] = (INT64*) malloc(jmax*2*sizeof(INT64));
    for (j = 0; j < jmax; j++) {
      htab[i][2*j] = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
      htab[i][2*j+1] =
        (((j*invtbl[i+1])%prime)) +
        (((j*invtbl[i+2])%prime)<<21) + 
        (((j*invtbl[i+3])%prime)<<42);
    }
  }

  for (i=4; i<7; i++) {
#ifdef COMPRESS
    jmax = 65536+4*(prime-65536);
#else
    jmax = 4*(prime-1)+1;
#endif
    htab[i] = (INT64*) malloc(jmax*sizeof(INT64));
    for (j = 0; j < jmax; j++) {
      htab[i][j] = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
    }
  }

  return htab;
}

/* tabulation based hashing for 64-bit key x using 16-bit characters.*/
inline INT64 HashShortTab5univ_64to64(INT64 x, HashShortTab5univID_64to64 htab) {
  INT64 *a0, *a1, *a2, *a3, c;
  int64views v;

#ifdef COMPRESS  
  const INT64 m1 = (65535ull<<42) + (65535ull<<21) + (65535ull);
  const INT64 m2 = (31ull<<58)    + (31ull<<37)    + (31ull<<16);
  const INT64 m3 = (4ull<<42)     + (4ull<<21)     + (4ull);
#endif

  v.as_int64 = x;
#ifdef BIGENDIAN
  a0 = htab[0] + (v.as_int16s[3]<<1);
  a1 = htab[1] + (v.as_int16s[2]<<1);
  a2 = htab[2] + (v.as_int16s[1]<<1);
  a3 = htab[3] + (v.as_int16s[0]<<1);
#else
  a0 = htab[0] + (v.as_int16s[0]<<1);
  a1 = htab[1] + (v.as_int16s[1]<<1);
  a2 = htab[2] + (v.as_int16s[2]<<1);
  a3 = htab[3] + (v.as_int16s[3]<<1);
#endif
  
  c = a0[1] + a1[1] + a2[1] + a3[1];

#ifdef COMPRESS  
  c = m3 - ((c&m2)>>16) + (c&m1);
#endif

  return a0[0] ^ a1[0] ^ a2[0] ^ a3[0] ^
    htab[4][c&2097151] ^
    htab[5][(c>>21)&2097151] ^
    htab[6][c>>42];
}

inline HashCharTab2univID_32to32 InitHashCharTab2univ_32to32() {
  unsigned int i,j;
  HashCharTab2univID_32to32 htab;
  INT64 coef[4][5];

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 5; j++) {
      coef[i][j] = (INT64) RandINT32();
    }
  }

  htab = (HashCharTab2univID_32to32) malloc(4*sizeof(INT32*));
  for (i = 0; i < 4; i++) {
    htab[i] = (INT32*) malloc(256*sizeof(INT32));
    for (j = 0; j < 256; j++) {
      htab[i][j] = kwise_32to32(j, coef[i], 5);
    }
  }
  
  return htab;
}

/* SHOULD BE CHAR tabulation based hashing for 32-bit key x using 16-bit characters.*/
inline INT32 HashCharTab2univ_32to32(INT32 x, HashCharTab2univID_32to32 htab) {
  int32views v;
  v.as_int32 = x;
#ifdef BIGENDIAN
  return htab[0][v.as_int8s[3]] ^
    htab[1][v.as_int8s[2]] ^
    htab[2][v.as_int8s[1]] ^
    htab[3][v.as_int8s[0]];
#else
  return htab[0][v.as_int8s[0]] ^
    htab[1][v.as_int8s[1]] ^
    htab[2][v.as_int8s[2]] ^
    htab[3][v.as_int8s[3]];
#endif
}

inline HashCharTab5univID_32to32 InitHashCharTab5univ_32to32() {
  unsigned int i,j,jmax;
  HashCharTab5univID_32to32 htab;
  INT64 coef[7][5], prime, inv_tbl[6], add_tbl[6];

  for (i = 0; i < 7; i++) {
    for (j = 0; j < 5; j++) {
      coef[i][j] = (INT64) RandINT32();
    }
  }

  prime = Prime8;

  // ensure that
  //    (inv_tbl[i]*x + add_tbl[i]) % prime == prime-1
  // holds if and only if x = prime-1.  That is, for any x < prime-1,
  //    (inv_tbl[i]*x + add_tbl[i]) % prime != prime-1
  for (i = 0; i < 6; i++) {
    inv_tbl[i] = gfInv(i+1,prime);
    add_tbl[i] = (prime-1)-((inv_tbl[i]*(prime-1))%prime);
  }
    
  htab = (HashCharTab5univID_32to32) malloc(7*sizeof(INT32*));
  for (i = 0; i < 4; i++) {
    htab[i] = (INT32*) malloc(2*256*sizeof(INT32));
    for (j = 0; j < 256; j++) {
      htab[i][j*2]    = kwise_32to32(j, coef[i], 5);
      htab[i][j*2+1]  =
        ((j*inv_tbl[i+0]+add_tbl[i+0])%prime) +
        (((j*inv_tbl[i+1]+add_tbl[i+0])%prime) << 10) +
        (((j*inv_tbl[i+2]+add_tbl[i+0])%prime) << 20);
    }
  }
  
#ifdef COMPRESS
  // NOTE: we ensure that the raw sum of four numbers is at most 255*4.
  // So our compresssion is more efficient that described in the paper.
  // In particular, we only need 3*(prime-256) instead of 4*(prime-256).
  jmax = 256 + 3*(prime-256);
#else
  jmax = (prime-2)*4 + 1;
#endif
  for (i=4;i<7;i++) {
    htab[i] = (INT32*) malloc(jmax*sizeof(INT32));
    for (j = 0; j < jmax; j++) {
      htab[i][j] = kwise_32to32(j, coef[i], 5);
    }
  }
  
  return htab;
}

/* SHOULD BE CHAR tabulation based hashing for 32-bit key x using 16-bit characters.*/
inline INT32 HashCharTab5univ_32to32(INT32 x, HashCharTab5univID_32to32 htab) {
  int32views v;
  INT32 *a0, *a1, *a2, *a3, c;

#ifdef COMPRESS
  // NOTE: we ensure that the raw sum of four numbers is at most 255*4.
  // So our compresssion is more efficient that described in the paper.
  // In particular, m3 only requires 3U instead of 4U.
  const INT32 m1 = (255U<<20) + (255U<<10) + (255U);
  const INT32 m2 =   (3U<<28) +   (3U<<18) +   (3U<<8);
  const INT32 m3 =   (3U<<20) +   (3U<<10) +   (3U);
#endif  
  
  v.as_int32 = x;
#ifdef BIGENDIAN
  a0 = htab[0] + (v.as_int8s[3]<<1);
  a1 = htab[1] + (v.as_int8s[2]<<1);
  a2 = htab[2] + (v.as_int8s[1]<<1);
  a3 = htab[3] + (v.as_int8s[0]<<1);
#else
  a0 = htab[0] + (v.as_int8s[0]<<1);
  a1 = htab[1] + (v.as_int8s[1]<<1);
  a2 = htab[2] + (v.as_int8s[2]<<1);
  a3 = htab[3] + (v.as_int8s[3]<<1);
#endif

  // there is no overflow, as we just need 10 bits for each number
  c  = a0[1] + a1[1] + a2[1] + a3[1];

  //
  // optional compression:
  //
#ifdef COMPRESS
  c  = m3 - ((c&m2)>>8) + (c&m1);
#endif
  
  return a0[0] ^ a1[0] ^ a2[0] ^ a3[0] ^
    htab[4][c&1023] ^
    htab[5][(c>>10)&1023] ^
    htab[6][c>>20];
}

inline HashCharTab5univID_32to64 InitHashCharTab5univ_32to64() {
  unsigned int i,j,jmax;
  HashCharTab5univID_32to64 htab;
  INT64 coef[7*2][5], rv, prime, inv_tbl[6], add_tbl[6];

  for (i = 0; i < 7; i++) {
    for (j = 0; j < 5; j++) {
      rv = RandINT64();
      coef[2*i][j]   = HIGH(rv);
      coef[2*i+1][j] = LOW(rv);
    }
  }

  prime = Prime8;

  // ensure that
  //    (inv_tbl[i]*x + add_tbl[i]) % prime == prime-1
  // holds if and only if x = prime-1.  That is, for any x < prime-1,
  //    (inv_tbl[i]*x + add_tbl[i]) % prime != prime-1
  for (i = 0; i < 6; i++) {
    inv_tbl[i] = gfInv(i+1,prime);
    add_tbl[i] = (prime-1)-((inv_tbl[i]*(prime-1))%prime);
  }
    
  htab = (HashCharTab5univID_32to64) malloc(7*sizeof(INT64*));
  for (i = 0; i < 4; i++) {
    htab[i] = (INT64*) malloc(2*256*sizeof(INT64));
    for (j = 0; j < 256; j++) {
      htab[i][j*2]    = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
      htab[i][j*2+1]  =
        ((j*inv_tbl[i+0]+add_tbl[i+0])%prime) +
        (((j*inv_tbl[i+1]+add_tbl[i+0])%prime) << 10) +
        (((j*inv_tbl[i+2]+add_tbl[i+0])%prime) << 20);
    }
  }
  
#ifdef COMPRESS
  // NOTE: we ensure that the raw sum of four numbers is at most 255*4.
  // So our compresssion is more efficient that described in the paper.
  // In particular, we only need 3*(prime-256) instead of 4*(prime-256).
  jmax = 256 + 3*(prime-256);
#else
  jmax = (prime-2)*4 + 1;
#endif
  for (i=4;i<7;i++) {
    htab[i] = (INT64*) malloc(jmax*sizeof(INT64));
    for (j = 0; j < jmax; j++) {
      htab[i][j] = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
    }
  }
  
  return htab;
}

/* SHOULD BE CHAR tabulation based hashing for 32-bit key x using 16-bit characters.*/
inline INT64 HashCharTab5univ_32to64(INT32 x, HashCharTab5univID_32to64 htab) {
  int32views v;
  INT64 *a0, *a1, *a2, *a3;
  INT32 c;

#ifdef COMPRESS
  // NOTE: we ensure that the raw sum of four numbers is at most 255*4.
  // So our compresssion is more efficient that described in the paper.
  // In particular, m3 only requires 3U instead of 4U.
  const INT32 m1 = (255U<<20) + (255U<<10) + (255U);
  const INT32 m2 =   (3U<<28) +   (3U<<18) +   (3U<<8);
  const INT32 m3 =   (3U<<20) +   (3U<<10) +   (3U);
#endif  
  
  v.as_int32 = x;
#ifdef BIGENDIAN
  a0 = htab[0] + (v.as_int8s[3]<<1);
  a1 = htab[1] + (v.as_int8s[2]<<1);
  a2 = htab[2] + (v.as_int8s[1]<<1);
  a3 = htab[3] + (v.as_int8s[0]<<1);
#else
  a0 = htab[0] + (v.as_int8s[0]<<1);
  a1 = htab[1] + (v.as_int8s[1]<<1);
  a2 = htab[2] + (v.as_int8s[2]<<1);
  a3 = htab[3] + (v.as_int8s[3]<<1);
#endif
  
  // there is no overflow, as we just need 10 bits for each number
  c  = a0[1] + a1[1] + a2[1] + a3[1];

  //
  // optional compression:
  //
#ifdef COMPRESS
  c  = m3 - ((c&m2)>>8) + (c&m1);
#endif
  
  return a0[0] ^ a1[0] ^ a2[0] ^ a3[0] ^
    htab[4][c&1023] ^
    htab[5][(c>>10)&1023] ^
    htab[6][c>>20];
}


inline HashCharTab5univID_40to64 InitHashCharTab5univ_40to64() {
  unsigned int i,j,jmax;
  HashCharTab5univID_40to64 htab;
  INT64 coef[9*2][5], rv, prime, inv_tbl[8], add_tbl[8];

  for (i = 0; i < 9; i++) {
    for (j = 0; j < 5; j++) {
      rv = RandINT64();
      coef[2*i][j]   = HIGH(rv);
      coef[2*i+1][j] = LOW(rv);
    }
  }

  prime = Prime8;

  // ensure that
  //    (inv_tbl[i]*x + add_tbl[i]) % prime == prime-1
  // holds if and only if x = prime-1.  That is, for any x < prime-1,
  //    (inv_tbl[i]*x + add_tbl[i]) % prime != prime-1
  for (i = 0; i < 8; i++) {
    inv_tbl[i] = gfInv(i+1,prime);
    add_tbl[i] = (prime-1)-((inv_tbl[i]*(prime-1))%prime);
  }

  htab = (HashCharTab5univID_40to64) malloc(9*sizeof(INT64*));
  for (i = 0; i < 5; i++) {
    htab[i] = (INT64*) malloc(2*256*sizeof(INT64));
    for (j = 0; j < 256; j++) {
      htab[i][j*2]    = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
      htab[i][j*2+1]  =
        ((j*inv_tbl[i+0]+add_tbl[i+0])%prime) +
        (((j*inv_tbl[i+1]+add_tbl[i+1])%prime) << 11) +
        (((j*inv_tbl[i+2]+add_tbl[i+2])%prime) << 22) + 
        (((j*inv_tbl[i+3]+add_tbl[i+3])%prime) << 33);
    }
  }
  
#ifdef COMPRESS
  // NOTE: we ensure that the raw sum of five numbers is at most 255*5.
  // So our compresssion is more efficient that described in the paper.
  // In particular, we only need 4*(prime-256) instead of 5*(prime-256).
  jmax = 256 + 4*(prime-256);
#else
  jmax = (prime-2)*5 + 1;
#endif
  for (i=5;i<9;i++) {
    htab[i] = (INT64*) malloc(jmax*sizeof(INT64));
    for (j = 0; j < jmax; j++) {
      htab[i][j] = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
    }
  }
  
  return htab;
}

/* SHOULD BE CHAR tabulation based hashing for 32-bit key x using 16-bit characters.*/
inline INT64 HashCharTab5univ_40to64(INT64 x, HashCharTab5univID_40to64 htab) {
  int64views v;
  INT64 *a0, *a1, *a2, *a3, *a4, c;

#ifdef COMPRESS
  const INT64 m1 = (255ull<<33) + (255ull<<22) + (255ull<<11) + (255ull);
  const INT64 m2 =   (7ull<<41) +   (7ull<<30) +   (7ull<<19) +   (7ull<<8);
  const INT64 m3 =   (4ull<<33) +   (4ull<<22) +   (4ull<<11) +   (4ull);
#endif  
  
  v.as_int64 = x;
#ifdef BIGENDIAN
  a0 = htab[0] + (v.as_int8s[7]<<1);
  a1 = htab[1] + (v.as_int8s[6]<<1);
  a2 = htab[2] + (v.as_int8s[5]<<1);
  a3 = htab[3] + (v.as_int8s[4]<<1);
  a4 = htab[4] + (v.as_int8s[3]<<1);
#else
  a0 = htab[0] + (v.as_int8s[0]<<1);
  a1 = htab[1] + (v.as_int8s[1]<<1);
  a2 = htab[2] + (v.as_int8s[2]<<1);
  a3 = htab[3] + (v.as_int8s[3]<<1);
  a4 = htab[4] + (v.as_int8s[4]<<1);
#endif
  
  c  = a0[1] + a1[1] + a2[1] + a3[1] + a4[1];

  //
  // optional compression:
  //
#ifdef COMPRESS
  c  = m3 - ((c&m2)>>8) + (c&m1);
#endif

  return a0[0] ^ a1[0] ^ a2[0] ^ a3[0] ^ a4[0] ^
    htab[5][c&2047] ^
    htab[6][((c>>11)&2047)] ^
    htab[7][((c>>22)&2047)] ^
    htab[8][(c>>33)];
}


inline HashCharTab5univID_48to64 InitHashCharTab5univ_48to64() {
  unsigned int i,j,jmax;
  HashCharTab5univID_48to64 htab;
  INT64 coef[11*2][5], prime, inv_tbl[10], add_tbl[10];

  for (i = 0; i < 11*2; i++) {
    for (j = 0; j < 5; j++) {
      coef[i][j] = RandINT64();
    }
  }

  prime = Prime8;

  // ensure that
  //    (inv_tbl[i]*x + add_tbl[i]) % prime == prime-1
  // holds if and only if x = prime-1.  That is, for any x < prime-1,
  //    (inv_tbl[i]*x + add_tbl[i]) % prime != prime-1
  for (i = 0; i < 10; i++) {
    inv_tbl[i] = gfInv(i+1,prime);
    add_tbl[i] = (prime-1)-((inv_tbl[i]*(prime-1))%prime);
  }

  htab = (HashCharTab5univID_48to64) malloc(11*sizeof(INT64*));
  for (i = 0; i < 6; i++) {
    htab[i] = (INT64*) malloc(2*256*sizeof(INT64));
    for (j = 0; j < 256; j++) {
      htab[i][j*2]    = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
      htab[i][j*2+1]  =
        ((j*inv_tbl[i+0]+add_tbl[i+0])%prime) +
        (((j*inv_tbl[i+1]+add_tbl[i+1])%prime) << 11) +
        (((j*inv_tbl[i+2]+add_tbl[i+2])%prime) << 22) + 
        (((j*inv_tbl[i+3]+add_tbl[i+3])%prime) << 33) +
        (((j*inv_tbl[i+4]+add_tbl[i+4])%prime) << 44);
    }
  }
  
#ifdef COMPRESS
  // NOTE: we ensure that the raw sum of six numbers is at most 255*6.
  // So our compresssion is more efficient that described in the paper.
  // In particular, we only need 5*(prime-256) instead of 6*(prime-256).
  jmax = 256 + 5*(prime-256);
#else
  jmax = (prime-2)*6 + 1;
#endif
  for (i=6;i<11;i++) {
    htab[i] = (INT64*) malloc(jmax*sizeof(INT64));
    for (j = 0; j < jmax; j++) {
      htab[i][j] = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
    }
  }
  
  return htab;
}

/* SHOULD BE CHAR tabulation based hashing for 32-bit key x using 16-bit characters.*/
inline INT64 HashCharTab5univ_48to64(INT64 x, HashCharTab5univID_48to64 htab) {
  int64views v;
  INT64 *a0, *a1, *a2, *a3, *a4, *a5, c;
  
#ifdef COMPRESS
  const INT64 m1 = (255ull<<44) + (255ull<<33) + (255ull<<22) + (255ull<<11) + (255ull);
  const INT64 m2 =   (7ull<<52) +   (7ull<<41) +   (7ull<<30) +   (7ull<<19) +   (7ull<<8);
  const INT64 m3 =   (5ull<<44) +   (5ull<<33) +   (5ull<<22) +   (5ull<<11) +   (5ull);
#endif  

  v.as_int64 = x;
#ifdef BIGENDIAN
  a0 = htab[0] + (v.as_int8s[7]<<1);
  a1 = htab[1] + (v.as_int8s[6]<<1);
  a2 = htab[2] + (v.as_int8s[5]<<1);
  a3 = htab[3] + (v.as_int8s[4]<<1);
  a4 = htab[4] + (v.as_int8s[3]<<1);
  a5 = htab[5] + (v.as_int8s[2]<<1);
#else
  a0 = htab[0] + (v.as_int8s[0]<<1);
  a1 = htab[1] + (v.as_int8s[1]<<1);
  a2 = htab[2] + (v.as_int8s[2]<<1);
  a3 = htab[3] + (v.as_int8s[3]<<1);
  a4 = htab[4] + (v.as_int8s[4]<<1);
  a5 = htab[5] + (v.as_int8s[5]<<1);
#endif
  
  c  = a0[1] + a1[1] + a2[1] + a3[1] + a4[1] + a5[1];

  //
  // optional compression:
  //
#ifdef COMPRESS
  c  = m3 - ((c&m2)>>8) + (c&m1);
#endif

  return a0[0] ^ a1[0] ^ a2[0] ^ a3[0] ^ a4[0] ^ a5[0] ^
    htab[6][(c&2047)] ^
    htab[7][((c>>11)&2047)] ^
    htab[8][((c>>22)&2047)] ^
    htab[9][((c>>33)&2047)] ^
    htab[10][(c>>44)];
}


inline HashCharTab5univID_56to64 InitHashCharTab5univ_56to64() {
  unsigned int i,j,jmax;
  HashCharTab5univID_56to64 htab;
  INT64 coef[13*2][5], rv, prime, inv_tbl[12], add_tbl[12];

  for (i = 0; i < 13; i++) {
    for (j = 0; j < 5; j++) {
      rv = RandINT64();
      coef[2*i][j]   = HIGH(rv);
      coef[2*i+1][j] = LOW(rv);
    }
  }

  prime = Prime8;

  // ensure that
  //    (inv_tbl[i]*x + add_tbl[i]) % prime == prime-1
  // holds if and only if x = prime-1.  That is, for any x < prime-1,
  //    (inv_tbl[i]*x + add_tbl[i]) % prime != prime-1
  for (i = 0; i < 12; i++) {
    inv_tbl[i] = gfInv(i+1,prime);
    add_tbl[i] = (prime-1)-((inv_tbl[i]*(prime-1))%prime);
  }

  htab = (HashCharTab5univID_56to64) malloc(13*sizeof(INT64*));
  for (i = 0; i < 7; i++) {
    htab[i] = (INT64*) malloc(2*256*sizeof(INT64));
    for (j = 0; j < 256; j++) {
      htab[i][j*2]    = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
      htab[i][j*2+1]  =
        ((j*inv_tbl[i+0]+add_tbl[i+0])%prime) +
        (((j*inv_tbl[i+1]+add_tbl[i+1])%prime) << 10) +
        (((j*inv_tbl[i+2]+add_tbl[i+2])%prime) << 20) + 
        (((j*inv_tbl[i+3]+add_tbl[i+3])%prime) << 32) +
        (((j*inv_tbl[i+4]+add_tbl[i+4])%prime) << 42) +
        (((j*inv_tbl[i+5]+add_tbl[i+5])%prime) << 52);
    }
  }
  
#ifdef COMPRESS
  // NOTE: we ensure that the raw sum of four numbers is at most 255*3.
  // So our compresssion is more efficient that described in the paper.
  // In particular, we only need 3*(prime-256) instead of 4*(prime-256).
  jmax = 256 + 3*(prime-256);
#else
  // after compressing first 4 numbers, the sum of remaining 4 numbers is
  // at most 255*4 + 3 = 1023. so jmax=1024 is enough
  jmax = 1024;
#endif
  for (i=7;i<13;i++) {
    htab[i] = (INT64*) malloc(jmax*sizeof(INT64));
    for (j = 0; j < jmax; j++) {
      htab[i][j] = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
    }
  }
  
  return htab;
}

/* SHOULD BE CHAR tabulation based hashing for 32-bit key x using 16-bit characters.*/
inline INT64 HashCharTab5univ_56to64(INT64 x, HashCharTab5univID_56to64 htab) {
  int64views v;
  INT64 *a0, *a1, *a2, *a3, *a4, *a5, *a6, b, c;
  INT32 c0, c1;
  
  const INT64 m1 = (255ull<<52) + (255ull<<42) + (255ull<<32) + (255ull<<20) + (255ull<<10) + (255ull);
  const INT64 m2 =   (3ull<<60) +   (3ull<<50) +   (3ull<<40) +   (3ull<<28) +   (3ull<<18) +   (3ull<<8);
  const INT64 m3 =   (3ull<<52) +   (3ull<<42) +   (3ull<<32) +   (3ull<<20) +   (3ull<<10) +   (3ull);

  v.as_int64 = x;
#ifdef BIGENDIAN
  a0 = htab[0] + (v.as_int8s[7]<<1);
  a1 = htab[1] + (v.as_int8s[6]<<1);
  a2 = htab[2] + (v.as_int8s[5]<<1);
  a3 = htab[3] + (v.as_int8s[4]<<1);
  a4 = htab[4] + (v.as_int8s[3]<<1);
  a5 = htab[5] + (v.as_int8s[2]<<1);
  a6 = htab[6] + (v.as_int8s[1]<<1);
#else
  a0 = htab[0] + (v.as_int8s[0]<<1);
  a1 = htab[1] + (v.as_int8s[1]<<1);
  a2 = htab[2] + (v.as_int8s[2]<<1);
  a3 = htab[3] + (v.as_int8s[3]<<1);
  a4 = htab[4] + (v.as_int8s[4]<<1);
  a5 = htab[5] + (v.as_int8s[5]<<1);
  a6 = htab[6] + (v.as_int8s[6]<<1);
#endif
  
  b  = a0[1] + a1[1] + a2[1] + a3[1];

  // the sum is at most 3 - 0 + 255*4 = 1023. so there is no overflow!!
  c  = m3 - ((b&m2)>>8) + (b&m1) + a4[1] + a5[1] + a6[1];

  //
  // optional compression:
  //
#ifdef COMPRESS
  c  = m3 - ((c&m2)>>8) + (c&m1);
#endif

  c0 = c;
  c1 = (c>>32);
  
  return a0[0] ^ a1[0] ^ a2[0] ^ a3[0] ^ a4[0] ^ a5[0] ^ a6[0] ^
    htab[7][(c0&1023)] ^
    htab[8][((c0>>10)&1023)] ^
    htab[9][(c0>>20)] ^
    htab[10][(c1&1023)] ^
    htab[11][((c1>>10)&1023)] ^
    htab[12][(c1>>20)];
}


inline HashCharTab5univID_64to64 InitHashCharTab5univ_64to64() {
  unsigned int i,j,jmax;
  HashCharTab5univID_64to64 htab;
  charTab_64to64_s *p;
  INT64 coef[15*2][5], rv, prime, inv_tbl[14], add_tbl[14];

  for (i = 0; i < 15; i++) {
    for (j = 0; j < 5; j++) {
      rv = RandINT64();
      coef[2*i][j]   = HIGH(rv);
      coef[2*i+1][j] = LOW(rv);
    }
  }

  prime = Prime8;

  // ensure that
  //    (inv_tbl[i]*x + add_tbl[i]) % prime == prime-1
  // holds if and only if x = prime-1.  That is, for any x < prime-1,
  //    (inv_tbl[i]*x + add_tbl[i]) % prime != prime-1
  for (i = 0; i < 14; i++) {
    inv_tbl[i] = gfInv(i+1,prime);
    add_tbl[i] = (prime-1)-((inv_tbl[i]*(prime-1))%prime);
  }

  htab = (HashCharTab5univID_64to64) malloc(sizeof(HashCharTab5univID_64to64_s));
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 256; j++) {
      p = &htab->t0[i][j];
      p->h = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
      p->v0 = 
        ((j*inv_tbl[i+0]+add_tbl[i+0])%prime) +
        (((j*inv_tbl[i+1]+add_tbl[i+1])%prime) << 11) +
        (((j*inv_tbl[i+2]+add_tbl[i+2])%prime) << 22) + 
        (((j*inv_tbl[i+3]+add_tbl[i+3])%prime) << 33) + 
        (((j*inv_tbl[i+4]+add_tbl[i+4])%prime) << 44);
      p->v1 =
        ((j*inv_tbl[i+5]+add_tbl[i+5])%prime) + 
        (((j*inv_tbl[i+6]+add_tbl[i+6])%prime) << 11);
    }
  }
  
#ifdef COMPRESS
  // NOTE: we ensure that the raw sum of eight numbers is at most 255*8.
  // So our compresssion is more efficient that described in the paper.
  // In particular, we only need 7*(prime-256) instead of 8*(prime-256).
  jmax = 256 + 7*(prime-256);
#else
  jmax = (prime-2)*8 + 1;
#endif
  for (i=8;i<15;i++) {
    for (j = 0; j < jmax; j++) {
      htab->t1[i-8][j] = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
    }
  }
  
  return htab;
}

/* SHOULD BE CHAR tabulation based hashing for 32-bit key x using 16-bit characters.*/
inline INT64 HashCharTab5univ_64to64(INT64 x, HashCharTab5univID_64to64 htab) {
  int64views v;
  charTab_64to64_s *a0, *a1, *a2, *a3, *a4, *a5, *a6, *a7;
  INT64 c0;
  INT32 c1;
  
#ifdef COMPRESS
  const INT64 m1 = (255ull<<44) + (255ull<<33) + (255ull<<22) + (255ull<<11) + (255ull);
  const INT64 m2 =   (7ull<<52) +   (7ull<<41) +   (7ull<<30) +   (7ull<<19) +   (7ull<<8);
  const INT64 m3 =   (7ull<<44) +   (7ull<<33) +   (7ull<<22) +   (7ull<<11) +   (7ull);
  const INT32 n1 = (255u<<11) + (255u);
  const INT32 n2 =   (7u<<19) +   (7u<<8);
  const INT32 n3 =   (7u<<11) +   (7u);

#endif
  
  v.as_int64 = x;
#ifdef BIGENDIAN
  a0 = &htab->t0[0][v.as_int8s[7]];
  a1 = &htab->t0[1][v.as_int8s[6]];
  a2 = &htab->t0[2][v.as_int8s[5]];
  a3 = &htab->t0[3][v.as_int8s[4]];
  a4 = &htab->t0[4][v.as_int8s[3]];
  a5 = &htab->t0[5][v.as_int8s[2]];
  a6 = &htab->t0[6][v.as_int8s[1]];
  a7 = &htab->t0[7][v.as_int8s[0]];
#else
  a0 = &htab->t0[0][v.as_int8s[0]];
  a1 = &htab->t0[1][v.as_int8s[1]];
  a2 = &htab->t0[2][v.as_int8s[2]];
  a3 = &htab->t0[3][v.as_int8s[3]];
  a4 = &htab->t0[4][v.as_int8s[4]];
  a5 = &htab->t0[5][v.as_int8s[5]];
  a6 = &htab->t0[6][v.as_int8s[6]];
  a7 = &htab->t0[7][v.as_int8s[7]];
#endif
  
  c0  = a0->v0 + a1->v0 + a2->v0 + a3->v0 + a4->v0 + a5->v0 + a6->v0 + a7->v0;
  c1  = a0->v1 + a1->v1 + a2->v1 + a3->v1 + a4->v1 + a5->v1 + a6->v1 + a7->v1;

  //
  // optional compression:
  //
#ifdef COMPRESS
  c0 = m3 - ((c0&m2)>>8) + (c0&m1);
  c1 = n3 - ((c1&n2)>>8) + (c1&n1);
#endif

  return a0->h ^ a1->h ^ a2->h ^ a3->h ^ a4->h ^ a5->h ^ a6->h ^ a7->h ^
    htab->t1[8-8][(c0&2043)] ^
    htab->t1[9-8][((c0>>11)&2043)] ^
    htab->t1[10-8][((c0>>22)&2043)] ^
    htab->t1[11-8][((c0>>33)&2043)] ^
    htab->t1[12-8][(c0>>44)] ^
    htab->t1[13-8][(c1&2043)] ^
    htab->t1[14-8][(c1>>11)];
}

inline HashCharTab2univID_64to64 InitHashCharTab2univ_64to64() {
  unsigned int i,j;
  HashCharTab2univID_64to64 htab;
  INT64 coef[8*2][5], rv;

  for (i = 0; i < 8; i++) {
    for (j = 0; j < 5; j++) {
      rv = RandINT64();
      coef[2*i][j]   = HIGH(rv);
      coef[2*i+1][j] = LOW(rv);
    }
  }

  htab = (HashCharTab2univID_64to64) malloc(8*sizeof(INT64*));
  for (i = 0; i < 8; i++) {
    htab[i] = (INT64*) malloc(256*sizeof(INT64));
    for (j = 0; j < 256; j++) {
      htab[i][j] = kwise_32to64(j, coef[i*2], coef[i*2+1], 5);
    }
  }
  
  return htab;
}

/* SHOULD BE CHAR tabulation based hashing for 32-bit key x using 16-bit characters.*/
inline INT64 HashCharTab2univ_64to64(INT64 x, HashCharTab2univID_64to64 htab) {
  int64views v;
  v.as_int64 = x;
#ifdef BIGENDIAN
  return
    htab[0][v.as_int8s[7]] ^
    htab[1][v.as_int8s[6]] ^
    htab[2][v.as_int8s[5]] ^
    htab[3][v.as_int8s[4]] ^
    htab[4][v.as_int8s[3]] ^
    htab[5][v.as_int8s[2]] ^
    htab[6][v.as_int8s[1]] ^
    htab[7][v.as_int8s[0]];
#else
  return
    htab[0][v.as_int8s[0]] ^
    htab[1][v.as_int8s[1]] ^
    htab[2][v.as_int8s[2]] ^
    htab[3][v.as_int8s[3]] ^
    htab[4][v.as_int8s[4]] ^
    htab[5][v.as_int8s[5]] ^
    htab[6][v.as_int8s[6]] ^
    htab[7][v.as_int8s[7]];
#endif
}


#endif

