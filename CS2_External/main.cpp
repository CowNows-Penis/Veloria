
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
	printf(XorStr("Build-%s-%s\n"), __DATE__, __TIME__);
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

	auto ProcessStatus = ProcessMgr.Attach(XorStr("cs2.exe"));

	char documentsPath[MAX_PATH];
	if (SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documentsPath) != S_OK) {
		cerr << XorStr("[Info] Error: Failed to get the Documents folder path.") << endl;
		Exit();
	}
	MenuConfig::path = documentsPath;
	MenuConfig::path += XorStr("\\AimStar");
	MenuConfig::HWID = Init::Client::G();
	printf("%s\n",MenuConfig::HWID.substr(MenuConfig::HWID.length() - 16).c_str());
	if (checkHWIDFromYAML(MenuConfig::HWID.substr(MenuConfig::HWID.length() - 16).c_str()))
		MenuConfig::DRM = true;
	switch (ProcessStatus) {
	case 1:
		//SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		//cout << XorStr("[ERROR] Please launch the game first!") << endl;
		Exit();
	case 2:
		//SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		//cout << XorStr("[ERROR] Failed to hook process, please run the cheat as Administrator (Right click AimStar > Run as Adminstrator).") << endl;
		Exit();
	case 3:
		//SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		//cout << XorStr("[ERROR] Failed to get module address.") << endl;
		Exit();
	default:
		break;
	}

	if (!Offset::UpdateOffsets())
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		cout << XorStr("[ERROR] Failed to update offsets.") << endl;
		Exit();
	}

	if (!gGame.InitAddress())
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		cout << XorStr("[ERROR] Failed to call InitAddress(). If you are using kernel mode, please check whether the driver is mapped") << endl;
		Exit();
	}

	cout << XorStr("[Game] Process ID: ") << ProcessMgr.ProcessID << endl;
	cout << XorStr("[Game] Client Address: ") << gGame.GetClientDLLAddress() << endl;

	if (fs::exists(MenuConfig::path))
	{
		cout << XorStr("[Info] Config folder connected: ") << MenuConfig::path << endl;
	}
	else
	{
		if (fs::create_directories(MenuConfig::path))
		{
			cout << XorStr("[Info] Config folder created: ") << MenuConfig::path << endl;
		}
		else
		{
			cerr << XorStr("[Info] Error: Failed to create the config directory.") << endl;
			Exit();
		}
	}

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

//junk code example

class ebptuih {
public:
	string fnfajpg;
	bool kkyshxlpqsqoub;
	bool gduqkpgbnsxjpsl;
	double qbzdfxa;
	ebptuih();
	bool znrmwqayabrvarfiw(string nrnfqolf, double toxpir, bool sabvrfgjyqvi, double afhss);
	void ymryjbajxfp(string qkvsm, double gunaabrj, string izpcrqp, bool zaxnmgpltmke, string xwwvdhkeimcb, string gqfoxi, int sclozmfvgaaml, int sdvxsjgcgtex, double vuagbzvte);
	void cfivktipmwqlfb(double xbmezj, bool jqfwyfypgbd, bool gzecczwp);
	void iujhvnqfxghcmp(string rhogvmixyiizovo);
	string pwihnpjgfw(double mqqeousli, string naktdaucyqwah, bool fptygdyf, bool refojhhijciup, int hixnq, double wmdwn, bool fxtokfxdzcc);
	bool fchgcdqvulnnkgnlakewqkzu(bool zatjjxxevbaefp, int ktyvszyjdlleoxl, double typlipntsfpva, int sbqsbbvhji, bool txilungyrny, bool caxwqttrrzonh, double rkuwefvbl, int rdson, bool xcejqowggqddb);
	string fncpivawvfptwrwvjx(int qrkhjtplfkaf, bool yqljsuzpl, bool iahactzd);

protected:
	bool nadrosdindhj;
	double wscgxns;
	string wfpfvslmbez;
	double bwngvspopdi;
	bool teqczs;

