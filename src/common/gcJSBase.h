/*
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)
Copyright (C) 2014 Bad Juju Games, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.

Contact us at legal@badjuju.com.
*/

#ifndef DESURA_GCJSBASE_H
#define DESURA_GCJSBASE_H
#ifdef _WIN32
#pragma once
#endif

#include "cef_desura_includes/ChromiumBrowserI.h"
#include "Event.h"

typedef ChromiumDLL::JSObjHandle JSObjHandle;

void RegisterJSExtender( ChromiumDLL::JavaScriptExtenderI* jsextender );

template <class T>
class RegisterJSExtenderHelper
{
public:
	RegisterJSExtenderHelper()
		: m_tVal(new T())
	{
		::RegisterJSExtender( m_tVal );
	}

	void noOp()
	{
	}

	T* m_tVal;
};


#define REGISTER_JSEXTENDER( jsExtenderName ) static RegisterJSExtenderHelper< jsExtenderName > g_RJS;
#define REGISTER_JS_FUNCTION( function, class ) { registerFunction( #function , newJSDelegate(this, &class::function) ); }

#define REG_SIMPLE_JS_FUNCTION( function, class){ registerFunction( #function , newJSFunctionDelegate(this, &class::function, false) ); }
#define REG_SIMPLE_JS_VOIDFUNCTION( function, class){ registerFunction( #function , newJSVoidFunctionDelegate(this, &class::function, false) ); }

#define REG_SIMPLE_JS_OBJ_FUNCTION( function, class){ registerFunction( #function , newJSFunctionDelegate(this, &class::function, true)); }
#define REG_SIMPLE_JS_OBJ_VOIDFUNCTION( function, class){ registerFunction( #function , newJSVoidFunctionDelegate(this, &class::function, true)); }



class JSDelegateI
{
public:
	virtual ~JSDelegateI();
	virtual JSObjHandle operator()(ChromiumDLL::JavaScriptFactoryI *factory, ChromiumDLL::JavaScriptContextI* context, JSObjHandle obj, size_t argc, JSObjHandle* argv)=0;
};

class MapElementI
{
public:
	virtual JSObjHandle toJSObject(ChromiumDLL::JavaScriptFactoryI* factory)=0;
	virtual void destory()=0;
};

template <typename T>
class MapElement : public MapElementI
{
public:
	MapElement(T res)
	{
		t = res;
	}

	virtual JSObjHandle toJSObject(ChromiumDLL::JavaScriptFactoryI* factory)
	{
		return ToJSObject(factory, t);
	}

	virtual void destory()
	{
		delete this;
	}

	T t;
};


class PVoid
{
};

namespace UserCore
{
	namespace Item
	{
		class ItemInfoI;
	}
}

template <typename T>
void FromJSObject(T &t, JSObjHandle& arg)
{
	//Should not get here
	gcAssert(false);
}


template <typename U>
void FromJSObject(std::map<gcString, U> &map, JSObjHandle& arg)
{
	if (arg->isObject() == false)
		throw gcException(ERR_INVALID, "Cant convert js object into map (!isObject)");

	auto nKeyCount = arg->getNumberOfKeys();
	for (int x = 0; x<nKeyCount; x++)
	{
		char key[255] = {0};
		arg->getKey(x, key, 255);

		if (!key[0])
			continue;

		U t;
		JSObjHandle objH =  arg->getValue(key);
		FromJSObject(t, objH);
		map[gcString(key)] = t;
	}
}

template <typename U>
void FromJSObject(std::vector<U> &list, JSObjHandle& arg)
{
	if (arg->isArray() == false)
		throw gcException(ERR_INVALID, "Cant convert js object into vector (!isArray)");

	auto nCount = arg->getArrayLength();
	for (int x = 0; x<nCount; x++)
	{
		U t;
		JSObjHandle objH = arg->getValue(x);
		FromJSObject(t, objH);
		list.push_back(t);
	}
}

template <typename U>
void FromJSObject(gcRefPtr<U> &pRef, JSObjHandle& arg)
{
	if (!arg || !arg->isObject())
		return;

	auto sub = arg->getValue(typeid(U).name());

	if (!sub || !sub->isObject())
		return;

	pRef = gcRefPtr<U>(sub->getUserObject<U>());
}

template <>
void FromJSObject<UserCore::Item::ItemInfoI*>(UserCore::Item::ItemInfoI* &item, JSObjHandle& arg);

template <>
void FromJSObject<PVoid>(PVoid&, JSObjHandle& arg);

template <>
void FromJSObject<bool>(bool& ret, JSObjHandle& arg);

