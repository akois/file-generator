#include "stdafx.h"
#include "md5.h"
#include "FileKey.h"


bool GetFileKeyInfo64(LPCWSTR szFilePath, std::wstring& szFileHash, LARGE_INTEGER& liFileSize)
{
	PVOID pOldValue = NULL;
	BOOL bFileredirection = FALSE;
	liFileSize.QuadPart = 0;

	if (szFilePath == NULL)
	{
		return false;
	}
	// We are making sure we call the function for disable file system redirection on 64 bit OS. 
	UINT uiReturn = true;
	BYTE *pFile = NULL;
	HANDLE hFile = NULL, hMapFile = NULL;
	DWORD dwShare = 0;

	try
	{
		dwShare |= FILE_SHARE_READ;
		if ((hFile = ::CreateFile(szFilePath, GENERIC_READ, dwShare, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		liFileSize.QuadPart = 0;
		if (::GetFileSizeEx(hFile, &liFileSize) == FALSE)
		{
			return false;
		}

		// MapFileFromHandle...
		if ((hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL)) == NULL)
		{
			return false;
		}

		const DWORD dw4MB = 4 * 1024 * 1024; // 4MB
		// map file by 4MB blocks and caluclate hash

		MD5 md5;


		pFile = (LPBYTE)::MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, liFileSize.QuadPart);
		if (pFile != NULL)
		{
			md5.Update(pFile, liFileSize.QuadPart);
			::UnmapViewOfFile(pFile);
		}
		else
			return false;
		

		md5.Final();
		szFileHash = md5.digestChars;

	}
	catch (HRESULT hrCatch)
	{
		/*if (hrCatch == POSSIBLE_FILE_NOT_FOUND)
		{
		::CEReportEventFmt(CE_SEVERITY_INFORMATION, L"SMFUtil", L"SMFUtil::GetFileKeyInfo() File not found or file locked, scanning file: %ls", szFilePath);
		}
		else
		{
		::CEReportEventFmt(CE_SEVERITY_INFORMATION, L"SMFUtil", L"SMFUtil::GetFileKeyInfo() Error occurred during file scan: %ls. HRESULT: (%d) ", szFilePath, hrCatch);
		}*/
		uiReturn = false;
	}
	catch (...)
	{
		//	just absorb exceptions, most/all of these cases will be caused by corrupted files
		uiReturn = false;
	}

	if (hMapFile != NULL && hMapFile != INVALID_HANDLE_VALUE)
		::CloseHandle(hMapFile);
	if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
		::CloseHandle(hFile);

	return uiReturn;
}


bool GetFileKeyInfo32(LPCWSTR szFilePath, std::wstring& szFileHash, LARGE_INTEGER& liFileSize)
{
	PVOID pOldValue = NULL;
	BOOL bFileredirection = FALSE;
	liFileSize.QuadPart = 0;

	if (szFilePath == NULL)
	{
		return false;
	}
	// We are making sure we call the function for disable file system redirection on 64 bit OS. 
	UINT uiReturn = true;
	BYTE *pFile = NULL;
	HANDLE hFile = NULL, hMapFile = NULL;
	DWORD dwShare = 0;

	try
	{
		dwShare |= FILE_SHARE_READ;
		if ((hFile = ::CreateFile(szFilePath, GENERIC_READ, dwShare, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		liFileSize.QuadPart = 0;
		if (::GetFileSizeEx(hFile, &liFileSize) == FALSE)
		{
			return false;
		}

		// MapFileFromHandle...
		if ((hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL)) == NULL)
		{
			return false;
		}

		const DWORD dw4MB = 4 * 1024 * 1024; // 4MB
		// map file by 4MB blocks and caluclate hash

		MD5 md5;


		LARGE_INTEGER offset;
		DWORD dwBlockSize = 0;
		for (offset.QuadPart = 0; offset.QuadPart<liFileSize.QuadPart && uiReturn == true; offset.QuadPart += dwBlockSize)
		{
			dwBlockSize = (liFileSize.QuadPart - offset.QuadPart)>dw4MB ? dw4MB : DWORD(liFileSize.QuadPart - offset.QuadPart);

			pFile = (LPBYTE)::MapViewOfFile(hMapFile, FILE_MAP_READ, offset.HighPart, offset.LowPart, dwBlockSize);
			if (pFile != NULL)
			{
				md5.Update(pFile, dwBlockSize);
				::UnmapViewOfFile(pFile);
			}
			else
				return false;
		}

		md5.Final();
		szFileHash = md5.digestChars;

	}
	catch (HRESULT hrCatch)
	{
		/*if (hrCatch == POSSIBLE_FILE_NOT_FOUND)
		{
		::CEReportEventFmt(CE_SEVERITY_INFORMATION, L"SMFUtil", L"SMFUtil::GetFileKeyInfo() File not found or file locked, scanning file: %ls", szFilePath);
		}
		else
		{
		::CEReportEventFmt(CE_SEVERITY_INFORMATION, L"SMFUtil", L"SMFUtil::GetFileKeyInfo() Error occurred during file scan: %ls. HRESULT: (%d) ", szFilePath, hrCatch);
		}*/
		uiReturn = false;
	}
	catch (...)
	{
		//	just absorb exceptions, most/all of these cases will be caused by corrupted files
		uiReturn = false;
	}

	if (hMapFile != NULL && hMapFile != INVALID_HANDLE_VALUE)
		::CloseHandle(hMapFile);
	if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
		::CloseHandle(hFile);

	return uiReturn;
}

