// dllmain.cpp : Implementation of DllMain.
// 

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "UserInfo.h"  //  Goes without saying
#include "UserInfo_i.h"
//  removed dllmain.h include

// Here's your stuff

#define MAX_STRING_LENGTH 255
#define GUID_SIZE 128

#include<objbase.h>
#include<olectl.h>   //  for DLLRegisterServer and DLLUnregisterServer
#include<tchar.h>

//
//Forward declarations
//
BOOL SetRegKeyValue(LPTSTR lpszKey, LPTSTR lpszSubKey, LPTSTR lpszValue);
BOOL ServerCanUnloadNow(void);
void UnloadServer(void);
//
//Global variables
//
HMODULE g_hModule = NULL;
ULONG g_cObjects = 0;
ULONG g_cLocks = 0;

//
//DllRegisterServer
//
STDAPI DllRegisterServer(void)
{
	BOOL bOK;
	_TCHAR szModulePath[MAX_PATH + 1];
	_TCHAR szCLSID[GUID_SIZE + 1];
	_TCHAR szCLSIDKey[MAX_STRING_LENGTH + 1];
	wchar_t wszGUID[GUID_SIZE + 1];

	//Obtain the path to server's executable file for later use
	GetModuleFileName(g_hModule, szModulePath, sizeof(szModulePath) / sizeof(_TCHAR));
	//Convert the CLSID to the format {00000000-0000-0000-0000-000000000000}
	StringFromGUID2(CLSID_UserInfo, wszGUID, sizeof(wszGUID) / sizeof(wchar_t));
	#ifdef _UNICODE
		//UNICODE
		_tcscpy(szCLSID, wszGUID);
	#else
		//SBCS and MBCS
		//Convert from the wide character set to the multibyte character set
		WideCharToMultiByte(CP_ACP, 0, wszGUID, -1, szCLSID, sizeof(szCLSID) / sizeof(_TCHAR), NULL, NULL);
	#endif
	//HKEY_CLASSES_ROOT\CLSID\{00000000-0000-0000-0000-000000000000}
	_tcscpy(szCLSIDKey, _TEXT("CLSID\\"));
	_tcscat(szCLSIDKey, szCLSID);
	bOK = SetRegKeyValue(szCLSIDKey, NULL, _TEXT("DCOM Enterprise Apps - UserInfo Object."));
	if (bOK)
		bOK = SetRegKeyValue(szCLSIDKey, _TEXT("InProcServer32"), szModulePath);
	if (bOK)
		return NOERROR;
	else
		return SELFREG_E_CLASS;
}  //  DLLRegisterServer

//
//DllUnregisterServer
//
STDAPI DllUnregisterServer(void)
{
	long lErrorCode;
	_TCHAR szCLSID[GUID_SIZE + 1];
	_TCHAR szCLSIDKey[MAX_STRING_LENGTH + 1];
	_TCHAR szInProcServer32Key[MAX_STRING_LENGTH + 1];
	wchar_t wszGUID[GUID_SIZE + 1];

	//Convert the CLSID to the format
	//{00000000-0000-0000-0000-000000000000}
	StringFromGUID2(CLSID_UserInfo, wszGUID, sizeof(wszGUID) / sizeof(wchar_t));
	#ifdef _UNICODE
		//UNICODE
		_tcscpy(szCLSID, wszGUID);
	#else
		//SBCS and MBCS
		//Convert from the wide character set to the multibyte character set
		WideCharToMultiByte(CP_ACP, 0, wszGUID, -1, szCLSID, sizeof(szCLSID) / sizeof(_TCHAR), NULL, NULL);
	#endif
	//HKEY_CLASSES_ROOT\CLSID\{00000000-0000-0000-0000-000000000000}
	_tcscpy(szCLSIDKey, _TEXT("CLSID\\"));
	_tcscat(szCLSIDKey, szCLSID);
	_tcscpy(szInProcServer32Key, szCLSIDKey);
	_tcscat(szInProcServer32Key, _TEXT("\\InProcServer32"));
	
	//Delete sub-keys first
	lErrorCode = RegDeleteKey(HKEY_CLASSES_ROOT, szInProcServer32Key);

	//Delete the entry under CLSID
	if (ERROR_SUCCESS == lErrorCode)
		lErrorCode = RegDeleteKey(HKEY_CLASSES_ROOT, szCLSIDKey);

	if (ERROR_SUCCESS == lErrorCode)
		return NOERROR;
	else
		return SELFREG_E_CLASS;
}  //  DllUnregisterServer

