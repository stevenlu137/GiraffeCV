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
 
 
 
#ifndef G_FEATHER_RPC_CLIENT_H_
#define G_FEATHER_RPC_CLIENT_H_

#include <iostream>
#include <string>

#include "GFeatherMessage.h"
#include "GFeatherMetaProgramming.h"

template <typename TTalk2Server, typename TSerializer, int iCMD, typename T>
struct GFeatherCaller
{
	GFeatherCaller()
	{}
};

template <typename TTalk2Server, typename TSerializer, int iCMD>
struct GFeatherCaller<TTalk2Server, TSerializer, iCMD, void()>
{
public:
	typedef void tFunctionType();
	static const int ciNumOfArgs = 0;

	static void Run(TTalk2Server* pTalk)
	{
		//paras serialization.
		//make CMD Msg.
		GFeatherMessage requestMessage;
		requestMessage.iCommand = iCMD;

		//send & receive Msg.
		pTalk->WriteRequest(requestMessage);
		GFeatherMessage responseMessage;
		while (true)
		{
			//std::_sleep(1);
			pTalk->ReadResponse(responseMessage);
			if (responseMessage.iCommand == iCMD)
			{
				break;
			}
		}

		assert(ciNumOfArgs == responseMessage.vParameters.size());

		if ("exception" == responseMessage.sSatus)
		{
			//todo.
			//std::cout << itoa(iCMD) << sRet << std::endl;
			throw;
		}

		return;
	}
};

template <typename TTalk2Server, typename TSerializer, int iCMD, typename TRet>
struct GFeatherCaller<TTalk2Server, TSerializer, iCMD, TRet()>
{
public:
	typedef TRet tFunctionType();
	static const int ciNumOfArgs = 0;

	static TRet Run(TTalk2Server* pTalk)
	{
		//paras serialization.
		//make CMD Msg.
		GFeatherMessage requestMessage;
		requestMessage.iCommand = iCMD;

		//send & receive Msg.
		pTalk->WriteRequest(requestMessage);
		GFeatherMessage responseMessage;
		while (true)
		{
			//std::_sleep(1);
			pTalk->ReadResponse(responseMessage);
			if (responseMessage.iCommand == iCMD)
			{
				break;
			}
		}

		assert(ciNumOfArgs == responseMessage.vParameters.size());

		if ("exception" == responseMessage.sSatus)
		{
			//todo.
			//std::cout << itoa(iCMD) << sRet << std::endl;
			throw;
		}

		TRet ret;
		TSerializer::Deserialize(responseMessage.sRet, ret);
		return ret;
	}
};

template <typename TTalk2Server, typename TSerializer, int iCMD, typename... TArgs>
struct GFeatherCaller<TTalk2Server, TSerializer, iCMD, void(TArgs...)>
{
public:
	typedef void tFunctionType(TArgs...);
	static const int ciNumOfArgs = sizeof...(TArgs);

	static void Run(TTalk2Server* pTalk, TArgs... args)
	{
		//paras serialization.
		//make CMD Msg.
		GFeatherMessage requestMessage;
		requestMessage.iCommand = iCMD;
		ArgsSerialization(requestMessage.vParameters, args...);

		//send & receive Msg.
		pTalk->WriteRequest(requestMessage);
		GFeatherMessage responseMessage;
		while (true)
		{
			//std::_sleep(1);
			pTalk->ReadResponse(responseMessage);
			if (responseMessage.iCommand == iCMD)
			{
				break;
			}
		}

		assert(ciNumOfArgs == responseMessage.vParameters.size());

		if ("exception" == responseMessage.sSatus)
		{
			//todo.
			//std::cout << itoa(iCMD) << sRet << std::endl;
			throw;
		}

		//de-serialization.
		bool vDeserializationCtlFlag[sizeof...(TArgs)] =
		{ (std::is_lvalue_reference<TArgs>::value && (!std::is_const<typename std::remove_reference<TArgs>::type>::value))... };

		ArgsDeserialization(responseMessage.vParameters
			, BuildIndices<sizeof...(TArgs)-1>()
			, std::vector<bool>(&vDeserializationCtlFlag[0], &vDeserializationCtlFlag[ciNumOfArgs]), args...);

		return;
	}

private:
	static void ArgsSerialization(std::vector<std::string>& vParaVector)
	{}

