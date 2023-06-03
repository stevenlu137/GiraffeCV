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
 
 
 
#ifndef G_C_FRAME_FORMAT_H_
#define G_C_FRAME_FORMAT_H_



enum G_enFrameFormat
{
	G_enFrameFormatNone = 0,
	G_enFrameFormatRGB = 1,
	G_enFrameFormatBGR = 2,
	G_enFrameFormatYUV420P = 32,
	G_enFrameFormatYV12 = 33,
	G_enFrameFormatNV12 = 34,

	G_enFrameFormatMaxNum = 8192,
};

struct G_tVideoFrame
{
	G_enFrameFormat eFrameFormat;
	unsigned int uiWidth;
	unsigned int uiHeight;
	unsigned char* pData[4];
	unsigned int uiLineStep_Byte[4];
};








#endif //G_C_FRAME_FORMAT_H_

