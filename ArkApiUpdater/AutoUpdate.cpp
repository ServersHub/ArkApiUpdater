#include "AutoUpdate.h"
#include "Requests.h"
#include "Logger.h"
#include "zip_file.hpp"
#include "sha512.hh"
#include <fstream>
#include <locale>
#include <codecvt>
#include <filesystem>

#define LOGINFO(Data) Log::GetLog()->info(Data)
#define LOGERROR(Data) Log::GetLog()->error(Data)

AutoUpdate& AutoUpdate::Get()
{
	static AutoUpdate instance;
	return instance;
}

void AutoUpdate::OpenConsole()
{
	AllocConsole();
	FILE* p_cout;
	freopen_s(&p_cout, "conout$", "w", stdout);
}

nlohmann::json AutoUpdate::GetRepoData()
{
	try
	{
		const std::string res = Requests::Get().CreateGetRequest("https://api.github.com/repos/Michidu/ARK-Server-API/releases/latest");

		if (!res.empty())
		{
			return nlohmann::json::parse(res);
		}
	}
	catch (const std::exception& e)
	{
		LOGERROR(e.what());
	}

	return nlohmann::json();
}

std::string AutoUpdate::GetReleaseTag(const nlohmann::json Data)
{
	if (!Data.is_null())
	{
		if (Data.find("tag_name") != Data.end()) return Data["tag_name"].get<std::string>();
	}

	return std::string();
}

std::string AutoUpdate::GetDownloadURL(const nlohmann::json Data)
{
	if (!Data.is_null())
	{
		for (const auto& iter : Data["assets"])
		{
			if (iter.find("browser_download_url") != iter.end())
				return iter["browser_download_url"].get<std::string>();
		}
	}

	return std::string();
}

std::string AutoUpdate::GetCurrentDir()
{
	char Buffer[MAX_PATH];
	GetModuleFileNameA(NULL, Buffer, MAX_PATH);

	const std::string::size_type Pos = std::string(Buffer).find_last_of("\\/");

	return std::string(Buffer).substr(0, Pos);
}

void AutoUpdate::RemoveOldDll(const std::string& CurrentDir)
{
	remove((CurrentDir + "\\version.dll.old").c_str());
}

