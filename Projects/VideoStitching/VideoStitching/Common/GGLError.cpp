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
 
 
 
#include "GGLError.h"

#include <string>

#include "Common/GiraffeLogger/GiraffeLogger.h"

#include "GGLHeaders.h"

using namespace std;

void _check_gl_error(const char *file, int line)
{
	GLenum err(glGetError());

	while (err != GL_NO_ERROR)
	{
		string sError;

		switch (err)
		{
		case GL_INVALID_OPERATION:      sError = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           sError = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          sError = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          sError = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  sError = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}

		GLOGGER(error) << "GL_" << sError.c_str() << " - " << file << ":" << line;
		err = glGetError();
	}
}