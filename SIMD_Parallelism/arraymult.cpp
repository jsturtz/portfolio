#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>

#include "simd.p4.h"
#include<omp.h>
#ifndef ARRAYSIZE
#define ARRAYSIZE 1000000
#endif

#ifndef NUMTRIES
#define NUMTRIES 100
#endif

// vectorized array multiplication without SIMD
void NonSimdMul( float *a, float *b,   float *c,   int len )
{
  int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
  for( int i = 0; i < len; i++ )
  {
    c[i] = a[i] * b[i];
  }
}

// vectorized array multiplication with reduction without SIMD
float NonSimdMulSum( float *a, float *b, int len )
{
  float sum[4] = { 0., 0., 0., 0. };
  int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;

  for( int i = 0; i < len; i++ )
  {
    sum[0] += a[i] * b[i];
  }

  return sum[0];
}

int main() 
{
  // holds best performance results from timings
  double  SIMDMult   = 0, 
          SIMDRed    = 0,
          NoSIMDMult = 0, 
          NoSIMDRed  = 0;
    
  float* a = (float*) malloc(ARRAYSIZE * sizeof(float));
  float* b = (float*) malloc(ARRAYSIZE * sizeof(float));
  float* c = (float*) malloc(ARRAYSIZE * sizeof(float));
  double time0, time1;                                  // temporary timings for each loop
  double megaMultsSec;                                  // temporary performance variable

  // looking for the maximum performance:
  for( int t = 0; t < NUMTRIES; t++ )
  {
    // My non-simd two array multiplication algorithm
    time0 = omp_get_wtime();
    NonSimdMul(a, b, c, ARRAYSIZE);
    time1 = omp_get_wtime();
    megaMultsSec = (double) ARRAYSIZE / (time1 - time0) / 1000000.;
    if (megaMultsSec > NoSIMDMult)
    {
      NoSIMDMult = megaMultsSec;
    }
    
    // SIMD two-array multiplication algorithm with assembly
    time0 = omp_get_wtime();
    SimdMul(a, b, c, ARRAYSIZE);
    time1 = omp_get_wtime();
    megaMultsSec = (double) ARRAYSIZE / (time1 - time0) / 1000000.;
    if (megaMultsSec > SIMDMult) 
    {
      SIMDMult = megaMultsSec;
    }

    // My non-simd two-array multiplication and reduction algorithm
    time0 = omp_get_wtime();
    NonSimdMulSum(a, b, ARRAYSIZE);
    time1 = omp_get_wtime();
    megaMultsSec = (double) ARRAYSIZE / (time1 - time0) / 1000000.;
    if (megaMultsSec > NoSIMDRed) 
    {
      NoSIMDRed = megaMultsSec;
    }

    // SIMD two-array multiplication and reduction algorithm with assembly
    time0 = omp_get_wtime();
    SimdMulSum(a, b, ARRAYSIZE);
    time1 = omp_get_wtime();
    megaMultsSec = (double) ARRAYSIZE / (time1 - time0) / 1000000.;
    if (megaMultsSec > SIMDRed) 
    {
      SIMDRed = megaMultsSec;
    }
  }
  printf("%d\t%8.2lf\t%8.2lf\t%8.2lf\t%8.2lf\n", ARRAYSIZE, NoSIMDMult, SIMDMult, NoSIMDRed, SIMDRed);
  free(a);
  free(b);
  free(c);

}