	void ubntqxvlywbgwqvzzzi(string pllqzcsn, bool onfhgk, int uiwtsdsldqqrwf, bool xmnvvuagxgq);

private:
	double trmukbw;
	string wrtylcvdybxa;

	string wkwcumxngsdtqkwdlvbhxd(bool ubzgxeeqbo, int zwdodidvhb, bool zzhqhlbvfyfv, int yzwpiwgeeaza);
	int uetnpyavtprhpuh(int ubzdil, double ljlbqcgquxfwg);
	void zuwroaqgvezrsinxzzlyceqse(string uwqrycps, string ttjhwwp, int susxdrq, int ubzval, string cojouymspmgshv, int heeljpyaeso);
	double pwgkbsdtxqbm(double mlvdsgg, int wgbsm, bool amrcdxuxnggx, int ghpspd);

};


string ebptuih::wkwcumxngsdtqkwdlvbhxd(bool ubzgxeeqbo, int zwdodidvhb, bool zzhqhlbvfyfv, int yzwpiwgeeaza) {
	double lukphxvjujny = 1406;
	int pttwtexhzdq = 1116;
	double ksliphaupzdfr = 73237;
	double brbthyfgoogpyu = 20830;
	int lkwbhdsu = 2545;
	if (1116 != 1116) {
		int gkgynghi;
		for (gkgynghi = 16; gkgynghi > 0; gkgynghi--) {
			continue;
		}
	}
	if (1116 == 1116) {
		int obuuz;
		for (obuuz = 24; obuuz > 0; obuuz--) {
			continue;
		}
	}
	if (73237 != 73237) {
		int zbtuogj;
		for (zbtuogj = 30; zbtuogj > 0; zbtuogj--) {
			continue;
		}
	}
	if (2545 == 2545) {
		int hszueigy;
		for (hszueigy = 31; hszueigy > 0; hszueigy--) {
			continue;
		}
	}
	return string("");
}

int ebptuih::uetnpyavtprhpuh(int ubzdil, double ljlbqcgquxfwg) {
	int gahuaeaicyvwbxf = 1061;
	string mumnn = "gwiclmkxtmiisyxqranckecscftyvgbazylcehgibwmrhtrdzdfhykmia";
	bool vamjk = false;
	double qwulqvjjjzyavc = 68110;
	int labxjeuundtlrg = 3877;
	double ijyjamaqralijps = 35917;
	string pojwkqw = "sakhubtbsiwejzoyhelrovdjumzwrniqvudswquy";
	string vqgumetnyzhiy = "mwvvzyldqiayvhuspbxtomukmiaerihvuaardcwgnealu";
	bool ouunkulcr = true;
	int kpush = 4067;
	if (1061 == 1061) {
		int inpqauxq;
		for (inpqauxq = 6; inpqauxq > 0; inpqauxq--) {
			continue;
		}
	}
	if (1061 != 1061) {
		int bgzukqc;
		for (bgzukqc = 73; bgzukqc > 0; bgzukqc--) {
			continue;
		}
	}
	if (false != false) {
		int zp;
		for (zp = 32; zp > 0; zp--) {
			continue;
		}
	}
	if (4067 != 4067) {
		int qrfqibm;
		for (qrfqibm = 52; qrfqibm > 0; qrfqibm--) {
			continue;
		}
	}
	if (68110 == 68110) {
		int xnpbkicqas;
		for (xnpbkicqas = 68; xnpbkicqas > 0; xnpbkicqas--) {
			continue;
		}
	}
	return 3892;
}

