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
 
 
 
#ifndef G_DLL_EXPORT_H_
#define G_DLL_EXPORT_H_

#if defined _WIN32 || defined __CYGWIN__
#ifdef __GNUC__
#define G_DLL_IMPORT __attribute__ ((dllimport))
#define G_DLL_EXPORT __attribute__ ((dllexport))
#else
#define G_DLL_IMPORT __declspec(dllimport)
#define G_DLL_EXPORT __declspec(dllexport)
#endif
#define G_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define G_DLL_IMPORT __attribute__ ((visibility ("default")))
#define G_DLL_EXPORT __attribute__ ((visibility ("default")))
#define G_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define G_DLL_IMPORT
#define G_DLL_EXPORT
#define G_DLL_LOCAL
#endif
#endif



#endif //G_DLL_EXPORT_H_