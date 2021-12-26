#pragma once
#include <windows.h>

class ProxyLoader
{
public:
	static ProxyLoader& Get();
	bool SetupProxy(HMODULE hModule);
	void FreeProxy();
	FARPROC GetProcAddr(const int Index);

private:
	FARPROC Procs_[17];
	HINSTANCE OriginalDLL_ = 0;
};