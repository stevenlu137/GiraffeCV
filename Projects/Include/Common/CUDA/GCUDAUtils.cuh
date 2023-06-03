#ifndef G_CUDA_UTILS_H_
#define G_CUDA_UTILS_H_

#include <cuda.h>

namespace GCUDAUtils
{
	#define CUDA_SAFE_CALL(call) do {\
		cudaError_t err = call;\
		if (cudaSuccess != err) {\
			fprintf (stderr, "Cuda error in file '%s' in line %i : %s.", __FILE__, __LINE__, cudaGetErrorString(err) );\
			exit(EXIT_FAILURE);\
		}\
	} while (0)
	

	__host__ __device__ __forceinline__ int divUp(int total, int grain)
	{
		return (total + grain - 1) / grain;
	};

}


#endif //G_CUDA_UTILS_H_