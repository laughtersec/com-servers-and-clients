//
//UserInfo.h
//
#ifndef USERINFO_H
#define USERINFO_H

#include "UserInfo_i.h"

//
//CUserInfo object
//
class CUserInfo : IUserInfo
{
	private:
		ULONG	m_cRef;
		short	m_nAge;
		LPSTR	m_lpszName;
		BYTE	m_bySex;
	public:
		//  IUnknown
		STDMETHODIMP QueryInterface(REFIID iid, LPVOID* ppv);
		STDMETHODIMP_(ULONG)AddRef(void);
		STDMETHODIMP_(ULONG)Release(void);
		//  IUserInfo
		STDMETHODIMP get_Age(short* nRetAge);
		STDMETHODIMP put_Age(short nAge);
		STDMETHODIMP get_Name(LPSTR* lpszRetName);
		STDMETHODIMP put_Name(LPSTR lpszName);
		STDMETHODIMP get_Sex(BYTE* byRetSex);
		STDMETHODIMP put_Sex(BYTE bySex);
		//  Constructor
		CUserInfo();
		//  Destructor
		~CUserInfo();
};

class CUserInfoFactory : public IClassFactory
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
		CUserInfoFactory()
		{
			m_cRef = 0;
		}
};  //  CUserInfoFactory
#endif