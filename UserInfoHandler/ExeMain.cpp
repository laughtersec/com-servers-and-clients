//
//ExeMain.cpp
//

#include "pch.h"
#include <objbase.h>
#include <tchar.h>
#include "UserInfo.h"
#include "UserInfoHandler.h"

#define MAX_STRING_LENGTH 255
#define GUID_SIZE 128

//
//Forward declarations
//
BOOL RegisterServer(CLSID clsid, LPTSTR lpszDescription);
BOOL UnregisterServer(CLSID clsid);
BOOL SetRegKeyValue(LPTSTR lpszKey, LPTSTR lpszSubKey, LPTSTR lpszValue);
BOOL ServerCanUnloadNow(void);
void UnloadServer(void);

//
//Global variables
//
HMODULE g_hModule = NULL;
ULONG g_cObjects = 0;
ULONG g_cLocks = 0;
DWORD g_dwRegisterUserInfo;
DWORD g_dwRegisterUserInfoHandler;

//
//Register the server
//
BOOL RegisterServer(CLSID clsid, LPTSTR lpszDescription)
{
	BOOL bOK;
	_TCHAR szModulePath[MAX_PATH + 1];
	_TCHAR szCLSID[GUID_SIZE + 1];
	_TCHAR szCLSIDKey[MAX_STRING_LENGTH + 1];
	wchar_t wszGUID[GUID_SIZE + 1];

	//Obtain the path to server's executable file for later use
	GetModuleFileName(g_hModule, szModulePath, sizeof(szModulePath) / sizeof(_TCHAR));
	//Convert the CLSID to the format
	//{00000000-0000-0000-0000-000000000000}
	StringFromGUID2(clsid, wszGUID, sizeof(wszGUID) / sizeof(wchar_t));
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
		bOK = SetRegKeyValue(szCLSIDKey, NULL, lpszDescription);  //  Is this ok?
		if (bOK)
			bOK = SetRegKeyValue(szCLSIDKey, _TEXT("LocalServer32"), szModulePath);
	return bOK;
}//RegisterServer

//
//Unregister the server
//
BOOL UnregisterServer(CLSID clsid)
{
	long lErrorCode;
	_TCHAR szCLSID[GUID_SIZE + 1];
	_TCHAR szCLSIDKey[MAX_STRING_LENGTH + 1];
	_TCHAR szLocalServer32Key[MAX_STRING_LENGTH + 1];
	wchar_t wszGUID[GUID_SIZE + 1];
	//Convert the CLSID to the format
	//{00000000-0000-0000-0000-000000000000}
	StringFromGUID2(clsid, wszGUID, GUID_SIZE);
#ifdef _UNICODE
	//UNICODE
	_tcscpy(szCLSID, wszGUID);
#else
	//SBCS and MBCS
	//Convert from the wide character set to the multibyte character set
	WideCharToMultiByte(CP_ACP, 0, wszGUID, -1, szCLSID, sizeof(szCLSID) / sizeof(_TCHAR), NULL, NULL);
#endif // _UNICODE
	//HKEY_CLASSES_ROOT\CLSID\{00000000-0000-0000-0000-000000000000}
	_tcscpy(szCLSIDKey, _TEXT("CLSID\\"));
	_tcscat(szCLSIDKey, szCLSID);
	_tcscpy(szLocalServer32Key, szCLSIDKey);
	_tcscat(szLocalServer32Key, _TEXT("\\LocalServer32"));
	//Delete sub-keys first
	lErrorCode = RegDeleteKey(HKEY_CLASSES_ROOT, szLocalServer32Key);
	//Delete the entry under CLSID
	if (ERROR_SUCCESS == lErrorCode)
		lErrorCode = RegDeleteKey(HKEY_CLASSES_ROOT, szCLSIDKey);
	if (ERROR_SUCCESS == lErrorCode)
		return TRUE;
	else
		return FALSE;
}//UnregisterServer

