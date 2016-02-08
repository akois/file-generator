#include "stdafx.h"
#include "DirectoryStream.h"

directory_iterator &begin(DirectoryStream &ds) { return ds.begin(); }
directory_iterator &end(DirectoryStream &ds) { return ds.end(); }
file_iterator &begin(DirectoryEntry &de) { return de.begin(); }
file_iterator &end(DirectoryEntry &de) { return de.end(); }

DirectoryStream::DirectoryStream(LPCWSTR pszSeedDir) :
	m_endDirIterator(nullptr),
	m_curDirIterator(this),
	m_endFileIterator(nullptr),
	m_curFileIterator(nullptr)
{
	m_vDirectories.emplace_front(pszSeedDir, this);
	//m_vDirectories.emplace_back(L"c:\\", this);
}

DirectoryStream::~DirectoryStream()
{
}

directory_iterator &DirectoryStream::begin()
{
	return m_curDirIterator;
}
directory_iterator &DirectoryStream::end()
{
	return m_endDirIterator;
}


bool DirectoryStream::move_next_file()
{
	m_vFiles.pop_back();

	return m_vFiles.size() != 0;
}



DirectoryEntry::iterator &DirectoryEntry::begin() 
{
	if (!m_bInitialized)
	{
		m_bInitialized=m_pStream->read_directory_content(m_strName);
	}
	return m_pStream->m_curFileIterator; 
}


DirectoryEntry::iterator &DirectoryEntry::end() { 
	return m_pStream->m_endFileIterator; 
}

FileEntry &file_iterator::GetCurFileEntry() const {	return m_pStream->m_vFiles.back();}
bool file_iterator::MoveNextFileEntry() { return m_pStream->move_next_file(); }


DirectoryEntry &directory_iterator::GetCurDirEntry() const { return m_pStream->m_vDirectories.front(); }
bool directory_iterator::MoveNextDirEntry() { return m_pStream->move_next_directory(); }


bool DirectoryStream::read_directory_content(const std::wstring &strCurDir)
{
	//std::wcout << L"Reading content of Dir:" << strCurDir.c_str() << std::endl;

	m_vFiles.clear();
	std::wstring strSearchMask=strCurDir + L"\\*.*";

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(strSearchMask.c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FindFileData.dwFileAttributes &  FILE_ATTRIBUTE_DIRECTORY)
			{
				if (!(wcscmp(FindFileData.cFileName, L"..") == 0 ||
					wcscmp(FindFileData.cFileName, L".") == 0))
					m_vDirectories.emplace_back(strCurDir + L"\\" + FindFileData.cFileName, this);
			}
			else
			{
				m_vFiles.emplace_back(strCurDir + L"\\" + FindFileData.cFileName);
				m_curFileIterator.m_pStream = this;
			}
		} while (FindNextFile(hFind, &FindFileData));

		
		FindClose(hFind);
	}
	if (!m_vFiles.size())
	{
		m_vFiles.emplace_back(strCurDir + L"\\empty$$$$");
		m_curFileIterator.m_pStream = this;
	}

	return true;
}

bool DirectoryStream::move_next_directory()
{
	m_vDirectories.pop_front();

	return m_vDirectories.size() != 0;
}