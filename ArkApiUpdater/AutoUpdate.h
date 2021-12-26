#pragma once
#include <Windows.h>
#include <string>
#include "json.hpp"

class AutoUpdate
{
public:
	static AutoUpdate& Get();
	void Run(HMODULE hModule);
private:
	std::string TempDirPath_;
	std::string VersionDirPath_;
	std::string LocalReleaseTag_;
	bool RebootRequired_ = false;
	unsigned int UpdatedFiles_ = 0;

	void OpenConsole();
	nlohmann::json GetRepoData();
	std::string GetReleaseTag(const nlohmann::json Data);
	std::string GetDownloadURL(const nlohmann::json Data);
	std::string GetCurrentDir();
	void RemoveOldDll(const std::string& CurrentDir);
	bool CreateAutoUpdateDirs(const std::string& BaseDir);
	bool DownloadLatestRelease(const std::string& DownloadURL);
	void MoveUpdateFile(const std::string& FileName, const std::string& CurrentDir);
	bool ShouldUpdate(const std::string& FileName, const std::string& CurrentDir);
	bool UpdateFiles(const std::string& CurrentDir);
	bool WriteNewManifest(const std::string& ReleaseTag);
	void DeleteTempFiles();
	void RelaunchServer(const std::string& CurrentDir);
};