template <>
void FromJSObject<int>(int32 &ret, JSObjHandle& arg);

template <>
void FromJSObject<double>(double& ret, JSObjHandle& arg);

template <>
void FromJSObject<gcString>(gcString& ret, JSObjHandle& arg);

template <>
void FromJSObject<gcWString>(gcWString& ret, JSObjHandle& arg);

template <>
void FromJSObject<std::map<gcString, gcString>>(std::map<gcString, gcString> &map, JSObjHandle& arg);

template <>
inline void FromJSObject<JSObjHandle>(JSObjHandle& ret, JSObjHandle& arg)
{
	ret = arg;
}

JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const MapElementI* map);
JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const int32 intVal);
JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const bool boolVal);
JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const double doubleVal);
JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const gcString &stringVal);
JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const gcWString &stringVal);
JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const std::map<gcString, MapElementI*> &map);
JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const std::map<gcString, gcString> &map);
JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const std::vector<MapElementI*> &map);

template <typename T>
JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const std::vector<T> &list)
{
	JSObjHandle arr = factory->CreateArray();

	for (size_t x=0; x<list.size(); x++)
	{
		JSObjHandle jObj = ToJSObject(factory, list[x]);
		arr->setValue(x, jObj);
	}

	return arr;
}



bool FindJSObjectCleanup(const std::string &strKey);
void AddJSObjectCleanup(const std::string &strKey, std::function<void()> fnCleanup);


template <typename T>
JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const gcRefPtr<T> &pRef)
{
	if (!pRef)
		return factory->CreateNull();

	gcString strKey("{0}-{1}", typeid(T).name(), pRef.get());
	bool bFound = FindJSObjectCleanup(strKey);

	if (!bFound)
	{
		T* pT = pRef.get();
		pT->addRef();

		AddJSObjectCleanup(strKey, [pT](){
			pT->delRef();
		});
	}

	JSObjHandle ret = factory->CreateObject();
	ret->setValue(typeid(T).name(), factory->CreateObject(pRef.get()));
	return ret;
}

#include <type_traits>

template <typename T>
void getUserObject(T* &t, JSObjHandle pObj)
{
	typedef typename std::remove_pointer<T>::type X;
	t = pObj->getUserObject<X>();
}

template <typename T>
void getUserObject(gcRefPtr<T> &t, JSObjHandle pObj)
{
	FromJSObject(t, pObj);
}

template <typename T>
void getUserObject(T &t, JSObjHandle pObj)
{
	//should not get here
	gcAssert(false);
}

template <typename T>
T popAndConvert(JSObjHandle* argv, size_t &x, bool bFirstIsObj)
{
	T t = T();

	if (bFirstIsObj && x == 0)
	{
		if (argv[0]->isObject())
			getUserObject(t, argv[0]);
	}
	else
	{
		FromJSObject(t, argv[x]);
	}

#ifdef __clang__
	++x;
#else
	--x;
#endif

	return t;
}


template <typename R, typename ... Args>
class JSDelegateFunction : public JSDelegateI
{
public:
	JSDelegateFunction(std::function<R(Args...)> fnCallback, bool bFirstIsObj = false)
		: m_fnCallback(fnCallback)
		, m_bFirstIsObj(bFirstIsObj)
	{
	}

	JSObjHandle operator()(ChromiumDLL::JavaScriptFactoryI *factory, ChromiumDLL::JavaScriptContextI* context, JSObjHandle obj, size_t argc, JSObjHandle* argv) override
	{
		if (argc < sizeof...(Args))
			throw gcException(ERR_V8, "Not enough parameters supplied for javascript function call!");

#ifdef __clang__
		size_t x = 0;
#else
		size_t x = sizeof...(Args) - 1;
#endif

		try
		{
			R r = m_fnCallback(popAndConvert<Args>(argv, x, m_bFirstIsObj)...);
			return ToJSObject(factory, r);
		}
		catch (gcException &e)
		{
			Warning("Failed to convert js arg {0}: {1}\n", x, e);
			throw gcException(ERR_V8, gcString("Failed to convert javascript argument: {0}", e).c_str());
		}

		return factory->CreateUndefined();
	}

private:
	bool m_bFirstIsObj;
	std::function<R(Args...)> m_fnCallback;
};


template <typename ... Args>
class JSDelegateVoidFunction : public JSDelegateI
{
public:
	JSDelegateVoidFunction(std::function<void(Args...)> fnCallback, bool bFirstIsObj = false)
		: m_fnCallback(fnCallback)
		, m_bFirstIsObj(bFirstIsObj)
	{
	}

