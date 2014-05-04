// Multiply two matrices A * B = C

#define _CRT_SECURE_NO_DEPRECATE


#include <stdlib.h>
#include <math.h>
#include <CL/cl.h>
//#include < conio.h >
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WA 100
#define HA 100
#define WB 100
#define HB 100
#define WC 100
#define HC 100

#define MAX_SOURCE_SIZE (0x100000)

// Allocates a matrix with random float entries.
void randomInit(float* data, int size)
{ int i;
	for (i = 0; i < size; ++i)
		data[i] = rand() / (float)RAND_MAX;
}

/////////////////////////////////////////////////////////
// Program main
/////////////////////////////////////////////////////////

int
main(int argc, char** argv)
{

	// set seed for rand()
	srand(2006);

	// 1. allocate host memory for matrices A and B
	unsigned int size_A = WA * HA;
	unsigned int mem_size_A = sizeof(float)* size_A;
	float* h_A = (float*)malloc(mem_size_A);

	unsigned int size_B = WB * HB;
	unsigned int mem_size_B = sizeof(float)* size_B;
	float* h_B = (float*)malloc(mem_size_B);

	// 2. initialize host memory
	randomInit(h_A, size_A);
	randomInit(h_B, size_B);
	int i,j,k;
	// 3. print out A and B
	//printf("\n\nMatrix A\n");
	/*for (i = 0; i < size_A; i++)
	{
		printf("%f ", h_A[i]);
		if (((i + 1) % WA) == 0)
			printf("\n");
	}

	printf("\n\nMatrix B\n");
	for (i = 0; i < size_B; i++)
	{
		printf("%f ", h_B[i]);
		if (((i + 1) % WB) == 0)
			printf("\n");
	}*/

	// 4. allocate host memory for the result C
	unsigned int size_C = WC * HC;
	unsigned int mem_size_C = sizeof(float)* size_C;
	float* h_C = (float*)malloc(mem_size_C);

	// 5. Initialize OpenCL
	// OpenCL specific variables
	cl_context clGPUContext;
	cl_command_queue clCommandQue;
	cl_program clProgram;
	cl_kernel clKernel;
	
	cl_platform_id platform_id = NULL;   //
	cl_uint ret_num_devices;  //
	cl_uint ret_num_platforms;  //

	size_t dataBytes;
	size_t kernelLength;
	cl_int errcode;
	cl_device_id device_id = NULL;  //
	 
	cl_int ret; //

	// OpenCL device memory for matrices
	cl_mem d_A;
	cl_mem d_B;
	cl_mem d_C;

	/*****************************************/
	/* Initialize OpenCL */
	/*****************************************/
	//clGPUContext = clCreateContextFromType(0,
	//	CL_DEVICE_TYPE_GPU,
	//	NULL, NULL, &errcode);
	//shrCheckError(errcode, CL_SUCCESS);

	// get the list of GPU devices associated 
	// with context
	
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);

	/* Create OpenCL context */
	clGPUContext = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	/* Create Command Queue */
	clCommandQue = clCreateCommandQueue(clGPUContext, device_id, 0, &ret);




/*	errcode = clGetContextInfo(clGPUContext,
		CL_CONTEXT_DEVICES, 0, NULL,
		&dataBytes);
	cl_device_id *clDevices = (cl_device_id *)
		malloc(dataBytes);

	errcode |= clGetContextInfo(clGPUContext,
		CL_CONTEXT_DEVICES, dataBytes,
		clDevices, NULL);

*/

//	shrCheckError(errcode, CL_SUCCESS);


	//Create a command-queue
//	clCommandQue = clCreateCommandQueue(clGPUContext,
//		clDevices[0], 0, &errcode);
	

	// shrCheckError(errcode, CL_SUCCESS);

	// Setup device memory

	d_C = clCreateBuffer(clGPUContext,
		CL_MEM_READ_WRITE,
		mem_size_A, NULL, &ret);
	d_A = clCreateBuffer(clGPUContext,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		mem_size_A, h_A, &ret);
	d_B = clCreateBuffer(clGPUContext,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		mem_size_B, h_B, &ret);

	//printf("6. Load and build OpenCL kernel\n");
	// 6. Load and build OpenCL kernel

	FILE *fp;
	char fileName[] = "./kernel.cl";
	char *source_str;
	size_t source_size;

	/* Load the source code containing the kernel*/
	fp = fopen(fileName, "r");
	if (!fp) {
		printf("Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	clProgram = clCreateProgramWithSource(clGPUContext, 1, (const char **)&source_str,
		(const size_t *)&source_size, &ret);



	

	ret = clBuildProgram(clProgram, 0,
		NULL, NULL, NULL, NULL);
//	shrCheckError(errcode, CL_SUCCESS);

	clKernel = clCreateKernel(clProgram,
		"matrixMul", &ret);
//	shrCheckError(errcode, CL_SUCCESS);


	// 7. Launch OpenCL kernel
	size_t localWorkSize[2], globalWorkSize[2];

	int wA = WA;
	int wC = WC;
	ret = clSetKernelArg(clKernel, 0,
		sizeof(cl_mem), (void *)&d_C);
	ret |= clSetKernelArg(clKernel, 1,
		sizeof(cl_mem), (void *)&d_A);
	ret |= clSetKernelArg(clKernel, 2,
		sizeof(cl_mem), (void *)&d_B);
	ret |= clSetKernelArg(clKernel, 3,
		sizeof(int), (void *)&wA);
	ret |= clSetKernelArg(clKernel, 4,
		sizeof(int), (void *)&wC);
//	shrCheckError(errcode, CL_SUCCESS);

	localWorkSize[0] = 3;
	localWorkSize[1] = 3;
	globalWorkSize[0] = 3;
	globalWorkSize[1] = 3;

	ret = clEnqueueNDRangeKernel(clCommandQue,
		clKernel, 2, NULL, globalWorkSize,
		localWorkSize, 0, NULL, NULL);
//	shrCheckError(errcode, CL_SUCCESS);

	// 8. Retrieve result from device
	ret = clEnqueueReadBuffer(clCommandQue,
		d_C, CL_TRUE, 0, mem_size_C,
		h_C, 0, NULL, NULL);
	//shrCheckError(errcode, CL_SUCCESS);

	// 9. print out the results
	/*printf("\n\nMatrix C (Results)\n");
	for (i = 0; i < size_C; i++)
	{
		printf("%f ", h_C[i]);
		if (((i + 1) % WC) == 0)
			printf("\n");
	}
	printf("\n");*/

	// 10. clean up memory
	free(h_A);
	free(h_B);
	free(h_C);

	clReleaseMemObject(d_A);
	clReleaseMemObject(d_C);
	clReleaseMemObject(d_B);

	//free(clDevices);
	//free(clMatrixMul);
	clReleaseContext(clGPUContext);
	clReleaseKernel(clKernel);
	clReleaseProgram(clProgram);
	clReleaseCommandQueue(clCommandQue);

}
