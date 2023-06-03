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
 
 
 
#ifndef G_GL_HEADERS_H_
#define G_GL_HEADERS_H_

#ifdef WIN32
#include <GL/glew.h>
#include <GL/glu.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "gdi32.lib")    /* link Windows GDI lib        */
#pragma comment (lib, "winmm.lib")    /* link Windows MultiMedia lib */
#pragma comment (lib, "user32.lib")   /* link Windows user lib       */

#elif __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
//todo.
#elif __ANDROID__
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#elif __linux__
#include <GL/glew.h>
#include <EGL/egl.h>
//#include <GL/gl.h>

//#include <GL/glx.h>
#ifdef Success//a dirty workaround for a conflict between X and Eigen.
#undef Success
#endif

#endif



#endif //G_GL_HEADERS_H_