	JSObjHandle operator()(ChromiumDLL::JavaScriptFactoryI *factory, ChromiumDLL::JavaScriptContextI* context, JSObjHandle obj, size_t argc, JSObjHandle* argv) override
	{
		if (argc < sizeof...(Args))
			throw gcException(ERR_V8, "Not enough parameters supplied for javascript function call!");

		JSObjHandle ret(nullptr);

#ifdef __clang__
		size_t x = 0;
#else
		size_t x = sizeof...(Args) - 1;
#endif

		try
		{
			m_fnCallback(popAndConvert<Args>(argv, x, m_bFirstIsObj)...);
		}
		catch (gcException &e)
		{
			Warning("Failed to convert js arg {0}: {1}\n", x, e);
			throw gcException(ERR_V8, gcString("Failed to convert javascript argument: {0}", e).c_str());
		}

		return factory->CreateUndefined();
	}

private:
	bool m_bFirstIsObj;
	std::function<void(Args...)> m_fnCallback;
};







template <class T>
class JSDelegate : public JSDelegateI
{
public:
	typedef JSObjHandle (T::*JSItemFunction)(ChromiumDLL::JavaScriptFactoryI*, ChromiumDLL::JavaScriptContextI*, JSObjHandle, std::vector<JSObjHandle> &);

	JSDelegate(T *t, JSItemFunction function)
	{
		m_pItem = t;
		m_pFunction = function;
	}

	JSObjHandle operator()(ChromiumDLL::JavaScriptFactoryI *factory, ChromiumDLL::JavaScriptContextI* context, JSObjHandle obj, size_t argc, JSObjHandle* argv) override
	{
		std::vector<JSObjHandle> args;

		for (size_t x=0; x<argc; x++)
			args.push_back(argv[x]);

		return (*m_pItem.*m_pFunction)(factory, context, obj, args);
	}

private:
	T* m_pItem;
	JSItemFunction m_pFunction;
};


template <class TObj>
JSDelegateI* newJSDelegate(TObj* pObj, JSObjHandle (TObj::*function)(ChromiumDLL::JavaScriptFactoryI*, ChromiumDLL::JavaScriptContextI*, JSObjHandle, std::vector<JSObjHandle> &))
{
	return new JSDelegate<TObj>(pObj, function);
}

template <typename TObj, typename ... Args>
JSDelegateI* newJSVoidFunctionDelegate(TObj* pObj, void (TObj::*func)(Args...), bool bFirstIsObject)
{
	std::function<void(Args...)> fnCallback = [pObj, func](Args ... args)
	{
		(*pObj.*func)(args...);
	};

	return new JSDelegateVoidFunction<Args...>(fnCallback, bFirstIsObject);
}

template <class TObj, typename R, typename ... Args>
JSDelegateI* newJSFunctionDelegate(TObj* pObj, R(TObj::*func)(Args...), bool bFirstIsObject)
{
	std::function<R(Args...)> fnCallback = [pObj, func](Args ... args) -> R
	{
		return (*pObj.*func)(args...);
	};

	return new JSDelegateFunction<R, Args...>(fnCallback, bFirstIsObject);
}

class DesuraJSBaseNonTemplate : public ChromiumDLL::JavaScriptExtenderI
{
public:
	DesuraJSBaseNonTemplate(const char* name, const char* bindingFile);
	~DesuraJSBaseNonTemplate();

	virtual const char* getName();
	virtual const char* getRegistrationCode();

	JSObjHandle execute(ChromiumDLL::JavaScriptFunctionArgs* args);

	void registerFunction(const char* name, JSDelegateI *delegate);
	virtual bool preExecuteValidation(const char* function, uint32 functionHash, JSObjHandle object, size_t argc, JSObjHandle* argv){return true;}

	virtual ChromiumDLL::JavaScriptContextI* getContext()
	{
		return m_pContext;
	}

protected:
	gcString m_szRegCode;

private:
	gcString m_szBindingFile;
	gcString m_szName;
	std::map<uint32, JSDelegateI*> m_mDelegateList;
	ChromiumDLL::JavaScriptContextI* m_pContext;
};

template <class T>
class DesuraJSBase : public DesuraJSBaseNonTemplate
{
public:
	DesuraJSBase(const char* name, const char* bindingFile) : DesuraJSBaseNonTemplate(name, bindingFile)
	{
	}

	virtual JavaScriptExtenderI* clone()
	{
		return new T();
	}

	virtual void destroy()
	{
		delete this;
	}
};


#endif //DESURA_GCJSBASE_H
