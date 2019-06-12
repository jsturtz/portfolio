// 1. Program header

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
/* #include <omp.h> */
#include <time.h>
#include <sys/time.h>

#include <OpenCL/cl.h>
#include <OpenCL/cl_platform.h>
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


/* #ifndef NMB */
/* #define	NMB			64 */
/* #endif */

/* #define NUM_ELEMENTS		NMB*1024*1024 */

#ifndef NUM_ELEMENTS 
#define NUM_ELEMENTS            1024
#endif

#ifndef LOCAL_SIZE
#define	LOCAL_SIZE		512
#endif

#define	NUM_WORK_GROUPS         NUM_ELEMENTS / LOCAL_SIZE

const char *			CL_FILE_NAME = { "first.cl" };
const float			TOL = 0.0001f;

void				Wait( cl_command_queue );
int				LookAtTheBits( float );
double                          get_wall_time();


int
main( int argc, char *argv[ ] )
{
	// see if we can even open the opencl kernel program
	// (no point going on if we can't):
        //
        /* cl_uint numPlatforms; */

        /* int tempstatus = clGetPlatformIDs( 0, NULL, &numPlatforms ); */
        /* if( tempstatus != CL_SUCCESS ) */
        /* fprintf( stderr, "clGetPlatformIDs failed (1)\n" ); */
        /* fprintf( stderr, "Number of Platforms = %d\n", numPlatforms ); */
        /* cl_platform_id *platforms = new cl_platform_id[ numPlatforms ]; */
        /* tempstatus = clGetPlatformIDs( numPlatforms, platforms, NULL ); */
        /* if( tempstatus != CL_SUCCESS ) */
        /* fprintf( stderr, "clGetPlatformIDs failed (2)\n" ); */
        /* cl_uint numDevices; */
        /* cl_device_id *devices; */
        /* for( int i = 0; i < (int)numPlatforms; i++ ) */
        /* { */
        /*     fprintf( stderr, "Platform #%d:\n", i ); */
        /*     size_t size; */
        /*     char *str; */
        /*     clGetPlatformInfo( platforms[i], CL_PLATFORM_NAME, 0, NULL, &size ); */
        /*     str = new char [ size ]; */
        /*     clGetPlatformInfo( platforms[i], CL_PLATFORM_NAME, size, str, NULL ); */
        /*     fprintf( stderr, "\tName = '%s'\n", str ); */
        /*     delete[ ] str; */
        /*     clGetPlatformInfo( platforms[i], CL_PLATFORM_VENDOR, 0, NULL, &size ); */
        /*     str = new char [ size ]; */
        /*     clGetPlatformInfo( platforms[i], CL_PLATFORM_VENDOR, size, str, NULL ); */
        /*     fprintf( stderr, "\tVendor = '%s'\n", str ); */
        /*     delete[ ] str; */

        /*     clGetPlatformInfo( platforms[i], CL_PLATFORM_VERSION, 0, NULL, &size ); */
        /*     str = new char [ size ]; */
        /*     clGetPlatformInfo( platforms[i], CL_PLATFORM_VERSION, size, str, NULL ); */
        /*     fprintf( stderr, "\tVersion = '%s'\n", str ); */
        /*     delete[ ] str; */
        /*     clGetPlatformInfo( platforms[i], CL_PLATFORM_PROFILE, 0, NULL, &size ); */
        /*     str = new char [ size ]; */
        /*     clGetPlatformInfo( platforms[i], CL_PLATFORM_PROFILE, size, str, NULL ); */
        /*     fprintf( stderr, "\tProfile = '%s'\n", str ); */
        /*     delete[ ] str; */
        /*     // find out how many devices are attached to each platform and get their ids: */
        /*     tempstatus = clGetDeviceIDs( platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices ); */
        /*     if( tempstatus != CL_SUCCESS ) */
        /*     fprintf( stderr, "clGetDeviceIDs failed (2)\n" ); */
        /*     devices = new cl_device_id[ numDevices ]; */
        /*     tempstatus = clGetDeviceIDs( platforms[i], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL ); */
        /*     if( tempstatus != CL_SUCCESS ) */
        /*     fprintf( stderr, "clGetDeviceIDs failed (2)\n" ); */
        /*     for( int j = 0; j < (int)numDevices; j++ ) */
        /*     { */
        /*         fprintf( stderr, "\tDevice #%d:\n", j ); */
        /*         size_t size; */
        /*         cl_device_type type; */
        /*         cl_uint ui; */
        /*         size_t sizes[3] = { 0, 0, 0 }; */
        /*         clGetDeviceInfo( devices[j], CL_DEVICE_TYPE, sizeof(type), &type, NULL ); */
        /*         fprintf( stderr, "\t\tType = 0x%04x = ", type ); */

        /*         switch( type ) */
        /*         { */
        /*             case CL_DEVICE_TYPE_CPU: */
        /*             fprintf( stderr, "CL_DEVICE_TYPE_CPU\n" ); */
        /*             break; */
        /*             case CL_DEVICE_TYPE_GPU: */
        /*             fprintf( stderr, "CL_DEVICE_TYPE_GPU\n" ); */
        /*             break; */
        /*             case CL_DEVICE_TYPE_ACCELERATOR: */
        /*             fprintf( stderr, "CL_DEVICE_TYPE_ACCELERATOR\n" ); */
        /*             break; */
        /*             default: */
        /*             fprintf( stderr, "Other...\n" ); */
        /*             break; */
        /*         } */
        /*         clGetDeviceInfo( devices[j], CL_DEVICE_VENDOR_ID, sizeof(ui), &ui, NULL ); */
        /*         fprintf( stderr, "\t\tDevice Vendor ID = 0x%04x\n", ui ); */
        /*         clGetDeviceInfo( devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(ui), &ui, NULL ); */
        /*         fprintf( stderr, "\t\tDevice Maximum Compute Units = %d\n", ui ); */
        /*         clGetDeviceInfo( devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(ui), &ui, NULL ); */
        /*         fprintf( stderr, "\t\tDevice Maximum Work Item Dimensions = %d\n", ui ); */
        /*         clGetDeviceInfo( devices[j], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(sizes), sizes, NULL ); */
        /*         fprintf( stderr, "\t\tDevice Maximum Work Item Sizes = %d x %d x %d\n", sizes[0], sizes[1], sizes[2] ); */
        /*         clGetDeviceInfo( devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size), &size, NULL ); */
        /*         fprintf( stderr, "\t\tDevice Maximum Work Group Size = %d\n", size ); */
        /*         clGetDeviceInfo( devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(ui), &ui, NULL ); */
        /*         fprintf( stderr, "\t\tDevice Maximum Clock Frequency = %d MHz\n", ui ); */
        /*     } */
        /* } */

	FILE *fp;
#ifdef WIN32
	errno_t err = fopen_s( &fp, CL_FILE_NAME, "r" );
	if( err != 0 )
#else
	fp = fopen( CL_FILE_NAME, "r" );
	if( fp == NULL )
#endif
	{
		fprintf( stderr, "Cannot open OpenCL source file '%s'\n", CL_FILE_NAME );
		return 1;
	}

	cl_int status;		// returned status from opencl calls
				// test against CL_SUCCESS

	// get the platform id:

	cl_platform_id platform;
	status = clGetPlatformIDs( 1, &platform, NULL );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clGetPlatformIDs failed (2)\n" );
	
	// get the device id:

	cl_device_id device;
	status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clGetDeviceIDs failed (2)\n" );

	// 2. allocate the host memory buffers:

	float *hA = new float[ NUM_ELEMENTS ];
	float *hB = new float[ NUM_ELEMENTS ];
	float *hC = new float[ NUM_ELEMENTS ];
	float *hD = new float[ NUM_ELEMENTS ];

	// fill the host memory buffers:

	for( int i = 0; i < NUM_ELEMENTS; i++ )
	{
		hA[i] = hB[i] = hC[i] = (float) sqrt(  (double)i  );
	}

	size_t dataSize = NUM_ELEMENTS * sizeof(float);

	// 3. create an opencl context:

	cl_context context = clCreateContext( NULL, 1, &device, NULL, NULL, &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateContext failed\n" );

	// 4. create an opencl command queue:

	cl_command_queue cmdQueue = clCreateCommandQueue( context, device, 0, &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateCommandQueue failed\n" );

	// 5. allocate the device memory buffers:

	cl_mem dA = clCreateBuffer( context, CL_MEM_READ_ONLY,  dataSize, NULL, &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateBuffer failed (1)\n" );

	cl_mem dB = clCreateBuffer( context, CL_MEM_READ_ONLY,  dataSize, NULL, &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateBuffer failed (2)\n" );

	cl_mem dC = clCreateBuffer( context, CL_MEM_READ_ONLY, dataSize, NULL, &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateBuffer failed (3)\n" );

	cl_mem dD = clCreateBuffer( context, CL_MEM_WRITE_ONLY, dataSize, NULL, &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateBuffer failed (3)\n" );

	// 6. enqueue the 2 commands to write the data from the host buffers to the device buffers:

	status = clEnqueueWriteBuffer( cmdQueue, dA, CL_FALSE, 0, dataSize, hA, 0, NULL, NULL );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clEnqueueWriteBuffer failed (1)\n" );

	status = clEnqueueWriteBuffer( cmdQueue, dB, CL_FALSE, 0, dataSize, hB, 0, NULL, NULL );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clEnqueueWriteBuffer failed (2)\n" );

	status = clEnqueueWriteBuffer( cmdQueue, dC, CL_FALSE, 0, dataSize, hC, 0, NULL, NULL );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clEnqueueWriteBuffer failed (2)\n" );

	Wait( cmdQueue );

	// 7. read the kernel code from a file:

	fseek( fp, 0, SEEK_END );
	size_t fileSize = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	char *clProgramText = new char[ fileSize+1 ];		// leave room for '\0'
	size_t n = fread( clProgramText, 1, fileSize, fp );
	clProgramText[fileSize] = '\0';
	fclose( fp );
	if( n != fileSize )
		fprintf( stderr, "Expected to read %d bytes read from '%s' -- actually read %d.\n", fileSize, CL_FILE_NAME, n );

	// create the text for the kernel program:

	char *strings[1];
	strings[0] = clProgramText;
	cl_program program = clCreateProgramWithSource( context, 1, (const char **)strings, NULL, &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateProgramWithSource failed\n" );
	delete [ ] clProgramText;

	// 8. compile and link the kernel code:

	char *options = { "" };
	status = clBuildProgram( program, 1, &device, options, NULL, NULL );
	if( status != CL_SUCCESS )
	{
		size_t size;
		clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size );
		cl_char *log = new cl_char[ size ];
		clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_LOG, size, log, NULL );
		fprintf( stderr, "clBuildProgram failed:\n%s\n", log );
		delete [ ] log;
	}


	size_t globalWorkSize[3] = { NUM_ELEMENTS, 1, 1 };
	size_t localWorkSize[3]  = { LOCAL_SIZE,   1, 1 };
        double time0, time1;

	// 9.1. create the kernel object for ArrayMult

	cl_kernel kernel = clCreateKernel( program, "ArrayMult", &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateKernel failed\n" );

	// 10.1. setup the arguments to the kernel object:

	status = clSetKernelArg( kernel, 0, sizeof(cl_mem), &dA );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clSetKernelArg failed (1)\n" );

	status = clSetKernelArg( kernel, 1, sizeof(cl_mem), &dB );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clSetKernelArg failed (2)\n" );

	status = clSetKernelArg( kernel, 2, sizeof(cl_mem), &dD );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clSetKernelArg failed (3)\n" );


	// 11.1. enqueue the kernel object for execution:

	Wait( cmdQueue );
        time0 = get_wall_time();

	status = clEnqueueNDRangeKernel( cmdQueue, kernel, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clEnqueueNDRangeKernel failed: %d\n", status );

	Wait( cmdQueue );
        time1 = get_wall_time();

	// 12.1 read the results buffer back from the device to the host:

	status = clEnqueueReadBuffer( cmdQueue, dC, CL_TRUE, 0, dataSize, hC, 0, NULL, NULL );
	if( status != CL_SUCCESS )
			fprintf( stderr, "clEnqueueReadBuffer failed\n" );

        double multPerformance = (double)NUM_ELEMENTS/(time1-time0)/1048576.;

	// 9.2. create the kernel object for ArrayMultAdd

	kernel = clCreateKernel( program, "ArrayMultAdd", &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateKernel failed\n" );

	// 10.2 setup the arguments to the kernel object:

	status = clSetKernelArg( kernel, 0, sizeof(cl_mem), &dA );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clSetKernelArg failed (1)\n" );

	status = clSetKernelArg( kernel, 1, sizeof(cl_mem), &dB );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clSetKernelArg failed (2)\n" );

	status = clSetKernelArg( kernel, 2, sizeof(cl_mem), &dC );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clSetKernelArg failed (3)\n" );

	status = clSetKernelArg( kernel, 3, sizeof(cl_mem), &dD );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clSetKernelArg failed (3)\n" );


	// 11.2. enqueue the kernel object for execution:

	Wait( cmdQueue );
        time0 = get_wall_time();

	status = clEnqueueNDRangeKernel( cmdQueue, kernel, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clEnqueueNDRangeKernel failed: %d\n", status );

	Wait( cmdQueue );
        time1 = get_wall_time();

	// 12.2. read the results buffer back from the device to the host:

	status = clEnqueueReadBuffer( cmdQueue, dC, CL_TRUE, 0, dataSize, hC, 0, NULL, NULL );
	if( status != CL_SUCCESS )
			fprintf( stderr, "clEnqueueReadBuffer failed\n" );

        double multAddPerformance = (double)NUM_ELEMENTS/(time1-time0)/1048576.;

	/* // did it work? */

	/* for( int i = 0; i < NUM_ELEMENTS; i++ ) */
	/* { */
	/* 	float expected = hA[i] * hB[i]; */
	/* 	if( fabs( hC[i] - expected ) > TOL ) */
	/* 	{ */
	/* 		//fprintf( stderr, "%4d: %13.6f * %13.6f wrongly produced %13.6f instead of %13.6f (%13.8f)\n", */
	/* 			//i, hA[i], hB[i], hC[i], expected, fabs(hC[i]-expected) ); */
	/* 		//fprintf( stderr, "%4d:    0x%08x *    0x%08x wrongly produced    0x%08x instead of    0x%08x\n", */
	/* 			//i, LookAtTheBits(hA[i]), LookAtTheBits(hB[i]), LookAtTheBits(hC[i]), LookAtTheBits(expected) ); */
	/* 	} */
	/* } */

	printf("%8d\t%4d\t%10d\t%10.3lf\t%10.3lf\n",
		NUM_ELEMENTS, LOCAL_SIZE, NUM_WORK_GROUPS, multPerformance, multAddPerformance );

#ifdef WIN32
	Sleep( 2000 );
#endif


	// 13. clean everything up:

	clReleaseKernel(        kernel   );
	clReleaseProgram(       program  );
	clReleaseCommandQueue(  cmdQueue );
	clReleaseMemObject(     dA  );
	clReleaseMemObject(     dB  );
	clReleaseMemObject(     dC  );
	clReleaseMemObject(     dD  );

	delete [ ] hA;
	delete [ ] hB;
	delete [ ] hC;
	delete [ ] hD;

	return 0;
}


int
LookAtTheBits( float fp )
{
	int *ip = (int *)&fp;
	return *ip;
}


// wait until all queued tasks have taken place:

void
Wait( cl_command_queue queue )
{
      cl_event wait;
      cl_int      status;

      status = clEnqueueMarker( queue, &wait );
      if( status != CL_SUCCESS )
	      fprintf( stderr, "Wait: clEnqueueMarker failed\n" );

      status = clWaitForEvents( 1, &wait );
      if( status != CL_SUCCESS )
	      fprintf( stderr, "Wait: clWaitForEvents failed\n" );
}


double 
get_wall_time()
{
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