//
//DllGetClassObject
//
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	CUserInfoFactory* pCUserInfoFactory = NULL;
	HRESULT hr = NOERROR;

	if (CLSID_UserInfo == rclsid)
	{
		//Create the UserInfo classFactory
		pCUserInfoFactory = new CUserInfoFactory();
		//Check for out of memory error
		if (NULL == pCUserInfoFactory)
			return E_OUTOFMEMORY;
		//Get the requested interface
		hr = pCUserInfoFactory->QueryInterface(riid, ppv);
		if (FAILED(hr))
		{
			delete pCUserInfoFactory;
			pCUserInfoFactory = NULL;
			return hr;
		}
	}
	else
		hr = CLASS_E_CLASSNOTAVAILABLE;
	return hr;
} //  DllGetClassObject

//
//DllCanUnloadNow
//
STDAPI DllCanUnloadNow(void)
{
	if (ServerCanUnloadNow())
		return S_OK;
	else
		return S_FALSE;
}  //  DllCanUnloadNow

//
//SetRegKeyValue
//
BOOL SetRegKeyValue(LPTSTR lpszKey, LPTSTR lpszSubKey, LPTSTR lpszValue)
{
	BOOL bOk = FALSE;
	long lErrorCode;
	HKEY hKey;
	_TCHAR szKey[MAX_STRING_LENGTH + 1];

	_tcscpy(szKey, lpszKey);
	if (NULL != lpszSubKey)
	{
		_tcscat(szKey, _TEXT("\\"));
		_tcscat(szKey, lpszSubKey);
	}
	lErrorCode = RegCreateKeyEx(HKEY_CLASSES_ROOT, szKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (ERROR_SUCCESS == lErrorCode)
	{
		//lErrorCode = RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)lpszValue, sizeof(lpszValue) / sizeof(_TCHAR));  //  Wrong size calculation causing InProcServer value to be just "C:" and no description for COM class
		lErrorCode = RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)lpszValue, (_tcslen(lpszValue) + 1) * sizeof(_TCHAR));  //  This works fine
		if (ERROR_SUCCESS == lErrorCode)
			bOk = TRUE;
		RegCloseKey(hKey);
	}
	return bOk;
}  //  SetRegKeyValue

//
//ServerCanUnloadNow
//
BOOL ServerCanUnloadNow(void)
{
	//  The server can unload if there are no outstanding objects or class factory locks
	if (0 == g_cObjects && 0 == g_cLocks)
		return TRUE;
	else
		return FALSE;
}  //  ServerCanUnloadNow

//
//UnloadServer
//
void UnloadServer(void)
{
	//  Since DLLs aren't responsible for unloading themselves, simply return
	return;
}  //  UnloadServer


// DLL Entry Point
//STDAPI macro cannot be used because it contains HRESULT, which is returned by a COM interface and not a DLL's entry point.
//APIENTRY is really just WINAPI which expands to __stdcall calling convention
BOOL APIENTRY WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)  //  Check if this works with the rest of the implementation
{
	//  Save the dll module handle for later use
	if (DLL_PROCESS_ATTACH == dwReason)
		g_hModule = hModule;
	return TRUE;  //  If you don't add this line, regsvr32.exe will fail
}

//  A lot of guides will tell you that you need to add the DLL to a certain directory.
//  Not necessarily. Just use administrator cmd.exe and in it, use regsvr32.exe and pass it the full path to the DLL