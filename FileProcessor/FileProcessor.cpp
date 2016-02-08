// FileProcessor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DirectoryStream.h"
//#include "DirStream2.h"
#include "FileKey.h"
#include "md5.h"
using namespace std;

const DWORD dwPacketSize = 32 * 1024;

struct int_iter
	: std::iterator<std::random_access_iterator_tag, int, ptrdiff_t, int*, int>
{
	int_iter(value_type i = 0, value_type step = 1) : value(i), step(step)
	{}

	value_type operator*() const {
		return value;
	}
	int_iter& operator++() {
		value += step; return *this;
	}
	int_iter& operator--() {
		value -= step; return *this;
	}
	int_iter& operator+=(ptrdiff_t offset) {
		value += step*offset; return *this;
	}
	int_iter& operator-=(ptrdiff_t offset) {
		value -= step*offset; return *this;
	}
	ptrdiff_t operator-(int_iter rhs) const {
		return ptrdiff_t((value - rhs.value) / step);
	}
	bool operator==(int_iter other) const {
		return value == other.value;
	}
	bool operator!=(int_iter other) const {
		return !(*this == other);
	}
	bool operator<(int_iter other) const { return (*this - other) < 0; }
	bool operator>(int_iter other) const { return (*this - other) > 0; }
	bool operator<=(int_iter other) const { return (*this - other) <= 0; }
	bool operator>=(int_iter other) const { return (*this - other) >= 0; }

	value_type value;
	value_type step;
};
int_iter operator+(int_iter lhs, ptrdiff_t rhs) {
	return lhs += rhs;
}
int_iter operator+(ptrdiff_t lhs, int_iter rhs) {
	return rhs += lhs;
}
int_iter operator-(int_iter lhs, ptrdiff_t rhs) {
	return lhs -= rhs;
}
struct int_range
{
	typedef int_iter iterator;
	typedef int_iter::value_type value_type;
	int_range(value_type begin, value_type end, value_type step = 1)
		: b(begin, step), e(end, step)
	{
		if (step == 0) {
			throw std::logic_error("bad step");
		}
		if (abs(end - begin) % abs(step) != 0) {
			end -= (end - begin) % abs(step);
		}
	}
	int_iter begin() const { return int_iter(b); }
	int_iter end() const { return int_iter(e); }

	iterator b, e;
};

vector<int> vector_range(int start, int end)
{
	vector<int> v;
	for (int i = start; i < end; ++i)
		v.push_back(i);
	return v;
}

class FileHash
{
public:
	FileHash() : _hFile(INVALID_HANDLE_VALUE)
	{
		_pData = new byte[dwPacketSize];
	}

	~FileHash()
	{
		if (_hFile != INVALID_HANDLE_VALUE) ::CloseHandle(_hFile);
		delete[] _pData;
	}