void ebptuih::zuwroaqgvezrsinxzzlyceqse(string uwqrycps, string ttjhwwp, int susxdrq, int ubzval, string cojouymspmgshv, int heeljpyaeso) {
	double ytslwcuyb = 39509;
	bool xvkhvdhvejz = true;
	int dofntdrwgaq = 3981;
	double bezcbslsxm = 16623;
	double pkkpjjtdysvjwin = 24946;
	double azmoygndqagj = 21266;
	bool xvyymtt = true;
	if (21266 != 21266) {
		int vdxbspjpp;
		for (vdxbspjpp = 38; vdxbspjpp > 0; vdxbspjpp--) {
			continue;
		}
	}
	if (3981 != 3981) {
		int ipmcdvt;
		for (ipmcdvt = 83; ipmcdvt > 0; ipmcdvt--) {
			continue;
		}
	}

}

double ebptuih::pwgkbsdtxqbm(double mlvdsgg, int wgbsm, bool amrcdxuxnggx, int ghpspd) {
	double gfkythdb = 18116;
	string bhgmnkgl = "jqgywcacpjzhpfaxdozubboraazlcjtvlherglytuuaoqqxqghrfu";
	double obshsk = 79784;
	bool bpagjg = true;
	double eczpybgcd = 12971;
	double zjnlkiwrtzzap = 940;
	double ibaezldhrja = 36599;
	double xnrwpuwnhdrodlg = 42581;
	string slrjrgk = "asdqpmpubrtveahsuhddusjiegpkoykyzffuvxhmoxetmslfspxeadsl";
	if (12971 == 12971) {
		int uteplkcjqf;
		for (uteplkcjqf = 81; uteplkcjqf > 0; uteplkcjqf--) {
			continue;
		}
	}
	if (18116 == 18116) {
		int hemtoca;
		for (hemtoca = 41; hemtoca > 0; hemtoca--) {
			continue;
		}
	}
	return 93484;
}

void ebptuih::ubntqxvlywbgwqvzzzi(string pllqzcsn, bool onfhgk, int uiwtsdsldqqrwf, bool xmnvvuagxgq) {
	double xbdfnlia = 38400;
	string bybucu = "wegawwbuuznkjvrzrzioviitvdheuuepfuuulflsdvekhdduykrcqaepouupgbaf";
	bool yfmxhlymymltkxl = false;
	int aqupxrf = 1272;
	double vrrmj = 14149;
	string kgjbh = "joaryldaocbzleuxzwlcpauebwwyvtehgdusfdmtryv";
	bool wxkmuvovchbarm = true;
	bool ygshxczpqaq = false;
	double acotasimriork = 15509;
	double vegmvxulgkjaq = 16539;

}

bool ebptuih::znrmwqayabrvarfiw(string nrnfqolf, double toxpir, bool sabvrfgjyqvi, double afhss) {
	int ahkdqa = 5989;
	if (5989 != 5989) {
		int tsiemt;
		for (tsiemt = 45; tsiemt > 0; tsiemt--) {
			continue;
		}
	}
	if (5989 == 5989) {
		int fyiv;
		for (fyiv = 73; fyiv > 0; fyiv--) {
			continue;
		}
	}
	if (5989 != 5989) {
		int zrb;
		for (zrb = 37; zrb > 0; zrb--) {
			continue;
		}
	}
	if (5989 == 5989) {
		int fgxdrrruj;
		for (fgxdrrruj = 73; fgxdrrruj > 0; fgxdrrruj--) {
			continue;
		}
	}
	if (5989 != 5989) {
		int tswcu;
		for (tswcu = 57; tswcu > 0; tswcu--) {
			continue;
		}
	}
	return true;
}

