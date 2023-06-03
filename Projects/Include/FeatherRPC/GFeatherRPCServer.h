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
 
 
 
#ifndef G_FEATHER_RPC_SERVER_H_
#define G_FEATHER_RPC_SERVER_H_

#include <iostream>
#include <string>

#include "GFeatherMessage.h"
#include "GFeatherMetaProgramming.h"


template <typename TSerializer, int iCMD, typename T, auto pFunc>
struct GFeatherCallee
{
	GFeatherCallee()
	{}
};

template <typename TSerializer, int iCMD, auto pFunc>
struct GFeatherCallee<TSerializer, iCMD, void(), pFunc>
{
public:
	typedef void tFunctionType();
	static const int ciNumOfArgs = 0;

	GFeatherCallee()
	{}

	static GFeatherMessage Run(const std::vector<std::string>& vParas)
	{
		assert(ciNumOfArgs == vParas.size());

		//call CPP func.
		//serialization to reply Msg.
		GFeatherMessage responseMessage;
		responseMessage.vParameters.resize(ciNumOfArgs);
		pFunc();

		responseMessage.iCommand = iCMD;
		responseMessage.sSatus = "ok";
		return responseMessage;
	}
};

template <typename TSerializer, int iCMD, typename TRet, auto pFunc>
struct GFeatherCallee<TSerializer, iCMD, TRet(), pFunc>
{
public:
	typedef TRet tFunctionType();
	static const int ciNumOfArgs = 0;

	GFeatherCallee()
	{}

	static GFeatherMessage Run(const std::vector<std::string>& vParas)
	{
		assert(ciNumOfArgs == vParas.size());

		//call CPP func.
		//serialization to reply Msg.
		GFeatherMessage responseMessage;
		responseMessage.vParameters.resize(ciNumOfArgs);

		TRet ret = pFunc();
		TSerializer::Serialize(ret, responseMessage.sRet);

		responseMessage.iCommand = iCMD;
		responseMessage.sSatus = "ok";
		return responseMessage;
	}
};

template <typename TSerializer, int iCMD, typename... TArgs, auto pFunc>
struct GFeatherCallee<TSerializer, iCMD, void(TArgs...), pFunc>
{
public:
	typedef void tFunctionType(TArgs...);
	static const int ciNumOfArgs = sizeof...(TArgs);

	GFeatherCallee()
	{}

	static GFeatherMessage Run(const std::vector<std::string>& vParas)
	{
		assert(ciNumOfArgs == vParas.size());

		//prepare temp arg variables.
		std::tuple<typename std::decay<TArgs>::type...> tpArgs;

		//de-serialization.
		std::vector<bool> vSerializationCtlFlag(ciNumOfArgs, true);
		ArgsDeserialization(vParas, tpArgs, BuildIndices<sizeof...(TArgs)-1>(), vSerializationCtlFlag);

		//call CPP func.
		//serialization to reply Msg.
		GFeatherMessage responseMessage;
		responseMessage.vParameters.resize(ciNumOfArgs);
		FuncCaller(tpArgs);

		bool vDeserializationCtlFlag[sizeof...(TArgs)] =
		{ (std::is_lvalue_reference<TArgs>::value && (!std::is_const<typename std::remove_reference<TArgs>::type>::value))... };
		ArgsSerialization(tpArgs, responseMessage.vParameters
			, BuildIndices<sizeof...(TArgs)-1>()
			, std::vector<bool>(&vDeserializationCtlFlag[0], &vDeserializationCtlFlag[ciNumOfArgs]));

		responseMessage.iCommand = iCMD;
		responseMessage.sSatus = "ok";
		return responseMessage;
	}

private:
	template<typename TTuple, int... Indices>
	static void ArgsSerialization(const TTuple& tpArgs, std::vector<std::string>& vArgs, tIndices<Indices...>, std::vector<bool> vCtlFlag)
	{
		bool vTemp[std::tuple_size<TTuple>::value] = { (vCtlFlag[Indices] ? TSerializer::Serialize(std::get<Indices>(tpArgs), vArgs[Indices]) : true)... };
	}

