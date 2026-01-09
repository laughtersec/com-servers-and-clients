// UserInfo.cpp : Implementation of DLL Exports.

//
// Note: COM+ 1.0 Information:
//      Please remember to run Microsoft Transaction Explorer to install the component(s).
//      Registration is not done by default.

// Below is auto-generated btw

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "UserInfo_i.h"

/*
remove dllmain.h

ALL THIS IS ALREADY IMPLEMENTED IN DLLMAIN.CPP, CONTRARY TO WHAT THE TEMPLATE HAS DONE HERE WITH ITS ATL STUFF

using namespace ATL;

// Used to determine whether the DLL can be unloaded by OLE.
_Use_decl_annotations_
STDAPI DllCanUnloadNow(void)
{
	return _AtlModule.DllCanUnloadNow();
}

// Returns a class factory to create an object of the requested type.
_Use_decl_annotations_
STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID* ppv)
{
	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - Adds entries to the system registry.
_Use_decl_annotations_
STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}

// DllUnregisterServer - Removes entries from the system registry.
_Use_decl_annotations_
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

// DllInstall - Adds/Removes entries to the system registry per user per machine.
STDAPI DllInstall(BOOL bInstall, _In_opt_  LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";

	if (pszCmdLine != nullptr)
	{
		if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
		{
			ATL::AtlSetPerUserRegistration(true);
		}
	}

	if (bInstall)
	{
		hr = DllRegisterServer();
		if (FAILED(hr))
		{
			DllUnregisterServer();
		}
	}
	else
	{
		hr = DllUnregisterServer();
	}

	return hr;
}
*/
//  Here lies YOUR implementations

#include "UserInfo.h"

//
//Forward declarations
// because it is defined in dllmain.cpp, so extern
//
extern BOOL ServerCanUnloadNow(void);  
extern void UnloadServer(void);

//
//Global variables
//
extern ULONG g_cObjects;
extern ULONG g_cLocks;

//
//CUserInfo
//

//
//get_Age
//
STDMETHODIMP CUserInfo::get_Age(short* nRetAge)
{
	*nRetAge = m_nAge;
	return NOERROR;
}  //  get_Age

//
//put_Age
//
STDMETHODIMP CUserInfo::put_Age(short nAge)
{
	m_nAge = nAge;
	return NOERROR;
}  //  put_Age

//
//get_name
//
STDMETHODIMP CUserInfo::get_Name(LPSTR* lpszRetName)
{
	*lpszRetName = m_lpszName;
	return NOERROR;
}  //  get_Name

//
//put_Name
//
STDMETHODIMP CUserInfo::put_Name(LPSTR lpszName)
{
	long	lStringLen;
	
	//  Deallocate any previously allocated storage
	if (m_lpszName)
		delete[] m_lpszName;
	m_lpszName = NULL;
	//   Allocate enough storage for the string
	lStringLen = strlen(lpszName);
	if (lStringLen > 0)
	{
		m_lpszName = new char[lStringLen + 1];
		//  Copy the string
		strcpy(m_lpszName, lpszName);
	}
	return NOERROR;
}  //  put_Name

//
//get_Sex
//
STDMETHODIMP CUserInfo::get_Sex(BYTE* byRetSex)
{
	*byRetSex = m_bySex;
	return NOERROR;
}  //  get_Sex

//
//put_Sex
//
STDMETHODIMP CUserInfo::put_Sex(BYTE bySex)
{
	m_bySex = bySex;
	return NOERROR;
}  //  put_Sex

//
//QueryInterface
//
STDMETHODIMP CUserInfo::QueryInterface(REFIID iid, LPVOID* ppv)
{
	*ppv = NULL;
	if (IID_IUnknown == iid)
		*ppv = (LPVOID)(IUnknown*)this;
	else if (IID_IUserInfo == iid)
		*ppv = (LPVOID)(IUserInfo*)this;
	else
		return E_NOINTERFACE;
	//Perform reference count through the returned interface
	((IUnknown*)*ppv)->AddRef();
	return NOERROR;
}  //  QueryInterface

//
//AddRef
//
STDMETHODIMP_(ULONG)CUserInfo::AddRef(void)
{
	return ++m_cRef;
}  //  AddRef

//
//Release
//
STDMETHODIMP_(ULONG)CUserInfo::Release(void)
{
	m_cRef--;
	if (0 == m_cRef)
	{
		delete this;
		//Decrement the global object count
		g_cObjects--;
		//See if it's alright to unload the server
		if (::ServerCanUnloadNow())
			::UnloadServer();
		return 0;
	}
	return m_cRef;
}  //  Release

//
//Constructor
//
CUserInfo::CUserInfo()
{
	m_cRef = 0;
	m_nAge = 0;
	m_lpszName = NULL;
	m_bySex = 'M';
}  //  CUserInfo

//
//Destructor
//
CUserInfo::~CUserInfo()
{
	if (m_lpszName)
		delete[] m_lpszName;
}  //  ~CUserInfo

//
//CUserInfoFactory Class Factory
//

//
//CreateInstance
//
STDMETHODIMP CUserInfoFactory::CreateInstance(IUnknown* pUnknownOuter, REFIID iid, LPVOID* ppv)
{
	HRESULT hr;
	CUserInfo* pCUserInfo = NULL;

	*ppv = NULL;
	//  This object doesn't support aggregation
	if (NULL != pUnknownOuter)
		return CLASS_E_NOAGGREGATION;
	//  Create the CUserInfo object
	pCUserInfo = new CUserInfo();
	if (NULL == pCUserInfo)
		return E_OUTOFMEMORY;
	//  Retrieve the requested interface
	hr = pCUserInfo->QueryInterface(iid, ppv);
	if (FAILED(hr))
	{
		delete pCUserInfo;
		pCUserInfo = NULL;
		return hr;
	}
	//  Increment the global object counter
	g_cObjects++;
	return NOERROR;
}//  CreateInstance

//
//LockServer
//
STDMETHODIMP CUserInfoFactory::LockServer(BOOL bLock)
{
	if (bLock)
		g_cLocks++;
	else
	{
		g_cLocks--;
		//  See if it's alright to unload the server
		if (::ServerCanUnloadNow())
			::UnloadServer();
	}
	return NOERROR;
}//  LockServer

//
//QueryInterface
//
STDMETHODIMP CUserInfoFactory::QueryInterface(REFIID iid, LPVOID* ppv)
{
	*ppv = NULL;
	if (IID_IUnknown == iid)
		*ppv = (LPVOID)(IUnknown*)this;
	else if (IID_IClassFactory == iid)
		*ppv = (LPVOID)(IClassFactory*)this;
	else
		return E_NOINTERFACE;  //  Interface not supported
	//Perform reference count through the returned interface
	((IUnknown*)*ppv)->AddRef();
	return NOERROR;
}//  QueryInterface

//
//AddRef
//
STDMETHODIMP_(ULONG) CUserInfoFactory::AddRef(void)
{
	return ++m_cRef;
}//  AddRef

//
//Release
//
STDMETHODIMP_(ULONG) CUserInfoFactory::Release(void)
{
	m_cRef--;
	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}
	return m_cRef;
}//  Release