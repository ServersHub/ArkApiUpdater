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
	bool Enabled_;
	bool UseBeta_;
	std::string TempDirPath_;
	std::string VersionDirPath_;
	std::string LocalReleaseTag_;
	std::string LocalBranchName_;
	bool RebootRequired_ = false;
	unsigned int UpdatedFiles_ = 0;

	struct RepoData
	{
		std::string ReleaseTag;
		std::string DownloadURL;
	};

	void OpenConsole();
	std::string GetCurrentDir();
	bool ReadConfig(const std::string& CurrentDir);
	std::string GetBranchName();
	nlohmann::ordered_json GetRepoData();
	bool ParseRepoData(const nlohmann::ordered_json& Data, const std::string& BranchName, AutoUpdate::RepoData& RepoData);
	void RemoveOldDll(const std::string& CurrentDir);
	bool CreateAPIDirs(const std::string& BaseDir);
	bool CreateAutoUpdateDirs(const std::string& BaseDir);
	bool DownloadLatestRelease(const std::string& DownloadURL);
	void MoveUpdateFile(const std::string& FileName, const std::string& CurrentDir);
	bool ShouldUpdate(const std::string& FileName, const std::string& CurrentDir);
	bool UpdateFiles(const std::string& CurrentDir);
	bool WriteNewManifest(const std::string& ReleaseTag, const std::string& BranchName);
	void DeleteTempFiles();
	void RelaunchServer(const std::string& CurrentDir);
};