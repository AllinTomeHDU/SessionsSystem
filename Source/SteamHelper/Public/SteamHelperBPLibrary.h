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
	
	/*
	* 获取个人信息
	*/
public:
	UFUNCTION(BlueprintPure)
	static FString GetSteamID();

	UFUNCTION(BlueprintPure)
	static FString GetAccountID();

	UFUNCTION(BlueprintPure)
	static FString GetPersonaName();

	UFUNCTION(BlueprintPure)
	static FString GetIPCountry();

	UFUNCTION(BlueprintPure)
	static bool GetPersonalUserInfo(FSteamUserInfo& UserInfo);

	UFUNCTION(BlueprintPure)
	static FSlateBrush GetPersonalAvatarBrush();

	/*
	* 获取好友信息
	*/
	UFUNCTION(BlueprintPure)
	static int32 GetFriendCount();

	UFUNCTION(BlueprintCallable)
	static bool GetFriendsInfo(TArray<FSteamFriendInfo>& FriendsInfo);

	UFUNCTION(BlueprintCallable)
	static bool GetFriendUserInfo(const FCSteamID& FriendID, FSteamUserInfo& Info);

	UFUNCTION(BlueprintCallable)
	static ESteamFriendOnlineState GetFriendState(const FCSteamID& FriendID);

private:
	static bool GetAvatarBrush(const FCSteamID& SteamID, FSlateBrush& Brush);


};
