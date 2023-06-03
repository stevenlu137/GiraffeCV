#ifndef G_CUDA_TEXTURE_OBJECT_H_
#define G_CUDA_TEXTURE_OBJECT_H_

#include <memory>

#include <cuda.h>

#include "Common/CUDA/GCUDAUtils.cuh"
#include "Common/OpenCVUtils/OpenCVUtils.h"


namespace GCUDAObjects
{
	class GCUDAMemory
	{
	public:
		GCUDAMemory() :
			m_bIsWraper(false)
			, m_eCUDAResourceType(cudaResourceTypeArray)
			, m_iWidth(0)
			, m_iHeight(0)
			, m_pcuArray(nullptr)
			, m_pPitchData(nullptr)
			, m_szPitch(0)
		{
			memset(&m_channelFormatDescriptor, 0, sizeof(cudaChannelFormatDesc));
		}

		GCUDAMemory(cv::cuda::GpuMat& gmImg)
		{
			m_pPitchData = gmImg.data;
			m_szPitch = gmImg.step;

			m_eCUDAResourceType = cudaResourceTypePitch2D;

			m_channelFormatDescriptor = GetChannelDescriptor(gmImg.type());
			m_iWidth = gmImg.cols;
			m_iHeight = gmImg.rows;
			m_bIsWraper = true;
		}

		bool Create(int iWidth, int iHeight, const cudaChannelFormatDesc& channelFormatDescriptor, cudaResourceType eCUDAResourceType);
		bool CopyFrom(void* pData, int iWidth, int iHeight, int iStep_Byte);
		bool CopyTo(void* pData, int iStep_Byte);
		bool Upload(void* pData, int iWidth, int iHeight, int iStep_Byte);
		std::shared_ptr<cudaTextureObject_t> GetTextureObj(const cudaTextureDesc& textureDescriptor);
		std::shared_ptr<cudaSurfaceObject_t> GetSurfaceObj();
		void DestroyTextureObj(cudaTextureObject_t texObj);
		void DestroySurfaceObj(cudaSurfaceObject_t surfObj);
		cv::cuda::GpuMat GetCVGMat() const;

		void* GetData() const;
		int GetStep_Byte() const;
		void Release();

		int Width()const{ return m_iWidth; };
		int Height()const{ return m_iHeight; };
	private:
		cudaChannelFormatDesc GetChannelDescriptor(int eCVGMatType) const;
		int GetCVGMatType(const cudaChannelFormatDesc& channelDescriptor) const;

	private:
		bool m_bIsWraper;
		cudaResourceType m_eCUDAResourceType;
		cudaChannelFormatDesc m_channelFormatDescriptor;
		int m_iWidth;
		int m_iHeight;

		cudaArray* m_pcuArray;

		void* m_pPitchData;
		size_t m_szPitch;

	};

	bool GCUDAMemory::Create(int iWidth, int iHeight, const cudaChannelFormatDesc& channelFormatDescriptor, cudaResourceType eCUDAResourceType)
	{
		int iElementSize_Byte = (channelFormatDescriptor.x + channelFormatDescriptor.y + channelFormatDescriptor.z + channelFormatDescriptor.w + 7) / 8;

		if (cudaResourceTypeArray == eCUDAResourceType)
		{
			CUDA_SAFE_CALL(cudaMallocArray(&m_pcuArray, &channelFormatDescriptor, iWidth, iHeight, cudaArraySurfaceLoadStore));
		}
		else if (cudaResourceTypePitch2D == eCUDAResourceType)
		{
			CUDA_SAFE_CALL(cudaMallocPitch(&m_pPitchData, &m_szPitch, iWidth * iElementSize_Byte, iHeight));
		}
		else
		{
			return false;
		}

		m_eCUDAResourceType = eCUDAResourceType;
		m_channelFormatDescriptor = channelFormatDescriptor;
		m_iWidth = iWidth;
		m_iHeight = iHeight;
		return true;
	}


	bool GCUDAMemory::CopyFrom(void* pData, int iWidth, int iHeight, int iStep_Byte)
	{
		int iElementSize_Byte = (m_channelFormatDescriptor.x + m_channelFormatDescriptor.y + m_channelFormatDescriptor.z + m_channelFormatDescriptor.w + 7) / 8;

		if (cudaResourceTypeArray == m_eCUDAResourceType)
		{
			CUDA_SAFE_CALL(cudaMemcpy2DToArray(m_pcuArray, 0, 0, pData, iStep_Byte
				, iWidth * iElementSize_Byte, iHeight, cudaMemcpyDeviceToDevice));
		}
		else if (cudaResourceTypePitch2D == m_eCUDAResourceType)
		{
			CUDA_SAFE_CALL(cudaMemcpy2D(m_pPitchData, m_szPitch, pData, iStep_Byte
				, iWidth * iElementSize_Byte, iHeight, cudaMemcpyDeviceToDevice));
		}
		else
		{
			return false;
		}

		return true;
	}

