#ifndef USERINFOHANDLER_H
#define USERINFOHANDLER

#include "UserInfoHandler_i.h"

//
//UserInfoHandler.h
//
class CUserInfoHandler : public ICopyInfo, public IReverseInfo, public ISwapInfo
{
	private:
		ULONG	m_cRef;
	public:
		//IUnknown
		STDMETHODIMP QueryInterface(REFIID iid, LPVOID* ppv);
		STDMETHODIMP_(ULONG)AddRef(void);
		STDMETHODIMP_(ULONG)Release(void);
		//ICopyInfo
		STDMETHODIMP CopyAge(IUserInfo* lpDest, IUserInfo* lpSrc);
		STDMETHODIMP CopyName(IUserInfo* lpDest, IUserInfo* lpSrc);
		STDMETHODIMP CopySex(IUserInfo* lpDest, IUserInfo* lpSrc);
		STDMETHODIMP CopyAll(IUserInfo* lpDest, IUserInfo* lpSrc);
		//IReverseInfo
		STDMETHODIMP ReverseAge(IUserInfo* lpIUserInfo);
		STDMETHODIMP ReverseName(IUserInfo* lpIUserInfo);
		STDMETHODIMP ReverseSex(IUserInfo* lpIUserInfo);
		STDMETHODIMP ReverseAll(IUserInfo* lpIUserInfo);
		//ISwapInfo
		STDMETHODIMP SwapAge(IUserInfo* lpIUserInfo1, IUserInfo* lpIUserInfo2);
		STDMETHODIMP SwapName(IUserInfo* lpIUserInfo1, IUserInfo* lpIUserInfo2);
		STDMETHODIMP SwapSex(IUserInfo* lpIUserInfo1, IUserInfo* lpIUserInfo2);
		STDMETHODIMP SwapAll(IUserInfo* lpIUserInfo1, IUserInfo* lpIUserInfo2);
		//Constructor
		CUserInfoHandler() 
		{
			m_cRef = 0;
		}
};  //  CUserInfoHandler

class CUserInfoHandlerFactory : public IClassFactory
{
	private:
		ULONG	m_cRef;
	public:
		//  IUnknown
		STDMETHODIMP QueryInterface(REFIID iid, LPVOID* ppv);
		STDMETHODIMP_(ULONG)AddRef(void);
		STDMETHODIMP_(ULONG)Release(void);
		//  IClassFactory
		STDMETHODIMP LockServer(BOOL block);
		STDMETHODIMP CreateInstance(IUnknown* pUnknownOuter, REFIID iid, LPVOID* ppv);
		//  Constructor
		CUserInfoHandlerFactory()
		{
			m_cRef = 0;
		}
};  //  CUserInfoHandlerFactory

#endif