// Array multiplication: C = A * B:

// System includes
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
// CUDA runtime
#include <cuda_runtime.h>

// Helper functions and utilities to work with CUDA
#include "helper_functions.h"
#include "helper_cuda.h"


#ifndef BLOCKSIZE
#define BLOCKSIZE		32		// number of threads per block
#endif

#ifndef SIZE
#define SIZE			1*1024*1024	// array size
#endif

#ifndef NUMTRIALS
#define NUMTRIALS		100		// to make the timing more accurate
#endif

#ifndef TOLERANCE
#define TOLERANCE		0.00001f	// tolerance to relative error
#endif

// monte carlo simulation (CUDA Kernel) on the device

__global__  void MonteCarlo( float *X, float *Y, float *R , int *hits)
{
  // calculate global id
  /* unsigned int numItems = blockDim.x; */
  /* unsigned int tnum = threadIdx.x; */
  /* unsigned int wgNum = blockIdx.x; */
  unsigned int gid = blockIdx.x*blockDim.x + threadIdx.x;

  // randomize the location and radius of the circle:
  float xc = X[gid];
  float yc = Y[gid];
  float  r = R[gid];

  // solve for the intersection using the quadratic formula:
  float a = 2.;
  float b = -2.*( xc + yc );
  float c = xc*xc + yc*yc - r*r;
  float d = b*b - 4.*a*c;

  // If d is less than 0., then the circle was completely missed. (Case A) Ignore this case
  if (d >= 0) {

    // get the first intersection:
    d = sqrt( d );
    float t1 = (-b + d ) / ( 2.*a );	// time to intersect the circle
    float t2 = (-b - d ) / ( 2.*a );	// time to intersect the circle
    float tmin = t1 < t2 ? t1 : t2;		// only care about the first intersection
    
    // If tmin is less than 0., then the circle completely engulfs the laser pointer. (Case B) Ignore this case
    if (tmin >= 0.) {
      // where does it intersect the circle?
      float xcir = tmin;
      float ycir = tmin;

      // get the unitized normal vector at the point of intersection:
      float nx = xcir - xc;
      float ny = ycir - yc;
      float n = sqrt( nx*nx + ny*ny );
      nx /= n;	// unit vector
      ny /= n;	// unit vector

      // get the unitized incoming vector:
      float inx = xcir - 0.;
      float iny = ycir - 0.;
      float in = sqrt( inx*inx + iny*iny );
      inx /= in;	// unit vector
      iny /= in;	// unit vector

      // get the outgoing (bounced) vector:
      float dot = inx*nx + iny*ny;
      /* float outx = inx - 2.*nx*dot;	// angle of reflection = angle of incidence` */
      float outy = iny - 2.*ny*dot;	// angle of reflection = angle of incidence`

      // find out if it hits the infinite plate:
      float t = ( 0. - ycir ) / outy;

      // If t is less than 0., then the reflected beam went up instead of down. Ignore this case
      // Otherwise, this beam hit the infinite plate. (Case D) Set hit = 1 for this case
      if (t >= 0) {
        hits[gid] = 1;
      }
    }
  }
}

/* // ranges for the random numbers: */
const float XCMIN =	 0.0;
const float XCMAX =	 2.0;
const float YCMIN =	 0.0;
const float YCMAX =	 2.0;
const float RMIN  =	 0.5;
const float RMAX  =	 2.0;

// function prototypes:
float		Ranf( float, float );
int		Ranf( int, int );
void		TimeOfDaySeed( );