	bool GCUDAMemory::CopyTo(void* pData, int iStep_Byte)
	{
		int iElementSize_Byte = (m_channelFormatDescriptor.x + m_channelFormatDescriptor.y + m_channelFormatDescriptor.z + m_channelFormatDescriptor.w + 7) / 8;

		if (cudaResourceTypeArray == m_eCUDAResourceType)
		{
			CUDA_SAFE_CALL(cudaMemcpy2DFromArray(pData, iStep_Byte, m_pcuArray, 0, 0
				, m_iWidth * iElementSize_Byte, m_iHeight, cudaMemcpyDeviceToDevice));
		}
		else if (cudaResourceTypePitch2D == m_eCUDAResourceType)
		{
			CUDA_SAFE_CALL(cudaMemcpy2D(pData, iStep_Byte, m_pPitchData, m_szPitch
				, m_iWidth * iElementSize_Byte, m_iHeight, cudaMemcpyDeviceToDevice));
		}
		else
		{
			return false;
		}

		return true;
	}

	bool GCUDAMemory::Upload(void* pData, int iWidth, int iHeight, int iStep_Byte)
	{
		int iElementSize_Byte = (m_channelFormatDescriptor.x + m_channelFormatDescriptor.y + m_channelFormatDescriptor.z + m_channelFormatDescriptor.w + 7) / 8;

		if (cudaResourceTypeArray == m_eCUDAResourceType)
		{
			CUDA_SAFE_CALL(cudaMemcpy2DToArray(m_pcuArray, 0, 0, pData, iStep_Byte
				, iWidth * iElementSize_Byte, iHeight, cudaMemcpyHostToDevice));
		}
		else if (cudaResourceTypePitch2D == m_eCUDAResourceType)
		{
			CUDA_SAFE_CALL(cudaMemcpy2D(m_pPitchData, m_szPitch, pData, iStep_Byte
				, iWidth * iElementSize_Byte, iHeight, cudaMemcpyHostToDevice));
		}
		else
		{
			return false;
		}

		return true;
	}

	std::shared_ptr<cudaTextureObject_t> GCUDAMemory::GetTextureObj(const cudaTextureDesc& textureDescriptor)
	{
		struct cudaResourceDesc resDesc;
		memset(&resDesc, 0, sizeof(resDesc));

		if (cudaResourceTypeArray == m_eCUDAResourceType)
		{
			resDesc.resType = cudaResourceTypeArray;
			resDesc.res.array.array = m_pcuArray;
		}
		else if (cudaResourceTypePitch2D == m_eCUDAResourceType)
		{
			resDesc.resType = cudaResourceTypePitch2D;
			resDesc.res.pitch2D.devPtr = m_pPitchData;
			resDesc.res.pitch2D.desc = m_channelFormatDescriptor;
			resDesc.res.pitch2D.width = m_iWidth;
			resDesc.res.pitch2D.height = m_iHeight;
			resDesc.res.pitch2D.pitchInBytes = m_szPitch;
		}
		else
		{
			exit(EXIT_FAILURE);
		}

		cudaTextureObject_t* pTexObj = new cudaTextureObject_t();
		CUDA_SAFE_CALL(cudaCreateTextureObject(pTexObj, &resDesc, &textureDescriptor, NULL));

		return std::shared_ptr<cudaTextureObject_t>(pTexObj, [this](cudaTextureObject_t* p)
		{
			this->DestroyTextureObj(*p);
			delete p;
		});
	}

	std::shared_ptr<cudaSurfaceObject_t> GCUDAMemory::GetSurfaceObj()
	{
		struct cudaResourceDesc resDesc;
		memset(&resDesc, 0, sizeof(resDesc));

		if (cudaResourceTypeArray == m_eCUDAResourceType)
		{
			resDesc.resType = cudaResourceTypeArray;
			resDesc.res.array.array = m_pcuArray;
		}
		else
		{
			exit(EXIT_FAILURE);
		}

		cudaSurfaceObject_t* pSurfObj = new cudaSurfaceObject_t();
		CUDA_SAFE_CALL(cudaCreateSurfaceObject(pSurfObj, &resDesc));
		return std::shared_ptr<cudaSurfaceObject_t>(pSurfObj, [this](cudaSurfaceObject_t* p)
		{
			this->DestroySurfaceObj(*p);
			delete p;
		});
	}

	void GCUDAMemory::DestroyTextureObj(cudaTextureObject_t texObj)
	{
		CUDA_SAFE_CALL(cudaDestroyTextureObject(texObj));
	}

	void GCUDAMemory::DestroySurfaceObj(cudaSurfaceObject_t surfObj)
	{
		CUDA_SAFE_CALL(cudaDestroySurfaceObject(surfObj));
	}

