// Copyright Epic Games, Inc. All Rights Reserved.

#include "SteamHelper.h"
#include "Steamworks/Steamv151/sdk/public/steam/steam_api.h"

#define LOCTEXT_NAMESPACE "FSteamHelperModule"

void FSteamHelperModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	bool bSteamInit = SteamAPI_Init();
	if (!bSteamInit)
	{
		UE_LOG(LogTemp, Warning, TEXT("SteamAPI_Init failed!"));
	}
}

void FSteamHelperModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	SteamAPI_Shutdown();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSteamHelperModule, SteamHelper)