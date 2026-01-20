// UserInfoHandler.cpp : Implementation of WinMain

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "UserInfo.h"
#include "UserInfoHandler.h"

extern BOOL ServerCanUnloadNow(void);
extern void UnloadServer(void);

extern ULONG g_cObjects;
extern ULONG g_cLocks;

STDMETHODIMP CUserInfo::get_Age(short* nRetAge)
{
	*nRetAge = m_nAge;
	return NOERROR;
}  //  get_Age
STDMETHODIMP CUserInfo::put_Age(short nAge)
{
	m_nAge = nAge;
	return NOERROR;
}  //  put_Age
STDMETHODIMP CUserInfo::get_Name(LPSTR* lpszRetName)
{
	*lpszRetName = m_lpszName;
	return NOERROR;
}  //  get_Name
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
STDMETHODIMP CUserInfo::get_Sex(BYTE* byRetSex)
{
	*byRetSex = m_bySex;
	return NOERROR;
}  //  get_Sex
STDMETHODIMP CUserInfo::put_Sex(BYTE bySex)
{
	m_bySex = bySex;
	return NOERROR;
}  //  put_Sex
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
STDMETHODIMP_(ULONG)CUserInfo::AddRef(void)
{
	return ++m_cRef;
}  //  AddRef
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

CUserInfo::CUserInfo()
{
	m_cRef = 0;
	m_nAge = 0;
	m_lpszName = NULL;
	m_bySex = 'M';
}  //  CUserInfo
CUserInfo::~CUserInfo()
{
	if (m_lpszName)
		delete[] m_lpszName;
}  //  ~CUserInfo

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
STDMETHODIMP_(ULONG) CUserInfoFactory::AddRef(void)
{
	return ++m_cRef;
}//  AddRef
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

