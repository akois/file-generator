#include "stdafx.h"
#include "DirStream2.h"

using namespace std;

DirectoryEntry::DirectoryEntry(const wstring &strSeedDir, DirectoryStream2* pParentStream) :
	FileEntry(strSeedDir),
	m_bInitialized(false),
	//m_fileCollection(this),
	//m_directoryCollection(this),
	m_pParentStream(pParentStream)
{
}


bool DirectoryEntry::Initialize()
{
	return m_bInitialized || Populate();
}


/*const DirectoryStream2::FileCollection &GetFileList()
{
}

const DirectoryStream2::DirectoryCollection &GetDirList()
{
}
*/

bool DirectoryEntry::Populate()
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	std::wstring strCurDir = m_strName;
	strCurDir += L"\\*";

	hFind = FindFirstFile((strCurDir + L"*").c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FindFileData.dwFileAttributes &  FILE_ATTRIBUTE_DIRECTORY)
			{
				if (!(wcscmp(FindFileData.cFileName, L"..") == 0 ||
					wcscmp(FindFileData.cFileName, L".") == 0))
				{
					m_directoryCollection.emplace_back(std::make_shared<DirectoryEntry>(strCurDir + L"\\" + FindFileData.cFileName, m_pParentStream));
					//m_pParentStream->AddSeed(m_directoryCollection.back());
				}
			}
			else
			{
				m_fileCollection.emplace_back(strCurDir + L"\\" + FindFileData.cFileName);
			}
		} while (FindNextFile(hFind, &FindFileData));

		FindClose(hFind);
	}
	m_bInitialized = true;
	return true;
}



DirectoryStream2::DirectoryStream2(const std::wstring &strSeedDir)
{
	AddSeed(make_shared<DirectoryEntry>(strSeedDir, this));
}


/*DirectoryStream2::iterator DirectoryStream2::begin()
{
	return m_directoryCollection.begin ();
}
DirectoryStream2::iterator DirectoryStream2::end()
{
	return m_directoryCollection.end();
}

DirectoryStream2::iterator begin(DirectoryStream2 &ds) { return ds.begin(); }
DirectoryStream2::iterator end(DirectoryStream2 &ds) { return ds.end(); }

*/
