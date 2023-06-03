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
 
 
 
#ifndef GLOGGER_H_
#define GLOGGER_H_

#define G_USE_GLOGGER
//#define G_USE_FAKE_LOGGER
//#define G_USE_COUT_LOGGER

#if defined(G_USE_GLOGGER)

#include <boost/log/trivial.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>


namespace logging = boost::log;
namespace src = boost::log::sources;

class GFakeGiraffeLogger;

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(GiraffeLoggerSource, src::severity_logger_mt<logging::trivial::severity_level>)

#define GLOGGER(x) BOOST_LOG_SEV(GiraffeLoggerSource::get(),::boost::log::trivial::x)
#define GLOGGER_RELEASE() { boost::log::core::get()->remove_all_sinks(); }

class GiraffeLogger
{
public:
	static void Init();
private:

};

#elif defined(G_USE_FAKE_LOGGER)
#define GLOGGER(x) GFakeGiraffeLogger()
#define GLOGGER_RELEASE()

class GiraffeLogger
{
public:
	static void Init(){};
};

class GFakeGiraffeLogger
{
public:
	template<typename tAnything>
	const GFakeGiraffeLogger& operator << (const tAnything& anything) const
	{
		return (*this);
	}
};

#elif defined(G_USE_COUT_LOGGER)
#define GLOGGER(x) GCoutGiraffeLogger()
#define GLOGGER_RELEASE()

#include <iostream>

class GiraffeLogger
{
public:
	static void Init(){};
};

class GCoutGiraffeLogger
{
public:
	template<typename tAnything>
	const GCoutGiraffeLogger& operator << (const tAnything& anything) const
	{
		std::cout << anything;
		return (*this);
	}

	~GCoutGiraffeLogger()
	{
		std::cout << std::endl;
	}
};


#endif // G_USE_GLOGGER

#endif //GLOGGER_H_
