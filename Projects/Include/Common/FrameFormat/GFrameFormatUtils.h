/*
 * Copyright (c) 2015-2023 Pengju Lu, Yanli Wang

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
 
 
#ifndef G_FRAME_FORMAT_UTILS_H_
#define G_FRAME_FORMAT_UTILS_H_

#include "GCFrameFormat.h"
#include "Common/Exception/GiraffeChecker.h"

namespace GFrameFormat
{
	struct GVideoFrame4Que
	{
		G_enFrameFormat eFrameFormat;
		int iWidth;
		int iHeight;

		GVideoFrame4Que()
		{
			eFrameFormat = G_enFrameFormatNone;
			iWidth = 0;
			iHeight = 0;
		}

		GVideoFrame4Que(G_tVideoFrame frame)
		{
			eFrameFormat = frame.eFrameFormat;
			iWidth = static_cast<int>(frame.uiWidth);
			iHeight = static_cast<int>(frame.uiHeight);
		}

		int Size_Byte() const
		{
			if ((G_enFrameFormatRGB == eFrameFormat) ||
				(G_enFrameFormatBGR == eFrameFormat))
			{
				return iWidth * iHeight * 3;
			}
			else if ((G_enFrameFormatYUV420P == eFrameFormat) ||
				(G_enFrameFormatYV12 == eFrameFormat) ||
				(G_enFrameFormatNV12 == eFrameFormat))
			{
				return (iWidth * iHeight * 3) / 2;
			}
			else
			{
				return -1;
			}
		}
	};


	inline bool CheckFrame(const G_tVideoFrame* pFrame)
	{
		if ((G_enFrameFormatBGR == pFrame->eFrameFormat) ||
			(G_enFrameFormatRGB == pFrame->eFrameFormat))
		{
			return ((pFrame->pData[0]) && !(pFrame->pData[1]) && !(pFrame->pData[2]) && !(pFrame->pData[3])
				&& (pFrame->uiLineStep_Byte[0] >= pFrame->uiWidth) && (0 == pFrame->uiLineStep_Byte[1])
				&& (0 == pFrame->uiLineStep_Byte[2]) && (0 == pFrame->uiLineStep_Byte[3]));
		}
		else if ((G_enFrameFormatYUV420P == pFrame->eFrameFormat) ||
			(G_enFrameFormatYV12 == pFrame->eFrameFormat))
		{
			return ((pFrame->pData[0]) && (pFrame->pData[1]) && (pFrame->pData[2]) && !(pFrame->pData[3])
				&& (pFrame->uiLineStep_Byte[0] >= pFrame->uiWidth) && (pFrame->uiLineStep_Byte[1] >= (pFrame->uiWidth / 2))
				&& (pFrame->uiLineStep_Byte[2] >= (pFrame->uiWidth / 2)) && (0 == pFrame->uiLineStep_Byte[3]));
		}
		else if (G_enFrameFormatNV12 == pFrame->eFrameFormat)
		{
			return ((pFrame->pData[0]) && (pFrame->pData[1]) && !(pFrame->pData[2]) && !(pFrame->pData[3])
				&& (pFrame->uiLineStep_Byte[0] >= pFrame->uiWidth) && (pFrame->uiLineStep_Byte[1] >= pFrame->uiWidth)
				&& (0 == pFrame->uiLineStep_Byte[2]) && (0 == pFrame->uiLineStep_Byte[3]));
		}
		else
		{
			return false;
		}
	}

	inline void Frame2Buffer(const G_tVideoFrame* pFrameSrc, unsigned char* pucDst)
	{
		g_check_exception(CheckFrame(pFrameSrc), GCommonException());

		if ((G_enFrameFormatBGR == pFrameSrc->eFrameFormat) ||
			(G_enFrameFormatRGB == pFrameSrc->eFrameFormat))
		{
			unsigned int uiWidth_Byte = pFrameSrc->uiWidth * 3;
			for (int iLine = 0; iLine < pFrameSrc->uiHeight; ++iLine)
			{
				unsigned char* pSrc = (unsigned char*)(pFrameSrc->pData[0]) + iLine * pFrameSrc->uiLineStep_Byte[0];
				unsigned char* pDst = pucDst + iLine * uiWidth_Byte;
				std::copy(pSrc, pSrc + uiWidth_Byte, pDst);
			}
		}
		else if ((G_enFrameFormatYUV420P == pFrameSrc->eFrameFormat) ||
			(G_enFrameFormatYV12 == pFrameSrc->eFrameFormat))
		{
			unsigned char* pDataY = (unsigned char*)(pFrameSrc->pData[0]);
			unsigned char* pDataU = (unsigned char*)(pFrameSrc->pData[1]);
			unsigned char* pDataV = (unsigned char*)(pFrameSrc->pData[2]);
			unsigned int uiWidth = pFrameSrc->uiWidth;
			unsigned int uiHeight = pFrameSrc->uiHeight;

			for (int iLineY = 0; iLineY < uiHeight; ++iLineY)
			{
				unsigned char* pSrc = pDataY + iLineY * pFrameSrc->uiLineStep_Byte[0];
				unsigned char* pDst = pucDst + iLineY * uiWidth;
				std::copy(pSrc, pSrc + uiWidth, pDst);
			}

			unsigned int uiHalfWidth = uiWidth / 2;

			unsigned char* pUStart = pucDst + uiWidth * uiHeight;
			for (int iLineU = 0; iLineU < uiHeight / 2; ++iLineU)
			{
				unsigned char* pSrc = pDataU + iLineU * pFrameSrc->uiLineStep_Byte[1];
				std::copy(pSrc, pSrc + uiHalfWidth, pUStart + iLineU * uiHalfWidth);
			}

			unsigned char* pVStart = pucDst + (uiWidth * uiHeight * 5) / 4;
			for (int iLineV = 0; iLineV < uiHeight / 2; ++iLineV)
			{
				unsigned char* pSrc = pDataV + iLineV * pFrameSrc->uiLineStep_Byte[2];
				std::copy(pSrc, pSrc + uiHalfWidth, pVStart + iLineV * uiHalfWidth);
			}
		}
		else if (G_enFrameFormatNV12 == pFrameSrc->eFrameFormat)
		{
			if ((pFrameSrc->uiLineStep_Byte[0] == pFrameSrc->uiWidth)
				&& (pFrameSrc->uiLineStep_Byte[1] == pFrameSrc->uiWidth))
			{
				unsigned int uiWidth = pFrameSrc->uiWidth;
				unsigned int uiHeight = pFrameSrc->uiHeight;
				std::copy(pFrameSrc->pData[0], pFrameSrc->pData[0] + (uiWidth * uiHeight * 3) / 2, pucDst);
			}
			else
			{
				unsigned char* pDataY = (unsigned char*)(pFrameSrc->pData[0]);
				unsigned char* pDataUV = (unsigned char*)(pFrameSrc->pData[1]);
				unsigned int uiWidth = pFrameSrc->uiWidth;
				unsigned int uiHeight = pFrameSrc->uiHeight;

				for (int iLineY = 0; iLineY < uiHeight; ++iLineY)
				{
					unsigned char* pSrc = pDataY + iLineY * pFrameSrc->uiLineStep_Byte[0];
					unsigned char* pDst = pucDst + iLineY * uiWidth;
					std::copy(pSrc, pSrc + uiWidth, pDst);
				}

				unsigned char* pUVStart = pucDst + uiWidth * uiHeight;
				for (int iLineUV = 0; iLineUV < uiHeight / 2; ++iLineUV)
				{
					unsigned char* pSrc = pDataUV + iLineUV * pFrameSrc->uiLineStep_Byte[1];
					std::copy(pSrc, pSrc + uiWidth, pUVStart + iLineUV * uiWidth);
				}
			}
		}
		else
		{
			return;
		}
	}

	inline void Buffer2Frame(GVideoFrame4Que frame4Que, unsigned char* pucSrc, G_tVideoFrame* pFrameDst)
	{
		g_check_exception(pucSrc, GCommonException());

		memset(pFrameDst, 0, sizeof(G_tVideoFrame));

		pFrameDst->eFrameFormat = frame4Que.eFrameFormat;
		pFrameDst->uiWidth = static_cast<unsigned int>(frame4Que.iWidth);
		pFrameDst->uiHeight = static_cast<unsigned int>(frame4Que.iHeight);

		if ((G_enFrameFormatBGR == frame4Que.eFrameFormat) ||
			(G_enFrameFormatRGB == frame4Que.eFrameFormat))
		{
			pFrameDst->pData[0] = pucSrc;
			pFrameDst->pData[1] = nullptr;
			pFrameDst->pData[2] = nullptr;
			pFrameDst->pData[3] = nullptr;
			pFrameDst->uiLineStep_Byte[0] = pFrameDst->uiWidth;
			pFrameDst->uiLineStep_Byte[1] = 0;
			pFrameDst->uiLineStep_Byte[2] = 0;
			pFrameDst->uiLineStep_Byte[3] = 0;
		}
		else if ((G_enFrameFormatYUV420P == frame4Que.eFrameFormat) ||
			(G_enFrameFormatYV12 == frame4Que.eFrameFormat))
		{
			pFrameDst->pData[0] = pucSrc;
			pFrameDst->pData[1] = pucSrc + pFrameDst->uiWidth * pFrameDst->uiHeight;
			pFrameDst->pData[2] = pucSrc + (pFrameDst->uiWidth * pFrameDst->uiHeight * 5) / 4;
			pFrameDst->pData[3] = nullptr;
			pFrameDst->uiLineStep_Byte[0] = pFrameDst->uiWidth;
			pFrameDst->uiLineStep_Byte[1] = pFrameDst->uiWidth / 2;
			pFrameDst->uiLineStep_Byte[2] = pFrameDst->uiWidth / 2;
			pFrameDst->uiLineStep_Byte[3] = 0;
		}
		else if (G_enFrameFormatNV12 == frame4Que.eFrameFormat)
		{
			pFrameDst->pData[0] = pucSrc;
			pFrameDst->pData[1] = pucSrc + pFrameDst->uiWidth * pFrameDst->uiHeight;
			pFrameDst->pData[2] = nullptr;
			pFrameDst->pData[3] = nullptr;
			pFrameDst->uiLineStep_Byte[0] = pFrameDst->uiWidth;
			pFrameDst->uiLineStep_Byte[1] = pFrameDst->uiWidth;
			pFrameDst->uiLineStep_Byte[2] = 0;
			pFrameDst->uiLineStep_Byte[3] = 0;
		}
		else
		{
			return;
		}
	}
}




#endif //G_FRAME_FORMAT_UTILS_H_