	cv::cuda::GpuMat GCUDAMemory::GetCVGMat() const
	{
		if (cudaResourceTypePitch2D == m_eCUDAResourceType)
		{
			return cv::cuda::GpuMat(m_iHeight, m_iWidth, GetCVGMatType(m_channelFormatDescriptor), m_pPitchData, m_szPitch);
		}
		else
		{
			return cv::cuda::GpuMat();
		}
	}

	void* GCUDAMemory::GetData() const
	{
		if (cudaResourceTypePitch2D == m_eCUDAResourceType)
		{
			return m_pPitchData;
		}
		else
		{
			return nullptr;
		}
	}

	int GCUDAMemory::GetStep_Byte() const
	{
		if (cudaResourceTypePitch2D == m_eCUDAResourceType)
		{
			return m_szPitch;
		}
		else
		{
			return 0;
		}
	}

	void GCUDAMemory::Release()
	{
		if (!m_bIsWraper)
		{
			if (cudaResourceTypeArray == m_eCUDAResourceType)
			{
				if (m_pcuArray)
				{
					CUDA_SAFE_CALL(cudaFreeArray(m_pcuArray));
				}
			}
			else if (cudaResourceTypePitch2D == m_eCUDAResourceType)
			{
				if (m_pPitchData)
				{
					CUDA_SAFE_CALL(cudaFree(m_pPitchData));
				}
			}
			else
			{
				return;
			}
		}

		m_pcuArray = nullptr;
		m_pPitchData = nullptr;
		m_szPitch = 0;

		m_iWidth = 0;
		m_iHeight = 0;
		m_eCUDAResourceType = cudaResourceTypeArray;
		memset(&m_channelFormatDescriptor, 0, sizeof(cudaChannelFormatDesc));
	}

	cudaChannelFormatDesc GCUDAMemory::GetChannelDescriptor(int eCVGMatType) const
	{
		cudaChannelFormatKind channelFormatKind;
		if ((CV_8U == CV_MAT_DEPTH(eCVGMatType)) || (CV_16U == CV_MAT_DEPTH(eCVGMatType)))
		{
			channelFormatKind = cudaChannelFormatKindUnsigned;
		}
		else if ((CV_8S == CV_MAT_DEPTH(eCVGMatType)) || (CV_16S == CV_MAT_DEPTH(eCVGMatType)) || (CV_32S == CV_MAT_DEPTH(eCVGMatType)))
		{
			channelFormatKind = cudaChannelFormatKindSigned;
		}
		else if ((CV_32F == CV_MAT_DEPTH(eCVGMatType)) || (CV_64F == CV_MAT_DEPTH(eCVGMatType)))
		{
			channelFormatKind = cudaChannelFormatKindFloat;
		}

		int iElementSize_Byte = sizeof(CVRunTimeTypeInfo::GetTypeSize_Byte(eCVGMatType));
		int iChannels = CVRunTimeTypeInfo::GetChannels(eCVGMatType);

		cudaChannelFormatDesc channelFormatDescriptor = cudaCreateChannelDesc(iElementSize_Byte * 8 * (iChannels >= 1), iElementSize_Byte * 8 * (iChannels >= 2)
			, iElementSize_Byte * 8 * (iChannels >= 3), iElementSize_Byte * 8 * (iChannels >= 4)
			, channelFormatKind);

		return channelFormatDescriptor;
	}

	int GCUDAMemory::GetCVGMatType(const cudaChannelFormatDesc& channelDescriptor) const
	{
		unsigned int uiDepth = 0;
		unsigned int uiChannels = (m_channelFormatDescriptor.x != 0) + (m_channelFormatDescriptor.y != 0)
			+ (m_channelFormatDescriptor.z != 0) + (m_channelFormatDescriptor.w != 0);

		if (cudaChannelFormatKindUnsigned == m_channelFormatDescriptor.f)
		{
			if (8 == m_channelFormatDescriptor.x)
			{
				uiDepth = CV_8U;
			}
			else if (16 == m_channelFormatDescriptor.x)
			{
				uiDepth = CV_16U;
			}
		}
		else if (cudaChannelFormatKindSigned == m_channelFormatDescriptor.f)
		{
			if (8 == m_channelFormatDescriptor.x)
			{
				uiDepth = CV_8S;
			}
			else if (16 == m_channelFormatDescriptor.x)
			{
				uiDepth = CV_16S;
			}
			else if (32 == m_channelFormatDescriptor.x)
			{
				uiDepth = CV_32S;
			}
		}
		else if (cudaChannelFormatKindFloat == m_channelFormatDescriptor.f)
		{
			if (32 == m_channelFormatDescriptor.x)
			{
				uiDepth = CV_32F;
			}
			else if (64 == m_channelFormatDescriptor.x)
			{
				uiDepth = CV_64F;
			}
		}

		return CV_MAKETYPE(uiDepth, uiChannels);
	}
};







#endif //G_CUDA_TEXTURE_OBJECT_H_