int
main( int argc, char* argv[ ] )
{

  TimeOfDaySeed( );		// seed the random number generator

  // arrays on host to hold random values for x/y coords, radius, and hits
  float *hX = new float [SIZE];                 // x coords
  float *hY = new float [SIZE];                 // y coords
  float *hR = new float [SIZE];                 // radius coords
  int   *hH = new int   [SIZE];                 // holds hits from each block

  // fill the random-value arrays:
  for( int n = 0; n < SIZE; n++ )
  {       
    hX[n] = Ranf( XCMIN, XCMAX );
    hY[n] = Ranf( YCMIN, YCMAX );
    hR[n] = Ranf(  RMIN,  RMAX ); 
    hH[n] = 0; 
  }       

  /* int dev = findCudaDevice(argc, (const char **)argv); */

  // allocate device memory:

  float *dX, *dY, *dR;
  int *dH;

  dim3 dimsX( SIZE, 1, 1 );
  dim3 dimsY( SIZE, 1, 1 );
  dim3 dimsR( SIZE, 1, 1 );
  dim3 dimsH( SIZE, 1, 1 );

  cudaError_t status;
  status = cudaMalloc( reinterpret_cast<void **>(&dX), SIZE*sizeof(float) );
          checkCudaErrors( status );
  status = cudaMalloc( reinterpret_cast<void **>(&dY), SIZE*sizeof(float) );
          checkCudaErrors( status );
  status = cudaMalloc( reinterpret_cast<void **>(&dR), SIZE*sizeof(float) );
          checkCudaErrors( status );
  status = cudaMalloc( reinterpret_cast<void **>(&dH), (SIZE)*sizeof(int) );
          checkCudaErrors( status );

  // copy host memory to the device:
  status = cudaMemcpy( dX, hX, SIZE*sizeof(float), cudaMemcpyHostToDevice );
          checkCudaErrors( status );
  status = cudaMemcpy( dY, hY, SIZE*sizeof(float), cudaMemcpyHostToDevice );
          checkCudaErrors( status );
  status = cudaMemcpy( dR, hR, SIZE*sizeof(float), cudaMemcpyHostToDevice );
          checkCudaErrors( status );
  status = cudaMemcpy( dH, hH, SIZE*sizeof(int), cudaMemcpyHostToDevice );
          checkCudaErrors( status );

  // setup the execution parameters:

  dim3 threads(BLOCKSIZE, 1, 1 );
  dim3 grid( SIZE / threads.x, 1, 1 );

  // Create and start timer

  cudaDeviceSynchronize( );

  // allocate CUDA events that we'll use for timing:

  cudaEvent_t start, stop;
  status = cudaEventCreate( &start );
          checkCudaErrors( status );
  status = cudaEventCreate( &stop );
          checkCudaErrors( status );

  // record the start event:

  status = cudaEventRecord( start, NULL );
  checkCudaErrors( status );

  // execute the kernel:
  for( int t = 0; t < NUMTRIALS; t++)
  {
    MonteCarlo<<< grid, threads >>>( dX, dY, dR, dH );
  }

  // record the stop event:

  status = cudaEventRecord( stop, NULL );
  checkCudaErrors( status );

  // wait for the stop event to complete:

  status = cudaEventSynchronize( stop );
  checkCudaErrors( status );

  float msecTotal = 0.0f;
  status = cudaEventElapsedTime( &msecTotal, start, stop );
  checkCudaErrors( status );

  // compute and print the performance

  double secondsTotal = 0.001 * (double)msecTotal;
  double trialsPerSecond = (double)NUMTRIALS * (double)SIZE / secondsTotal;
  double megaTrialsPerSecond = trialsPerSecond / 1000000.;

  // copy result from the device to the host:
  status = cudaMemcpy( hH, dH, (SIZE)*sizeof(int), cudaMemcpyDeviceToHost );
  checkCudaErrors( status );

  // calculate probability (only uses results from last iteration of NUMTRIALS
  int numHits = 0;
  for (int i = 0; i < SIZE; i++) 
  {
    numHits += hH[i];
  }
  double probability = (double) numHits / (double) SIZE;

  printf("%10d\t%10d\t%10.2lf\t%10.2lf\n", BLOCKSIZE, SIZE, megaTrialsPerSecond, probability );

  // clean up memory:
  delete [ ] hX;
  delete [ ] hY;
  delete [ ] hR;
  delete [ ] hH;

  status = cudaFree( dX );
  checkCudaErrors( status );
  status = cudaFree( dY );
  checkCudaErrors( status );
  status = cudaFree( dR );
  checkCudaErrors( status );
  status = cudaFree( dH );
  checkCudaErrors( status );

  return 0;
}

float
Ranf( float low, float high )
{
  float r = (float) rand();               // 0 - RAND_MAX
  float t = r  /  (float) RAND_MAX;       // 0. - 1.

  return   low  +  t * ( high - low );
}

int
Ranf( int ilow, int ihigh )
{
  float low = (float)ilow;
  float high = ceil( (float)ihigh );

  return (int) Ranf(low,high);
}

void
TimeOfDaySeed( )
{
  struct tm y2k = { 0 };
  y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
  y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

  time_t  timer;
  time( &timer );
  double seconds = difftime( timer, mktime(&y2k) );
  unsigned int seed = (unsigned int)( 1000.*seconds );    // milliseconds
  srand( seed );
}
