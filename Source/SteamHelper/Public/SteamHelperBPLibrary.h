// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SteamHelperType.h"
#include "SteamHelperBPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STEAMHELPER_API USteamHelperBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static FString GetSteamID();

	UFUNCTION(BlueprintCallable)
	static FString GetAccountID();

	UFUNCTION(BlueprintCallable)
	static FString GetPersonaName();

	UFUNCTION(BlueprintCallable)
	static FString GetIPCountry();

	UFUNCTION(BlueprintCallable)
	static bool GetAvatarBrush(const FCSteamID& SteamID, FSlateBrush& Brush);

	UFUNCTION(BlueprintCallable)
	static bool GetPersonalUserInfo(FSteamUserInfo& UserInfo);

	UFUNCTION(BlueprintPure)
	static int32 GetFriendCount();

	UFUNCTION(BlueprintCallable)
	static bool GetFriendsInfo(TArray<FSteamFriendInfo>& FriendsInfo);

	UFUNCTION(BlueprintCallable)
	static bool GetFriendUserInfo(const FCSteamID& FriendID, FSteamUserInfo& Info);

	UFUNCTION(BlueprintCallable)
	static ESteamFriendOnlineState GetFriendState(const FCSteamID& FriendID);

private:


};
