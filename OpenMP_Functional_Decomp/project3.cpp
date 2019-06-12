#include <string>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <omp.h>
#include <ctype.h>
using namespace std;

// "state" of the system involves these globals
int	NowYear;		// 2019 - 2024
int	NowMonth;		// 0 - 11
float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population
int	ZombieBearAttack = 0;   // a boolean for whether zombie bears attack

// Other parameters
const float GRAIN_GROWS_PER_MONTH =		8.0;    // in inches
const float ONE_DEER_EATS_PER_MONTH =		0.5;

const float AVG_PRECIP_PER_MONTH =		6.0;	// average (in inches)
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				50.0;	// average (in Farenheit)
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;

// Joe Parallel functions
float SQR( float x )
{
  return x*x;
} 

float Ranf( unsigned int *seedp,  float low, float high )
{
  float r = (float) rand_r( seedp );              // 0 - RAND_MAX
  return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}


int Ranf( unsigned int *seedp, int ilow, int ihigh )
{
  float low = (float)ilow;
  float high = (float)ihigh + 0.9999f;

  return (int)(  Ranf(seedp, low,high) );
}

// ZombieBears are lazy zombies. They only come out when the weather is nice: 
// Warm temperatures with little to no rain. But when they do come out, they
// kill everything in sight. When there is a ZombieBearAttack, half the
// deer population is wiped out. 

void ZombieBears() 
{
  while (NowYear < 2025) {
    
    int attack;
    if (NowTemp > 60 && NowPrecip < 10)
      attack = 1; 
    else
      attack = 0;
    
    // done computing barrier
    #pragma omp barrier       
    
    if (attack) 
      ZombieBearAttack = 1;
    else
      ZombieBearAttack = 0;

    // done assigning barrier 
    #pragma omp barrier       

    // done printing barrier
    #pragma omp barrier       
  }
}

void GrainDeer() 
{
  while (NowYear < 2025)
  {
    // compute temporary next value to add based on current state
    int tempVal;

    if (ZombieBearAttack) 
      tempVal = NowNumDeer / 2;
    else 
    {
      if (NowNumDeer > NowHeight)
        tempVal = NowNumDeer-1;
      else 
        tempVal = NowNumDeer+1;
    }

    // done computing barrier
    #pragma omp barrier       
    
    NowNumDeer = tempVal;
    // done assigning barrier 
    #pragma omp barrier       

    // done printing barrier
    #pragma omp barrier       
  }
}

void Grain() 
{
  while (NowYear < 2025) {
    
    // compute temporary next value based on current state
    float tempVal = 0;
    float tempFactor = exp(-SQR(( NowTemp - MIDTEMP ) / 10.));
    float precipFactor = exp(-SQR(( NowPrecip - MIDPRECIP ) / 10.));
    tempVal += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
    tempVal -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

    // done computing barrier
    #pragma omp barrier       

    NowHeight += tempVal;

    // prevents NowHeight from being set to negative value
    if (NowHeight < 0) 
    {
      NowHeight = 0;
    }

    // done assigning barrier 
    #pragma omp barrier       

    // done printing barrier
    #pragma omp barrier       
  }
}

void Watcher() 
{
  while (NowYear < 2025) {
    
    // done computing barrier
    #pragma omp barrier       

    // done assigning barrier 
    #pragma omp barrier
    
    // Adjust month and year
    if (NowMonth < 11) 
    {
      NowMonth += 1;
    }
    else
    {
      NowMonth = 0; 
      NowYear += 1;
    }

    // Use new month to adjust temp and precip
    float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

    float temp = AVG_TEMP - AMP_TEMP * cos( ang );
    unsigned int seed = rand();
    NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
    NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
    if( NowPrecip < 0. )
      NowPrecip = 0.;

    printf("%d\t%d\t%8.2f\t%8.2f\t%8.2f\t%d\t%d\n", NowYear, NowMonth, NowPrecip, NowTemp, NowHeight, NowNumDeer, ZombieBearAttack);
    // done printing barrier
    #pragma omp barrier       
  }
}

int main() {
  
  srand(time(0));

  // starting date and time:
  NowMonth =    0;
  NowYear  = 2019;

  // starting state (feel free to change this if you want):
  NowNumDeer = 1;
  NowHeight =  1.;

  // temp and precip are functions of particular month:
  float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

  float temp = AVG_TEMP - AMP_TEMP * cos( ang );
  unsigned int seed = 0;
  NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

  float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
  NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
  if( NowPrecip < 0. )
    NowPrecip = 0.;

  /* In addition to this, you must add in some other phenomenon that directly or indirectly controls the growth of the grain and/or the graindeer population. Your choice of this is up to you. */ 

  printf("Year\tMonth\tPrecip\t\tTemp\t\tHeight\t\tNumDeer\tZombieBears?\n");
  printf("%d\t%d\t%8.2f\t%8.2f\t%8.2f\t%d\t%d\n", NowYear, NowMonth, NowPrecip, NowTemp, NowHeight, NowNumDeer, ZombieBearAttack);
  omp_set_num_threads( 4 );	// same as # of sections

#pragma omp parallel sections
  {
    #pragma omp section
    {
      GrainDeer( );
    }

    #pragma omp section
    {
      Grain( );
    }

    #pragma omp section
    {
      Watcher( );
    }

    #pragma omp section
    {
      ZombieBears( );
    }
  }       
}
