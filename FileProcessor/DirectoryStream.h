#pragma once


struct directory_iterator;
struct file_iterator;
struct DirectoryEntry;
struct FileEntry;
class DirectoryStream;

struct FileEntry
{
	FileEntry(const std::wstring &strName) :m_strName(strName){};

	FileEntry(){};

	~FileEntry(){};

	std::wstring m_strName;
};



struct DirectoryEntry : FileEntry
{
	DirectoryEntry(const std::wstring &strDirName, DirectoryStream *pStream) :
		FileEntry(strDirName),
		m_bInitialized(false),
		m_pStream(pStream) {};

	//DirectoryEntry(){};

	typedef file_iterator iterator;
	typedef file_iterator const const_iterator;

	//const_iterator begin() { return file_iterator(m_pStream->m_vFiles.size() ? m_pStream : nullptr); }
	//const_iterator end() { return file_iterator(nullptr); }

	iterator &begin();// { return file_iterator(m_pStream->m_vFiles.size() ? m_pStream : nullptr); }
	iterator &end();// { return file_iterator(nullptr); }

	DirectoryStream *m_pStream;

	bool m_bInitialized;
};


struct file_iterator : std::iterator< std::input_iterator_tag, FileEntry>
{
	file_iterator(DirectoryStream *pStream) :m_pStream(pStream)	{};

	file_iterator(const file_iterator &i) : m_pStream(i.m_pStream) {};

	FileEntry &operator*() const { return GetCurFileEntry(); }

	file_iterator operator++()
	{
		if (m_pStream && !MoveNextFileEntry())
			m_pStream = nullptr;

		return *this;
	}

	bool operator == (const file_iterator& cit) const
	{
		return m_pStream == cit.m_pStream;
	}

	bool operator != (const file_iterator& cit) const
	{
		return m_pStream != cit.m_pStream;
	}

private:
	DirectoryStream *m_pStream;

	FileEntry &GetCurFileEntry() const;
	bool MoveNextFileEntry();

	friend class DirectoryStream;
};



struct directory_iterator : std::iterator< std::input_iterator_tag, DirectoryEntry> 
{
	directory_iterator(DirectoryStream *pStream) :
		m_pStream(pStream) 
	{};

	directory_iterator(const directory_iterator &i) :
		m_pStream(i.m_pStream)
	{};

	DirectoryEntry &operator*() const { return GetCurDirEntry();}

	directory_iterator operator++()
	{
		if (m_pStream && !MoveNextDirEntry())
			m_pStream = nullptr;

		return *this;
	}

	bool operator == (const directory_iterator& cit) const
	{
		return m_pStream==cit.m_pStream;
	}

	bool operator != (const directory_iterator& cit) const
	{
		return m_pStream != cit.m_pStream;
	}

private:
	directory_iterator() {};
	DirectoryEntry &GetCurDirEntry() const;
	bool MoveNextDirEntry();


	DirectoryStream *m_pStream;

	friend class DirectoryStream;
};


class DirectoryStream
{
public:
	DirectoryStream(LPCWSTR pszSeedDir);
	~DirectoryStream();

	typedef directory_iterator iterator;
	// directory_iterator const const_iterator;

	iterator &begin();
	iterator &end();

	friend struct directory_iterator;
	friend struct file_iterator;
	friend struct DirectoryEntry;

private:
	//DirectoryStream() {};

	std::deque <DirectoryEntry> m_vDirectories;
	std::deque <FileEntry> m_vFiles;

	bool move_next_directory();
	bool read_directory_content(const std::wstring &strCurDir);

	bool move_next_file();

	file_iterator m_endFileIterator;
	file_iterator m_curFileIterator;

	directory_iterator m_endDirIterator;
	directory_iterator m_curDirIterator;
};

directory_iterator &begin(DirectoryStream &ds);
directory_iterator &end(DirectoryStream &ds);
file_iterator &begin(DirectoryEntry &de);
file_iterator &end(DirectoryEntry &de);