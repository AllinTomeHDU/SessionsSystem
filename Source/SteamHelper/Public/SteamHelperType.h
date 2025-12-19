#pragma once

#include "CoreMinimal.h"
#include "SteamHelperType.generated.h"


enum ESteamUniverse
{
	EUniverseInvalid = 0,
	EUniversePublic = 1,
	EUniverseBeta = 2,
	EUniverseInternal = 3,
	EUniverseDev = 4,
	// k_EUniverseRC = 5,				// no such universe anymore
	EUniverseMax
};

USTRUCT(BlueprintType)
struct STEAMHELPER_API FCSteamID
{
	GENERATED_BODY()
	
public:
	FCSteamID() {}
	FCSteamID(uint64 InSteamID64)
	{
		ID.SteamID = InSteamID64;
	}

	uint64 GetSteamID() const { return ID.SteamID; }

	uint32 GetAccountID() const { return ID.m_comp.m_unAccountID; }

private:
	union SteamID_t
	{
		struct SteamIDComponent_t
		{
			uint32				m_unAccountID : 32;			// unique account identifier
			unsigned int		m_unAccountInstance : 20;	// dynamic instance ID
			unsigned int		m_EAccountType : 4;			// type of account - can't show as EAccountType, due to signed / unsigned difference
			ESteamUniverse		m_EUniverse : 8;			// universe this account belongs to
		} m_comp;

		uint64 SteamID;
	} ID;
};

USTRUCT(BlueprintType)
struct STEAMHELPER_API FSteamUserInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SteamID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AccountID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PersonaName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString IPCountry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush ProfileBrush;
};

UENUM(BlueprintType)
enum class ESteamFriendOnlineState : uint8
{
	Offline,		// 离线
	Online,			// 在线
	Busy,			// 忙碌
	Away,			// 离开
	Snooze,			// 打盹
	LookingToTrade, // 想要交易
	LookingToPlay,  // 想要玩游戏
};

USTRUCT(BlueprintType)
struct STEAMHELPER_API FSteamFriendInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSteamUserInfo UserInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESteamFriendOnlineState OnlineState;
};