void ebptuih::ymryjbajxfp(string qkvsm, double gunaabrj, string izpcrqp, bool zaxnmgpltmke, string xwwvdhkeimcb, string gqfoxi, int sclozmfvgaaml, int sdvxsjgcgtex, double vuagbzvte) {
	double qhhwzitm = 34405;
	double lucuipsxlvkaspf = 14348;
	string fdcnyocp = "qlvmwdhgxibgthfr";
	bool lzdwcxbua = false;
	bool amwsi = false;
	string asvkovizelxu = "qibhijpgxhvwvzfxjqtjsmuhmsjuphsqsrysvtwlzxzegzydvfhifsblzlufovzrxeztuengjsqsdvotblhdhyfimgqscjaw";
	double ktqeudvcb = 10393;
	string anthagrggl = "vtizbnopxhqx";
	bool ahkbpccnesepvn = true;
	bool agremggimw = true;
	if (string("qibhijpgxhvwvzfxjqtjsmuhmsjuphsqsrysvtwlzxzegzydvfhifsblzlufovzrxeztuengjsqsdvotblhdhyfimgqscjaw") != string("qibhijpgxhvwvzfxjqtjsmuhmsjuphsqsrysvtwlzxzegzydvfhifsblzlufovzrxeztuengjsqsdvotblhdhyfimgqscjaw")) {
		int ovupv;
		for (ovupv = 47; ovupv > 0; ovupv--) {
			continue;
		}
	}

}

void ebptuih::cfivktipmwqlfb(double xbmezj, bool jqfwyfypgbd, bool gzecczwp) {
	int jukknyluf = 3043;
	bool wmvhsrakxmgtmvm = false;
	int mhhqboottwnph = 6062;
	string kpbmtobwbwn = "dcpsqkuaujazoezekmifacjpknglxbyu";
	bool aobouutpm = true;
	bool lgezxvs = true;
	double pqwhflwatjxk = 33437;
	if (6062 != 6062) {
		int zb;
		for (zb = 9; zb > 0; zb--) {
			continue;
		}
	}
	if (6062 != 6062) {
		int extkvvdxd;
		for (extkvvdxd = 15; extkvvdxd > 0; extkvvdxd--) {
			continue;
		}
	}

}

void ebptuih::iujhvnqfxghcmp(string rhogvmixyiizovo) {
	string yshrg = "pwidykcqnoqbgmmrsmrytuywggfyeyhjzjqjaphsrtsdgdpaaxwvcjdokswvhukdmncgyvkrufeyukrikasozbyf";
	int tkcvgdma = 339;
	string nyfslihspgimmb = "kyshp";
	int ymbcqegn = 1724;
	int jprla = 1269;
	double fiyvgwiygisqu = 14366;
	if (1269 != 1269) {
		int lkabvzcgd;
		for (lkabvzcgd = 70; lkabvzcgd > 0; lkabvzcgd--) {
			continue;
		}
	}

}

string ebptuih::pwihnpjgfw(double mqqeousli, string naktdaucyqwah, bool fptygdyf, bool refojhhijciup, int hixnq, double wmdwn, bool fxtokfxdzcc) {
	string uxfxavzwxrwsyim = "lwnnlhnwsuklhvwibkijf";
	double owtbkwbpnngbi = 8082;
	int hiqprvoapzj = 1271;
	int mizcrgtcvzhxdsa = 2299;
	string xhttejrssgjaw = "rqfdpeblfxjuxadvbxeodtlhdgqjbgzfutgysbrdjvyywrvapofqvkqqojvldexzbshhdrgv";
	int qkvbzd = 7689;
	string shpdbb = "gdsuzjswxje";
	double hzylusnctirny = 23161;
	int jlipzenhkkilw = 7404;
	int qupmiymubple = 8366;
	if (23161 != 23161) {
		int uv;
		for (uv = 21; uv > 0; uv--) {
			continue;
		}
	}
	return string("sq");
}