bool AutoUpdate::CreateAutoUpdateDirs(const std::string& BaseDir)
{
	try
	{
		if (!CreateDirectoryA((BaseDir + "\\ArkApi_AutoUpdate").c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS) return false;
		if (!CreateDirectoryA((BaseDir + "\\ArkApi_AutoUpdate\\Temp").c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS) return false;
		if (!CreateDirectoryA((BaseDir + "\\ArkApi_AutoUpdate\\Version").c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS) return false;

		TempDirPath_ = BaseDir + "\\ArkApi_AutoUpdate\\Temp";
		VersionDirPath_ = BaseDir + "\\ArkApi_AutoUpdate\\Version\\Version.json";

		std::fstream VersionConfig;

		VersionConfig.open(VersionDirPath_, std::fstream::in | std::fstream::out);

		if (!VersionConfig)
		{
			VersionConfig.open(VersionDirPath_, std::fstream::in | std::fstream::out | std::fstream::trunc);
			VersionConfig << "{}";
		}
		else
		{
			nlohmann::json JsonConf;
			VersionConfig >> JsonConf;

			if (!JsonConf.is_null() && JsonConf.find("ReleaseTag") != JsonConf.end()) LocalReleaseTag_ = JsonConf["ReleaseTag"].get<std::string>();
		}

		VersionConfig.close();

		return true;
	}
	catch (const std::exception& e)
	{
		LOGERROR(e.what());
		return false;
	}
}

bool AutoUpdate::DownloadLatestRelease(const std::string& DownloadURL)
{
	std::string FilePath = TempDirPath_ + "\\ArkApi.zip";
	return Requests::Get().DownloadFile(DownloadURL, FilePath);
}

void AutoUpdate::MoveUpdateFile(const std::string& FileName, const std::string& CurrentDir)
{
	if (FileName == "version.dll")
	{
		const std::string BaseStr = CurrentDir + "\\";
		rename((BaseStr + "version.dll").c_str(), (BaseStr + "version.dll.old").c_str());
		RebootRequired_ = true;
	}

	std::string ExtractedFile = TempDirPath_ + "\\" + FileName;
	std::string OldFile = CurrentDir + "\\" + FileName;

	remove(OldFile.c_str());
	const auto res = rename(ExtractedFile.c_str(), OldFile.c_str());

	if (res == 0)
	{
		LOGINFO("Sucesfully updated: " + FileName);
		UpdatedFiles_++;
	}
	else LOGERROR("Error updating: " + FileName + " Error code: " + std::to_string(res));
}

bool AutoUpdate::ShouldUpdate(const std::string& FileName, const std::string& CurrentDir)
{
	const std::string CurrentFileHash = sw::sha512::file(CurrentDir + "\\" + FileName);
	const std::string NewFileHash = sw::sha512::file(TempDirPath_ + "\\" + FileName);

	return CurrentFileHash != NewFileHash;
}

bool AutoUpdate::UpdateFiles(const std::string& CurrentDir)
{
	try
	{
		miniz_cpp::zip_file File(TempDirPath_ + "\\ArkApi.zip");
		const std::string UpdateFiles[] = { "config.json", "msdia140.dll", "version.dll", "version.pdb" };

		for (const auto& UpdateFile : UpdateFiles)
		{
			if (UpdateFile == "config.json")
			{
				std::fstream ConfigFile;
				ConfigFile.open(CurrentDir + "\\" + "config.json", std::fstream::in | std::fstream::out);

				if (!ConfigFile && File.has_file(UpdateFile))
				{
					File.extract(UpdateFile, TempDirPath_);
					MoveUpdateFile(UpdateFile, CurrentDir);
				}
				else ConfigFile.close();
			}
			else
			{
				File.extract(UpdateFile, TempDirPath_);

				if (ShouldUpdate(UpdateFile, CurrentDir))
				{
					MoveUpdateFile(UpdateFile, CurrentDir);
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		LOGERROR(e.what());
		return false;
	}

	return true;
}

bool AutoUpdate::WriteNewManifest(const std::string& ReleaseTag)
{
	try
	{
		nlohmann::json Manifest;
		Manifest["ReleaseTag"] = ReleaseTag;

		std::fstream VersionConfig;
		VersionConfig.open(VersionDirPath_, std::fstream::out | std::fstream::trunc);

		if (VersionConfig)
		{
			VersionConfig << Manifest.dump();
			VersionConfig.close();
		}
		else return false;
	}
	catch (const std::exception& e)
	{
		LOGERROR(e.what());
		return false;
	}

	return true;
}

void AutoUpdate::DeleteTempFiles()
{
	for (const auto& File : std::filesystem::directory_iterator(TempDirPath_))
	{
		std::filesystem::remove_all(File.path());
	}
}

void AutoUpdate::RelaunchServer(const std::string& CurrentDir)
{
	LOGINFO("This update requires a server reboot, rebooting in 5 seconds...");
	Sleep(5000);

	const DWORD PID = GetCurrentProcessId();

	std::wstring WCurrentDir(CurrentDir.begin(), CurrentDir.end());

	STARTUPINFOW SI;
	PROCESS_INFORMATION PI;

	ZeroMemory(&SI, sizeof SI);
	SI.cb = sizeof SI;
	ZeroMemory(&PI, sizeof PI);

	CreateProcessW((WCurrentDir + LR"(\ShooterGameServer.exe)").c_str(), GetCommandLineW(), nullptr, nullptr, FALSE, CREATE_NEW_CONSOLE, nullptr, nullptr, &SI, &PI);

	CloseHandle(PI.hProcess);
	CloseHandle(PI.hThread);

	system(("taskkill /PID " + std::to_string(PID)).c_str());
}

void AutoUpdate::Run(HMODULE hModule)
{
	OpenConsole();

	LOGINFO("Starting automatic update process");

	const nlohmann::json RepoData = GetRepoData();
	const std::string DownloadURL = GetDownloadURL(RepoData);
	const std::string ReleaseTag = GetReleaseTag(RepoData);

	if (DownloadURL.empty() || ReleaseTag.empty())
	{
		LOGERROR("Could not obtain download data");
		return;
	}

	const std::string CurrentDir = GetCurrentDir();

	if (CurrentDir.empty())
	{
		LOGERROR("Failed to read directory");
		return;
	}

	RemoveOldDll(CurrentDir);

	if (!CreateAutoUpdateDirs(CurrentDir))
	{
		LOGERROR("Failed to create directories");
		return;
	}

	if (LocalReleaseTag_ == ReleaseTag)
	{
		LOGINFO("Latest ArkApi version is installed, skipping update");
		return;
	}

	if (!DownloadLatestRelease(DownloadURL))
	{
		LOGERROR("Failed to download release files");
		return;
	}

	if (!UpdateFiles(CurrentDir))
	{
		LOGERROR("Failed to unpack release files");
		return;
	}

	if (!WriteNewManifest(ReleaseTag))
	{
		LOGERROR("Failed to write new update version");
		return;
	}

	DeleteTempFiles();

	if (UpdatedFiles_ > 0) LOGINFO("Update complete, " + std::to_string(UpdatedFiles_) + " x File(s) required an update");
	else LOGINFO("Update complete, no files required updates");

	if (RebootRequired_) RelaunchServer(CurrentDir);
}