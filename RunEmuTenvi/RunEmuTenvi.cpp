#include"../Share/Simple/Simple.h"

#define EXE_NAME L"RunEmuTenvi"
#define DLL_NAME L"EmuLoaderTenvi"

bool RunEmu(std::wstring wDroppedTarget) {
	Config conf(EXE_NAME".ini");
	std::wstring wTarget, wCmdLine, wLoaderDLL, wMainDLL;
	bool check = true;

	if (wDroppedTarget.length()) {
		conf.Update(EXE_NAME, L"TargetEXE", wDroppedTarget);

		std::wstring wDeafultLoaderDLL = DLL_NAME".dll";
		conf.Update(EXE_NAME, L"LoaderDLL", wDeafultLoaderDLL);
	}

	check &= conf.Read(EXE_NAME, L"TargetEXE", wTarget);
	conf.Read(EXE_NAME, L"CmdLine", wCmdLine);
	check &= conf.Read(EXE_NAME, L"LoaderDLL", wLoaderDLL);

	if (!check) {
		return false;
	}

	std::wstring dir;
	if (!GetDir(dir)) {
		return false;
	}

	Injector injector(wTarget, dir + wLoaderDLL);
	return injector.Run(wCmdLine);
}


bool RunEmuTest(std::wstring wDroppedTarget, std::wstring wRegion) {
	Config conf(EXE_NAME".ini");
	std::wstring wTarget, wCmdLine, wLoaderDLL, wMainDLL;
	bool check = true;
	conf.Read(EXE_NAME, L"CmdLine", wCmdLine);
	check &= conf.Read(EXE_NAME, L"LoaderDLL", wLoaderDLL);
	conf.Update(EXE_NAME, L"Region", wRegion);

	if (!check) {
		return false;
	}

	std::wstring dir;
	if (!GetDir(dir)) {
		return false;
	}

	Injector injector(wDroppedTarget, dir + wLoaderDLL);
	return injector.Run(wCmdLine);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {

	if (__argc < 2) {
		if (!RunEmu(L"")) {
			MessageBoxW(NULL, L"Please check target exe file path", EXE_NAME, MB_OK);
		}
	}
	else {
		if (__argc == 3) {
			if (!RunEmuTest(__wargv[1], __wargv[2])) {
				MessageBoxW(NULL, L"error: cmdline = path region", EXE_NAME, MB_OK);
			}
			return 0;
		}
		if (!RunEmu(__wargv[1])) {
			MessageBoxW(NULL, L"Please drop target exe file", EXE_NAME, MB_OK);
		}
	}

	return 0;
}