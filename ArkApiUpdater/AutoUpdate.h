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
	std::unique_ptr<std::istream> DownloadFile(const std::string& DownloadURL);
	bool PermissionsInstallRequired(const std::string& PermissionsDir);
	bool InstallPermissions(const std::string& PermissionsDir, const std::unique_ptr<std::istream> FileData);
	void WriteFile(void* Zip, const std::string& ZipFileName, const std::string& ExtractDir, const std::string& ExtractFileName, const bool CheckExisting);
	bool ShouldUpdate(const std::string& FileName, const std::string& CurrentDir, const std::string& NewFileData);
	bool UpdateFiles(const std::string& CurrentDir, const std::unique_ptr<std::istream> FileData);
	bool WriteNewManifest(const std::string& ReleaseTag, const std::string& BranchName);
	void RelaunchServer(const std::string& CurrentDir);
	void PrintUpdateInfo();
};