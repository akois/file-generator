#pragma once
#include <string>
#include <deque>
#include <memory>

struct FileEntry
{
	FileEntry(const std::wstring &strName) { m_strName = strName; }
	std::wstring m_strName;
};


class DirectoryEntry;
class DirectoryStream2;

typedef std::vector<FileEntry> FileCollection;
typedef std::vector<std::shared_ptr<DirectoryEntry>> DirectoryCollection;


/*template <class  T> class LazyCollection : public T
{
	typedef typename T::iterator inner_iterator;

public:
	LazyCollection(DirectoryEntry *pParentEnty) : m_pParentEnty(pParentEnty) {};
	~LazyCollection() { clear(); } //todo since we have no virtual destructor we should call cleanup manually

	inner_iterator begin() { m_pParentEnty->Initialize();  return T::begin(); }
	inner_iterator end() { m_pParentEnty->Initialize();  return T::end(); }

private:
	DirectoryEntry *m_pParentEnty;
};
*/

class DirectoryEntry : public FileEntry
{
public:
	DirectoryEntry(const std::wstring &strDir, DirectoryStream2* pParentStream);
	~DirectoryEntry() {};

	bool Initialize ();

	FileCollection &GetFileList() { Initialize();  return m_fileCollection; }
	DirectoryCollection &GetDirList() { Initialize();  return m_directoryCollection; }


private:
	bool Populate();

	//friend class LazyCollection < FileCollection > ;
	//friend class LazyCollection < DirectoryCollection >;

	FileCollection m_fileCollection;
	DirectoryCollection m_directoryCollection;

	bool m_bInitialized;
	DirectoryStream2* m_pParentStream;
};


class DirectoryStream2
{
public:
	typedef std::deque<std::shared_ptr<DirectoryEntry>> DirectoryCollection;
	typedef DirectoryCollection::iterator iterator;

	DirectoryStream2(const std::wstring &strSeedDir);


	DirectoryCollection &GetCollection() { return m_directoryCollection; }

	//iterator begin();
	//iterator end();
private:

	friend class DirectoryEntry;

	DirectoryCollection m_directoryCollection;

	void AddSeed(std::shared_ptr<DirectoryEntry> &ptrDirEntry) { m_directoryCollection.emplace_back(ptrDirEntry); }
};

//DirectoryStream2::iterator begin(DirectoryStream2 &ds);
//DirectoryStream2::iterator end(DirectoryStream2 &ds);