//
//SetRegKeyValue
//
BOOL SetRegKeyValue(LPTSTR lpszKey, LPTSTR lpszSubKey, LPTSTR lpszValue)
{
	BOOL bOK = FALSE;
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
			bOK = TRUE;
		RegCloseKey(hKey);
	}
	return bOK;
}//  SetRegKeyValue

//
//ServerCanUnloadNow
//
BOOL ServerCanUnloadNow(void)
{
	//The server can unload if there are no outstanding objects or class factory locks
	if (0 == g_cObjects && 0 == g_cLocks)
		return TRUE;
	else
		return FALSE;
}

//
//UnloadServer
//
void UnloadServer(void)
{
	//Unload the server by posting the WM_QUIT to the message queue
	PostQuitMessage(0);
}//UnloadServer

//
//WinMain
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HRESULT hr;
	MSG msg;
	_TCHAR szTokens[] = _TEXT("-/");
	LPTSTR szNextToken;
	LPTSTR szCmdLine;
	CUserInfoFactory* pCUserInfoFactory = NULL;
	CUserInfoHandlerFactory* pCUserInfoHandlerFactory = NULL;

	g_hModule = GetModuleHandle(NULL);
	//  Read the command line.
#ifdef _UNICODE
	//UNICODE
	szCmdLine = GetCommandLine();
#else // _UNICODE
	//SBCS and MBCS
	szCmdLine = lpCmdLine;
#endif
	//Find the first token
	szNextToken = _tcstok(szCmdLine, szTokens);
	while (NULL != szNextToken)
	{
		if (0 == _tcsicmp(szNextToken, _TEXT("UnregServer")))
		{
			::UnregisterServer(CLSID_UserInfoHandler);
			::UnregisterServer(CLSID_UserInfo);
			return FALSE;
		}
		else if (0 == _tcsicmp(szNextToken, _TEXT("RegServer")))  //  
		{
			::RegisterServer(CLSID_UserInfoHandler, _TEXT("DCOM Enterprise Apps - UserInfoHandler Object."));  //  Is this ok?
			::RegisterServer(CLSID_UserInfo, _TEXT("DCOM Enterprise Apps - UserInfo Object."));
			return FALSE;
		}
		else if (0 == _tcsicmp(szNextToken, _TEXT("Embedding")))
			break;
		//Find the next token
		szNextToken = _tcstok(NULL, szTokens);
	}
	//Initialize the COM Library
	hr = CoInitialize(NULL);
	if (FAILED(hr))
		return FALSE;
	//Create the UserInfo class factory
	pCUserInfoFactory = new CUserInfoFactory();
	//Check for out of memory error
	if (NULL != pCUserInfoFactory)
	{
		hr = CoRegisterClassObject(CLSID_UserInfo, (IUnknown*)pCUserInfoFactory, CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &g_dwRegisterUserInfo);
		if (FAILED(hr))
		{
			delete pCUserInfoFactory;
			pCUserInfoFactory = NULL;
		}
		else
		{
			//Create the UserInfoHandler class factory
			pCUserInfoHandlerFactory = new CUserInfoHandlerFactory();
			//Check for out of memory error
			if (NULL != pCUserInfoHandlerFactory)
			{
				//Register the UserInfoHandler class factory
				hr = CoRegisterClassObject(CLSID_UserInfoHandler, (IUnknown*)pCUserInfoHandlerFactory, CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &g_dwRegisterUserInfoHandler);
				if (FAILED(hr))
				{
					delete pCUserInfoHandlerFactory;
					pCUserInfoHandlerFactory = NULL;
				}
				else
				{
					while (GetMessage(&msg, NULL, 0, 0))
						DispatchMessage(&msg);
					//Unregister the UserInfoHandler
					//class factory
					CoRevokeClassObject(g_dwRegisterUserInfoHandler);
				}
			}
			//Unregister the UserInfo class factory
			CoRevokeClassObject(g_dwRegisterUserInfo);
		}
	}
	CoUninitialize();
	return (msg.wParam);
}//WinMain