	template<typename TTuple, int... Indices>
	static void ArgsDeserialization(const std::vector<std::string>& vArgs, TTuple& tpArgs, tIndices<Indices...>, std::vector<bool> vCtlFlag)
	{
		bool vTemp[std::tuple_size<TTuple>::value] = { (vCtlFlag[Indices] ? TSerializer::Deserialize(vArgs[Indices], std::get<Indices>(tpArgs)) : true)... };
	}

	template<typename TTuple, int... Indices>
	static void FuncCaller_Indices(TTuple& tpArgs, tIndices<Indices...>)
	{
		return pFunc(std::get<Indices>(tpArgs)...);
	}

	template<typename TTuple>
	static void FuncCaller(TTuple& tpArgs)
	{
		return FuncCaller_Indices(tpArgs, BuildIndices<std::tuple_size<TTuple>::value - 1>());
	}
};

template <typename TSerializer, int iCMD, typename TRet, typename... TArgs, auto pFunc>
struct GFeatherCallee<TSerializer, iCMD, TRet(TArgs...), pFunc>
{
public:
	typedef TRet tFunctionType(TArgs...);
	static const int ciNumOfArgs = sizeof...(TArgs);

	GFeatherCallee()
	{}

	static GFeatherMessage Run(const std::vector<std::string>& vParas)
	{
		assert(ciNumOfArgs == vParas.size());

		//prepare temp arg variables.
		std::tuple<typename std::decay<TArgs>::type...> tpArgs;

		//de-serialization.
		std::vector<bool> vSerializationCtlFlag(ciNumOfArgs, true);
		ArgsDeserialization(vParas, tpArgs, BuildIndices<sizeof...(TArgs)-1>(), vSerializationCtlFlag);

		//call CPP func.
		//serialization to reply Msg.
		GFeatherMessage responseMessage;
		responseMessage.vParameters.resize(ciNumOfArgs);
		
		TRet ret = FuncCaller(tpArgs);
		TSerializer::Serialize(ret, responseMessage.sRet);

		bool vDeserializationCtlFlag[sizeof...(TArgs)] =
		{ (std::is_lvalue_reference<TArgs>::value && (!std::is_const<typename std::remove_reference<TArgs>::type>::value))... };
		ArgsSerialization(tpArgs, responseMessage.vParameters
			, BuildIndices<sizeof...(TArgs)-1>()
			, std::vector<bool>(&vDeserializationCtlFlag[0], &vDeserializationCtlFlag[ciNumOfArgs]));

		responseMessage.iCommand = iCMD;
		responseMessage.sSatus = "ok";
		return responseMessage;
	}

private:
	template<typename TTuple, int... Indices>
	static void ArgsSerialization(const TTuple& tpArgs, std::vector<std::string>& vArgs, tIndices<Indices...>, std::vector<bool> vCtlFlag)
	{
		bool vTemp[std::tuple_size<TTuple>::value] = { (vCtlFlag[Indices] ? TSerializer::Serialize(std::get<Indices>(tpArgs), vArgs[Indices]) : true)... };
	}

	template<typename TTuple, int... Indices>
	static void ArgsDeserialization(const std::vector<std::string>& vArgs, TTuple& tpArgs, tIndices<Indices...>, std::vector<bool> vCtlFlag)
	{
		bool vTemp[std::tuple_size<TTuple>::value] = { (vCtlFlag[Indices] ? TSerializer::Deserialize(vArgs[Indices], std::get<Indices>(tpArgs)) : true)... };
	}

	template<typename TTuple, int... Indices>
	static TRet FuncCaller_Indices(TTuple& tpArgs, tIndices<Indices...>)
	{
		return pFunc(std::get<Indices>(tpArgs)...);
	}

	template<typename TTuple>
	static TRet FuncCaller(TTuple& tpArgs)
	{
		return FuncCaller_Indices(tpArgs, BuildIndices<std::tuple_size<TTuple>::value - 1>());
	}
};

//typedef GFeatherCallee<eEstimatorCMDTester, decltype(func1), &func1> tfnCallTester;


#endif //G_FEATHER_RPC_SERVER_H_