	template<typename TArgCurrent, typename... TArgsLeft>
	static void ArgsSerialization(std::vector<std::string>& vParaVector, TArgCurrent argCurrent, const TArgsLeft... args)
	{
		std::string sData;
		TSerializer::Serialize(argCurrent, sData);
		vParaVector.push_back(sData);
		ArgsSerialization(vParaVector, args...);
	}

	template<int... Indices, typename... TArguments>
	static void ArgsDeserialization(const std::vector<std::string>& vArgs, tIndices<Indices...>, std::vector<bool> vCtlFlag, TArguments&... args)
	{
		bool vTemp[sizeof...(TArguments)] = { (vCtlFlag[Indices] ? TSerializer::Deserialize(vArgs[Indices], args) : true)... };
	}
};


template <typename TTalk2Server, typename TSerializer, int iCMD, typename TRet, typename... TArgs>
struct GFeatherCaller<TTalk2Server, TSerializer, iCMD, TRet(TArgs...)>
{
public:
	typedef TRet tFunctionType(TArgs...);
	static const int ciNumOfArgs = sizeof...(TArgs);

	static TRet Run(TTalk2Server* pTalk, TArgs... args)
	{
		//paras serialization.
		//make CMD Msg.
		GFeatherMessage requestMessage;
		requestMessage.iCommand = iCMD;
		ArgsSerialization(requestMessage.vParameters, args...);

		//send & receive Msg.
		pTalk->WriteRequest(requestMessage);
		GFeatherMessage responseMessage;
		while (true)
		{
			//std::_sleep(1);
			pTalk->ReadResponse(responseMessage);
			if (responseMessage.iCommand == iCMD)
			{
				break;
			}
		}

		assert(ciNumOfArgs == responseMessage.vParameters.size());

		if ("exception" == responseMessage.sSatus)
		{
			//todo.
			//std::cout << itoa(iCMD) << sRet << std::endl;
			throw;
		}

		//de-serialization.
		bool vDeserializationCtlFlag[sizeof...(TArgs)] =
		{ (std::is_lvalue_reference<TArgs>::value && (!std::is_const<typename std::remove_reference<TArgs>::type>::value))... };

		ArgsDeserialization(responseMessage.vParameters
			, BuildIndices<sizeof...(TArgs)-1>()
			, std::vector<bool>(&vDeserializationCtlFlag[0], &vDeserializationCtlFlag[ciNumOfArgs]), args...);

		TRet ret;
		TSerializer::Deserialize(responseMessage.sRet, ret);
		return ret;
	}

private:
	static void ArgsSerialization(std::vector<std::string>& vParaVector)
	{}

	template<typename TArgCurrent, typename... TArgsLeft>
	static void ArgsSerialization(std::vector<std::string>& vParaVector, TArgCurrent argCurrent, const TArgsLeft... args)
	{
		std::string sData;
		TSerializer::Serialize(argCurrent, sData);
		vParaVector.push_back(sData);
		ArgsSerialization(vParaVector, args...);
	}

	template<int... Indices, typename... TArguments>
	static void ArgsDeserialization(const std::vector<std::string>& vArgs, tIndices<Indices...>, std::vector<bool> vCtlFlag, TArguments&... args)
	{
		bool vTemp[sizeof...(TArguments)] = { (vCtlFlag[Indices] ? TSerializer::Deserialize(vArgs[Indices], args) : true)... };
	}
};

//typedef GFeatherCaller<eEstimatorCMDVertex, void(std::vector<std::string>)> fnCallAddVertices2Graph;
//typedef GFeatherCaller<eEstimatorCMDTester, bool(int, long)> fnCallTester2;
//typedef GFeatherCaller<eEstimatorCMDTester, bool(int)> fnCallTester;


#endif //G_FEATHER_RPC_CLIENT_H_
