/********************************************************************
 Count-Min Space-Saving
 
 An algorithm for tracking frequent items in data streams
 
 by Massimo Cafaro, Italo Epicoco and Marco Pulimeno
 University of Salento, Italy
 
 *********************************************************************/


j#include <stdio.h>
#include <math.h>
#include "hash.h"
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

#if defined(UNIV) || defined(UNIV_32)
#define HASHLEN 32
#define KEYLEN 32
#define HASHTYPE "Hash32bitUniv"
#define HASH Hash32bitUniv
#define HASHID Hash32bitUnivID
#define INITHASH InitHash32bitUniv
#endif
#ifdef UNIV_48
#define HASHLEN 64
#define KEYLEN 48
#define HASHTYPE "Hash48bitUniv"
#define HASH Hash48bitUniv
#define HASHID Hash48bitUnivID
#define INITHASH InitHash48bitUniv
#endif
#ifdef UNIV_64
#define HASHLEN 64
#define KEYLEN 64
#define HASHTYPE "Hash64bitUniv"
#define HASH Hash64bitUniv
#define HASHID Hash64bitUnivID
#define INITHASH InitHash64bitUniv
#endif
#if defined(UNIV2) || defined(UNIV2_32)
#define HASHLEN 32
#define KEYLEN 32
#define HASHTYPE "Hash32bit2univ"
#define HASH Hash32bit2univ
#define HASHID Hash32bit2univID
#define INITHASH InitHash32bit2univ
#endif
#ifdef UNIV2_48
#define HASHLEN 64
#define KEYLEN 48
#define HASHTYPE "Hash48bit2univ"
#define HASH Hash48bit2univ
#define HASHID Hash48bit2univID
#define INITHASH InitHash48bit2univ
#endif
#ifdef UNIV2_64
#define HASHLEN 64
#define KEYLEN 64
#define HASHTYPE "Hash64bit2univ"
#define HASH Hash64bit2univ
#define HASHID Hash64bit2univID
#define INITHASH InitHash64bit2univ
#endif
#ifdef CW5UNIV_32to32
#define HASHLEN 32
#define KEYLEN 32
#define HASHTYPE "Hash61bit5univ_32to32"
#define HASH Hash61bit5univ_32to32
#define HASHID Hash61bit5univID
#define INITHASH InitHash61bit5univ
#endif
#ifdef CW5UNIV_32to61
#define HASHLEN 64
#define KEYLEN 32
#define HASHTYPE "Hash61bit5univ_32to61"
#define HASH Hash61bit5univ_32to61
#define HASHID Hash61bit5univID
#define INITHASH InitHash61bit5univ
#endif
#ifdef CW5UNIV_48to61
#define HASHLEN 64
#define KEYLEN 48
#define HASHTYPE "Hash61bit5univ_48to61"
#define HASH Hash61bit5univ_48to61
#define HASHID Hash61bit5univID
#define INITHASH InitHash61bit5univ
#endif
#ifdef CW5UNIV_64to64
#define HASHLEN 64
#define KEYLEN 64
#define HASHTYPE "Hash89bit5univ_64to64"
#define HASH Hash89bit5univ_64to64
#define HASHID Hash89bit5univID
#define INITHASH InitHash89bit5univ
#endif
#ifdef CW4UNIV_32to32
#define HASHLEN 32
#define KEYLEN 32
#define HASHTYPE "Hash61bit4univ_32to32"
#define HASH Hash61bit4univ_32to32
#define HASHID Hash61bit5univID
#define INITHASH InitHash61bit5univ
#endif
#ifdef CW4UNIV_32to61
#define HASHLEN 64
#define KEYLEN 32
#define HASHTYPE "Hash61bit4univ_32to61"
#define HASH Hash61bit4univ_32to61
#define HASHID Hash61bit5univID
#define INITHASH InitHash61bit5univ
#endif
#ifdef CW4UNIV_48to61
#define HASHLEN 64
#define KEYLEN 48
#define HASHTYPE "Hash61bit4univ_48to61"
#define HASH Hash61bit4univ_48to61
#define HASHID Hash61bit5univID
#define INITHASH InitHash61bit5univ
#endif
#ifdef CW4UNIV_64to64
#define HASHLEN 64
#define KEYLEN 64
#define HASHTYPE "Hash89bit4univ_64to64"
#define HASH Hash89bit4univ_64to64
#define HASHID Hash89bit5univID
#define INITHASH InitHash89bit5univ
#endif
#ifdef SHORTTAB5UNIV_32to32
#define HASHLEN 32
#define KEYLEN 32
#ifdef COMPRESS
#define HASHTYPE "HashCompShortTab5univ_32to32"
#else
#define HASHTYPE "HashShortTab5univ_32to32"
#endif
#define HASH HashShortTab5univ_32to32
#define HASHID HashShortTab5univID_32to32
#define INITHASH InitHashShortTab5univ_32to32
#endif
#ifdef SHORTTAB5UNIV_32to64
#define HASHLEN 64
#define KEYLEN 32
#ifdef COMPRESS
#define HASHTYPE "HashCompShortTab5univ_32to64"
#else
#define HASHTYPE "HashShortTab5univ_32to64"
#endif
#define HASH HashShortTab5univ_32to64
#define HASHID HashShortTab5univID_32to64
#define INITHASH InitHashShortTab5univ_32to64
#endif
#ifdef SHORTTAB5UNIV_48to64
#define HASHLEN 64
#define KEYLEN 48
#ifdef COMPRESS
#define HASHTYPE "HashCompShortTab5univ_48to64"
#else
#define HASHTYPE "HashShortTab5univ_48to64"
#endif
#define HASH HashShortTab5univ_48to64
#define HASHID HashShortTab5univID_48to64
#define INITHASH InitHashShortTab5univ_48to64
#endif
#ifdef SHORTTAB5UNIV_64to64
#define HASHLEN 64
#define KEYLEN 64
#ifdef COMPRESS
#define HASHTYPE "HashCompShortTab5univ_64to64"
#else
#define HASHTYPE "HashShortTab5univ_64to64"
#endif
#define HASH HashShortTab5univ_64to64
#define HASHID HashShortTab5univID_64to64
#define INITHASH InitHashShortTab5univ_64to64
#endif
#ifdef CHARTAB2UNIV_32to32
#define HASHLEN 32
#define KEYLEN 32
#define HASHTYPE "HashCharTab2univ_32to32"
#define HASH HashCharTab2univ_32to32
#define HASHID HashCharTab2univID_32to32
#define INITHASH InitHashCharTab2univ_32to32
#endif
#ifdef CHARTAB2UNIV_64to64
#define HASHLEN 64
#define KEYLEN 64
#define HASHTYPE "HashCharTab2univ_64to64"
#define HASH HashCharTab2univ_64to64
#define HASHID HashCharTab2univID_64to64
#define INITHASH InitHashCharTab2univ_64to64
#endif
#ifdef CHARTAB5UNIV_32to32
#define HASHLEN 32
#define KEYLEN 32
#ifdef COMPRESS
#define HASHTYPE "HashCompCharTab5univ_32to32"
#else
#define HASHTYPE "HashCharTab5univ_32to32"
#endif
#define HASH HashCharTab5univ_32to32
#define HASHID HashCharTab5univID_32to32
#define INITHASH InitHashCharTab5univ_32to32
#endif
#ifdef CHARTAB5UNIV_32to64
#define HASHLEN 64
#define KEYLEN 32
#ifdef COMPRESS
#define HASHTYPE "HashCompCharTab5univ_32to64"
#else
#define HASHTYPE "HashCharTab5univ_32to64"
#endif
#define HASH HashCharTab5univ_32to64
#define HASHID HashCharTab5univID_32to64
#define INITHASH InitHashCharTab5univ_32to64
#endif
#ifdef CHARTAB5UNIV_40to64
#define HASHLEN 64
#define KEYLEN 40
#ifdef COMPRESS
#define HASHTYPE "HashCompCharTab5univ_40to64"
#else
#define HASHTYPE "HashCharTab5univ_40to64"
#endif
#define HASH HashCharTab5univ_40to64
#define HASHID HashCharTab5univID_40to64
#define INITHASH InitHashCharTab5univ_40to64
#endif
#ifdef CHARTAB5UNIV_48to64
#define HASHLEN 64
#define KEYLEN 48
#ifdef COMPRESS
#define HASHTYPE "HashCompCharTab5univ_48to64"
#else
#define HASHTYPE "HashCharTab5univ_48to64"
#endif
#define HASH HashCharTab5univ_48to64
#define HASHID HashCharTab5univID_48to64
#define INITHASH InitHashCharTab5univ_48to64
#endif
#ifdef CHARTAB5UNIV_56to64
#define HASHLEN 64
#define KEYLEN 56
#ifdef COMPRESS
#define HASHTYPE "HashCompCharTab5univ_56to64"
#else
#define HASHTYPE "HashCharTab5univ_56to64"
#endif
#define HASH HashCharTab5univ_56to64
#define HASHID HashCharTab5univID_56to64
#define INITHASH InitHashCharTab5univ_56to64
#endif
#ifdef CHARTAB5UNIV_64to64
#define HASHLEN 64
#define KEYLEN 64
#ifdef COMPRESS
#define HASHTYPE "HashCompCharTab5univ_64to64"
#else
#define HASHTYPE "HashCharTab5univ_64to64"
#endif
#define HASH HashCharTab5univ_64to64
#define HASHID HashCharTab5univID_64to64
#define INITHASH InitHashCharTab5univ_64to64
#endif
