#include <Windows.h>
#include "vlizer.h"
#include "Detours.h"
#include "se.h"
#include "DetourControl.h"
#include "WindowName.h"
#include "Splash.h"
#include "PakFilter.h"
#include "Misc.h"
#include "Guard.h"
#include "Crypto.h"
#include "Security.h"
#include "DEBUG.h"
#include "Radio.h"
#include "RamReducer.h"
#include <VMProtectSDK.h>

#pragma warning(push)
#pragma warning(disable:4005)
#pragma warning(disable:4200)
#pragma warning(pop)

void Main()
{
	LOAD_DEBUG_SWITCHES();


	//VMProtectBeginVirtualization("Entry Point");
	//Do integrity check!
#if defined(RO) || defined (CHN) || defined(CHN_MSTERE)
	s.IntegrityCheck();
#endif

	//Hide DLL
	s.hideModule();

	//Check Arguments
	checkCommandLine();

	//New CRYPTO
#if defined(RO) || defined(CA) || defined(CHN) || defined(CHN_MSTERE) || defined(THA)
	ChangeCryptoSeedKey();
#endif
	//Init Guard!
	InitGuard();

	//Load Filter
#if defined (AR) || defined(THA) || defined(RU) || defined (VTM)
#if !defined(DEBUG)
	CreateThread(NULL, 0, LoadFilterFile, NULL, NULL, 0); //Filter Thread
#endif
#endif

#if !defined (RU) && !defined (AR)
	//Multi Client
	generateRandomMutex();
#endif

	//Show Splash Screen
#if defined (AR)	
	SplashScreenGuard();
#else
#if !defined(CHN_MSTERE)
	SplashScreen();
#endif
#endif
	
	//Set Window Name
	CreateThread(NULL, 0, setWindowThread, NULL, NULL, 0);

	//Load Detours.
	startDetours();

#ifdef RADIO
	//--
	CreateThread(NULL, 0, InitRadioThread, NULL, NULL, 0);
	CreateThread(NULL, 0, HideRadioThread, NULL, NULL, 0);
#ifdef THA
	CreateThread(NULL, 0, RadioAutoStart, NULL, NULL, 0);
#endif
#endif

#ifdef OPTI
	//Game Optimizations
	//CreateThread(NULL, 0, ReduceRAMThread, NULL, NULL, 0);
#endif
	

	//Load DEBUG Console
#ifdef DEBUG
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
#endif

	//VMProtectEnd();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, PVOID lpReserved)
{
	//VIRTUALIZER_DOLPHIN_WHITE_START
		switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
				gHInst = hModule;
				DisableThreadLibraryCalls(hModule);
				Main();
			break;


		case DLL_PROCESS_DETACH:
				destroyDetours();
				//WSACleanup();
			break;
	}
	//VIRTUALIZER_DOLPHIN_WHITE_END

	return TRUE;
}