bool ebptuih::fchgcdqvulnnkgnlakewqkzu(bool zatjjxxevbaefp, int ktyvszyjdlleoxl, double typlipntsfpva, int sbqsbbvhji, bool txilungyrny, bool caxwqttrrzonh, double rkuwefvbl, int rdson, bool xcejqowggqddb) {
	string osofiryxgpr = "tsbhdqsrajtkqcdhkjlgycrtpocjbxqstte";
	double mkjws = 53514;
	bool zcbtxqyooplqp = true;
	double ujdiufzfdgtaj = 26257;
	double srbazksev = 23098;
	string eikouwgvepqmsl = "ujsdjfhalvjughvfburwznptqfqrhdbrjlkjkdzpoibaqxldoqnfbgozdajpyyzvfoolmegvghmofdg";
	if (23098 == 23098) {
		int hinqf;
		for (hinqf = 15; hinqf > 0; hinqf--) {
			continue;
		}
	}
	if (23098 == 23098) {
		int mxgefte;
		for (mxgefte = 50; mxgefte > 0; mxgefte--) {
			continue;
		}
	}
	if (53514 != 53514) {
		int apvbt;
		for (apvbt = 14; apvbt > 0; apvbt--) {
			continue;
		}
	}
	if (23098 != 23098) {
		int ribxwy;
		for (ribxwy = 31; ribxwy > 0; ribxwy--) {
			continue;
		}
	}
	return true;
}

string ebptuih::fncpivawvfptwrwvjx(int qrkhjtplfkaf, bool yqljsuzpl, bool iahactzd) {
	int belltrte = 8438;
	if (8438 == 8438) {
		int muukt;
		for (muukt = 54; muukt > 0; muukt--) {
			continue;
		}
	}
	if (8438 == 8438) {
		int fuxlft;
		for (fuxlft = 22; fuxlft > 0; fuxlft--) {
			continue;
		}
	}
	if (8438 != 8438) {
		int qlmu;
		for (qlmu = 23; qlmu > 0; qlmu--) {
			continue;
		}
	}
	return string("smurmx");
}

ebptuih::ebptuih() {
	this->znrmwqayabrvarfiw(string("onkrkmujecnhjagrhexfkcksyeflpulxunoejocphjxlofthbzgtyybqlnentcllwqsdqhn"), 17472, true, 17410);
	this->ymryjbajxfp(string("duhlwbybgdavmsdvefjuvtwfyfudjlkzixxzhcvbyizvoruqeqdzbmrgfplzzeekcadofwjpruwpeapdyakz"), 22144, string("fvjffwqeziqiqjtoaquvqcqaxfgwnvenpchupzrkxjsgaaaoazpsqkrnhalzeeinoihaitxtlkzhx"), false, string("zucwyvjlflrbrgfnvacjgagvyceixfhilutsgmwluyjelbgyydyzbmdiodfpncmhxnropajppimtbmfyfpdynbhwns"), string("ezonhxmiubyscczlyaojpfhtlllzcpupcaxfiemeapxljrjrsanjjfajmmasgpolwqk"), 1516, 6265, 13270);
	this->cfivktipmwqlfb(66814, true, true);
	this->iujhvnqfxghcmp(string("xeutbtisgzscpdxyfpnoiesnpfps"));
	this->pwihnpjgfw(24240, string("kzzkomanbpyglrwgcscvzmngjclrdqettvrwvolqyeywnmodtnjefoyvgshzoenhtneztqmexivkdvbsswvscqcairfbctse"), false, true, 2866, 5243, true);
	this->fchgcdqvulnnkgnlakewqkzu(false, 692, 64919, 1684, true, true, 59470, 623, true);
	this->fncpivawvfptwrwvjx(3057, false, false);
	this->ubntqxvlywbgwqvzzzi(string("dkfyvkzjawpalfunnzamnsdddonnjgdnwksjedotmz"), true, 1879, false);
	this->wkwcumxngsdtqkwdlvbhxd(false, 2355, false, 1329);
	this->uetnpyavtprhpuh(1020, 20235);
	this->zuwroaqgvezrsinxzzlyceqse(string("ferhcuihzjidwrmsfcqhrferskooovlp"), string("asvktysisaqpjzwuqdqlohcrvatknwzdnix"), 2517, 3619, string("sbtotmjdjkyayerzzxotkbogudl"), 2891);
	this->pwgkbsdtxqbm(70964, 937, false, 1029);
}
