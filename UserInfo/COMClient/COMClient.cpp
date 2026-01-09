#include<Windows.h>
#include<objbase.h>
#include<tchar.h>
#include<UserInfo_i.h>

//
//Forward declarations
//
void DisplayMessage(LPTSTR lpMessage);
void DisplayUserInfo(IUserInfo* pIUserInfo, LPTSTR lpszRetrievedMsg);

//
//Global variables
//
const _TCHAR g_lpszApplicationTitle[] = _TEXT("UserInfoClient");

//
//DisplayMessage
//
void DisplayMessage(LPTSTR lpszMessage)
{
	MessageBox(NULL, lpszMessage, g_lpszApplicationTitle, MB_OK | MB_ICONEXCLAMATION);
}//  DisplayMessage

//
//DisplayUserInfo
//
void DisplayUserInfo(IUserInfo* pIUserInfo, LPTSTR lpszRetrievedMsg)
{
	char szAge[25];
	LPSTR lpszName = NULL;
	short nAge;
	unsigned char bySex;
	char szDisplayText[255];
	_TCHAR szMsgText[255];
	long lStringLen;

	//  Retrieve each property
	pIUserInfo->get_Name(&lpszName);
	pIUserInfo->get_Age(&nAge);
	pIUserInfo->get_Sex(&bySex);

	DisplayMessage(lpszRetrievedMsg);

	//Format the name
	strcpy(szDisplayText, "Name: ");
	if (lpszName)
		strcat(szDisplayText, lpszName);
	lStringLen = strlen(szDisplayText);
		//Add a carriage return
	lStringLen = strlen(szDisplayText);
	szDisplayText[lStringLen] = '\r';
	//Null terminate the string
	szDisplayText[lStringLen + 1] = '\0';

	//Format the Sex
	strcat(szDisplayText, "Sex: ");
	lStringLen = strlen(szDisplayText);
	szDisplayText[lStringLen] = bySex;
	//Null terminate the string
	szDisplayText[lStringLen + 1] = '\0';

#ifdef _UNICODE
	// UNICODE
	// Convert from the Multibyte character set to the wide character set
	mbstowcs(szMsgText, szDisplayText, sizeof(szMsgText) / sizeof(_TCHAR));
#else
	// SBCS and MBCS
	_tcscpy(szMsgText, szDisplayText);
#endif
	DisplayMessage(szMsgText);
}  //  DisplayUserInfo

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HRESULT hr;
	IUnknown* pIUnknown = NULL;
	IUserInfo* pIUserInfo = NULL;
	
	//  Initialize the COM library
	hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		DisplayMessage(LPTSTR("The COM library has been initialized."));
		//  Ask the COM library to instantiate the UserInfo object
		//  and return us an initial pointer to IUnknown
		hr = CoCreateInstance(CLSID_UserInfo, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&pIUnknown);  //  You can, absolutely, mention the GUID and not rely on UserInfo_i.c and UserInfo_i.h
		if (SUCCEEDED(hr))
		{
			DisplayMessage(LPTSTR("The UserInfo object has been created."));
			//  Begin using the object

			//QueryInterface for the the IUserInfo interface
			hr = pIUnknown->QueryInterface(IID_IUserInfo, (LPVOID*)&pIUserInfo);

			//  QueryInterface for the IUserInfo interface

			if (SUCCEEDED(hr))
			{
				DisplayMessage(LPTSTR("Changed to the IUserInfo interface."));

				// Set each property
				pIUserInfo->put_Name(LPSTR("laughtersec"));
				pIUserInfo->put_Age(25);
				pIUserInfo->put_Sex(BYTE("M"));

				DisplayMessage(LPTSTR("Each UserInfo property as been set."));
				DisplayUserInfo(pIUserInfo, LPTSTR("Each UserInfo property has been retrieved."));
				//  Release the IUserInfo interface
				pIUserInfo->Release();
				DisplayMessage(LPTSTR("Released the IUserInfo interface."));
			}
			else
				DisplayMessage(LPTSTR("Couldn't change to the IUserInfo interface."));

			//Release the IUnknown interface
			pIUnknown->Release();
			DisplayMessage(LPTSTR("Released the IUnknown interface."));

		}
		else
			DisplayMessage(LPTSTR("The UserInfo object could not be created."));

		//Shut down the COM library
		CoUninitialize();
		DisplayMessage(LPTSTR("Shut down the COM library."));
	}
	else
		DisplayMessage(LPTSTR("The COM library initialization failed."));

	DisplayMessage(LPTSTR("Terminating the application"));
	//Terminate the application
	return FALSE;
}//  WinMain