	bool IsStarted() const { return _hFile != INVALID_HANDLE_VALUE; }
	bool Start(const wstring &strFileName)
	{
		::ZeroMemory(&ovl, sizeof(ovl));
		_strFileName = strFileName;
		md5.Init();
		_hFile = CreateFile(_strFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		return _hFile != INVALID_HANDLE_VALUE;
	}

	void Process(DWORD dwSize)
	{
		md5.Update(_pData, dwSize);
	}

	bool Read()
	{
		BOOL bRead = ReadFile(_hFile, _pData, dwPacketSize, NULL, &ovl);

		DWORD dwError = ::GetLastError();
		return bRead == TRUE;
	}

	wstring Complete()
	{
		if (_hFile != INVALID_HANDLE_VALUE)
			::CloseHandle(_hFile);

		_hFile = INVALID_HANDLE_VALUE;
		md5.Final();
		return md5.digestChars;
	}

	const wstring &GetFileName() const { return _strFileName; }

	HANDLE GetHandle() const { return _hFile; }

private:
	MD5 md5;
	wstring _strFileName;
	HANDLE _hFile;

	OVERLAPPED ovl;
	byte *_pData;
};

bool hasEnding(std::wstring const &fullString, std::wstring const &ending)
{
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else {
		return false;
	}
}

const std::wstring cstr_dll = L".dll";
const std::wstring cstr_exe = L".exe";

int _tmain(int argc, _TCHAR* argv[])
{
	using std::chrono::system_clock;
	using std::chrono::milliseconds;
	using std::chrono::seconds;
	using std::chrono::duration_cast;

	auto start = std::chrono::system_clock::now();

	using namespace cpplinq;
	//DirectoryStream ds(L"d:\\work\\Inventory\\trunk\\InventoryWin");
	DirectoryStream ds(L"d:\\");// work\\Inventory\\trunk\\InventoryWin\\Components\\x64");
	//DirectoryStream ds(L"d:\\Work\\SMP\\Environment\\Third Party Tools");
	

	auto result = from(ds)
		.where([](DirectoryEntry &de){
			return de.m_strName.find(L"2") == std::wstring::npos;
		})
		.select_many([](DirectoryEntry &de)
		{
			//std::wcout << L"Dir:" << de.m_strName.c_str() << std::endl;
			return de;
		})
		;// .where([](const FileEntry &fi){return hasEnding(fi.m_strName, cstr_dll) || hasEnding(fi.m_strName, cstr_exe); });

		/*for (auto &fi : result)
		{
		std::wcout << L"File:" << fi.m_strName.c_str() << std::endl;
		}*/

		
		bool bMethod1 = true;
		bool bMethod2 = false;

		auto itCur = result.begin();
		auto itEnd = result.end();
		std::mutex m;

		auto getNext = [&itCur, &itEnd, &m](std::wstring &strName)
		{
			std::lock_guard<std::mutex> lock(m);

			if (itCur == itEnd) return false;
			strName = itCur->m_strName;
			++itCur;
			return true;
		};


		if (bMethod1)
		{
			int nPCount = 6;

			vector<FileHash> v;

			v.reserve(nPCount);
			for (int i = 0; i < nPCount; i++)
				v.emplace_back();

			HANDLE hCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,
				0, // no existing port
				0, // ignored
				0);

			if (0 == hCompletionPort)
			{
				// Call GetLastError for more information.
			}


			wstring strName;

			for (int i = 0; i < nPCount && getNext(strName); )
			{
				if (v[i].Start(strName))
				{

					if (0 == ::CreateIoCompletionPort(v[i].GetHandle(),
						hCompletionPort,
						i,
						0))
					{
						//error
					}
					if (!v[i].Read()) //todo move to init)
					{
						v[i].Complete();
						continue;
					}
					++i;
				}
			}

			auto threadFunc = [&getNext, &v, hCompletionPort]()
			{
				DWORD dwBytesCopied;
				ULONG_PTR key;
				LPOVERLAPPED pOvl;
				//block
				while (::GetQueuedCompletionStatus(hCompletionPort,
					&dwBytesCopied,
					&key,
					&pOvl,
					INFINITE))
				{
					int nIndex = (int)key;

					if (nIndex == 33) 
						return; //???

					if (dwBytesCopied)
					{

						pOvl->Offset += dwBytesCopied;
						v[nIndex].Process(dwBytesCopied);
						v[nIndex].Read();
					}

					//std::wcout << ::GetLastError() << std::endl;
					if (dwBytesCopied < dwPacketSize || ::GetLastError() == ERROR_HANDLE_EOF)
					{
						wstring strHash = v[nIndex].Complete();
						//std::wcout << L"File:" << v[nIndex].GetFileName().c_str() << L" " << strHash.c_str() << std::endl;
						
						//next file
						
						while (true)
						{
							wstring strFileName;
							if (!getNext(strFileName))
							{
								ULONG_PTR key=33;
								PostQueuedCompletionStatus(hCompletionPort, 0, key, pOvl);
								return;
							}

							if (v[nIndex].Start(strFileName))
							{

								if (0 == ::CreateIoCompletionPort(v[nIndex].GetHandle(),
									hCompletionPort,
									nIndex,
									0))
								{
									//error
								}
								if (!v[nIndex].Read()) //todo move to init)
								{
									v[nIndex].Complete();
								}
								else break;
							}
						}
					}

				}
			};

			std::vector<std::thread> vThreads;

			for (int i = 0; i < nPCount; ++i)
				vThreads.emplace_back(threadFunc);

			for (auto &t : vThreads)
				t.join();


			::CloseHandle(hCompletionPort);
		}
	
		if (bMethod2)
		{
			int nFileCount = 0;

		
			auto threadFunc = [&getNext]()
			{
				std::wstring strName;
				while (getNext(strName))
				{
					LARGE_INTEGER liFileSize;
					liFileSize.QuadPart = 0;
					std::wstring szFileHash;
#ifdef _WIN64
					GetFileKeyInfo64(strName.c_str(), szFileHash, liFileSize);
#else
					GetFileKeyInfo32(strName.c_str(), szFileHash, liFileSize);
#endif

					//std::wcout << L"File:" << strName.c_str() << L" " << szFileHash.c_str() << std::endl;
					//nFileCount++;
				}
			};

			int nThreadCount = 6;

			std::vector<std::thread> vThreads;

			for (int i = 0; i < nThreadCount; ++i)
				vThreads.emplace_back(threadFunc);

			for (auto &t : vThreads)
				t.join();
		}
	//std::wcout << L"   File:" << fe.m_strName.c_str() << std::endl;

	auto stop = std::chrono::system_clock::now();
	const auto d_actual = duration_cast<seconds>(stop - start).count();

	//std::wcout << L"Procesed " << nFileCount << L" file(s).";
	std::wcout << L"Time to execute " << d_actual << L" seconds." << std::endl;

	return 0;
	
}


