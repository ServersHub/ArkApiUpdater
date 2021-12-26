#include "ProxyLoader.h"
#include "AutoUpdate.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		if (!ProxyLoader::Get().SetupProxy(hModule)) return FALSE;

		AutoUpdate::Get().Run(hModule);

		return TRUE;
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		ProxyLoader::Get().FreeProxy();
	}

	return TRUE;
}
