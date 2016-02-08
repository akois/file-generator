#pragma once

bool GetFileKeyInfo32(LPCWSTR szFilePath, std::wstring& szFileHash, LARGE_INTEGER& liFileSize);
bool GetFileKeyInfo64(LPCWSTR szFilePath, std::wstring& szFileHash, LARGE_INTEGER& liFileSize);
