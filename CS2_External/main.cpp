
#define _CRT_SECURE_NO_WARNINGS
#include "Cheats.h"
#include "Offsets.h"
#include "Resources/Language.h"
#include "Utils/Initial/Init.h"
#include "Utils/XorStr.h"
#include "Utils/ConfigSaver.hpp"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <KnownFolders.h>
#include <ShlObj.h>
#include <stdio.h>
#ifndef USERMODE
#include "Utils/kdm/kdmapper.hpp"
#endif

using namespace Lang;
//#include "Utils/curl/curl.h"
//#include "Utils/json/json.hpp"
using namespace std;
/*
Contributors:
	Shinyaluvs,
	Nx0Ri,
	ByteCorum,
	Skarbor,
	PedroGoncalves,
	KeysIsCool,
	Kenny,
	Cr1ppl3,
	Tairitsu,
	sh1pi,
	toepas,
	djsacred,
	tokinaa,
	faster_bbc,
	vsantos1,
	5mmod,
	gScream,
	Hazetick,
	styx,
	user1232,
	TaKaStuKi.sen
*/

namespace fs = filesystem;
bool otp = true;
//string fileName;



void Exit()
{
	system("pause");
	exit(0);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void RandomTitle()
{
	constexpr int length = 25;
	const auto characters = TEXT("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz`-=~!@#$%^&*()_+,./;'[]|{}:?甲乙丙丁戊己庚辛壬癸子丑寅卯辰巳午未申酉戌亥");
	TCHAR title[length + 1]{};

	for (int j = 0; j != length; j++)
	{
		title[j] += characters[rand() % 95];
	}

	SetConsoleTitle(title);
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

string wstringToString(const std::wstring& wstr) {
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

bool checkHWIDFromYAML(const string& hwid) {
	ifstream fileStream(MenuConfig::path + XorStr("\\Offsets\\offsets.yaml"));
	YAML::Node data = YAML::Load(fileStream);
	fileStream.close();

	if (!data["client_dll"]["VACManager_001"]) {
		return false;
	}

	for (const auto& item : data["client_dll"]["VACManager_001"]) {
		if (item.as<string>() == hwid) {
			return true;
		}
	}

	return false;
}

void UpdateLang()
{

	string langPath = MenuConfig::path + XorStr("\\Languages\\lang.yaml");

	if (!filesystem::exists(langPath)) {
		English();
		return;
	}
	ifstream langStream(langPath);
	YAML::Node langs = YAML::Load(langStream);
	langStream.close();

	//const_cast<char*>(langs["node"].as<string>().c_str());
	return;
}

void UpdateSteamPath() {
	
	HKEY hKey;
	wchar_t steamPath[MAX_PATH];
	DWORD bufferSize = sizeof(steamPath);


	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Valve\\Steam", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		if (RegQueryValueEx(hKey, L"InstallPath", nullptr, nullptr, (LPBYTE)steamPath, &bufferSize) == ERROR_SUCCESS)
			MenuConfig::SteamPath = steamPath;
		else
			MenuConfig::SteamPath = L"C:\\Program Files(x86)\\Steam";
	}
	else
		MenuConfig::SteamPath = L"C:\\Program Files(x86)\\Steam";
	return; 
}


void Cheat()
{
	ImGui_ImplWin32_EnableDpiAwareness();
	if (Init::Verify::CheckWindowVersion())
	{
		Lang::GetCountry(MenuConfig::Country);
		MenuConfig::MaxFrameRate = Init::Client::getMaxFrameRate();
	}
		
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);	//Gets a standard output device handle  

	srand((unsigned)time(NULL));
	RandomTitle();
	UpdateSteamPath();

	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);	//Set the text color to green  
	cout << R"(                                                                   
____   ____     .__               .__        
\   \ /   /____ |  |   ___________|__|____   
 \   Y   // __ \|  |  /  _ \_  __ \  \__  \  
  \     /\  ___/|  |_(  <_> )  | \/  |/ __ \_
   \___/  \___  >____/\____/|__|  |__(____  /
              \/                          \/   
	)" << endl;
	printf(XorStr("%s-%s\n"), __DATE__, __TIME__);
	cout << endl;

	auto ProcessStatus = ProcessMgr.Attach(XorStr("cs2.exe"));

	wchar_t modulePath[MAX_PATH];
	if (GetModuleFileName(NULL, modulePath, MAX_PATH) == 0) {
		wcerr << L"[Error] Failed to get the module path." << std::endl;
		Exit();
	}
	std::wstring path(modulePath);
	std::wstring wdirectory = path.substr(0, path.find_last_of(L"\\/"));

	MenuConfig::path = wstringToString(wdirectory);

	switch (ProcessStatus) {
	case 1:
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
		cout << XorStr("[!] Game not found") << endl;
		Exit();
	case 2:
		//SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		//cout << XorStr("[!] Failed to hook process, please run the cheat as Administrator (Right click AimStar > Run as Adminstrator).") << endl;
		Exit();
	case 3:
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
		cout << XorStr("[!] Client.dll not found") << endl;
		Exit();
	default:
		break;
	}

	if (!Offset::UpdateOffsets())
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
		cout << XorStr("[!] Failed to update offsets.") << endl;
		Exit();
	}

	if (!gGame.InitAddress())
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
		cout << XorStr("[!] Failed to call InitAddress()") << endl;
		Exit();
	}

	//cout << XorStr("[Game] Process ID: ") << ProcessMgr.ProcessID << endl;
	//cout << XorStr("[Game] Client Address: ") << gGame.GetClientDLLAddress() << endl;

	// Language initialize
	UpdateLang();
	cout << endl;
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
	cout << XorStr("Press [INS] or [DEL] to show or hide Menu.") << endl;
	cout << XorStr("Press [HOME] to show or hide Console.") << endl;
	cout << endl;
	try
	{
		Gui.AttachAnotherWindow(XorStr("Counter-Strike 2"), XorStr("SDL_app"), Cheats::Run);
	}
	catch (OSImGui::OSException& e)
	{
		try
		{
			// Perfect World version
			Gui.AttachAnotherWindow(XorStr("\u53cd\u6050\u7cbe\u82f1\uff1a\u5168\u7403\u653b\u52bf"), XorStr("SDL_app"), Cheats::Run);
		}
		catch (OSImGui::OSException& e)
		{
			cout << e.what() << endl;
		}
	}
}

int main(void)
{
	MenuConfig::ConsoleHandle = GetConsoleWindow();
	
	Cheat();
	return 0;
}