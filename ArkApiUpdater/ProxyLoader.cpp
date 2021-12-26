#include "ProxyLoader.h"
#include <cstdio>

ProxyLoader& ProxyLoader::Get()
{
	static ProxyLoader instance;
	return instance;
}

bool ProxyLoader::SetupProxy(HMODULE hModule)
{
	CHAR SystemDir[MAX_PATH];
	GetSystemDirectoryA(SystemDir, MAX_PATH);

	char Buffer[MAX_PATH];
	sprintf_s(Buffer, "%s\\d3d9.dll", SystemDir);

	OriginalDLL_ = LoadLibraryA(Buffer);
	if (!OriginalDLL_) return false;

	Procs_[0] = GetProcAddress(hModule, "D3DPERF_BeginEvent");
	Procs_[1] = GetProcAddress(hModule, "D3DPERF_EndEvent");
	Procs_[2] = GetProcAddress(hModule, "D3DPERF_GetStatus");
	Procs_[3] = GetProcAddress(hModule, "D3DPERF_QueryRepeatFrame");
	Procs_[4] = GetProcAddress(hModule, "D3DPERF_SetMarker");
	Procs_[5] = GetProcAddress(hModule, "D3DPERF_SetOptions");
	Procs_[6] = GetProcAddress(hModule, "D3DPERF_SetRegion");
	Procs_[7] = GetProcAddress(hModule, "DebugSetLevel");
	Procs_[8] = GetProcAddress(hModule, "DebugSetMute");
	Procs_[9] = GetProcAddress(hModule, "Direct3D9EnableMaximizedWindowedModeShim");
	Procs_[10] = GetProcAddress(hModule, "Direct3DCreate9");
	Procs_[11] = GetProcAddress(hModule, "Direct3DCreate9Ex");
	Procs_[12] = GetProcAddress(hModule, "Direct3DCreate9On12");
	Procs_[13] = GetProcAddress(hModule, "Direct3DCreate9On12Ex");
	Procs_[14] = GetProcAddress(hModule, "Direct3DShaderValidatorCreate9");
	Procs_[15] = GetProcAddress(hModule, "PSGPError");
	Procs_[16] = GetProcAddress(hModule, "PSGPSampleTexture");

	return true;
}

void ProxyLoader::FreeProxy()
{
	FreeLibrary(OriginalDLL_);
}

FARPROC ProxyLoader::GetProcAddr(const int Index)
{
	return Procs_[Index];
}

extern "C"
{
	FARPROC PA = NULL;
	int RunASM();

	void PROXY_D3DPERF_BeginEvent() 
	{
		PA = ProxyLoader::Get().GetProcAddr(0);
		RunASM();
	}
	void PROXY_D3DPERF_EndEvent() 
	{
		PA = ProxyLoader::Get().GetProcAddr(1);
		RunASM();
	}
	void PROXY_D3DPERF_GetStatus() 
	{
		PA = ProxyLoader::Get().GetProcAddr(2);
		RunASM();
	}
	void PROXY_D3DPERF_QueryRepeatFrame() 
	{
		PA = ProxyLoader::Get().GetProcAddr(3);
		RunASM();
	}
	void PROXY_D3DPERF_SetMarker() 
	{
		PA = ProxyLoader::Get().GetProcAddr(4);
		RunASM();
	}
	void PROXY_D3DPERF_SetOptions() 
	{
		PA = ProxyLoader::Get().GetProcAddr(5);
		RunASM();
	}
	void PROXY_D3DPERF_SetRegion() 
	{
		PA = ProxyLoader::Get().GetProcAddr(6);
		RunASM();
	}
	void PROXY_DebugSetLevel() 
	{
		PA = ProxyLoader::Get().GetProcAddr(7);
		RunASM();
	}
	void PROXY_DebugSetMute() 
	{
		PA = ProxyLoader::Get().GetProcAddr(8);
		RunASM();
	}
	void PROXY_Direct3D9EnableMaximizedWindowedModeShim() 
	{
		PA = ProxyLoader::Get().GetProcAddr(9);
		RunASM();
	}
	void PROXY_Direct3DCreate9() 
	{
		PA = ProxyLoader::Get().GetProcAddr(10);
		RunASM();
	}
	void PROXY_Direct3DCreate9Ex() 
	{
		PA = ProxyLoader::Get().GetProcAddr(11);
		RunASM();
	}
	void PROXY_Direct3DCreate9On12() 
	{
		PA = ProxyLoader::Get().GetProcAddr(12);
		RunASM();
	}
	void PROXY_Direct3DCreate9On12Ex() 
	{
		PA = ProxyLoader::Get().GetProcAddr(13);
		RunASM();
	}
	void PROXY_Direct3DShaderValidatorCreate9() 
	{
		PA = ProxyLoader::Get().GetProcAddr(14);
		RunASM();
	}
	void PROXY_PSGPError() 
	{
		PA = ProxyLoader::Get().GetProcAddr(15);
		RunASM();
	}
	void PROXY_PSGPSampleTexture() 
	{
		PA = ProxyLoader::Get().GetProcAddr(16);
		RunASM();
	}
}