STDMETHODIMP CUserInfoHandler::CopyName(IUserInfo* lpDest, IUserInfo* lpSrc)
{
	HRESULT hr;
	LPSTR nTmpName;

	//Retrieve information from the source
	hr = lpSrc->get_Name(&nTmpName);
	if (SUCCEEDED(hr))
		hr = lpDest->put_Name(nTmpName);
	return hr;
}//CopyName
STDMETHODIMP CUserInfoHandler::CopyAge(IUserInfo* lpDest, IUserInfo* lpSrc)
{
	HRESULT hr;
	SHORT nTmpAge;

	//Retrieve information from the source
	hr = lpSrc->get_Age(&nTmpAge);
	if (SUCCEEDED(hr))
		hr = lpDest->put_Age(nTmpAge);  //  Apply it to the destination
	return hr;
}//CopyAge
STDMETHODIMP CUserInfoHandler::CopySex(IUserInfo* lpDest, IUserInfo* lpSrc)
{
	HRESULT hr;
	PUCHAR nTmpSex = NULL;

	//Retrieve information from the source
	hr = lpSrc->get_Sex(nTmpSex);
	if (SUCCEEDED(hr))
		hr = lpDest->put_Sex(*nTmpSex);
	return hr;
}
STDMETHODIMP CUserInfoHandler::CopyAll(IUserInfo* lpDest, IUserInfo* lpSrc)
{
	//  Not sure if this is a good way, there is no error handling.
	lpDest = lpSrc;
	return S_OK;
}
STDMETHODIMP CUserInfoHandler::ReverseName(IUserInfo* pIUserInfo) { return S_OK; }
STDMETHODIMP CUserInfoHandler::ReverseAge(IUserInfo* pIUserInfo) { return S_OK; }
STDMETHODIMP CUserInfoHandler::ReverseSex(IUserInfo* pIUserInfo) { return S_OK; }
STDMETHODIMP CUserInfoHandler::ReverseAll(IUserInfo* pIUserInfo) { return S_OK; }
STDMETHODIMP CUserInfoHandler::SwapName(IUserInfo* lpIUserInfo1, IUserInfo* lpIUserInfo2)
{
	HRESULT hr1, hr2;
	LPSTR nTmpName1, nTmpName2;

	//Retrieve information from the source
	hr1 = lpIUserInfo1->get_Name(&nTmpName1);
	hr2 = lpIUserInfo2->get_Name(&nTmpName2);
	if (SUCCEEDED(hr1) && SUCCEEDED(hr2))
	{
		hr1 = lpIUserInfo2->put_Name(nTmpName1);
		hr2 = lpIUserInfo1->put_Name(nTmpName2);
	}
	return S_OK; 
}
STDMETHODIMP CUserInfoHandler::SwapAge(IUserInfo* lpIUserInfo1, IUserInfo* lpIUserInfo2)
{ 
	HRESULT hr1, hr2;
	SHORT nTmpAge1, nTmpAge2;

	//Retrieve information from the source
	hr1 = lpIUserInfo1->get_Age(&nTmpAge1);
	hr2 = lpIUserInfo2->get_Age(&nTmpAge2);
	if (SUCCEEDED(hr1) && SUCCEEDED(hr2))
	{
		hr1 = lpIUserInfo2->put_Age(nTmpAge1);
		hr2 = lpIUserInfo1->put_Age(nTmpAge2);
	}
	return S_OK;
}
STDMETHODIMP CUserInfoHandler::SwapSex(IUserInfo* lpIUserInfo1, IUserInfo* lpIUserInfo2) 
{
	HRESULT hr1, hr2;
	PUCHAR nTmpSex1 = NULL, nTmpSex2 = NULL;

	//Retrieve information from the source
	hr1 = lpIUserInfo1->get_Sex(nTmpSex1);
	hr2 = lpIUserInfo2->get_Sex(nTmpSex2);
	if (SUCCEEDED(hr1) && SUCCEEDED(hr2))
	{
		hr1 = lpIUserInfo1->put_Sex(*nTmpSex2);
		hr2 = lpIUserInfo2->put_Sex(*nTmpSex1);
	}
	return S_OK; 
}
STDMETHODIMP CUserInfoHandler::SwapAll(IUserInfo* lpIUserInfo1, IUserInfo* lpIUserInfo2) 
{ return S_OK; }
STDMETHODIMP CUserInfoHandler::QueryInterface(REFIID iid, LPVOID* ppv)
{
	*ppv = NULL;
	if (IID_IUnknown == iid)
		*ppv = (LPVOID)(IUnknown*)(ICopyInfo*)this;  //  ICopyInfo, because its the default in IDL
	else if (IID_ICopyInfo == iid)
		*ppv = (LPVOID)(ICopyInfo*)this;
	else if (IID_IReverseInfo == iid)
		*ppv = (LPVOID)(IReverseInfo*)this;
	else if (IID_ISwapInfo == iid)
		*ppv = (LPVOID)(ISwapInfo*)this;
	else
		return E_NOINTERFACE;  //  Interface not supported
	//Perform reference count through the returned interface
	((IUnknown*)*ppv)->AddRef();
	return NOERROR;
}//QueryInterface
STDMETHODIMP_(ULONG)CUserInfoHandler::AddRef(void)
{
	return ++m_cRef;
}//AddRef
STDMETHODIMP_(ULONG)CUserInfoHandler::Release(void)
{
	m_cRef--;
	if (0 == m_cRef)
	{
		delete this;
		g_cObjects--;
		if (::ServerCanUnloadNow())
			::UnloadServer();
		return 0;
	}
	return m_cRef;
}//Release

CUserInfoHandler::CUserInfoHandler()
{

}
CUserInfoHandler::~CUserInfoHandler()
{

}

STDMETHODIMP CUserInfoHandlerFactory::LockServer(BOOL bLock)
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
STDMETHODIMP CUserInfoHandlerFactory::QueryInterface(REFIID iid, LPVOID* ppv)
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
STDMETHODIMP_(ULONG) CUserInfoHandlerFactory::AddRef(void)
{
	return ++m_cRef;
}//  AddRef
STDMETHODIMP_(ULONG) CUserInfoHandlerFactory::Release(void)
{
	m_cRef--;
	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}
	return m_cRef;
}//  Release
STDMETHODIMP CUserInfoHandlerFactory::CreateInstance(IUnknown* pUnknownOuter, REFIID iid, LPVOID* ppv)
{
	HRESULT hr;
	CUserInfoHandler* pCUserInfoHandler = NULL;

	*ppv = NULL;
	//This object doesn't support aggregation
	if (NULL != pUnknownOuter)
		return CLASS_E_NOAGGREGATION;
	//Create the CUserInfo object
	pCUserInfoHandler = new CUserInfoHandler();
	if (NULL == pCUserInfoHandler)
		return E_OUTOFMEMORY;
	//Retrieve the requested interface
	hr = pCUserInfoHandler->QueryInterface(iid, ppv);
	if (FAILED(hr))
	{
		delete pCUserInfoHandler;
		pCUserInfoHandler = NULL;
		return hr;
	}
	//Increment the global object counter
	g_cObjects++;

	return NOERROR;
}//CreateInstance